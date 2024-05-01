
//Definicion de las variables a utilizar
//Se definen las varibales para el teclado, definimos las filas y columnas
#define ROW_1 2
#define ROW_2 3
#define ROW_3 4
#define ROW_4 5
#define COL_1 6
#define COL_2 7
#define COL_3 8
#define COL_4 9

//definimos la matriz del teclado
char teclas[4][4] = {{'1', '2', '3', 'A'},
                     {'4', '5', '6', 'B'},
                     {'7', '8', '9', 'C'},
                     {'*', '0', '#', 'D'}
};

//Se define el pin del boton para cambiar el tipo de la señal
uint botton = 0;

//Definición de variables para las señales
unsigned int amplitud = 1000; //Valor de la amplitud 
unsigned int nivel_dc = 500; //Valor del nivel DC
unsigned int frecuencia = 10; //Frecuencia de muestreo 
unsigned long tiempo_m = micros();
unsigned long t_botton = micros(); //Tiempo para muestrear la señal 
unsigned int muestreo = (1000000/100)/frecuencia; //Periodo de muestreo
unsigned int tipoSenal = 0; //ipo de señal actual
byte actual[100]; //Señal actual
unsigned int cont = 0;// Contador para seguir la secuencia de la señal


//Puntos para muestrear la señal Seno
byte seno[] = {128, 144, 160, 175, 189, 203, 215, 226, 236, 243, 249, 253, 255, 255, 252, 248, 242, 234, 224, 213, 200, 186,
              171, 156, 140, 123, 107, 92, 76, 62, 49, 37, 26, 17, 10, 5, 1, 0, 1, 4, 8, 15, 24, 34,46, 59, 73, 88, 103, 119,
              136, 152, 167, 182, 196, 209, 221, 231, 240, 247, 251, 254, 255, 254, 250, 245, 238, 229, 218, 206, 193, 179, 163, 
              148, 132, 115, 99, 84, 69, 55, 42, 31, 21, 13, 7, 3, 0, 0, 2, 6, 12, 19, 29, 40, 52, 66, 80, 95, 111, 127};

//Puntos para muestrear la señal Triangular
byte triangular[] = {0, 10, 21, 31, 41, 52, 62, 72, 82, 93, 103, 113, 124, 134, 144, 155, 165, 175, 185, 196, 206, 216, 
                    227, 237, 247, 252, 242, 232, 222, 211, 201, 191, 180, 170, 160, 149, 139, 129, 118, 108, 98, 88, 77, 67,
                    57, 46, 36, 26, 15, 5, 5, 15, 26, 36, 46, 57, 67, 77, 88, 98, 108, 118, 129, 139, 149, 160,
                    170, 180, 191, 201, 211, 222, 232, 242, 252, 247, 237, 227, 216, 206, 196, 185, 175, 165, 155, 144, 134, 124,
                    113, 103, 93, 82, 72, 62, 52, 41, 31, 21, 10, 0};

//Puntos para muestrear la señal Diente de sierra
byte diente[] = {0, 5, 10, 15, 21, 26, 31, 36, 41, 46, 52, 57, 62, 67, 72, 77, 82, 88, 93, 98, 103,
                108, 113, 118, 124, 129, 134, 139, 144, 149, 155, 160, 165, 170, 175, 180, 185, 191,
                196, 201, 206, 211, 216, 222, 227, 232, 237, 242, 247, 252, 3, 8, 13, 18, 23, 28, 33, 
                39, 44, 49, 54, 59, 64, 70, 75, 80, 85, 90, 95, 100, 106, 111, 116, 121, 126, 131, 137, 
                142, 147, 152, 157, 162, 167, 173, 178, 183, 188, 193, 198, 203, 209, 214, 219, 224, 229,
                234, 240, 245, 250, 0};

//Puntos para muestrear la señal cuadrada
byte cuadrada [] = {255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255,
                   255 ,255 ,255 ,255 ,255 , 0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0,
                   255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,255,
                   255 ,255 ,255 ,255 ,255 , 0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0};

//Se definen los pines de salida del DAC
byte pines_DAC[] = {21, 20, 19, 18, 13, 12, 11, 10};

void setup() {
  //Definimos el pin del boton que indica el tipo de la señal
  pinMode(botton, INPUT);
  
  //Definimos de los pines del teclado
  pinMode(ROW_1, OUTPUT);
  pinMode(ROW_2, OUTPUT);
  pinMode(ROW_3, OUTPUT);
  pinMode(ROW_4, OUTPUT);
  pinMode(COL_1, INPUT_PULLUP);
  pinMode(COL_2, INPUT_PULLUP);
  pinMode(COL_3, INPUT_PULLUP);
  pinMode(COL_4, INPUT_PULLUP);

  //Definimos de los 8 pines del DAC
  for (unsigned short i=0; i <8; i++) {
    pinMode(pines_DAC[i], OUTPUT);
    digitalWrite(pines_DAC[i], LOW);
  }

  Serial.begin(9600);

  //Se genera la señal
  nueva_senal(nivel_dc, amplitud, tipoSenal);
}

char teclado_scan() {
  for (unsigned short row = 0; row < 4; row++) {
    //Establecemos todas las filas en HIGH 
    digitalWrite(ROW_1, HIGH);
    digitalWrite(ROW_2, HIGH);
    digitalWrite(ROW_3, HIGH);
    digitalWrite(ROW_4, HIGH);

    // Se activa la fila actual
    switch (row) {
      case 0:
        digitalWrite(ROW_1, LOW);
        break;
      case 1:
        digitalWrite(ROW_2, LOW);
        break;
      case 2:
        digitalWrite(ROW_3, LOW);
        break;
      case 3:
        digitalWrite(ROW_4, LOW);
        break;
    }

    // Se comprueba si hay teclas presionadas en la fila actual
    if (digitalRead(COL_1) == LOW) {
      return teclas[row][0];
    } 
    else if (digitalRead(COL_2) == LOW) {
      return teclas[row][1];
    } 
    else if (digitalRead(COL_3) == LOW) {
      return teclas[row][2];
    } 
    else if (digitalRead(COL_4) == LOW) {
      return teclas[row][3];
    }
  }
  return '\0'; //No se presionado ninguna tecla
}
 
void nueva_senal(unsigned int tipo, unsigned int Amp, unsigned int DC) {
  byte aux;
  //se define el tipo de la señal
  for (unsigned short i=0; i < 100; i++) {
    switch (tipo) {
      case 0:
        aux = seno[i];
        break;
      case 1:
        aux = triangular[i];
        break;
      case 2:
        aux = diente[i];
        break;
      case 3:
        aux = cuadrada[i];
        break;
      default:
        break;
    }
    //Normaliza la amplitud y el valor DC 
    byte dc_n = 255 - ((DC*255) / 1250);
    byte amp_n = 2500 / Amp;
    //Se almacena los valores en el arreglo de la señal
    actual[i] = (aux/amp_n) - dc_n; 
  }
}

void signal_out() {
  gpio_put_masked(0xFFCF, actual[cont] << 2); //Se pone en los pines el valor actual de la señal
  cont += 1; // Se incrementa el contador 
  cont %= 100; // Se usa módulo para que no pase de 99
}

char* string_input() {
  //Se crea un vector para almacenar los datos ingresados
  static char entrada[7] = {0};
  int index = 0;
  t_botton = micros(); //tiempo para el atirrebote

  while (1) {
    char tecla = teclado_scan();
    // si se presiona el '*' se a terminado de ingresar el dato y se retorna la cadena obtenida
    if (tecla == 'D') {
      entrada[index] = '\0';  // Caracter nulo de final
      Serial.print('\n');
      return entrada;
    } 
    //Se verifica si ingreso un numero y  se almacena en el arreglo
    else if (tecla != '\0' && (index < 7 - 1) && (micros() - t_botton > 200000)) {
      entrada[index++] = tecla;
      Serial.print(tecla);
      t_botton = micros();
    }
  }
}

void loop() {
  // Se mide el tiempo que ha pasado desde la última vez que se cambió el valor de la señal
  if (micros() - tiempo_m > muestreo) {
    signal_out();
    tiempo_m = micros(); // Se reinicia el contador
  }

  // Se detecta si se cambio el tipo de señal
  if (digitalRead(botton) && (micros() - t_botton > 20000)) {
    tipoSenal += 1; // Se suma 1 al contador para cambiar el tipo de señal
    tipoSenal %= 4; // Se aplica el módulo para que el contador no pase de 3
    nueva_senal(tipoSenal, amplitud, nivel_dc);
    t_botton = micros(); // Se reinicia el tiempo del antirrebote
  }

  //se escanea el teclado por si se ha presionado alguna letra
  char tecla = teclado_scan();
  if (tecla != '\0' && (micros() - t_botton > 200000)) {
    //Al presionar la 'A' se recibe el dato de amplitud del teclado
    if (tecla == 'A') {
      Serial.println("Ingrese una amplitud: ");
      char* ampl_str = string_input();
      char* endptr;
      amplitud = strtoul(ampl_str, &endptr, 10);  // Se convierte de char a numero
      if (*endptr == '\0') {
        // Successful conversion
        Serial.print("Amplitud: ");
        Serial.println(amplitud);
      }
      nueva_senal(tipoSenal, amplitud, nivel_dc); //Actualiza el vector de la señal 
    } 
    
    //Al presionar la 'B' se recibe el dato de offset del teclado
    else if (tecla == 'B') {
      Serial.println("Ingrese un valor DC: ");
      char* offset_str = string_input();
      char* endptr;
      nivel_dc = strtoul(offset_str, &endptr, 10);  // Se convierte de char a numero
      if (*endptr == '\0') {
        //Successful conversion
        Serial.print("Offset: ");
        Serial.println(nivel_dc);
      }
      nueva_senal(tipoSenal, amplitud, nivel_dc); //Actualiza el vector de la señal 
    }

    //Al presionar la 'C' se recibe el dato de frecuencia del teclado
    else if (tecla == 'C') {
      Serial.println("Ingrese una frecuencia: ");
      char* frec_str = string_input();
      char* endptr;
      frecuencia = strtoul(frec_str, &endptr, 10);  //Se convierte de char a numero
      if (*endptr == '\0') {
        // Successful conversion
        Serial.print("Frecuencia: ");
        Serial.println(frecuencia);
      }
      muestreo = (1000000/100)/frecuencia; //Se actualiza el periodo de muestreo
    } 
    t_botton = micros(); //Reinicia el tiempo del antirrebote
  }
}