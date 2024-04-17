#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include <time.h>
#include <unistd.h>
#include <stdbool.h>
#include "hardware/gpio.h"
#include "hardware/timer.h"
#include <math.h>


// Definir pines para el decodificador 
// #define PIN_A 21    //Menos significativo DCBA
// #define PIN_B 20
// #define PIN_C 18
// #define PIN_D 19
// #define PIN_E 13
// #define PIN_F 12
// #define PIN_G 11
// #define PIN_H 10
#define NUM_puntos 255

uint8_t binarios_senoidal [NUM_puntos] = {
         127, 126, 124, 123, 121, 119, 118, 116, 
         115, 113, 111, 110, 108, 107, 105, 104, 
         102, 100, 99, 97, 96, 94, 93, 91, 90, 
         88, 87, 85, 83, 82, 80, 79, 78, 76, 75, 
         73, 72, 70, 69, 67, 66, 65, 63, 62, 60, 
         59, 58, 56, 55, 54, 52, 51, 50, 49, 47, 
         46, 45, 44, 42, 41, 40, 39, 38, 37, 36, 
         34, 33, 32, 31, 30, 29, 28, 27, 26, 25, 
         24, 23, 22, 21, 21, 20, 19, 18, 17, 16, 
         16, 15, 14, 13, 13, 12, 11, 11, 10, 9,
          9, 8, 8, 7, 7, 6, 6, 5, 5, 4, 4, 4, 3, 
          3, 2, 2, 2, 2, 1, 1, 1, 1, 1, 0, 0, 0, 
          0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1,
           1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 4, 4, 5, 
           5, 5, 6, 6, 7, 7, 8, 9, 9, 10, 10, 11, 
           12, 12, 13, 14, 15, 15, 16, 17, 18, 18, 
           19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 
           29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 
           41, 42, 43, 44, 45, 47, 48, 49, 50, 52, 53, 
           54, 56, 57, 58, 60, 61, 62, 64, 65, 67, 68, 
           70, 71, 72, 74, 75, 77, 78, 80, 81, 83, 84, 
           86, 87, 89, 90, 92, 93, 95, 97, 98, 100, 101, 
           103, 104, 106, 108, 109, 111, 112, 114, 115, 
           117, 119, 120, 122, 123, 125, 127, 128, 130, 
           132, 133, 135, 136, 138, 140, 141, 143, 144, 
           146, 147, 149, 151, 152, 154, 155, 157, 158, 
           160, 162, 163, 165, 166, 168, 169, 171, 172, 
           174, 175, 177, 178, 180, 181, 183, 184, 185, 
           187, 188, 190, 191, 193, 194, 195, 197, 198, 
           199, 201, 202, 203, 205, 206, 207, 208, 210, 
           211, 212, 213, 214, 216, 217, 218, 219, 220, 
           221, 222, 223, 224, 225, 226, 227, 228, 229, 
           230, 231, 232, 233, 234, 235, 236, 237, 237, 
           238, 239, 240, 240, 241, 242, 243, 243, 244, 
           245, 245, 246, 246, 247, 248, 248, 249, 249, 250, 250,
            250, 251, 251, 252, 252, 252, 253, 253, 253, 253, 254, 
            254, 254, 254, 254, 255, 255, 255, 255, 255, 255, 255,
             255, 255, 255, 255, 255, 255, 255, 254, 254, 254, 254,
              254, 253, 253, 253, 253, 252, 252, 251, 251, 251, 250, 
              250, 249, 249, 248, 248, 247, 247, 246, 246, 245, 244, 
              244, 243, 242, 242, 241, 240, 239, 239, 238, 237, 236, 
              235, 234, 234, 233, 232, 231, 230, 229, 228, 227, 226, 
              225, 224, 223, 222, 221, 219, 218, 217, 216, 215, 214, 
              213, 211, 210, 209, 208, 206, 205, 204, 203, 201, 200, 
              199, 197, 196, 195, 193, 192, 190, 189, 188, 186, 185, 
              183, 182, 180, 179, 177, 176, 175, 173, 172, 170, 168, 
              167, 165, 164, 162, 161, 159, 158, 156, 155, 153, 151,
               150, 148, 147, 145, 144, 142, 140, 139, 137, 136, 134,
               132, 131, 129, 128};

const int keypad_rows[8] = {21, 20, 19, 18,13,12,11,10};
#define AMPLITUDE 3.3 // Amplitud máxima de la señal en voltios
#define FREQUENCY 1000 // Frecuencia de la señal en Hz
#define SAMPLE_RATE 20000 // Tasa de muestreo en muestras por segundo
//#define M_PI=3.141592
void intToBinaryVector(int num, int binaryVector[8]) {
    int i;
    for (i = 7; i >= 0; i--) {
        binaryVector[i] = num & 1;
        num >>= 1;
    }
}
void GenDienteSierra(int binaryNumber[8],int ampl){
    intToBinaryVector(ampl, binaryNumber);
    for (int row = 0; row < 8; row++) {
        //decimalToBinary(ampl, binaryNumber); 
        //printf(binaryNumber);
        gpio_put(keypad_rows[row], binaryNumber[row]);

    }
    //printf(binaryNumber);
    ampl++;
    if(ampl>=255){ampl=0;};
    sleep_ms(10);
    }
int main() {
    // gpio_init(PIN_A);
    // gpio_set_dir(PIN_A, GPIO_OUT);
    // gpio_init(PIN_B);
    // gpio_set_dir(PIN_B, GPIO_OUT);
    // gpio_init(PIN_C);
    // gpio_set_dir(PIN_C, GPIO_OUT);
    // gpio_init(PIN_D);
    // gpio_set_dir(PIN_D, GPIO_OUT);
    // gpio_init(PIN_E);
    // gpio_set_dir(PIN_E, GPIO_OUT);
    // gpio_init(PIN_F);
    // gpio_set_dir(PIN_F, GPIO_OUT);
    // gpio_init(PIN_G);
    // gpio_set_dir(PIN_G, GPIO_OUT);
    // gpio_init(PIN_H);
    // gpio_set_dir(PIN_H, GPIO_OUT);
    stdio_init_all();
    // Definir un arreglo para almacenar los valores de la señal
    uint8_t samples[SAMPLE_RATE];

    for (int i = 0; i < 8; i++) {
        gpio_init(keypad_rows[i]);
        gpio_set_dir(keypad_rows[i], GPIO_OUT);
    }
    //    // Generar la señal senoidal
    // for (int i = 0; i < SAMPLE_RATE; i++) {
    //     float value = AMPLITUDE * sin(2 * 3.1416 * FREQUENCY * i / SAMPLE_RATE);
    //     // Escalar el valor al rango de 0 a 65535 (16 bits)
    //     samples[i] = (uint8_t)(value * 255 / AMPLITUDE);
    // }
    int binaryNumber[8];
    int ampl=0;
    while(1) {
        intToBinaryVector(binarios_senoidal[ampl], binaryNumber);
        for (int row = 0; row < 8; row++) {
            //decimalToBinary(ampl, binaryNumber); 
            //printf(binaryNumber);
            gpio_put(keypad_rows[row], binaryNumber[row]);

        }
        ampl++;
        if(ampl>=255){ampl=0;};
        sleep_ms(10);



        //ampl=100;
        
        
        // gpio_put(PIN_A, 1);
        // gpio_put(PIN_B, 1);
        // gpio_put(PIN_C, 1);
        // gpio_put(PIN_D, 1);
        // gpio_put(PIN_E, 1);
        // gpio_put(PIN_F, 1);
        // gpio_put(PIN_G, 1);
        // gpio_put(PIN_H, 1);
        // sleep_ms(100);  // Esperar 2ms antes de cambiar el valor
        // // Generar una señal pulsada en el canal 0 del DAC0808
        // gpio_put(PIN_A, 0);
        // gpio_put(PIN_B, 0);
        // gpio_put(PIN_C, 0);
        // gpio_put(PIN_D, 0);
        // gpio_put(PIN_E, 0);
        // gpio_put(PIN_F, 0);
        // gpio_put(PIN_G, 0);
        // gpio_put(PIN_H, 0);
        // sleep_ms(100);  // Esperar 2ms antes de cambiar el valor
        
    }
    
    return 0;
}

