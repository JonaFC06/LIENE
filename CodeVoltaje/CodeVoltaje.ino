#include <Wire.h>
#include <Adafruit_ADS1X15.h>
#include <math.h>

Adafruit_ADS1115 ads;


const int numSamples = 1000;
const unsigned long sampleInterval_us = 500; // 2000 Hz
float Vrms;


float samples[numSamples];

void setup() {
  Serial.begin(115200);
  Serial.print("Hello");
  ads.begin(0x48);
  ads.setGain(GAIN_ONE); // ±4.096 V, resolución: 0.125 mV por bit
}

void loop() {
  float sum = 0;

  // 1. Leer las muestras
  for (int i = 0; i < numSamples; i++) {
    int16_t raw = ads.readADC_SingleEnded(2);
    float voltage = raw * 0.125 / 1000.0; // 0.125 mV por bit con GAIN_ONE
    samples[i] = voltage;
    sum += voltage;
    delayMicroseconds(sampleInterval_us);
  }

  // 2. Calcular offset (voltaje medio)
  float offset = sum / numSamples;

  // 3. Calcular RMS
  float sqSum = 0;
  for (int i = 0; i < numSamples; i++) {
    //float centered = samples[i] - offset;
    float centered = samples[i];
    sqSum += centered * centered;
  }
  float rms = sqrt(sqSum / numSamples);


  // 4. Buscar dos picos consecutivos
  int peak1 = -1, peak2 = -1;
  for (int i = 1; i < numSamples - 1; i++) {
    float prev = samples[i - 1] - offset;
    float curr = samples[i] - offset;
    float next = samples[i + 1] - offset;

    if (curr > prev && curr > next && curr > 0.1) {
      if (peak1 == -1) {
        peak1 = i;
      } else {
        peak2 = i;
        break;
      }
    }
  }

  // 5. Calcular frecuencia
  float frequency = 0.0;
  if (peak1 != -1 && peak2 != -1) {
    unsigned long deltaSamples = peak2 - peak1;
    float period = (deltaSamples * sampleInterval_us) / 1e6;
    frequency = 1.0 / period;
  }

  // 6. Mostrar resultados
  Serial.print("Voltaje RMS: ");
  Serial.print(rms, 4);
  Serial.print(" V | Frecuencia: ");
  Serial.print(frequency, 2);
  Serial.println(" Hz");
  Serial.print("Offset:  ");
  Serial.println(offset);
  delay(1000);
}

