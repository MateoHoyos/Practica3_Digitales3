import machine
import time

tiempo_inicio = 0
tiempo_10s = 0
PIN_BT_START = 0
timer_ticks = 0  # Contador de ticks del temporizador
NUM_puntos = 100
INTERVAL_US = 100  # Por ejemplo, actualiza cada 3 milisegundos

binarios_senoidal = [128, 144, 160, 175, 189, 203, 215, 226, 236, 243, 249, 253, 255, 255, 252, 248, 242, 234, 224, 213, 200, 186,
                     171, 156, 140, 123, 107, 92, 76, 62, 49, 37, 26, 17, 10, 5, 1, 0, 1, 4, 8, 15, 24, 34, 46, 59, 73, 88, 103, 119,
                     136, 152, 167, 182, 196, 209, 221, 231, 240, 247, 251, 254, 255, 254, 250, 245, 238, 229, 218, 206, 193, 179, 163,
                     148, 132, 115, 99, 84, 69, 55, 42, 31, 21, 13, 7, 3, 0, 0, 2, 6, 12, 19, 29, 40, 52, 66, 80, 95, 111, 127]

binarios_triac = [0, 10, 21, 31, 41, 52, 62, 72, 82, 93, 103, 113, 124, 134, 144, 155, 165, 175, 185, 196, 206, 216,
                  227, 237, 247, 252, 242, 232, 222, 211, 201, 191, 180, 170, 160, 149, 139, 129, 118, 108, 98, 88, 77, 67,
                  57, 46, 36, 26, 15, 5, 5, 15, 26, 36, 46, 57, 67, 77, 88, 98, 108, 118, 129, 139, 149, 160,
                  170, 180, 191, 201, 211, 222, 232, 242, 252, 247, 237, 227, 216, 206, 196, 185, 175, 165, 155, 144, 134, 124,
                  113, 103, 93, 82, 72, 62, 52, 41, 31, 21, 10, 0]

binarios_sao = [0, 5, 10, 15, 21, 26, 31, 36, 41, 46, 52, 57, 62, 67, 72, 77, 82, 88, 93, 98, 103,
                108, 113, 118, 124, 129, 134, 139, 144, 149, 155, 160, 165, 170, 175, 180, 185, 191,
                196, 201, 206, 211, 216, 222, 227, 232, 237, 242, 247, 252, 3, 8, 13, 18, 23, 28, 33,
                39, 44, 49, 54, 59, 64, 70, 75, 80, 85, 90, 95, 100, 106, 111, 116, 121, 126, 131, 137,
                142, 147, 152, 157, 162, 167, 173, 178, 183, 188, 193, 198, 203, 209, 214, 219, 224, 229,
                234, 240, 245, 250, 0]

binarios_qua = [255] * 100 + [0] * 100  # Un ejemplo de inicialización utilizando listas en Python

keypad_rows = [21, 20, 19, 18, 13, 12, 11, 10]

def timer_callback1(timer):
    global timer_ticks
    timer_ticks += 1  # Incrementar el contador de ticks

def int_to_binary_vector(num):
    binary_vector = [0] * 8
    for i in range(7, -1, -1):
        binary_vector[i] = num & 1
        num >>= 1
    return binary_vector

def GenSenal(typeSenal, frecuencia, Amp):
    global ampl, timer_ticks
    binaryNumber = [0] * 8
    Amp1 = Amp / 2500.0

    if timer_ticks >= (10000 / (frecuencia * 250)):
        timer_ticks = 0
        global ampl
        if typeSenal == 0:
            binaryNumber = int_to_binary_vector(Amp1 * binarios_senoidal[ampl])
        elif typeSenal == 1:
            binaryNumber = int_to_binary_vector(Amp1 * binarios_sao[ampl])
        elif typeSenal == 2:
            binaryNumber = int_to_binary_vector(Amp1 * binarios_triac[ampl])
        elif typeSenal == 3:
            binaryNumber = int_to_binary_vector(Amp1 * binarios_qua[ampl])
        else:
            binaryNumber = int_to_binary_vector(Amp1 * binarios_senoidal[ampl])

        for row in range(8):
            machine.Pin(keypad_rows[row], machine.Pin.OUT).value(binaryNumber[row])

        ampl -= 1
        if ampl < 0:
            ampl = 100

def verificarTiempoTranscurrido(segundos, rst):
    global inicio
    if rst:
        inicio = 0
    if inicio == 0:
        inicio = time.time()

    actual = time.time()
    tiempo_transcurrido = actual - inicio

    return tiempo_transcurrido >= segundos


# Definir las variables
ampl = 2500
DC = 0
freq = 1
rst = False
AntR = True
Signal = 0

# Definir el temporizador
timer1 = machine.Timer()

# Inicializar pines GPIO
for i in range(8):
    pin = machine.Pin(keypad_rows[i], machine.Pin.OUT)
    pin.value(0)  # Establecer el pin en bajo

pin_bt_start = machine.Pin(PIN_BT_START, machine.Pin.IN)

# Funciones
def timer_callback1(timer):
    global timer_ticks
    timer_ticks += 1

# Bucle principal
while True:
    # Lógica para generar la señal
    GenSenal(Signal, freq, ampl)

    # Verificar el botón de inicio
    if pin_bt_start.value() and AntR:
        AntR = False
        Signal += 1

    if not AntR:
        if verificarTiempoTranscurrido(1, rst):
            rst = True
            AntR = True
        else:
            rst = False

    if Signal >= 4:
        Signal = 0



