/**
 * @file Lector_freq_arduino
 * @mainpage Medidor de frecuencia usando el flujo de programación de Arduino y mostrando el valor de frecuencia 
 * en 6 displays 7 segmentos, la frecuencia máxima obtenida fue 246kHz.
 * 
 * 
 * 
 * @section Descripción 
 * Para poder medir la frecuencia usando Arduino, se usa un código muy parecido al de 
 * Polling+ interrupciones cada segundo se lee el valor de contador
 * que es la variable que almacena el número de flancos positivos y se le asigna este valor 
 * a la variable frecuencia que por lo tanto cambia cada segundo y es el valor 
 * que se muestra en los display de 7 segmentos. Para saber la canitdad de flancos de subida, se usa la función 
 * propia de la raspberry pi pico y del lenguaje C para las interrupciones, que cada que se produce un cambio 
 * en el GPIO17, se ejecuta la función contador frecuencia que suma de a uno a un contador. 
*/

/**
 * @brief Pines para controlar cada transistor por separado, esto es para la multiplexación de los 6 displays
*/
const int transistorPin1 = 8;
const int transistorPin2 = 9;
const int transistorPin3 = 10;
const int transistorPin4 = 11;
const int transistorPin5 = 12;
const int transistorPin6 = 13;


/**
 * @param Pines_segmentos arreglo que sirve para almacenar los pines de los segmentos de los 6 displays, debido a que están 
 * multiplexados, por lo tanto en el GPIO1 se conecta el segmento a, en el GPIO2 en segmento b y así sucesivamente,
 * que el tipo de variable sea arreglo sirve para luego acceder a cada posición y poder encender un segmento u otro.
*/
const int segmentPins[] = {1, 2,  3, 4, 5, 6, 7};

 /**
    * @param contador Variable que sirve para contar el número de flancos de subida que hay en el GPIO17. 
    * Esta variable se modifica a partir de una interrupción
*/

/**
 * @param frecuencia Variable global que luego se le asigna el valor del número de pulsos positivos contados en un segundo.
*/
volatile unsigned long Contador = 0; // frecuencia de pulsos
unsigned long frecuencia = 0;

/**
  * @param Tiempo_transcurrido Esta variable se usa para poner como tope máximo captura del valor de frecuencia 
  * en 1s, para esto en primera instancia se le asigna 1000ms que son 1s, luego se le suma el valor de 
  * tiempo_actual+1s que hace que cada segundo se entre al condicional que lee el valor de frecuencia. 
*/
unsigned long Tiempo_transcurrido = 1000;
int PIN_Raspberry = 17; // Pin de entrada del PWM generado desde el Arduino


/**
 * @param digitos Variable que almacena las combinaciones para los número del 0 al 9 en formato display 7 segmentos cátodo común.
*/
const byte digitos[] = {
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



void setup() {
  Serial.begin(9600);
  //Interrupción que asociada al GPIO17, que se activa en el flanco de subida y se asocia a la función de rutina de 
  //atención de interrupción llamada Contador_frecuencia.
  attachInterrupt(digitalPinToInterrupt(PIN_Raspberry), Contador_frecuencia, RISING);
  // Configurar los pines de los transistores como salidas
  pinMode(transistorPin1, OUTPUT);
  pinMode(transistorPin2, OUTPUT);
  pinMode(transistorPin3, OUTPUT);
  pinMode(transistorPin4, OUTPUT);
  pinMode(transistorPin5, OUTPUT);
  pinMode(transistorPin6, OUTPUT);

  // Configurar los pines de segmentos como salidas
  for (int i = 0; i < 7; i++) {
    pinMode(segmentPins[i], OUTPUT);
  }
}

void loop() {
    /**
      * @param tiempo_actual variable que almacena el tiempo de ejecución actual del programa y cuando es 
      * mayor a 1s de lee el valor del contador. 
    */
  unsigned long Tiempo_actual = millis();
    /**
      *  Cada segundo se le asigna el valor de contador a frecuencia, es decir que se lee cuanta frecuencia se genera 
      * en 1s y así con la variable global frecuencia se muestra el valor en los displays de 7 segmentos. 
    */


  if (Tiempo_actual >= Tiempo_transcurrido) {
    // Calcula la frecuencia cada segundo
    frecuencia = Contador;
    Serial.println(frecuencia);
    Contador = 0;
    

   


    

    // Actualiza el tiempo de la última actualización del display
    Tiempo_transcurrido = Tiempo_actual+1000;
    
    
  } 
  /**
  *Se llama la función Mostrar_numero a la que se le asigna un valor según el dígito de frecuencia que se quiere mostrar y se le manda el número del transistor a encender, 
  *donde el transitor 6 corresponde a centenas de miles, el transistor 5 a decenas de miles y así sucesivamente. Se llama la función cada 1ms para que
  *el tiempo * sea lo suficientemente grande como para que el número escrito se pueda ver bien en el display y un tiempo 
  * lo suficientemente pequeño para que parezca que los 6 displays están encendidos al tiempo.
  */
  Mostrar_Numero(frecuencia / 100000,6 );
  delay(1);
  Mostrar_Numero((frecuencia / 10000) % 10, 5);
  delay(1);
  Mostrar_Numero((frecuencia / 1000) % 10, 4);
  delay(1);
  Mostrar_Numero((frecuencia / 100) % 10, 1);
  delay(1);
  Mostrar_Numero((frecuencia / 10) % 10, 2);
  delay(1);
  Mostrar_Numero(frecuencia % 10, 3);
  delay(1);

}


void Contador_frecuencia() {
  // Esta función se llama cuando se produce un pulso en el pin 15 y se le va sumando a un pulsador
  Contador++;
}


/**
 * @brief Mostrar_Numero 
 * Función que se ejecuta en el segundo núcleo de la raspberry pi pico, en este a partir del valor de frecuencia 
 * se le envía el número completo a cada display, el primer display muestra las centenas de miles, el segundo display las decenas 
 * de miles y así sucesivamente hasta llegar al último display que muestra las unidades. Cabe resaltar que entre cada display 
 * hay 1ms de diferencia para escribir en el, con este tiempo tan bajo se da la ilusión que los 6 displays están prendidos al mismo 
 * tiempo y así se puede leer el valor de frecuencia que cambia cada segundo. Sus párametros son número y Pin_display para asignar
 *un valor según el dígito de frecuencia que se quiere mostrar y se le manda el número del transistor a encender.
*/
// Función para mostrar un dígito en un display específico
void Mostrar_Numero(int numero, int Pin_display) {
  // Desactivar todos los transistores
  digitalWrite(transistorPin1, LOW);
  digitalWrite(transistorPin2, LOW);
  digitalWrite(transistorPin3, LOW);
  digitalWrite(transistorPin4, LOW);
  digitalWrite(transistorPin5, LOW);
  digitalWrite(transistorPin6, LOW);


  /**
    * Según el pin del display se activa solo un transistor mientras los otros siguen desactivados, 
    * para así solo escribir en un display al tiempo, para esto en un principio se desactivan todos los transistores.
  */
  if (Pin_display == 1) {
    digitalWrite(transistorPin1, HIGH);
  } else if (Pin_display == 2) {
    digitalWrite(transistorPin2, HIGH);
  } else if (Pin_display == 3) {
    digitalWrite(transistorPin3, HIGH);
  }
   else if (Pin_display == 4) {
    digitalWrite(transistorPin4, HIGH);
  } else if (Pin_display == 5) {
    digitalWrite(transistorPin5,HIGH);
  
  } else if (Pin_display == 6) {
    digitalWrite(transistorPin6, HIGH);
  }


  /**
    * Se recorren los 7 segmentos que tiene un display y se revisa al transformar el valor leído de frecuencia 
    * en el formato que hay en la variable digitos, con esto cuando se detecta que hay un 1, se manda un 1 
    * al segmento correspondiente en el display. 
  */
  for (int i = 0; i < 7; i++) {
    if (bitRead(digitos[numero], i)) {
      digitalWrite(segmentPins[i], HIGH);
    } else {
      digitalWrite(segmentPins[i], LOW);
    }
  }
}
