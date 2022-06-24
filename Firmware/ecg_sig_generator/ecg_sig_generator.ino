/********************************************************************************************
 * PROGRAM ECG SIGNAL GENERATOR
 * ----------------------------
 * Oleh:
 * Reyhan Issatyadi Darmawan
 * Jurusan Fisika Fakultas MIPA
 * Universitas Lampung
 * 
 * Deskripsi singkat:
 * Proyek ini berfungsi untuk membangkitkan sinyal EKG tiruan yang digunakan sebagai
 * sumber sinyal uji rangkaian EKG, tanpa harus menyadap langsung dari tubuh pasien.
 * Berguna untuk tes cepat (quick test) apakah EKG dapat menampilkan sinyal atau tidak.
 * 
 * Ada delapan jenis sinyal yang dapat dipilih:
 * 1) EKG 1 mV 60 bpm
 * 2) EKG 1 mV 120 bpm
 * 3) EKG AF (atrial fibrillation)
 * 4) EKG VF (ventricular fibrillation)
 * 5) Sinusoida 1 mV 10 Hz
 * 6) Segitiga 1 mV 10 Hz
 * 7) Pulse 1 mV durasi 1 detik lebar pulsa 150 ms
 * 8) Impuls pendek
 ********************************************************************************************/

// Library tambahan
#include <TimerOne.h>
#include <ButtonDebounce.h>
#include <SPI.h>
#include "wavedata.h"

// Define
#define BUTTON_DN A4
#define BUTTON_UP A5
#define SS_PIN 10
#define DAC_CS_PIN 8

// Deklarasi objek tombol
ButtonDebounce buttonDown(BUTTON_DN, 40);
ButtonDebounce buttonUp(BUTTON_UP, 40);

// Array digit seven segment
byte displayDigitByte[8] = {
  0x79,   // 1
  0x24,   // 2
  0x30,   // 3
  0x19,   // 4
  0x12,   // 5
  0x02,   // 6
  0x78,   // 7
  0x00    // 8
};

// Variabel global
byte waveIndex = 0;
int tick = 0;
int dacIndex = 0;
byte waveBuffer[500];   // Buffer gelombang untuk 1 detik (500 sampel)
byte waveChanged = 0;

void setup() {
  // put your setup code here, to run once:
  // Inisialisasi tombol
  pinMode(BUTTON_DN, INPUT_PULLUP);
  pinMode(BUTTON_UP, INPUT_PULLUP);
  buttonDown.setCallback(onButtonDownChange);
  buttonUp.setCallback(onButtonUpChange);

  // Inisialisasi Timer1
  Timer1.initialize(2000);  // 2 ms (500 Hz)
  Timer1.attachInterrupt(timCallback);

  // Inisialisasi DAC
  pinMode(DAC_CS_PIN, OUTPUT);
  digitalWrite(DAC_CS_PIN, HIGH);
  DDRD = 0xFF;
  PORTD = 0xFF;

  // Inisialisasi display
  pinMode(SS_PIN, OUTPUT);  // Set pin SS sebagai output SPI
  SPI.begin(); // Inisialisasi SPI
  SPI.setClockDivider(SPI_CLOCK_DIV2);
  SPI.setDataMode(SPI_MODE0);
  SPI.transfer(displayDigitByte[waveIndex]); // Tampilkan gelombang pertama saat dihidupkan

  // Inisialisasi buffer
  for(int i=0; i<500; i++)
  {
    waveBuffer[i] = pgm_read_byte(wave_data1 + i);
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  buttonDown.update();
  buttonUp.update();

  // Ubah data gelombang di RAM
  if(waveChanged > 0)
  {
    switch(waveIndex)
    {
      case 0:
        // EKG 60 bpm
        for(int i=0; i<500; i++)
        {
          waveBuffer[i] = pgm_read_byte(wave_data1 + i);
        }
        break;
      case 1:
        // EKG 120 bpm
        for(int i=0; i<500; i++)
        {
          waveBuffer[i] = pgm_read_byte(wave_data2 + i);
        }
        break;
      case 2:
        // EKG AF
        for(int i=0; i<500; i++)
        {
          waveBuffer[i] = pgm_read_byte(wave_data3 + i);
        }
        break;
      case 3:
        // EKG VF
        for(int i=0; i<500; i++)
        {
          waveBuffer[i] = pgm_read_byte(wave_data4 + i);
        }
        break;
      case 4:
        // Sinus 10 Hz
        for(int i=0; i<500; i++)
        {
          waveBuffer[i] = pgm_read_byte(wave_data5 + i);
        }
        break;
      case 5:
        // Segitiga 10 Hz
        for(int i=0; i<500; i++)
        {
          waveBuffer[i] = pgm_read_byte(wave_data6 + i);
        }
        break;
      case 6:
        // Pulse 150 ms
        for(int i=0; i<500; i++)
        {
          waveBuffer[i] = pgm_read_byte(wave_data7 + i);
        }
        break;
      case 7:
        // Pulse 500 ms
        for(int i=0; i<500; i++)
        {
          waveBuffer[i] = pgm_read_byte(wave_data8 + i);
        }
        break;
      default:
        break;
    }
    waveChanged = 0;
  }
}

void timCallback()
{
  static byte dpBlink = 0x00;
  
  tick++;
  if(tick > 499) // Tambahkan kedap-kedip di DP
  {
    digitalWrite(SS_PIN, LOW);
    SPI.transfer(displayDigitByte[waveIndex] | dpBlink);
    digitalWrite(SS_PIN, HIGH);
    dpBlink ^= 0x80;
    tick = 0;
  }

  // Update output DAC
  digitalWrite(DAC_CS_PIN, LOW);
  PORTD = waveBuffer[dacIndex];
  digitalWrite(DAC_CS_PIN, HIGH);
  dacIndex = (dacIndex + 1) % 500;
}

void onButtonDownChange()   // Callback ketika DOWN ditekan
{
  // Update display
  if(buttonDown.state() == LOW)
  {
    SPI.transfer(displayDigitByte[waveIndex]);
    waveIndex = (waveIndex + 8 - 1) & 7;
  }
  // Tandai bahwa butuh mengubah gelombang output
  waveChanged = 1;
}

void onButtonUpChange()   // Callback ketika UP ditekan
{
  // Update display
  if(buttonUp.state() == LOW)
  {
    SPI.transfer(displayDigitByte[waveIndex]);
    waveIndex = (waveIndex + 1) & 7;
  }

  // Tandai bahwa butuh mengubah gelombang output
  waveChanged = 1;
}
