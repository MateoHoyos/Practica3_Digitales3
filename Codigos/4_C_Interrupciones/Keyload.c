#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/timer.h"
#include "hardware/pwm.h"
#include "hardware/irq.h"
#include "hardware/gpio.h"
#include "hardware/sync.h"
#include "Teclado.h"
uint64_t tiempo_inicio = 0,tiempo_10s=0;
u_int8_t fila_M=0;
int ampl = 2500;
int DC = 0;
int freq = 1;
char letra_ori = '0';
//u_int8_t columna_M=0;
#define PIN_BT_START 0 
volatile uint32_t timer_ticks = 0; // Contador de ticks del temporizador
#define NUM_puntos 100
#define INTERVAL_US 100 // Por ejemplo, actualiza cada 3 milisegundos
uint8_t hKeys[10]={0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
volatile uint8_t gKeyCnt = 0;
volatile uint8_t gSeqCnt = 0;
volatile bool gDZero = false;
uint32_t out_t = 1000000ULL;
const char vec_keys[16] = {'1', '2', '3', 'A','4', '5', '6','B','7', '8', '9', 'C','*', '0', '#', 'D'};
uint8_t binarios_senoidal [NUM_puntos] = {128, 144, 160, 175, 189, 203, 215, 226, 236, 243, 249, 253, 255, 255, 252, 248, 242, 234, 224, 213, 200, 186,
                                          171, 156, 140, 123, 107, 92, 76, 62, 49, 37, 26, 17, 10, 5, 1, 0, 1, 4, 8, 15, 24, 34,46, 59, 73, 88, 103, 119,
                                          136, 152, 167, 182, 196, 209, 221, 231, 240, 247, 251, 254, 255, 254, 250, 245, 238, 229, 218, 206, 193, 179, 163, 
                                          148, 132, 115, 99, 84, 69, 55, 42, 31, 21, 13, 7, 3, 0, 0, 2, 6, 12, 19, 29, 40, 52, 66, 80, 95, 111, 127};

//Puntos para muestrear la señal Triangular
uint8_t binarios_triac [NUM_puntos] = {0, 10, 21, 31, 41, 52, 62, 72, 82, 93, 103, 113, 124, 134, 144, 155, 165, 175, 185, 196, 206, 216, 
                                       227, 237, 247, 252, 242, 232, 222, 211, 201, 191, 180, 170, 160, 149, 139, 129, 118, 108, 98, 88, 77, 67,
                                       57, 46, 36, 26, 15, 5, 5, 15, 26, 36, 46, 57, 67, 77, 88, 98, 108, 118, 129, 139, 149, 160,
                                       170, 180, 191, 201, 211, 222, 232, 242, 252, 247, 237, 227, 216, 206, 196, 185, 175, 165, 155, 144, 134, 124,
                                       113, 103, 93, 82, 72, 62, 52, 41, 31, 21, 10, 0};

//Puntos para muestrear la señal Diente de sierra
uint8_t binarios_sao [NUM_puntos] = {0, 5, 10, 15, 21, 26, 31, 36, 41, 46, 52, 57, 62, 67, 72, 77, 82, 88, 93, 98, 103,
                                    108, 113, 118, 124, 129, 134, 139, 144, 149, 155, 160, 165, 170, 175, 180, 185, 191,
                                    196, 201, 206, 211, 216, 222, 227, 232, 237, 242, 247, 252, 3, 8, 13, 18, 23, 28, 33, 
                                    39, 44, 49, 54, 59, 64, 70, 75, 80, 85, 90, 95, 100, 106, 111, 116, 121, 126, 131, 137, 
                                    142, 147, 152, 157, 162, 167, 173, 178, 183, 188, 193, 198, 203, 209, 214, 219, 224, 229,
                                    234, 240, 245, 250, 0};
// uint8_t binarios_senoidal [NUM_puntos] = {128 ,131 ,134 ,137 ,140 ,144 ,147 ,150 ,153 ,156 ,159 ,162 ,165 ,168 ,172 ,175 ,177 ,180 ,183 ,186 ,189 ,192 ,195 ,197 ,200 ,203 ,205 ,208 ,210 ,213 ,215 ,217 ,219 ,222 ,224 ,226 ,228 ,230 ,232 ,234 ,235 ,237 ,239 ,240 ,242 ,243 ,244 ,246 ,247 ,248 ,249 ,250 ,251 ,251 ,252 ,253 ,253 ,254 ,254 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,254 ,254 ,254 ,253 ,253 ,252 ,251 ,250 ,250 ,249 ,248 ,246 ,245 ,244 ,243 ,241 ,240 ,238 ,237 ,235 ,233 ,231 ,229 ,227 ,225 ,223 ,221 ,219 ,217 ,214 ,212 ,210 ,207 ,205 ,202 ,199 ,197 ,194 ,191 ,188 ,185 ,183 ,180 ,177 ,174 ,171 ,168 ,165 ,162 ,158 ,155 ,152 ,149 ,146 ,143 ,140 ,136 ,133 ,130 ,127 ,123 ,120 ,117 ,114 ,111 ,108 ,104 ,101 ,98 ,95 ,92 ,89 ,86 ,83 ,80 ,77 ,74 ,71 ,68 ,65 ,62 ,60 ,57 ,54 ,52 ,49 ,47 ,44 ,42 ,39 ,37 ,35 ,33 ,31 ,29 ,27 ,25 ,23 ,21 ,19 ,18 ,16 ,15 ,13 ,12 ,10 ,9 ,8 ,7 ,6 ,5 ,4 ,3 ,3 ,2 ,2 ,1 ,1 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,1 ,1 ,1 ,2 ,2 ,3 ,4 ,5 ,6 ,7 ,8 ,9 ,10 ,11 ,13 ,14 ,16 ,17 ,19 ,21 ,22 ,24 ,26 ,28 ,30 ,32 ,34 ,37 ,39 ,41 ,44 ,46 ,49 ,51 ,54 ,56 ,59 ,62 ,65 ,67 ,70 ,73 ,76 ,79 ,82 ,85 ,88 ,91 ,94 ,97 ,100 ,104 ,107 ,110 ,113 ,116 ,119 ,123 ,126 ,129 ,132 ,136 ,139 ,142 ,145 ,148 ,151 ,155 ,158 ,161 ,164 ,167 ,170 ,173 ,176 ,179 ,182 ,185 ,188 ,190 ,193 ,196 ,199 ,201 ,204 ,206 ,209 ,211 ,214 ,216 ,218 ,221 ,223 ,225 ,227 ,229 ,231 ,233 ,234 ,236 ,238 ,239 ,241 ,242 ,244 ,245 ,246 ,247 ,248 ,249 ,250 ,251 ,252 ,253 ,253 ,254 ,254 ,254 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,254 ,254 ,253 ,253 ,252 ,252 ,251 ,250 ,249 ,248 ,247 ,246 ,245 ,243 ,242 ,240 ,239 ,237 ,236 ,234 ,232 ,230 ,228 ,226 ,224 ,222 ,220 ,218 ,216 ,213 ,211 ,208 ,206 ,203 ,201 ,198 ,195 ,193 ,190 ,187 ,184 ,181 ,178 ,175 ,172 ,169 ,166 ,163 ,160 ,157 ,154 ,151 ,147 ,144 ,141 ,138 ,135 ,132 ,128 ,125 ,122 ,119 ,115 ,112 ,109 ,106 ,103 ,100 ,97 ,93 ,90 ,87 ,84 ,81 ,78 ,75 ,72 ,70 ,67 ,64 ,61 ,58 ,56 ,53 ,50 ,48 ,45 ,43 ,41 ,38 ,36 ,34 ,32 ,30 ,28 ,26 ,24 ,22 ,20 ,18 ,17 ,15 ,14 ,12 ,11 ,10 ,9 ,7 ,6 ,5 ,5 ,4 ,3 ,2 ,2 ,1 ,1 ,1 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,1 ,1 ,2 ,2 ,3 ,4 ,4 ,5 ,6 ,7 ,8 ,9 ,11 ,12 ,13 ,15 ,16 ,18 ,20 ,21 ,23 ,25 ,27 ,29 ,31 ,33 ,36 ,38 ,40 ,42 ,45 ,47 ,50 ,52 ,55 ,58 ,60 ,63 ,66 ,69 ,72 ,75 ,78 ,80 ,83 ,87 ,90 ,93 ,96 ,99 ,102 ,105 ,108 ,111 ,115 ,118 ,121 ,124 ,127};
// uint8_t binarios_triac [NUM_puntos] = {0 ,2 ,4 ,6 ,8 ,10 ,12 ,14 ,16 ,18 ,20 ,22 ,25 ,27 ,29 ,31 ,33 ,35 ,37 ,39 ,41 ,43 ,45 ,47 ,49 ,51 ,53 ,55 ,57 ,59 ,61 ,63 ,65 ,67 ,69 ,72 ,74 ,76 ,78 ,80 ,82 ,84 ,86 ,88 ,90 ,92 ,94 ,96 ,98 ,100 ,102 ,104 ,106 ,108 ,110 ,112 ,114 ,117 ,119 ,121 ,123 ,125 ,127 ,129 ,131 ,133 ,135 ,137 ,139 ,141 ,143 ,145 ,147 ,149 ,151 ,153 ,155 ,157 ,159 ,161 ,164 ,166 ,168 ,170 ,172 ,174 ,176 ,178 ,180 ,182 ,184 ,186 ,188 ,190 ,192 ,194 ,196 ,198 ,200 ,202 ,204 ,206 ,208 ,211 ,213 ,215 ,217 ,219 ,221 ,223 ,225 ,227 ,229 ,231 ,233 ,235 ,237 ,239 ,241 ,243 ,245 ,247 ,249 ,251 ,253 ,254 ,252 ,250 ,248 ,246 ,244 ,242 ,240 ,238 ,236 ,234 ,232 ,230 ,228 ,226 ,224 ,222 ,220 ,218 ,216 ,214 ,212 ,210 ,207 ,205 ,203 ,201 ,199 ,197 ,195 ,193 ,191 ,189 ,187 ,185 ,183 ,181 ,179 ,177 ,175 ,173 ,171 ,169 ,167 ,165 ,163 ,160 ,158 ,156 ,154 ,152 ,150 ,148 ,146 ,144 ,142 ,140 ,138 ,136 ,134 ,132 ,130 ,128 ,126 ,124 ,122 ,120 ,118 ,115 ,113 ,111 ,109 ,107 ,105 ,103 ,101 ,99 ,97 ,95 ,93 ,91 ,89 ,87 ,85 ,83 ,81 ,79 ,77 ,75 ,73 ,71 ,68 ,66 ,64 ,62 ,60 ,58 ,56 ,54 ,52 ,50 ,48 ,46 ,44 ,42 ,40 ,38 ,36 ,34 ,32 ,30 ,28 ,26 ,24 ,21 ,19 ,17 ,15 ,13 ,11 ,9 ,7 ,5 ,3 ,1 ,1 ,3 ,5 ,7 ,9 ,11 ,13 ,15 ,17 ,19 ,21 ,24 ,26 ,28 ,30 ,32 ,34 ,36 ,38 ,40 ,42 ,44 ,46 ,48 ,50 ,52 ,54 ,56 ,58 ,60 ,62 ,64 ,66 ,68 ,71 ,73 ,75 ,77 ,79 ,81 ,83 ,85 ,87 ,89 ,91 ,93 ,95 ,97 ,99 ,101 ,103 ,105 ,107 ,109 ,111 ,113 ,115 ,118 ,120 ,122 ,124 ,126 ,128 ,130 ,132 ,134 ,136 ,138 ,140 ,142 ,144 ,146 ,148 ,150 ,152 ,154 ,156 ,158 ,160 ,163 ,165 ,167 ,169 ,171 ,173 ,175 ,177 ,179 ,181 ,183 ,185 ,187 ,189 ,191 ,193 ,195 ,197 ,199 ,201 ,203 ,205 ,207 ,210 ,212 ,214 ,216 ,218 ,220 ,222 ,224 ,226 ,228 ,230 ,232 ,234 ,236 ,238 ,240 ,242 ,244 ,246 ,248 ,250 ,252 ,254 ,253 ,251 ,249 ,247 ,245 ,243 ,241 ,239 ,237 ,235 ,233 ,231 ,229 ,227 ,225 ,223 ,221 ,219 ,217 ,215 ,213 ,211 ,208 ,206 ,204 ,202 ,200 ,198 ,196 ,194 ,192 ,190 ,188 ,186 ,184 ,182 ,180 ,178 ,176 ,174 ,172 ,170 ,168 ,166 ,164 ,161 ,159 ,157 ,155 ,153 ,151 ,149 ,147 ,145 ,143 ,141 ,139 ,137 ,135 ,133 ,131 ,129 ,127 ,125 ,123 ,121 ,119 ,117 ,114 ,112 ,110 ,108 ,106 ,104 ,102 ,100 ,98 ,96 ,94 ,92 ,90 ,88 ,86 ,84 ,82 ,80 ,78 ,76 ,74 ,72 ,69 ,67 ,65 ,63 ,61 ,59 ,57 ,55 ,53 ,51 ,49 ,47 ,45 ,43 ,41 ,39 ,37 ,35 ,33 ,31 ,29 ,27 ,25 ,22 ,20 ,18 ,16 ,14 ,12 ,10 ,8 ,6 ,4 ,2 ,0};
// uint8_t binarios_sao [NUM_puntos] = {0 ,1 ,2 ,3 ,4 ,5 ,6 ,7 ,8 ,9 ,10 ,11 ,12 ,13 ,14 ,15 ,16 ,17 ,18 ,19 ,20 ,21 ,22 ,24 ,25 ,26 ,27 ,28 ,29 ,30 ,31 ,32 ,33 ,34 ,35 ,36 ,37 ,38 ,39 ,40 ,41 ,42 ,43 ,44 ,45 ,46 ,47 ,48 ,49 ,50 ,51 ,52 ,53 ,54 ,55 ,56 ,57 ,58 ,59 ,60 ,61 ,62 ,63 ,64 ,65 ,66 ,67 ,68 ,69 ,71 ,72 ,73 ,74 ,75 ,76 ,77 ,78 ,79 ,80 ,81 ,82 ,83 ,84 ,85 ,86 ,87 ,88 ,89 ,90 ,91 ,92 ,93 ,94 ,95 ,96 ,97 ,98 ,99 ,100 ,101 ,102 ,103 ,104 ,105 ,106 ,107 ,108 ,109 ,110 ,111 ,112 ,113 ,114 ,115 ,117 ,118 ,119 ,120 ,121 ,122 ,123 ,124 ,125 ,126 ,127 ,128 ,129 ,130 ,131 ,132 ,133 ,134 ,135 ,136 ,137 ,138 ,139 ,140 ,141 ,142 ,143 ,144 ,145 ,146 ,147 ,148 ,149 ,150 ,151 ,152 ,153 ,154 ,155 ,156 ,157 ,158 ,159 ,160 ,161 ,163 ,164 ,165 ,166 ,167 ,168 ,169 ,170 ,171 ,172 ,173 ,174 ,175 ,176 ,177 ,178 ,179 ,180 ,181 ,182 ,183 ,184 ,185 ,186 ,187 ,188 ,189 ,190 ,191 ,192 ,193 ,194 ,195 ,196 ,197 ,198 ,199 ,200 ,201 ,202 ,203 ,204 ,205 ,206 ,207 ,208 ,210 ,211 ,212 ,213 ,214 ,215 ,216 ,217 ,218 ,219 ,220 ,221 ,222 ,223 ,224 ,225 ,226 ,227 ,228 ,229 ,230 ,231 ,232 ,233 ,234 ,235 ,236 ,237 ,238 ,239 ,240 ,241 ,242 ,243 ,244 ,245 ,246 ,247 ,248 ,249 ,250 ,251 ,252 ,253 ,254 ,1 ,2 ,3 ,4 ,5 ,6 ,7 ,8 ,9 ,10 ,11 ,12 ,13 ,14 ,15 ,16 ,17 ,18 ,19 ,20 ,21 ,22 ,23 ,24 ,25 ,26 ,27 ,28 ,29 ,30 ,31 ,32 ,33 ,34 ,35 ,36 ,37 ,38 ,39 ,40 ,41 ,42 ,43 ,44 ,45 ,47 ,48 ,49 ,50 ,51 ,52 ,53 ,54 ,55 ,56 ,57 ,58 ,59 ,60 ,61 ,62 ,63 ,64 ,65 ,66 ,67 ,68 ,69 ,70 ,71 ,72 ,73 ,74 ,75 ,76 ,77 ,78 ,79 ,80 ,81 ,82 ,83 ,84 ,85 ,86 ,87 ,88 ,89 ,90 ,91 ,92 ,94 ,95 ,96 ,97 ,98 ,99 ,100 ,101 ,102 ,103 ,104 ,105 ,106 ,107 ,108 ,109 ,110 ,111 ,112 ,113 ,114 ,115 ,116 ,117 ,118 ,119 ,120 ,121 ,122 ,123 ,124 ,125 ,126 ,127 ,128 ,129 ,130 ,131 ,132 ,133 ,134 ,135 ,136 ,137 ,138 ,140 ,141 ,142 ,143 ,144 ,145 ,146 ,147 ,148 ,149 ,150 ,151 ,152 ,153 ,154 ,155 ,156 ,157 ,158 ,159 ,160 ,161 ,162 ,163 ,164 ,165 ,166 ,167 ,168 ,169 ,170 ,171 ,172 ,173 ,174 ,175 ,176 ,177 ,178 ,179 ,180 ,181 ,182 ,183 ,184 ,186 ,187 ,188 ,189 ,190 ,191 ,192 ,193 ,194 ,195 ,196 ,197 ,198 ,199 ,200 ,201 ,202 ,203 ,204 ,205 ,206 ,207 ,208 ,209 ,210 ,211 ,212 ,213 ,214 ,215 ,216 ,217 ,218 ,219 ,220 ,221 ,222 ,223 ,224 ,225 ,226 ,227 ,228 ,229 ,230 ,231 ,233 ,234 ,235 ,236 ,237 ,238 ,239 ,240 ,241 ,242 ,243 ,244 ,245 ,246 ,247 ,248 ,249 ,250 ,251 ,252 ,253 ,254 ,0};
uint8_t binarios_qua [NUM_puntos] = {255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,255};
// const int keypad_rows[8] = {21, 20, 19, 18,13,12,11,10};

void insertKey(uint8_t key){
    for(int i=8;i>0;i--){
        hKeys[i+i]=hKeys[i];
    }
    hKeys[0] = key;
}

uint8_t keyDecode(uint32_t keyc){
    uint8_t keyd = 0xFF;
    switch (keyc)
    {
    case 0x88:
        keyd = 0x0F;
        break;
    case 0x48:
        keyd = 0x0E;
        break;
    case 0x28:
        keyd = 0x0D;
        break;
    case 0x18://
        keyd = 0x0C;
        break;
    case 0x84://
        keyd = 0x0B;
        break;
    case 0x44://
        keyd = 0x0A;
        break;
    case 0x24://
        keyd = 0x09;
        break;
    case 0x14://
        keyd = 0x08;
        break;
    case 0x82://
        keyd = 0x07;
        break;
    case 0x42://
        keyd = 0x06;
        break;
    case 0x22://
        keyd = 0x05;
        break;
    case 0x12:
        keyd = 0x04;
        break;
    case 0x81://
        keyd = 0x03;
        break;
    case 0x41://
        keyd = 0x02;
        break;
    case 0x21:
        keyd = 0x01;
        break;
    case 0x11://
        keyd = 0x00;
        break;
    }
    return keyd;
}

/// @brief 
/// @param vecID 
/// @param ID 
/// @return 
int8_t checkID(uint8_t *vecID, uint8_t *ID){
    for(int i=0;i<10;i++){
        bool flag = true;
        for(int j=0;j<4;j++){
            if(vecID[4*i+j] != ID[j]){
                flag = false;
                break;
            }
        }
        if(flag==true)
            return i;
    }
    return -1;
}

/// @brief 
/// @param idxID 
/// @param vecPSWD 
/// @param PSWD 
/// @return 
bool checkPSWD(int8_t idxID, uint8_t *vecPSWD, uint8_t *PSWD){
    for(int j=0;j<4;j++)
        if(vecPSWD[6*idxID+j] != PSWD[j]){
            return false;
        }
    return true;
}

/// @brief 
/// @param num 
void timerCallback(uint num){
    switch (num)
    {
    case 0:
        /* Yellow Led */
        //led_toggle(YELLOW_LED);
        timer_hw->alarm[0] = (uint32_t)(time_us_64() + 1000000);
        timer_hw->intr = 0x00000001;
        break;
    case 1:
        //led_off(GREEN_LED);
        //led_on(YELLOW_LED);
        timer_hw->intr = 0x00000002;
        pwm_set_enabled(0,true);
        pwm_set_enabled(1,false);
        /* code */
        break;
    case 2:
        //led_off(RED_LED);
        //led_on(YELLOW_LED);
        timer_hw->intr = 0x00000004;
        pwm_set_enabled(0,true);
        pwm_set_enabled(1,false);
        /* code */
        break;
    case 3:
        /* code */
        //led_on(RED_LED);
        timer_hw->alarm[2] = (uint32_t)(time_us_64() + 1000000);
        timer_hw->intr = 0x00000008;
        pwm_set_enabled(0,false);
        pwm_set_enabled(1,false);
        gpio_clr_mask(0x0000003C);
        break;
    default:
        break;
    }
}
/// @brief 
/// @param num 
/// @param mask 
void keyboardCallback(uint num, uint32_t mask){
    pwm_set_enabled(0,false);                                           ///< Froze the row sequence
    pwm_set_enabled(1,true);
    gpio_set_irq_enabled(6,GPIO_IRQ_EDGE_RISE,false);  
    gpio_set_irq_enabled(7,GPIO_IRQ_EDGE_RISE,false);  
    gpio_set_irq_enabled(8,GPIO_IRQ_EDGE_RISE,false);  
    gpio_set_irq_enabled(9,GPIO_IRQ_EDGE_RISE,false);                                            ///< Enable the debouncer PIT
    uint32_t KeyData = (gpio_get_all()>>2) & 0x000000FF;
    uint8_t keyd = keyDecode(KeyData);
    if(keyd!=0xFF){
        insertKey(keyd);
        printf("El valor de numero es: %u\n", keyd);
        printf("La tecla ingresada: %c\n", vec_keys[keyd]);
    }
    gKeyCnt++;
    if(gKeyCnt==1){
        //led_off(YELLOW_LED);
        timer_hw->alarm[3] = (uint32_t)(time_us_64()+out_t);
    }
    else if(gKeyCnt==4){
        //led_on(YELLOW_LED);
        timer_hw->alarm[0] = (uint32_t)(time_us_64()+1000000);
    }
    else if(gKeyCnt==10){
        //led_off(YELLOW_LED);
        timer_hw->armed = 0x00000009;
        //int8_t idxID = checkID(vecIDs,&hKeys[6]);
        
        
    }
    ///gMainFlags.bits.keyFlag = true;
    gpio_acknowledge_irq(num,mask);                                     ///< gpio IRQ acknowledge
 }

/// @brief 
/// @param  
void pwmIRQ(void){
    uint32_t gpioValue;
    uint32_t keyc;
    switch (pwm_get_irq_status_mask())
    {
    case 0x01UL:                                                          ///< PWM slice 0 ISR used as a PIT to generate row sequence
        gSeqCnt = (gSeqCnt + 1) % 4;
        gpio_put_masked(0x0000003C,0x00000001 << (gSeqCnt+2));
        pwm_clear_irq(0);                                               ///< Acknowledge slice 0 PWM IRQ
        break;
    case 0x02UL:                                                          ///< PWM slice 1 ISR used as a PIT to implement debouncer
        keyc = gpio_get_all() & 0x000003C0; ///< Get raw gpio values
        if(gDZero){                                 
            if(!keyc){  
                            
                pwm_set_enabled(0,true);                                           ///< Froze the row sequence
                pwm_set_enabled(1,false);
                 gpio_set_irq_enabled(6,GPIO_IRQ_EDGE_RISE,true);  
                gpio_set_irq_enabled(7,GPIO_IRQ_EDGE_RISE,true);  
                gpio_set_irq_enabled(8,GPIO_IRQ_EDGE_RISE,true);  
                gpio_set_irq_enabled(9,GPIO_IRQ_EDGE_RISE,true);
            }
            gDZero = false;
        }
        else{
            gDZero = true;
        }

        pwm_clear_irq(1);                                               ///< Acknowledge slice 1 PWM IRQ
        break;
    default:
        printf("Paso lo que no debería pasar en PWM IRQ\n");
        break;
    }
 }

/// @brief 
/// @param slice 
/// @param milis 
/// @param enable 
void initPWMasPIT(uint8_t slice, uint16_t milis, bool enable){
    assert(milis<=262);                                             ///< PWM can manage interrupt periods greater than 262 milis
    float prescaler = (float)SYS_CLK_KHZ/500;
    assert(prescaler<256); ///< the integer part of the clock divider can be greater than 255 
    uint32_t wrap = 500000*milis/2000;
    assert(wrap<((1UL<<17)-1));
    pwm_config cfg =  pwm_get_default_config();
    pwm_config_set_phase_correct(&cfg,true);
    pwm_config_set_clkdiv(&cfg,prescaler);
    pwm_config_set_clkdiv_mode(&cfg,PWM_DIV_FREE_RUNNING);
    pwm_config_set_wrap(&cfg,wrap);
    pwm_set_irq_enabled(slice,true);
    irq_set_enabled(PWM_IRQ_WRAP,true);
    pwm_init(slice,&cfg,enable);
 }

/// @brief 
/// @param  
void initMatrixKeyboard4x4(void){
    /// GPIOs 5 to 2 control keyboard rows (one hot sequence)
    /// GPIOS 9 to 6 control keyboard columns (GPIO IRQs)
    /// Lets configure who controls the GPIO PAD
    gpio_set_function(2,GPIO_FUNC_SIO);
    gpio_set_function(3,GPIO_FUNC_SIO);
    gpio_set_function(4,GPIO_FUNC_SIO);
    gpio_set_function(5,GPIO_FUNC_SIO);
    gpio_set_function(6,GPIO_FUNC_SIO);
    gpio_set_function(7,GPIO_FUNC_SIO);
    gpio_set_function(8,GPIO_FUNC_SIO);
    gpio_set_function(9,GPIO_FUNC_SIO);

    gpio_set_dir_in_masked(0x000003C0);                                 ///< Set gpios 6 to 9 as inputs (columns)
    gpio_set_dir_out_masked(0x0000003C);                                ///< Set gpios 2 to 5 as outputs (rows)
    gpio_put_masked(0x0000003C,0);                                      ///< Write 0 to rows

    gpio_set_irq_enabled_with_callback(6,GPIO_IRQ_EDGE_RISE,true,keyboardCallback);
    gpio_set_irq_enabled_with_callback(7,GPIO_IRQ_EDGE_RISE,true,keyboardCallback);
    gpio_set_irq_enabled_with_callback(8,GPIO_IRQ_EDGE_RISE,true,keyboardCallback);
    gpio_set_irq_enabled_with_callback(9,GPIO_IRQ_EDGE_RISE,true,keyboardCallback);
 }

/// @brief 
/// @param  
void initTimer(void){
    /// claim alarm0 for yellow LED
    if(!hardware_alarm_is_claimed (0))
        hardware_alarm_claim(0);
    else
        printf("Tenemos un problemaj alarm 0\n");

    /// Claim alarm 1 for Green LED
    if(!hardware_alarm_is_claimed (1))
        hardware_alarm_claim(1);
    else
        printf("Tenemos un problema alarm 1\n");
    
    /// Claim alarm 2 for Red LED
    if(!hardware_alarm_is_claimed (2))
        hardware_alarm_claim(2);
    else
        printf("Tenemos un problema alarm 2\n");

    /// Claim alarm 3 for Time Out
    if(!hardware_alarm_is_claimed (3))
        hardware_alarm_claim(3);
    else
        printf("Tenemos un problema alarm3\n");

    /// Set callback for each alarm. TODO: replace with an exclusive handler
    hardware_alarm_set_callback(0,timerCallback);
    hardware_alarm_set_callback(1,timerCallback);
    hardware_alarm_set_callback(2,timerCallback);
    hardware_alarm_set_callback(3,timerCallback);

    
    timer_hw->intr = 0x0000000F;    ///< Clear interrupt flags that maybe pendant
    //timer_hw->inte = 0x0000000F;    ///< Activate interrupts for all 4 alarms

}

int main() {
    stdio_init_all();

    sleep_ms(5000);
    printf("Hola!!!");

    initPWMasPIT(0,2,true);
    initPWMasPIT(1,100,false);
    irq_set_exclusive_handler(PWM_IRQ_WRAP,pwmIRQ);
    irq_set_priority(PWM_IRQ_WRAP, 0xC0);

    initMatrixKeyboard4x4();
    /// Init Polling base Periodic Time Base
    
    


    while(1){
        __wfi();
    }
}