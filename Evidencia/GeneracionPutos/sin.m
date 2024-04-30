clear all;clc 

% Definir el dominio de la señal desde -pi a pi
t = linspace(0, 4*pi, 100);

% Generar la señal senoidal
senal = sin(t);


% Escalar la señal al rango de 0 a 255
senal_escalada = (senal + 1) * 127.5;

% Redondear a enteros
senal_redondeada = round(senal_escalada);

% Asegurarse de que la señal está en el rango de 0 a 255
senal_redondeada(senal_redondeada < 0) = 0;
senal_redondeada(senal_redondeada > 255) = 255;

% Graficar la señal generada
plot(t, senal_redondeada);
%title('Señal senoidal de 0 a 4*pi con resolución de 8 bits');
xlabel('Tiempo (rad)');
ylabel('Valor');
ylim([-10 260])
grid on;
