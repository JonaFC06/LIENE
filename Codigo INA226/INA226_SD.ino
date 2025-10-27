/***************************************************************************
 * INA226 + SD card example for ESP32
 * Guarda lecturas de voltaje, corriente y potencia en una microSD
 ***************************************************************************/

#include <Wire.h>
#include <INA226_WE.h>
#include <SD.h>
#include <SPI.h>

#define I2C_ADDRESS 0x40   // Dirección I2C del INA226
#define SD_CS 5            // Pin CS del módulo SD (ajustar según tu placa)

INA226_WE ina226 = INA226_WE(I2C_ADDRESS);

File dataFile;

void setup() {
  Serial.begin(9600);
  Wire.begin();

  // Inicializar INA226
  if (!ina226.init()) {
    Serial.println("Error: no se pudo inicializar INA226. Verifica conexiones.");
    while (1) {}
  }
  ina226.waitUntilConversionCompleted();

  // Inicializar SD
  if (!SD.begin(SD_CS)) {
    Serial.println("Error al inicializar la SD");
    while (1) {}
  }
  Serial.println("SD iniciada correctamente");

  // Crear archivo CSV y agregar encabezados
  dataFile = SD.open("ina226_data.csv", FILE_WRITE);
  if (dataFile) {
    dataFile.println("ShuntVoltage_mV,BusVoltage_V,LoadVoltage_V,Current_mA,BusPower_mW");
    dataFile.close();
  } else {
    Serial.println("Error creando archivo CSV");
  }
}

void loop() {
  float shuntVoltage_mV = ina226.getShuntVoltage_mV();
  float busVoltage_V    = ina226.getBusVoltage_V();
  float current_mA      = ina226.getCurrent_mA();
  float power_mW        = ina226.getBusPower();
  float loadVoltage_V   = busVoltage_V + (shuntVoltage_mV / 1000.0);

  // Mostrar datos en Serial
  Serial.print("Shunt Voltage [mV]: "); Serial.println(shuntVoltage_mV, 4);
  Serial.print("Bus Voltage [V]: "); Serial.println(busVoltage_V, 4);
  Serial.print("Load Voltage [V]: "); Serial.println(loadVoltage_V, 4);
  Serial.print("Current [mA]: "); Serial.println(current_mA, 4);
  Serial.print("Bus Power [mW]: "); Serial.println(power_mW, 4);
  Serial.println("-----------------------------");

  // Guardar datos en SD
  dataFile = SD.open("ina226_data.csv", FILE_APPEND);
  if (dataFile) {
    dataFile.print(shuntVoltage_mV, 4); dataFile.print(",");
    dataFile.print(busVoltage_V, 4);    dataFile.print(",");
    dataFile.print(loadVoltage_V, 4);   dataFile.print(",");
    dataFile.print(current_mA, 4);      dataFile.print(",");
    dataFile.println(power_mW, 4);
    dataFile.close();
  } else {
    Serial.println("Error abriendo archivo para escribir");
  }

  delay(3000); // Espera 3 segundos entre mediciones
}

