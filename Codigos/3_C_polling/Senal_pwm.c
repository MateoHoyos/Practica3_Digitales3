/**
 * \file        Senal_pwm.c
 * \brief       Generador Digital de Señales (GDS) en C con estrategia de polling
 * \details    En esta práctica desarrollaremos en un dispositivo Generador 
 * Digital de Señales (GDS). El dispositivo GDS genera 
 * 4 formas de onda diferentes: seno, triangular, diente de sierra y cuadrada. 
 * El usuario puede por medio de un pulsador seleccionar el tipo de forma de 
 * onda que genera el GDS. Así cada que se presiona el botón el sistema pasa 
 * de una forma de onda a la siguiente de forma secuencial. La amplitud, 
 * el nivel DC (offset) y la frecuencia de la señal se introduce por medio
 * de un teclado matricial de 4x4. Para ingresar la amplitud, el usuario 
 * presionara la tecla A seguida del valor de voltaje deseado en milivoltios
 * y la tecla D para finalizar. De forma similar para ingresar el nivel DC,
 * el usuario presionara la tecla B seguida del valor de voltaje DC deseado 
 * en milivoltios y la tecla D para finalizar. Finalmente, para ingresar la frecuencia,
 * es necesario presionar la tecla C seguida del valor de frecuencia en Hertz y la tecla D
 * para finalizar. Los valores actuales de Amplitud, Nivel DC y Frecuencia junto con el
 * tipo de forma de onda actual serán impresos cada segundo a través de la interfaz serial 
 * o USB a una herramienta tipo terminal. La Amplitud deberá poder variarse entre 100mV y 
 * 2500mV, el Nivel DC entre 50mV y 1250mV. La frecuencia debe poder variarse entre 1 Hz y 
 * 12000000 Hz. Por defecto el dispositivo GDS inicia generando una señal de tipo senoidal 
 * con una amplitud de 1000 mV, Nivel DC de 500mV y frecuencia de 10Hz. La señal generada 
 * y sus características deberán desde luego poder ser corroboradas por un instrumento 
 * de medida, multímetro u osciloscopio.
 *
 * \author      Lopez_Mutis_Hoyos
 * \version     0.0.1
 * \date        29/04/2024
 * \copyright   Unlicensed
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include <time.h>
#include <unistd.h>
#include <stdbool.h>
#include "hardware/gpio.h"
#include "hardware/timer.h"
#include <math.h>

/**
 * @brief Creación de constantes
 * 
*/
#define PIN_BT_START 0 
#define NUM_puntos 500
#define INTERVAL_US 100 // Por ejemplo, actualiza cada 3 milisegundos

/**
 * @brief Creación de variables
 * 
*/
uint64_t tiempo_inicio = 0,tiempo_10s=0;
volatile uint32_t timer_ticks = 0; // Contador de ticks del temporizador
uint8_t one_second  = 0; // variable global para contar segundos
uint8_t binarios_senoidal [NUM_puntos] = {128 ,131 ,134 ,137 ,140 ,144 ,147 ,150 ,153 ,156 ,159 ,162 ,165 ,168 ,172 ,175 ,177 ,180 ,183 ,186 ,189 ,192 ,195 ,197 ,200 ,203 ,205 ,208 ,210 ,213 ,215 ,217 ,219 ,222 ,224 ,226 ,228 ,230 ,232 ,234 ,235 ,237 ,239 ,240 ,242 ,243 ,244 ,246 ,247 ,248 ,249 ,250 ,251 ,251 ,252 ,253 ,253 ,254 ,254 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,254 ,254 ,254 ,253 ,253 ,252 ,251 ,250 ,250 ,249 ,248 ,246 ,245 ,244 ,243 ,241 ,240 ,238 ,237 ,235 ,233 ,231 ,229 ,227 ,225 ,223 ,221 ,219 ,217 ,214 ,212 ,210 ,207 ,205 ,202 ,199 ,197 ,194 ,191 ,188 ,185 ,183 ,180 ,177 ,174 ,171 ,168 ,165 ,162 ,158 ,155 ,152 ,149 ,146 ,143 ,140 ,136 ,133 ,130 ,127 ,123 ,120 ,117 ,114 ,111 ,108 ,104 ,101 ,98 ,95 ,92 ,89 ,86 ,83 ,80 ,77 ,74 ,71 ,68 ,65 ,62 ,60 ,57 ,54 ,52 ,49 ,47 ,44 ,42 ,39 ,37 ,35 ,33 ,31 ,29 ,27 ,25 ,23 ,21 ,19 ,18 ,16 ,15 ,13 ,12 ,10 ,9 ,8 ,7 ,6 ,5 ,4 ,3 ,3 ,2 ,2 ,1 ,1 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,1 ,1 ,1 ,2 ,2 ,3 ,4 ,5 ,6 ,7 ,8 ,9 ,10 ,11 ,13 ,14 ,16 ,17 ,19 ,21 ,22 ,24 ,26 ,28 ,30 ,32 ,34 ,37 ,39 ,41 ,44 ,46 ,49 ,51 ,54 ,56 ,59 ,62 ,65 ,67 ,70 ,73 ,76 ,79 ,82 ,85 ,88 ,91 ,94 ,97 ,100 ,104 ,107 ,110 ,113 ,116 ,119 ,123 ,126 ,129 ,132 ,136 ,139 ,142 ,145 ,148 ,151 ,155 ,158 ,161 ,164 ,167 ,170 ,173 ,176 ,179 ,182 ,185 ,188 ,190 ,193 ,196 ,199 ,201 ,204 ,206 ,209 ,211 ,214 ,216 ,218 ,221 ,223 ,225 ,227 ,229 ,231 ,233 ,234 ,236 ,238 ,239 ,241 ,242 ,244 ,245 ,246 ,247 ,248 ,249 ,250 ,251 ,252 ,253 ,253 ,254 ,254 ,254 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,254 ,254 ,253 ,253 ,252 ,252 ,251 ,250 ,249 ,248 ,247 ,246 ,245 ,243 ,242 ,240 ,239 ,237 ,236 ,234 ,232 ,230 ,228 ,226 ,224 ,222 ,220 ,218 ,216 ,213 ,211 ,208 ,206 ,203 ,201 ,198 ,195 ,193 ,190 ,187 ,184 ,181 ,178 ,175 ,172 ,169 ,166 ,163 ,160 ,157 ,154 ,151 ,147 ,144 ,141 ,138 ,135 ,132 ,128 ,125 ,122 ,119 ,115 ,112 ,109 ,106 ,103 ,100 ,97 ,93 ,90 ,87 ,84 ,81 ,78 ,75 ,72 ,70 ,67 ,64 ,61 ,58 ,56 ,53 ,50 ,48 ,45 ,43 ,41 ,38 ,36 ,34 ,32 ,30 ,28 ,26 ,24 ,22 ,20 ,18 ,17 ,15 ,14 ,12 ,11 ,10 ,9 ,7 ,6 ,5 ,5 ,4 ,3 ,2 ,2 ,1 ,1 ,1 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,1 ,1 ,2 ,2 ,3 ,4 ,4 ,5 ,6 ,7 ,8 ,9 ,11 ,12 ,13 ,15 ,16 ,18 ,20 ,21 ,23 ,25 ,27 ,29 ,31 ,33 ,36 ,38 ,40 ,42 ,45 ,47 ,50 ,52 ,55 ,58 ,60 ,63 ,66 ,69 ,72 ,75 ,78 ,80 ,83 ,87 ,90 ,93 ,96 ,99 ,102 ,105 ,108 ,111 ,115 ,118 ,121 ,124 ,127};
uint8_t binarios_triac [NUM_puntos] = {0 ,2 ,4 ,6 ,8 ,10 ,12 ,14 ,16 ,18 ,20 ,22 ,25 ,27 ,29 ,31 ,33 ,35 ,37 ,39 ,41 ,43 ,45 ,47 ,49 ,51 ,53 ,55 ,57 ,59 ,61 ,63 ,65 ,67 ,69 ,72 ,74 ,76 ,78 ,80 ,82 ,84 ,86 ,88 ,90 ,92 ,94 ,96 ,98 ,100 ,102 ,104 ,106 ,108 ,110 ,112 ,114 ,117 ,119 ,121 ,123 ,125 ,127 ,129 ,131 ,133 ,135 ,137 ,139 ,141 ,143 ,145 ,147 ,149 ,151 ,153 ,155 ,157 ,159 ,161 ,164 ,166 ,168 ,170 ,172 ,174 ,176 ,178 ,180 ,182 ,184 ,186 ,188 ,190 ,192 ,194 ,196 ,198 ,200 ,202 ,204 ,206 ,208 ,211 ,213 ,215 ,217 ,219 ,221 ,223 ,225 ,227 ,229 ,231 ,233 ,235 ,237 ,239 ,241 ,243 ,245 ,247 ,249 ,251 ,253 ,254 ,252 ,250 ,248 ,246 ,244 ,242 ,240 ,238 ,236 ,234 ,232 ,230 ,228 ,226 ,224 ,222 ,220 ,218 ,216 ,214 ,212 ,210 ,207 ,205 ,203 ,201 ,199 ,197 ,195 ,193 ,191 ,189 ,187 ,185 ,183 ,181 ,179 ,177 ,175 ,173 ,171 ,169 ,167 ,165 ,163 ,160 ,158 ,156 ,154 ,152 ,150 ,148 ,146 ,144 ,142 ,140 ,138 ,136 ,134 ,132 ,130 ,128 ,126 ,124 ,122 ,120 ,118 ,115 ,113 ,111 ,109 ,107 ,105 ,103 ,101 ,99 ,97 ,95 ,93 ,91 ,89 ,87 ,85 ,83 ,81 ,79 ,77 ,75 ,73 ,71 ,68 ,66 ,64 ,62 ,60 ,58 ,56 ,54 ,52 ,50 ,48 ,46 ,44 ,42 ,40 ,38 ,36 ,34 ,32 ,30 ,28 ,26 ,24 ,21 ,19 ,17 ,15 ,13 ,11 ,9 ,7 ,5 ,3 ,1 ,1 ,3 ,5 ,7 ,9 ,11 ,13 ,15 ,17 ,19 ,21 ,24 ,26 ,28 ,30 ,32 ,34 ,36 ,38 ,40 ,42 ,44 ,46 ,48 ,50 ,52 ,54 ,56 ,58 ,60 ,62 ,64 ,66 ,68 ,71 ,73 ,75 ,77 ,79 ,81 ,83 ,85 ,87 ,89 ,91 ,93 ,95 ,97 ,99 ,101 ,103 ,105 ,107 ,109 ,111 ,113 ,115 ,118 ,120 ,122 ,124 ,126 ,128 ,130 ,132 ,134 ,136 ,138 ,140 ,142 ,144 ,146 ,148 ,150 ,152 ,154 ,156 ,158 ,160 ,163 ,165 ,167 ,169 ,171 ,173 ,175 ,177 ,179 ,181 ,183 ,185 ,187 ,189 ,191 ,193 ,195 ,197 ,199 ,201 ,203 ,205 ,207 ,210 ,212 ,214 ,216 ,218 ,220 ,222 ,224 ,226 ,228 ,230 ,232 ,234 ,236 ,238 ,240 ,242 ,244 ,246 ,248 ,250 ,252 ,254 ,253 ,251 ,249 ,247 ,245 ,243 ,241 ,239 ,237 ,235 ,233 ,231 ,229 ,227 ,225 ,223 ,221 ,219 ,217 ,215 ,213 ,211 ,208 ,206 ,204 ,202 ,200 ,198 ,196 ,194 ,192 ,190 ,188 ,186 ,184 ,182 ,180 ,178 ,176 ,174 ,172 ,170 ,168 ,166 ,164 ,161 ,159 ,157 ,155 ,153 ,151 ,149 ,147 ,145 ,143 ,141 ,139 ,137 ,135 ,133 ,131 ,129 ,127 ,125 ,123 ,121 ,119 ,117 ,114 ,112 ,110 ,108 ,106 ,104 ,102 ,100 ,98 ,96 ,94 ,92 ,90 ,88 ,86 ,84 ,82 ,80 ,78 ,76 ,74 ,72 ,69 ,67 ,65 ,63 ,61 ,59 ,57 ,55 ,53 ,51 ,49 ,47 ,45 ,43 ,41 ,39 ,37 ,35 ,33 ,31 ,29 ,27 ,25 ,22 ,20 ,18 ,16 ,14 ,12 ,10 ,8 ,6 ,4 ,2 ,0};
uint8_t binarios_sao [NUM_puntos] = {0 ,1 ,2 ,3 ,4 ,5 ,6 ,7 ,8 ,9 ,10 ,11 ,12 ,13 ,14 ,15 ,16 ,17 ,18 ,19 ,20 ,21 ,22 ,24 ,25 ,26 ,27 ,28 ,29 ,30 ,31 ,32 ,33 ,34 ,35 ,36 ,37 ,38 ,39 ,40 ,41 ,42 ,43 ,44 ,45 ,46 ,47 ,48 ,49 ,50 ,51 ,52 ,53 ,54 ,55 ,56 ,57 ,58 ,59 ,60 ,61 ,62 ,63 ,64 ,65 ,66 ,67 ,68 ,69 ,71 ,72 ,73 ,74 ,75 ,76 ,77 ,78 ,79 ,80 ,81 ,82 ,83 ,84 ,85 ,86 ,87 ,88 ,89 ,90 ,91 ,92 ,93 ,94 ,95 ,96 ,97 ,98 ,99 ,100 ,101 ,102 ,103 ,104 ,105 ,106 ,107 ,108 ,109 ,110 ,111 ,112 ,113 ,114 ,115 ,117 ,118 ,119 ,120 ,121 ,122 ,123 ,124 ,125 ,126 ,127 ,128 ,129 ,130 ,131 ,132 ,133 ,134 ,135 ,136 ,137 ,138 ,139 ,140 ,141 ,142 ,143 ,144 ,145 ,146 ,147 ,148 ,149 ,150 ,151 ,152 ,153 ,154 ,155 ,156 ,157 ,158 ,159 ,160 ,161 ,163 ,164 ,165 ,166 ,167 ,168 ,169 ,170 ,171 ,172 ,173 ,174 ,175 ,176 ,177 ,178 ,179 ,180 ,181 ,182 ,183 ,184 ,185 ,186 ,187 ,188 ,189 ,190 ,191 ,192 ,193 ,194 ,195 ,196 ,197 ,198 ,199 ,200 ,201 ,202 ,203 ,204 ,205 ,206 ,207 ,208 ,210 ,211 ,212 ,213 ,214 ,215 ,216 ,217 ,218 ,219 ,220 ,221 ,222 ,223 ,224 ,225 ,226 ,227 ,228 ,229 ,230 ,231 ,232 ,233 ,234 ,235 ,236 ,237 ,238 ,239 ,240 ,241 ,242 ,243 ,244 ,245 ,246 ,247 ,248 ,249 ,250 ,251 ,252 ,253 ,254 ,1 ,2 ,3 ,4 ,5 ,6 ,7 ,8 ,9 ,10 ,11 ,12 ,13 ,14 ,15 ,16 ,17 ,18 ,19 ,20 ,21 ,22 ,23 ,24 ,25 ,26 ,27 ,28 ,29 ,30 ,31 ,32 ,33 ,34 ,35 ,36 ,37 ,38 ,39 ,40 ,41 ,42 ,43 ,44 ,45 ,47 ,48 ,49 ,50 ,51 ,52 ,53 ,54 ,55 ,56 ,57 ,58 ,59 ,60 ,61 ,62 ,63 ,64 ,65 ,66 ,67 ,68 ,69 ,70 ,71 ,72 ,73 ,74 ,75 ,76 ,77 ,78 ,79 ,80 ,81 ,82 ,83 ,84 ,85 ,86 ,87 ,88 ,89 ,90 ,91 ,92 ,94 ,95 ,96 ,97 ,98 ,99 ,100 ,101 ,102 ,103 ,104 ,105 ,106 ,107 ,108 ,109 ,110 ,111 ,112 ,113 ,114 ,115 ,116 ,117 ,118 ,119 ,120 ,121 ,122 ,123 ,124 ,125 ,126 ,127 ,128 ,129 ,130 ,131 ,132 ,133 ,134 ,135 ,136 ,137 ,138 ,140 ,141 ,142 ,143 ,144 ,145 ,146 ,147 ,148 ,149 ,150 ,151 ,152 ,153 ,154 ,155 ,156 ,157 ,158 ,159 ,160 ,161 ,162 ,163 ,164 ,165 ,166 ,167 ,168 ,169 ,170 ,171 ,172 ,173 ,174 ,175 ,176 ,177 ,178 ,179 ,180 ,181 ,182 ,183 ,184 ,186 ,187 ,188 ,189 ,190 ,191 ,192 ,193 ,194 ,195 ,196 ,197 ,198 ,199 ,200 ,201 ,202 ,203 ,204 ,205 ,206 ,207 ,208 ,209 ,210 ,211 ,212 ,213 ,214 ,215 ,216 ,217 ,218 ,219 ,220 ,221 ,222 ,223 ,224 ,225 ,226 ,227 ,228 ,229 ,230 ,231 ,233 ,234 ,235 ,236 ,237 ,238 ,239 ,240 ,241 ,242 ,243 ,244 ,245 ,246 ,247 ,248 ,249 ,250 ,251 ,252 ,253 ,254 ,0};
uint8_t binarios_qua [NUM_puntos] = {255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,255};
const int keypad_rows[8] = {21, 20, 19, 18,13,12,11,10};

/**
 * @brief Callback de temporizador para timer1.
 * 
 * Esta función se llama cada vez que el temporizador timer1 se dispara.
 * Incrementa el contador de ticks.
 * 
 * @param t Puntero al temporizador que ha disparado la llamada de retorno.
 */
void timer_callback1(struct repeating_timer *t) {
    timer_ticks++; // Incrementar el contador de ticks
}

/**
 * @brief Callback de temporizador para timer_1.
 * 
 * Esta función se llama cada vez que el temporizador timer_1 se dispara.
 * Incrementa el índice de la secuencia.
 * Reinicia el índice si llegamos al final de la secuencia.
 * 
 * @param t Puntero al temporizador que ha disparado la llamada de retorno.
 */
void timer_callback_1(struct repeating_timer *t){
    // Incrementar el índice de la secuencia
    one_second++;
    // Reiniciar el índice si llegamos al final de la secuencia
}

/**
 * @brief Convierte un número entero en un vector binario.
 * 
 * Esta función toma un número entero y lo convierte en un vector binario de 8 bits.
 * Cada elemento del vector binario representa un bit del número entero.
 * 
 * @param num El número entero que se va a convertir en binario.
 * @param binaryVector El vector donde se almacenará la representación binaria del número.
 *        Se espera que tenga una longitud de al menos 8 elementos.
 */
void intToBinaryVector(int num, int binaryVector[8]) {
    int i;
    for (i = 7; i >= 0; i--) {
        binaryVector[i] = num & 1;
        num >>= 1;
    }
}

/**
 * @brief Genera una señal de salida en los puertos GPIO.
 * 
 * Esta función genera una señal de salida en los puertos GPIO en función del tipo de señal,
 * la frecuencia, la amplitud y el desplazamiento especificados.
 * 
 * @param typeSenal Tipo de señal a generar (0: senoidal, 1: SAO, 2: triac, 3: quadrature).
 * @param frecuency Frecuencia de la señal en Hz.
 * @param Amp Amplitud de la señal.
 * @param offset Desplazamiento de la señal.
 */
void GenSenal(int typeSenal,int frecuency,int Amp, int offset){
    static int16_t ampl=3;
    int binaryNumber[8]; // Vector para almacenar la representación binaria de la señal.
    float Amp1=0; // Amplitud normalizada de la señal.
    Amp1=Amp/2500.0;  // Normaliza la amplitud a un valor entre 0 y 1.

     // Verifica si ha transcurrido el tiempo necesario para generar la señal.
    if(timer_ticks>=(10000/(frecuency*250))){
        timer_ticks=0;
        switch (typeSenal)
        {
        case 0:
            intToBinaryVector(Amp1*binarios_senoidal[ampl], binaryNumber);
            break;
        case 1:
            intToBinaryVector(Amp1*binarios_sao[ampl], binaryNumber);
            break;
        case 2:
            intToBinaryVector(Amp1*binarios_triac[ampl], binaryNumber);
            break;
        case 3:
            intToBinaryVector(Amp1*binarios_qua[ampl], binaryNumber);
            break;
        default:
            intToBinaryVector(Amp1*binarios_senoidal[ampl], binaryNumber);
            break;
        }
    for (int row = 0; row < 8; row++) {
        gpio_put(keypad_rows[row], binaryNumber[row]-offset);
    }
    ampl--;
    if(ampl<0){ampl=249;}
    }
}

/**
 * @brief Verifica si ha transcurrido un cierto número de segundos desde el inicio.
 * 
 * Esta función calcula el tiempo transcurrido desde el inicio y compara con el número
 * de segundos especificado. Devuelve verdadero si ha transcurrido el tiempo especificado,
 * de lo contrario, devuelve falso.
 * 
 * @param segundos El número de segundos a verificar.
 * @param rst Booleano que indica si se debe reiniciar el tiempo de inicio.
 * @return Verdadero si han transcurrido los segundos especificados, falso de lo contrario.
 */
bool verificarTiempoTranscurrido(int segundos,bool rst) {
    static time_t inicio = 0; // Variable estática para almacenar el tiempo de inicio
    if(rst){inicio=0;}
    if (inicio == 0) {
        inicio = time(NULL); // Obtener el tiempo de inicio al llamar por primera vez
    }
    time_t actual = time(NULL); // Obtener el tiempo actual
    time_t tiempo_transcurrido = actual - inicio; // Calcular el tiempo transcurrido
    return tiempo_transcurrido >= segundos; // Devolver verdadero si ha transcurrido el tiempo especificado
}

/**
 * @brief Función principal del programa.
 * 
 * Esta función inicializa la biblioteca de entrada/salida estándar y el teclado.
 * Luego, entra en un bucle infinito donde genera una señal, verifica si se presiona
 * un botón, controla el antirrebote, obtiene los valores del teclado y los imprime
 * por la terminal cada segundo.
 * 
 * @return 0 si la ejecución del programa es exitosa.
 */
int main() {
    stdio_init_all(); // Inicializa la biblioteca de entrada/salida estándar para permitir el uso de funciones de entrada/salida, como printf().
    setup(); //Se inicializa el teclado según los pines para las columnas y filas 
    printf("Ingrese el valor del teclado\n");
    char letra_ori = '0';
    float flag_a = false; //bandera tecla A
    float flag_b = false; //bandera tecla B
    float flag_c = false; //bandera tecla C

    //Valores por defecto de señal
    int ampl = 2500;  
    int DC = 0;
    int freq = 1;

    // definicion de la alarma para controlar los tiempos de funcionamiento del programa
    struct repeating_timer timer_1, timer1;
    add_repeating_timer_ms(1000, timer_callback_1, NULL, &timer_1);
    add_repeating_timer_us(INTERVAL_US, timer_callback1, NULL, &timer1);
    
    //Iniciamos los puertos para el dac como salida
    for (int i = 0; i < 8; i++) {
        gpio_init(keypad_rows[i]);
        gpio_set_dir(keypad_rows[i], GPIO_OUT);
    }
    gpio_init(PIN_BT_START);
    gpio_set_dir(PIN_BT_START, GPIO_IN);
    bool rst=false; //bandera reset para el tiempo
    bool AntR=true; //bandera anti-rebote
    int Signal=0;

    while(1) {
        GenSenal(Signal,freq,ampl,DC);

        // Verifica si se presiona el botón 
        if (gpio_get(PIN_BT_START) && AntR) {
            AntR=false;
            Signal++; // Incrementa el valor de Signal.
        }

        //Antirebote del boton
        if(!AntR){
            if(verificarTiempoTranscurrido(1,rst)){
                rst=true;
                AntR=true;
                }
            else{rst=false;}
        }
        if(Signal>=4){Signal=0;}

        //Tecla A de amplitud 
        get_value(&letra_ori, &ampl, &DC, &freq);
        if ((letra_ori == 'A') || (flag_a)){
            flag_a = true;

            if (letra_ori == 'D'){
                flag_a = false;
                printf("Amplitud: %d\n", ampl);
            }
        }

        //Tecla B de Nivel dc 
        if ((letra_ori == 'B') || (flag_b)){
            flag_b = true;

            if (letra_ori == 'D'){
                flag_b = false;
                printf("Nivel dc: %d\n", DC);
            }
        }

        //Tecla C de frecuencia
        if ((letra_ori == 'C') || (flag_c)){
            flag_c = true;

            if (letra_ori == 'D'){
                flag_c = false;
                printf("frecuencia: %d\n", freq);
            }
        }

        //Imprimir valores cada segundo por la terminal
        if(one_second == 1){
            printf("Amplitud, Nivel DC, Frecuencia %d, %d, %d \n", ampl, DC, freq);
            one_second=0;
            }
    }
    return 0;
}
