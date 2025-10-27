#include <SPI.h>
#include <SD.h>

#define SD_CS 5  // Ajusta si usas otro pin para CS

SPIClass spi(VSPI);  // usa bus VSPI (SCK=18, MISO=19, MOSI=23)

void setup() {
  Serial.begin(115200);
  delay(200);

  // Inicia el bus con pines explícitos (SCK, MISO, MOSI, SS)
  spi.begin(18, 19, 23, SD_CS);

  Serial.println("\nInicializando SD...");
  // Prueba con 1 MHz primero (estable). Luego sube a 4–8 MHz.
  const uint32_t SD_FREQ = 1'000'000;

  if (!SD.begin(SD_CS, spi, SD_FREQ)) {
    Serial.println("Error al inicializar la tarjeta SD");
    Serial.println("Revisa: alimentacion (5V si hay AMS1117), pines (18/19/23/CS), y FAT32.");
    return;
  }

  Serial.println("Tarjeta SD inicializada correctamente.");

  uint8_t cardType = SD.cardType();
  Serial.print("Tipo de tarjeta: ");
  if (cardType == CARD_NONE)        Serial.println("NINGUNA");
  else if (cardType == CARD_MMC)    Serial.println("MMC");
  else if (cardType == CARD_SD)     Serial.println("SDSC");
  else if (cardType == CARD_SDHC)   Serial.println("SDHC/SDXC");
  else                              Serial.println("DESCONOCIDA");

  uint64_t cardSize = SD.cardSize() / (1024ULL * 1024ULL);
  Serial.print("Tamano: "); Serial.print(cardSize); Serial.println(" MB");

  File f = SD.open("/test.txt", FILE_WRITE);
  if (!f) {
    Serial.println("No se pudo crear /test.txt (permiso/ruta)");
    return;
  }
  f.println("Prueba OK");
  f.close();
  Serial.println("Escritura correcta: /test.txt");

  // Lista raiz
  File root = SD.open("/");
  if (root) {
    File entry;
    while ((entry = root.openNextFile())) {
      Serial.println(entry.name());
      entry.close();
    }
    root.close();
  }
}

void loop() {}
