/*
 
 TBZ HF - IG1
 ============

 Version: 13.01.2020
 
 Autoren:
 - Senti Laurin <laurin.senti@edu.tbz.ch>
 - Severin Steiner <severin.steiner@edu.tbz.ch>
 - Markus Ritzmann <markus.ritzmann@edu.tbz.ch>

 Im Unterricht von Christoph Jäger.
 Einzelne Code Snippets stammen aus der jeweilig offiziellen Librarie Dokumentation.

*/

/*
 Abhängigkeiten
 ==============

 Installieren via "Tools -> Manage Libraries"

 Für den Sensor:
 - Adafruit BME680 Library by Adafruit
 - Adafruit Unified Sensor by Adafruit

 Für das WLAN:
 - WiFiNNA by Arduino

 Für den TFT Display:
 - TFT Built-in by Arduino, Adafruit
 
*/

#include <Wire.h>
#include <SPI.h>

// TFT: Libraries
#include <Adafruit_GFX.h>    // Adafruit Grafik-Bibliothek wird benötigt "Adafruit GFX Library"
#include <Adafruit_ST7735.h> // Adafruit ST7735-Bibliothek wird benötigt "Adafruit ST7735 and ST7789 Library"

// Sensor: Libraries
#include <Adafruit_Sensor.h>
#include "Adafruit_BME680.h"

// TFT: Variablen
#define TFT_PIN_CS 0         // Arduino-Pin an Display CS
#define TFT_PIN_DC 6         // Arduino-Pin an
#define TFT_PIN_RST 1        // Arduino Reset-Pin

// Sensor: Variablen
#define BME_SCK 9
#define BME_MISO 10
#define BME_MOSI 8
#define BME_CS 7
#define SEALEVELPRESSURE_HPA (1013.25)

// TFT: Einstellungen
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_PIN_CS, TFT_PIN_DC, TFT_PIN_RST);

// Sensor: Einstellugen
Adafruit_BME680 bme(BME_CS); // hardware SPI

void setup()
{
 // TFT
 tft.initR(INITR_GREENTAB); // Initialisierung der Bibliothek
 tft.fillScreen(ST7735_BLACK); // Färbt Hintergund Schwarz

 // Sensor
 Serial.begin(9600);

 // Unklar was nachfolgende Zeile macht
 // TFT funktioniert aber teilweise nur, wenn diese Zeile auskommentiert ist
 //while (!Serial);

 Serial.println(F("BME680 test"));
 
 if (!bme.begin()) {
  Serial.println("Could not find a valid BME680 sensor, check wiring!");
  while (1);
 }
  
 bme.setTemperatureOversampling(BME680_OS_8X);
 bme.setHumidityOversampling(BME680_OS_2X);
 bme.setPressureOversampling(BME680_OS_4X);
 bme.setIIRFilterSize(BME680_FILTER_SIZE_3);
 bme.setGasHeater(320, 150); // 320*C for 150 ms
}

void loop()
{
 // TFT
 tft.fillScreen(ST7735_BLACK);
 
 tft.setCursor(7,0); //Setze Position
 tft.setTextSize(1); //Schriftgröße einstellen
 tft.print(bme.temperature); //Text ausgeben

 tft.setCursor(7,10);
 tft.setTextSize(1);
 tft.print(bme.pressure);

 tft.setCursor(7,20);
 tft.setTextSize(1);
 tft.print(bme.humidity);

 tft.setCursor(7,30);
 tft.setTextSize(1);
 tft.print(bme.gas_resistance / 1000.0);
 
 delay(250); //Wartet 0,25 Sekunden
  
 // Sensor
 if (! bme.performReading()) {
  Serial.println("Failed to perform reading :(");
  return;
 }
 Serial.print("Temperature = ");
 Serial.print(bme.temperature);
 Serial.println(" *C");

 Serial.print("Pressure = ");
 Serial.print(bme.pressure / 100.0);
 Serial.println(" hPa");

 Serial.print("Humidity = ");
 Serial.print(bme.humidity);
 Serial.println(" %");

 Serial.print("Gas = ");
 Serial.print(bme.gas_resistance / 1000.0);
 Serial.println(" KOhms");

 Serial.print("Approx. Altitude = ");
 Serial.print(bme.readAltitude(SEALEVELPRESSURE_HPA));
 Serial.println(" m");

 Serial.println();
 delay(2000);
}
