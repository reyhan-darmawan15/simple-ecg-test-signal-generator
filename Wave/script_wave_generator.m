clear all;
clc;

load('ecg_wave_data.mat');
load('ecg_af_n04.mat');
load('ecg_vf_cu03.mat');

fs = 500;
t = linspace(0,1,fs);
h = hamming(fs);

% EKG 60 bpm
wave_ecg60 = ecg_data_60(1:500,1)';
wave_ecg60 = ((wave_ecg60 / max(wave_ecg60)) * 1000) + 120;
wave_ecg60_dac = floor(wave_ecg60 / 13.8);
csvwrite('ecg60.csv',wave_ecg60_dac);

% EKG 120 bpm
wave_ecg120 = ecg_data_120(150:649,1)';
wave_ecg120 = ((wave_ecg120 / max(wave_ecg120)) * 1000) + 200;
wave_ecg120_dac = floor(wave_ecg120 / 13.8);
csvwrite('ecg120.csv',wave_ecg120_dac);

% EKG AF
wave_ecgAF = (ecg_af(300:799,1)' + 0.4) * 1000;
wave_ecgAF_dac = floor(wave_ecgAF / 13.8);
csvwrite('ecgAF.csv',wave_ecgAF_dac);

% EKG VF
wave_ecgVF = ((ecg_vf' / max(ecg_vf)) + 1.0) * 1000;
wave_ecgVF_dac = floor(wave_ecgVF / 13.8);
csvwrite('ecgVF.csv',wave_ecgVF_dac);

% Sinus
wave_sin = 0.5*sin(2*pi*10*t) + 0.5;
wave_sin = 1000 * wave_sin;
wave_sin_dac = floor(wave_sin / 13.8);
csvwrite('sine.csv',wave_sin_dac);

% Segitiga
temp = 0;
wave_triangle = 0.5*sawtooth(2*pi*10*t,0.5) + 0.5;
wave_triangle = 1000 * wave_triangle;
wave_triangle_dac = floor(wave_triangle / 13.8);
csvwrite('triangle.csv',wave_triangle_dac);

% Pulse 1
wave_pulse = zeros(1,length(t));
wave_pulse(1,1:75) = 1000;
wave_pulse_dac = floor(wave_pulse / 13.8);
csvwrite('pulse150.csv',wave_pulse_dac);

% Pulse 2
wave_pulse = zeros(1,length(t));
wave_pulse(1,1:250) = 1000;
wave_pulse_dac = floor(wave_pulse / 13.8);
csvwrite('pulse500.csv',wave_pulse_dac);