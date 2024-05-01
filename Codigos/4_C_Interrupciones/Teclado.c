/**
 * @file Teclado_de_usuario
 *
 * @brief Código principal para el sistema de acceso ID-Clave con teclado matricial 4x4
 * Definición del teclado, captura de teclas, función antirrebote, creación de usuarios y 
 * contraseñas, chequeo de usuarios y contraseñas, llamado al archivo Led.h que sirve 
 * para todas las señales luminosas, manejo de temporizadores para las señales luminosas 
 * y tiempos de ejecución del sistema 
 */


#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/time.h"
#include "string.h"
#include "hardware/gpio.h"
#include "hardware/timer.h"
#include "Teclado.h"
#include <time.h>

/**
 * @brief Creación del teclado matricial 4x4
*/
const char matrix_keys[4][4] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}
};


/**
 * @brief Creación de variables
 * Se crean variables tanto para la lectura del teclado en filas y columnas 
 * Variables para el antirrebote.
*/
const int keypad_rows1[4] = {2, 3, 4, 5};
const int keypad_columns[4] = {6, 7, 8, 9};
char amplitud[6] = "";
char nivelDC[6] = "";
char frecuencia[7] = "";
bool flag_a = false;
bool flag_b = false;
bool flag_c = false;
int contA = 0;
int contB = 0;
int contC = 0;
int valueA = 0;
int valueDC = 0;
int valueF = 0;
int value = 0;
bool AntR1=true;
bool rst1=false;
bool tecla_antirrebote[4][4] = {false};
bool tecla_presionada = false;

/**
 * @brief Setup
 * Se inicializa el teclado según los pines para las columnas y filas 
*/
bool verificarTiempoTranscurrido1(int segundos,bool rst) {
    static time_t inicio = 0; // Variable estática para almacenar el tiempo de inicio
    if(rst){inicio=0;}
    if (inicio == 0) {
        inicio = time(NULL); // Obtener el tiempo de inicio al llamar por primera vez
    }

    time_t actual = time(NULL); // Obtener el tiempo actual
    time_t tiempo_transcurrido = actual - inicio; // Calcular el tiempo transcurrido

    return tiempo_transcurrido >= segundos; // Devolver verdadero si ha transcurrido el tiempo especificado
}
void setup() {
    stdio_init_all();
    
    /**
     * @param keypad_rows variable en la que se almacena la fila del teclado 
     * Se configura como salida, ya que como se verá dentro de la función scan_keys()
     * las filas siempre van en 1, debido a que se busca cuando la columna que se presiones 
     * esté en uno y así se detecte la tecla que se presiona 
     * 
    */
    for (int i = 0; i < 4; i++) {
        gpio_init(keypad_rows1[i]);
        gpio_set_dir(keypad_rows1[i], GPIO_OUT);
    }
    /**
     * @param keypad_columns variable que almacena las columnas del teclado
     * Se inicializa como salida y como pull_down ya que cuando se presiona una tecla, 
     * esta variable se pone en uno, encontrando así la tecla presionada, es por esto 
     * que se pone @param gpio_pulldown_keypad_columns para que se mantenga el estado 
     * lógico de 0 cuando no se está aplicando una señal externa al pin de la Raspberry.
    */
    for (int i = 0; i < 4; i++) {
        gpio_init(keypad_columns[i]);
        gpio_set_dir(keypad_columns[i], GPIO_IN);
        gpio_pull_down(keypad_columns[i]);
    }
}

/**
 * @brief 
 *
 * Esta función es la entrada por teclado y extrae la contraseña de que se está ingresando
 * que se encuentra en el índice 4 al 9 de la y se almacena en la cadena de caracteres 'password'. 
 * La contraseña se termina con el carácter nulo '\0'.
 *
 * @param entrada arreglo de caracteres que contiene la entrada completa del usuario.
 * @param password arreglo de caracteres donde se almacena la contraseña extraída de entrada.
 */
int convert_to_int(char entrada[], char inicio, char fin){
    // Variable para almacenar el número entero resultante
    int numero = 0;
    // Variable para indicar si estamos dentro del rango entre inicio y fin
    int Rango = 0;

    // Recorremos el vector de caracteres
    for (int i = 0; entrada[i] != '\0'; i++) {
        // Si encontramos el carácter de inicio, activamos la bandera
        if (entrada[i] == inicio) {
            Rango = 1;
        }
        // Si estamos dentro del rango y encontramos un dígito, lo acumulamos en el número
        else if (Rango && entrada[i] >= '0' && entrada[i] <= '9') {
            numero = numero * 10 + (entrada[i] - '0');
        }
        // Si encontramos el carácter de fin, salimos del ciclo
        else if (Rango && entrada[i] == fin) {
            break;
        }
    }
    return numero;
}




void get_value(char *let, int *ampl, int *DC, int *freq){
    printf("Presionaste algo\n");
    for (int row = 0; row < 4; row++) {
        gpio_put(keypad_rows1[row], 1);
        for (int column = 0; column < 4; column++) {
            if (gpio_get(keypad_columns[column]) && AntR1) {
                *let = matrix_keys[row][column];
                //sleep_ms(500);
                tecla_antirrebote[row][column] = true;
                AntR1=false;
                // se obtiene la amplitud de la señal que se ingresa por el teclado
                if ((*let == 'A') || (flag_a)){
                    flag_a = true;
                    printf("Presionaste %c\n", *let);
                    amplitud[contA] = *let;
                    contA++;
                    if (*let == 'D'){
                        flag_a = false;
                        value = convert_to_int(amplitud, 'A', 'D');

                        if(value >= 100 && value <= 2500){
                            *ampl = value; 
                        }
                        contA = 0;
                        memset(amplitud, 0, sizeof(amplitud));
                    }
                    if (contA > 5){
                        contA = 0;
                    }
                }

                // se obtiene el nivel DC de la señal que se ingresa por el teclado
                if ((*let == 'B') || (flag_b)){
                    flag_b = true;
                    printf("Presionaste %c\n", *let);
                    nivelDC[contB] = *let;
                    contB++;
                    if (*let == 'D'){
                        flag_b = false;
                        value = convert_to_int(nivelDC, 'B', 'D');
                        if(value >= 0 && value <= 1250){
                            *DC = value; 
                        }
                        contB = 0;
                        memset(nivelDC, 0, sizeof(nivelDC));
                    }
                    if (contB > 5){
                        contB = 0;
                    }
                }

                // se obtiene la frecuencia de la señal que se ingresa por el teclado
                if ((*let == 'C') || (flag_c)){
                    flag_c = true;
                    printf("Presionaste %c\n", *let);
                    frecuencia[contC] = *let;
                    contC++;

                    if (*let == 'D'){
                        flag_c = false;
                        value = convert_to_int(frecuencia, 'C', 'D');
                        if(value >= 1 && value <= 10000){
                            *freq = value; 
                        }
                        contC = 0;
                        memset(frecuencia, 0, sizeof(frecuencia));
                    }
                    if (contC > 6){
                        contC = 0;
                    }
                }

            }
            else if (!gpio_get(keypad_columns[column])) {
                // La tecla se ha soltado, ayuda en el antirrebote 
                tecla_antirrebote[row][column] = false;
            }
        }
        if(!AntR1){
            if(verificarTiempoTranscurrido1(1,rst1)){
                rst1=true;
                AntR1=true;
                }
            else{rst1=false;}
        }
        gpio_put(keypad_rows1[row], 0);
    }
}