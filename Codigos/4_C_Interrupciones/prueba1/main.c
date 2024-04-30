/**
 
 @file main.c
 
 @brief Archivo principal.
 
En este codigo se hace lectura de la frecuencia leida por un pin GPIO y se muestra en 6 LEDS 7 segmentos la variable 
que se está leyendo esto con el objetivo de cononcer cual es la frecuencia maxima leída por interrupciones en la raspberry pi pico

 */

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"

#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include <stdio.h>
#include "hardware/timer.h"
#include <pico/time.h>
#include "pico/multicore.h"

/**
 * @brief Se definen cada uno de los pines a utilizar en el sistema donde se le asigna a los transistores y lectura de 
 * frecuencia
*/
#define PIN 17
#define TIMER_MS 1000
#define transistorPin1 10
#define transistorPin2 9
#define transistorPin3 8
#define transistorPin4 11
#define transistorPin5 12
#define transistorPin6 13


const int Pines_segmentos[] = {1, 2, 3, 4, 5, 6, 7};
int frecuencia=0;
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

/***
 * @brief impresión en 6 LEDS 7 SEGMENTOS
 * Esta función se usa para la impresión de la frecuencia leida por el sistema utilizando interrupciones , donde se hace
 * un swithcheo rapido entre todos los transistores para encenderlos y apagarlos rapidamente para dar la impresión de que el
 * sistema se encuentra encendido todo el tiempo , pero en realidad se está actualizando constantemente cada display. 
 * la variable de frecuencia  @param frecuencia, es la encargada de separar en unidad, centenas, decenas la variable de
 * frecuencia leida y mandarle el valor a cada uno de los 7 segmentos 

*/

// Función para mostrar un número en un display específico
void Mostrar_Numero() {
    int Pin_display=6;
    int contador_mostrar=0;
    
    while (1) {
        // Esperar a recibir datos desde el otro núcleo
       
       
        

        // Desactivar todos los transistores
        gpio_put(transistorPin1, 0);
        gpio_put(transistorPin2, 0);
        gpio_put(transistorPin3, 0);
        gpio_put(transistorPin4, 0);
        gpio_put(transistorPin5, 0);
        gpio_put(transistorPin6, 0);
        
        int frecuencias[]={frecuencia / 100000,(frecuencia / 10000) % 10,(frecuencia / 1000) % 10,(frecuencia / 100) % 10,
        (frecuencia / 10) % 10,frecuencia % 10
        };
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


    sleep_ms(1);
    
    }
}





int contador =0;


void interrupt_handler();


/**
 * @brief timer_callback Funcion callback que se usa en la definición del timer, se accede a ella cada x tiempo definido.
 * @param repeating_timer_t *rt Puntero a información para configuración del timer.
 * @return Booleano para resultado de lo evaluado.
*/
bool timer_callback(repeating_timer_t *rt);

/***
 * @brief Ejecución de función main
 * Esta función es la encargada de inicializar cada uno de los pines GPIO utilizados, tanto para escribir en los 7 
 * segmentos , como para encender o apagar los transistores y para hacer la lectura de frecuencia leida. Se inicializa
 * el multicore para separar la lectura de frecuencia con la impresión de la lectura leida. Ademas se usa la función
 * gpio_set_irq_enabled_with_callback() donde se le indica que capture los flancos de subida leidos en el puerto para
 * activar la interrupción y viajar a la función interrupt_handler() e inicializar el timer de la interrupción. 

*/

int main() {

    stdio_init_all();
    
    gpio_init(PIN);

    gpio_set_dir(PIN,GPIO_IN);
   
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

    multicore_launch_core1(Mostrar_Numero); // Lanzar la función en el otro núcleo





    gpio_set_irq_enabled_with_callback(PIN, GPIO_IRQ_EDGE_RISE, true, &interrupt_handler);

    repeating_timer_t my_timer;

    repeating_timer_callback_t my_callback = timer_callback;
    // Configura el número de pin que deseas usar para el LED
   
    if (add_repeating_timer_ms(TIMER_MS, my_callback, NULL, &my_timer)) {
        printf("Temporizador creado con éxito!\n");
    } else {
        printf("Error al crear el temporizador.\n");
        return 1;
    }

    while(true){

        printf("%d \n ", frecuencia );


        
    }
    return 0;
}

/***
 * @brief Interrupt Handler
 * Esta función es la encargada de aumentar en 1 la variable @param contador esta función se activa cada que 
 * un flanco de subida es captado.
*/

void interrupt_handler() {
 contador++;
}

/***
 * @brief Función callBack del temporizador
 * Esta función es la encargada de ejecutarse cada que el temporizador expire,y se le lleva a frecuencia el valor contado
 * esta función se usa para actualizar la variable @param frecuencia , con la cantidad de conteos que realizó el @param contador
*/
// Implementa la función de callback del temporizador
bool timer_callback(repeating_timer_t *rt) {
    // Esta función se ejecutará cada vez que el temporizador expire
    // Puedes realizar acciones periódicas aquí
    frecuencia =  contador;
    contador = 0 ;
    //printf("%d \n ", frecuencia );
    return true;
}



