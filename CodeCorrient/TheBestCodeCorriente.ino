#include <Wire.h>
#include <Adafruit_ADS1X15.h>
 
Adafruit_ADS1115 ads;
  
const float FACTOR = 100; //100A/1V
const float multiplier = 0.03125F;
 
void setup()
{
  Serial.begin(115200);
  Wire.begin();
  if (!ads.begin(0x48)) {
    Serial.println("No se encontró el ADS1115 en 0x48");
    while (1);
  }
 
  ads.setGain(GAIN_FOUR);                  // ±2.048V  1 bit = 0.0625mV
  ads.setDataRate(RATE_ADS1115_860SPS);    // Configura 860 muestras por segundo
  ads.begin();
}

void printMeasure(String prefix, float value, String postfix)
{
 Serial.print(prefix);
 Serial.print(value, 6);   // ahora con 6 decimales para no saturar
 Serial.println(postfix);
}
 
void loop()
{
 float currentRMS = getCorriente();
 
 printMeasure("Irms: ", currentRMS, " A");
 delay(1000);
}
 
float getCorriente()
{
 float voltage;
 float corriente;
 float sum = 0;
 long tiempo = millis();
 int counter = 0;
 
 while (millis() - tiempo < 1000)   // toma muestras durante 1 segundo
 {
   voltage = ads.readADC_Differential_0_1() * multiplier;
   corriente = voltage * FACTOR;
   corriente /= 1000.0;
 
   sum += sq(corriente);
   counter = counter + 1;
 }
 
 corriente = sqrt(sum / counter);

 // mostrar cuántas muestras realmente tomó
 Serial.print("Muestras en 1s: ");
 Serial.println(counter);

 return(corriente);
}

