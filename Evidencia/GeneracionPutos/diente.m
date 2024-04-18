clear all
clc 

% Definir el dominio de la señal desde -pi a pi
t = linspace(0, 4*pi, 500);

% Generar la señal de diente de sierra
senal = sawtooth(t);

% Escalar la señal al rango de 0 a 255
senal_escalada = (senal + 1) * 127.5;

% Redondear a enteros
senal_redondeada = round(senal_escalada);

% Asegurarse de que la señal está en el rango de 0 a 255
senal_redondeada(senal_redondeada < 0) = 0;
senal_redondeada(senal_redondeada > 255) = 255;

% Graficar la señal generada
plot(t, senal_redondeada);
title('Señal de diente de sierra de 0 a 4*pi con resolución de 8 bits');
xlabel('Tiempo (rad)');
ylabel('Valor');
grid on;
