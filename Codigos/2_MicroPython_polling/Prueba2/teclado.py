from machine import Pin
from time import sleep

TECLA_ARRIBA = const(0)
TECLA_ABAJO = const(1)

teclas = [['1', '2', '3', 'A'], ['4', '5', '6', 'B'], ['7', '8', '9', 'C'], ['*', '0', '#', 'D']]

# Pines del GPIO  
filas = [1, 2, 3, 4]
columnas = [6, 7, 8, 9]

# Define los pines de filas como salidas
fila_pines = [Pin(nombre_pin, mode=Pin.OUT) for nombre_pin in filas]

# Define los pines de columnas como entradas
columna_pines = [Pin(nombre_pin, mode=Pin.IN, pull=Pin.PULL_DOWN) for nombre_pin in columnas]

# Constantes para los rangos de amplitud, nivel DC y frecuencia
MIN_AMPLITUD = 100
MAX_AMPLITUD = 2500
MIN_DC = 50
MAX_DC = 1250
MIN_FRECUENCIA = 1
MAX_FRECUENCIA = 12000000

def init():
    for fila in range(0, 4):
        for columna in range(0, 4):
            fila_pines[fila].low()

def scan():
    """ escanea todo el teclado """
    while True:
        for fila in range(4):
            for columna in range(4): 
                # define la columna actual en alto -high-
                fila_pines[fila].high()
                tecla = None

                # verifica por teclas si hay teclas presionadas
                if columna_pines[columna].value() == TECLA_ABAJO:
                    tecla = TECLA_ABAJO
                if columna_pines[columna].value() == TECLA_ARRIBA:
                    tecla = TECLA_ARRIBA
                fila_pines[fila].low()
                
                if tecla == TECLA_ABAJO:
                    sleep(0.4)
                    print("Tecla Presionada", teclas[fila][columna])
                    valor = teclas[fila][columna]
                    return valor     
    
#capturamos la amplitud   
def set_amplitud():
    amplitud = ""
    while True:
        numero = scan()
        if numero == "0" or numero == "1" or numero == "2" or numero == "3" or numero == "4" or numero == "5" or numero == "6" or numero == "7" or numero == "8" or numero == "9":
            amplitud = amplitud + numero
        elif numero == "D":
            amplitud = int(amplitud)
            if (MIN_AMPLITUD <= amplitud <= MAX_AMPLITUD):
                print("La amplitud es:", amplitud)
                return amplitud
            else:
                print("La amplitud es:", MIN_AMPLITUD)
                return MIN_AMPLITUD

#capturamos la frecuencia   
def set_frecuencia():
    frecuencia = ""
    while True:
        numero = scan()
        if numero == "0" or numero == "1" or numero == "2" or numero == "3" or numero == "4" or numero == "5" or numero == "6" or numero == "7" or numero == "8" or numero == "9":
            frecuencia = frecuencia + numero
        elif numero == "D":
            frecuencia = int(frecuencia)
            if (MIN_FRECUENCIA <= frecuencia <= MAX_FRECUENCIA):
                print("La frecuencia es:", frecuencia)
                return frecuencia
            else:
                print("La frecuencia es:", MIN_FRECUENCIA)
                return MIN_frecuencia

#capturamos el nivel_DC  
def set_nivelDC():
    nivelDC = ""
    while True:
        numero = scan()
        if numero == "0" or numero == "1" or numero == "2" or numero == "3" or numero == "4" or numero == "5" or numero == "6" or numero == "7" or numero == "8" or numero == "9":
            nivelDC = nivelDC + numero
        elif numero == "D":
            nivelDC = int(nivelDC)
            if (MIN_DC <= nivelDC <= MAX_DC):
                print("La nivelDC es:", nivelDC)
                return nivelDC
            else:
                print("La nivelDC es:", MIN_DC)
                return MIN_DC
        


init()
print("Presiona una telcla")
bandera = None
bandera = scan()

while True:
    if bandera == "A":
        amp = set_amplitud()
    elif bandera == "B":
        fre = set_frecuencia()
    elif bandera == "C":
        DC = set_nivelDC()


