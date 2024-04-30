#ifndef TECLADO_ANTI_H
#define TECLADO_ANTI_H

/**
 * Se llaman todas las funciones definidas en Teclado_anti.c
*/
void setup();
void get_value(char *let, int *ampl, int *DC, int *freq);
int convert_to_int(char entrada[], char inicio, char fin);

#endif