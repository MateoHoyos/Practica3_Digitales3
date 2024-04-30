from machine import Pin
from time import sleep_ms, sleep_us
import machine
import utime

##
#@brief Define los pines de los transistores que van activar o desactivar los leds 7 segmentos
##   
# Definir los pines para los transistores de control de los displays
transistorPin1 = 8
transistorPin2 = 9
transistorPin3 = 10
transistorPin4 = 11
transistorPin5 = 12

##
#@brief Definir los pines de segmentos del display
## 
# Definir los pines de segmentos del display
Pines_segmentos = [1, 2, 3, 4, 5, 6, 7]

##
#@brief Define los pines de 7 segmentos que debe encender para mostrar los numeros del 0 al 9 
## 

digitos = [
    0b0111111, # 0
    0b0000110, # 1
    0b1011011, # 2
    0b1001111, # 3
    0b1100110, # 4
    0b1101101, # 5
    0b1111101, # 6
    0b0000111, # 7
    0b1111111, # 8
    0b1101111  # 9
]

##
#@brief Muestra en el display el valor leido
# Define en que momentos se deben encender o apagar los transistores par mostrar el valor digito que se debe mostrar,
# es decir, la función tiene 2 parametros de entrada @param numero que contiene el valor a encender y @param pin_display
# que indica en que led se debe encender el valor enviado 
## 

def Mostrar_Numero(numero,pin_display):
    transistorPin1.value(0)
    transistorPin2.value(0)
    transistorPin3.value(0)
    transistorPin4.value(0)
    transistorPin5.value(0)
   
    if(pin_display==1):
        transistorPin1.value(1)
    elif(pin_display==2):
        transistorPin2.value(1)
    elif(pin_display==3):
        transistorPin3.value(1)
    elif(pin_display==4):
        transistorPin4.value(1)
    elif(pin_display==5):
        transistorPin5.value(1)
           
    for i in range(7):
        if (digitos[numero] & (1 << i)):
            Pines_segmentos[i].value(1)
        else:
            Pines_segmentos[i].value(0)

cont=0
##
#@brief interrupt_handler Define la función que se llamará cuando ocurra la interrupción
#@return Void
##   
def interrupt_handler(pin):
    global cont
    cont += 1

# Define el número del pin que deseas usar
pin_numero = 17  # Por ejemplo, usando el pin 16 como ejemplo

# Crea un objeto Pin para representar el pin GPIO
pin_gpio = Pin(pin_numero, Pin.IN)
transistorPin1 = Pin(transistorPin1,Pin.OUT)
transistorPin2 = Pin(transistorPin2,Pin.OUT)
transistorPin3 = Pin(transistorPin3,Pin.OUT)
transistorPin4 = Pin(transistorPin4,Pin.OUT)
transistorPin5 = Pin(transistorPin5,Pin.OUT)

Pines_segmentos = [Pin(pin, Pin.OUT) for pin in Pines_segmentos]
# Configura la interrupción en el flanco ascendente (cuando el pin pasa de bajo a alto)
pin_gpio.irq(trigger=Pin.IRQ_RISING, handler=interrupt_handler)
tiempo_transcurrido=1000000

##
#@brief El while true permite que cuando se sobrepase el tiempo deseado, se actualice el valor de frecuencia y seguido de este
# se haga la división en unidad, decenas,centenas  y se envié a la función de Mostrar_numero para actualizar en los leds
#el valor leído 
## 
while True:
    
    tiempo_actual=utime.ticks_us()
    if(tiempo_actual>=tiempo_transcurrido):
        
        frec = cont
        cont = 0
        tiempo_transcurrido=tiempo_actual+1000000
    print(frec)
   


    
    Mostrar_Numero(int((frec // 10000) % 10), 5)
    sleep_ms(3)
    Mostrar_Numero(int((frec // 1000) % 10), 4)
    sleep_ms(3)
    Mostrar_Numero(int((frec // 100) % 10), 1)
    sleep_ms(3)
    Mostrar_Numero(int((frec // 10) % 10), 2)
    sleep_ms(3)
    Mostrar_Numero(int(frec % 10), 3)
    sleep_ms(3)
  
   
    
   
    
