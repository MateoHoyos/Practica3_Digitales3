/**
 * @file polling+interrupciones_7seg
 * @mainpage Medidor de frecuencia usando el flujo de programación de polling+interrpuciones y mostrando el valor de frecuencia 
 * en 6 displays 7 segmentos, la frecuencia máxima obtenida fue 495kHz.
 * 
 * 
 * 
 * @section Descripción 
 * Para poder medir la frecuencia usando polling+interrupciones cada segundo se lee el valor de contador
 * que es la variable que almacena el número de flancos positivos y se le asigna este valor 
 * a la variable frecuencia que por lo tanto cambia cada segundo y es el valor 
 * que se muestra en los display de 7 segmentos. Para saber la canitdad de flancos de subida, se usa la función 
 * propia de la raspberry pi pico y del lenguaje C para las interrupciones, que cada que se produce un cambio 
 * en el GPIO17, se ejecuta la función contador frecuencia que suma de a uno a un contador. 
*/


#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/timer.h"
#include "pico/multicore.h"

/**
 * @brief Pines para controlar cada transistor por separado, esto es para la multiplexación de los 6 displays
*/
#define PIN_Raspberry 17 //Pin de entrada del pwm generado desde el arduino
#define transistorPin1 10
#define transistorPin2 9
#define transistorPin3 8
#define transistorPin4 11
#define transistorPin5 12
#define transistorPin6 13

 /**
    * @param contador Variable que sirve para contar el número de flancos de subida que hay en el GPIO17. 
    * Esta variable se modifica a partir de una interrupción
*/
int contador=0;
/**
 * @param Pines_segmentos arreglo que sirve para almacenar los pines de los segmentos de los 6 displays, debido a que están 
 * multiplexados, por lo tanto en el GPIO1 se conecta el segmento a, en el GPIO2 en segmento b y así sucesivamente,
 * que el tipo de variable sea arreglo sirve para luego acceder a cada posición y poder encender un segmento u otro.
*/

const int Pines_segmentos[] = {1, 2, 3, 4, 5, 6, 7};
/**
 * @param frecuencia Variable global que luego se le asigna el valor del número de pulsos positivos contados en un segundo, 
 * con lo que corresponde a la frecuencia. Es global porque se modifica en todo el código, para poder que en el segundo núcleo 
 * de la Raspberry se pueda usar el valor de esta variable para ser escrito en los 6 displays. 
*/
int frecuencia=0;

/**
 * @param digitos Variable que almacena las combinaciones para los número del 0 al 9 en formato display 7 segmentos cátodo común.
*/
const uint64_t digitos[] = {
    0b0111111, // 0
    0b0000110, // 1
    0b1011011, // 2
    0b1001111, // 3
    0b1100110, // 4
    0b1101101, // 5
    0b1111101, // 6
    0b0000111, // 7
    0b1111111, // 8
    0b1101111  // 9
};


/**
 * @brief Contador_frecuencia 
 * Función asociada a la interrupción, es decir la rutina de atención a interrupciones, que cada que se produce una interrupción, 
 * se le suma uno a la variable contador. 
*/
void Contador_frecuencia(){
    contador++;

}


/**
 * @brief Mostrar_Numero 
 * Función que se ejecuta en el segundo núcleo de la raspberry pi pico, en este a partir del valor de frecuencia 
 * se le envía el número completo a cada display, el primer display muestra las centenas de miles, el segundo display las decenas 
 * de miles y así sucesivamente hasta llegar al último display que muestra las unidades. Cabe resaltar que entre cada display 
 * hay 1ms de diferencia para escribir en el, con este tiempo tan bajo se da la ilusión que los 6 displays están prendidos al mismo 
 * tiempo y así se puede leer el valor de frecuencia que cambia cada segundo. 
*/

void Mostrar_Numero() {
     /**
     * @param Pin_display se empieza en 6 para empezar escribiendo el display que corresponde a las centenas de miles y se reduce en 
     * 1 cada vez que se ejecuta el ciclo para que luego se escriba en el display 5(Activando solo el transistor 5) para que 
     * se escriba el valor de las decenas de miles y así sucesivamente, cuando este valor es 0 es porque se tiene que volver 
     * a escribir el display 6.
    */
    int Pin_display=6;
    /**
     * @param contado_mostrar este contador permite escribir en los displays en los que se lea que un dígito es 1 según 
     * el dígito leído de frecuencia, empieza en 0 y llega hasta 7, que son el número de segmentos que tiene un display de 
     * 7 segmentos. 
    */
    int contador_mostrar=0;
    
    while (1) {

       
       
        

        // Primero se desactivan todos los transistores
        gpio_put(transistorPin1, 0);
        gpio_put(transistorPin2, 0);
        gpio_put(transistorPin3, 0);
        gpio_put(transistorPin4, 0);
        gpio_put(transistorPin5, 0);
        gpio_put(transistorPin6, 0);
        
        
        /**
         * @param frecuencias Se segmentan el valor de frecuencia en 6 valores individuales y se guarda en un arreglo 
         * para luego poder acceder a cada dígito y escribir en el correspondiente display el dígito, para así 
         * escribir la frecuencia leída en orden en los 6 displays. 
        */
        int frecuencias[]={frecuencia / 100000,(frecuencia / 10000) % 10,(frecuencia / 1000) % 10,(frecuencia / 100) % 10,
        (frecuencia / 10) % 10,frecuencia % 10
        };

        /**
         * Según el pin del display se activa solo un transistor mientras los otros siguen desactivados, 
         * para así solo escribir en un display al tiempo, para esto en un principio se desactivan todos los transistores.
        */
        if (Pin_display == 1) {
            gpio_put(transistorPin1, 1);
        } else if (Pin_display == 2) {
            gpio_put(transistorPin2, 1);
        } else if (Pin_display == 3) {
            gpio_put(transistorPin3, 1);
        }
        else if (Pin_display == 4) {
            gpio_put(transistorPin4, 1);
        } else if (Pin_display == 5) {
            gpio_put(transistorPin5, 1);
        
        } else if (Pin_display == 6) {
            gpio_put(transistorPin6, 1);
        }

        /**
         * Se recorren los 7 segmentos que tiene un display y se revisa al transformar el valor leído de frecuencia 
         * en el formato que hay en la variable digitos, con esto cuando se detecta que hay un 1, se manda un 1 
         * al segmento correspondiente en el display. 
        */
        for (int i = 0; i < 7; i++) {
            if (digitos[frecuencias[contador_mostrar]] & (1 << i)) {
                gpio_put(Pines_segmentos[i], 1);
            } else {
                gpio_put(Pines_segmentos[i], 0);
            }
    }
        Pin_display--;
        contador_mostrar++;
        if(Pin_display==0){
            Pin_display=6;
            contador_mostrar=0;
        }

    /**
     * @param sleep_ms Se espera 1ms entre que se escribe un display y otro, esto con la finalidad de que el tiempo 
     * sea lo suficientemente grande como para que el número escrito se pueda ver bien en el display y un tiempo 
     * lo suficientemente pequeño para que parezca que los 6 displays están encendidos al tiempo.
    */
    sleep_ms(1);
    
    }
}
// Main funtion 
int main() {
    // SDIO Initialization
    stdio_init_all();
    /**
     * @param tiempo_transcurrido Esta variable se usa para poner como tope máximo captura del valor de frecuencia 
     * en 1s, para esto en primera instancia se le asigna 1000000us que son 1s, luego se le suma el valor de 
     * tiempo_actual+1s que hace que cada segundo se entre al condicional que lee el valor de frecuencia. 
    */

    uint64_t tiempo_transcurrido=1000000;
    //Inicialización de todos los pines de la raspberry usados. 
    gpio_init(PIN_Raspberry);
    gpio_set_dir(PIN_Raspberry, GPIO_IN);
    //Interrupción que asociada al GPIO17, que se activa en el flanco de subida y se asocia a la función de rutina de 
    //atención de interrupción llamada Contador_frecuencia.
    gpio_set_irq_enabled_with_callback(PIN_Raspberry,GPIO_IRQ_EDGE_RISE,true,&Contador_frecuencia);
    gpio_init(transistorPin1);
    gpio_set_dir(transistorPin1, GPIO_OUT);
    gpio_init(transistorPin2);
    gpio_set_dir(transistorPin2, GPIO_OUT);
    gpio_init(transistorPin3);
    gpio_set_dir(transistorPin3, GPIO_OUT);
    gpio_init(transistorPin4);
    gpio_set_dir(transistorPin4, GPIO_OUT);
    gpio_init(transistorPin5);
    gpio_set_dir(transistorPin5, GPIO_OUT);
    gpio_init(transistorPin6);
    gpio_set_dir(transistorPin6, GPIO_OUT);
    for (int i = 0; i < 7; i++) {
        gpio_init(Pines_segmentos[i]);
        gpio_set_dir(Pines_segmentos[i], GPIO_OUT);
    }
    multicore_launch_core1(Mostrar_Numero); // Lanzar la función Mostrar_numero en el otro núcleo de la raspberry.

    while(1) {        
    /**
        * @param tiempo_actual variable que almacena el tiempo de ejecución actual del programa y cuando es 
        * mayor a 1s de lee el valor del contador. 
    */
    
    uint64_t tiempo_actual = time_us_64();   
      // Se compara para saber si ha pasado 1 segundo

        /**
         *  Cada segundo se le asigna el valor de contador a frecuencia, es decir que se lee cuanta frecuencia se genera 
         * en 1s y así con la variable global frecuencia se muestra el valor en los displays de 7 segmentos. 
        */
        if (tiempo_actual >= tiempo_transcurrido)
        {
            
            frecuencia = contador;
          
            
            printf("contador leidaaa : %d\n",frecuencia);
            // Se reinicia el contador de pulsos
            contador = 0;
            tiempo_transcurrido=tiempo_actual+1000000;
    }
    }
    return 0;

}