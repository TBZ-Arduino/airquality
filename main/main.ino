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

//WiFi: Libraries
#include <WiFiNINA.h>
#include "arduino_secrets.h"
#include <InfluxDb.h>

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

//WiFi Einstellungen
char ssid[] = SECRET_SSID;        // your network SSID (name)
char pass[] = SECRET_PASS;    // your network password (use for WPA, or use as key for WEP)
int status = WL_IDLE_STATUS;     // the Wifi radio's status

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

 //WiFi Verbindung aufbauen
 // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true);
  }

  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
    Serial.println("Please upgrade the firmware");
  }

  // attempt to connect to Wifi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network:
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(10000);
  }

  // you're connected now, so print out the data:
  Serial.print("You're connected to the network");
  printCurrentNet();
  printWifiData();
}

//Variables outside of loop
String line;

void loop()
{
 // =========
 // TFT
 // =========

 // Überschreibt den TFT mit einer Farbe.
 // Damit werden die Werte gelöscht und nicht überschrieben (was zu seltsamen effekten führt)
 tft.fillScreen(ST7735_BLACK);

 String Line1  = "Temparatur:";
 String Line2  = String(getTemperature()) + "*C";
 
 String Line3  = "Luftdruck:";
 String Line4  = String(getPressure()) + "hPa";
 
 String Line5  = "Luftfeuchtigkeit:";
 String Line6  = String(getHumidity()) + "%";
 
 String Line7  = "Gas:";
 String Line8  = String(getGas()) + "KOhm";
 
 String Line9  = "Hoehe:";
 String Line10 = String(getAltitude()) + "m";

 // Gibt Text aus
 tft.setCursor(7,0);             // Setze Position
 tft.setTextSize(1);             // Schriftgröße einstellen
 tft.print(Line1);               // Text ausgeben

 tft.setCursor(7,10);
 tft.setTextSize(2);
 tft.print(Line2);
 

 tft.setCursor(7,30);
 tft.setTextSize(1);
 tft.print(Line3);

 tft.setCursor(7,40);
 tft.setTextSize(2);
 tft.print(Line4);
 

 tft.setCursor(7,60);
 tft.setTextSize(1);
 tft.print(Line5);

 tft.setCursor(7,70);
 tft.setTextSize(2);
 tft.print(Line6);


 tft.setCursor(7,90);
 tft.setTextSize(1);
 tft.print(Line7);

 tft.setCursor(7,100);
 tft.setTextSize(2);
 tft.print(Line8);


 tft.setCursor(7,120);
 tft.setTextSize(1);
 tft.print(Line9);

 tft.setCursor(7,130);
 tft.setTextSize(2);
 tft.print(Line10);

 delay(250); // Wartet 0,25 Sekunden

 // =========
 // Sensor
 // =========
 
 if (! bme.performReading()) {
  Serial.println("Failed to perform reading :(");
  return;
 }
 Serial.print("Temperature = ");
 Serial.print(getTemperature());
 Serial.println(" *C");

 Serial.print("Pressure = ");
 Serial.print(getPressure());
 Serial.println(" hPa");

 Serial.print("Humidity = ");
 Serial.print(getHumidity());
 Serial.println(" %");

 Serial.print("Gas = ");
 Serial.print(getGas());
 Serial.println(" KOhms");

 Serial.print("Approx. Altitude = ");
 Serial.print(getAltitude());
 Serial.println(" m");

 Serial.println();
 printCurrentNet();
 delay(5000);
}

//Void functions
void printWifiData() {
  // print your board's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);
  Serial.println(ip);

  // print your MAC address:
  byte mac[6];
  WiFi.macAddress(mac);
  Serial.print("MAC address: ");
  printMacAddress(mac);
}

void printCurrentNet() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print the MAC address of the router you're attached to:
  byte bssid[6];
  WiFi.BSSID(bssid);
  Serial.print("BSSID: ");
  printMacAddress(bssid);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.println(rssi);

  // print the encryption type:
  byte encryption = WiFi.encryptionType();
  Serial.print("Encryption Type:");
  Serial.println(encryption, HEX);
  Serial.println();
}

void printMacAddress(byte mac[]) {
  for (int i = 5; i >= 0; i--) {
    if (mac[i] < 16) {
      Serial.print("0");
    }
    Serial.print(mac[i], HEX);
    if (i > 0) {
      Serial.print(":");
    }
  }
  Serial.println();
}


//Float functions
float getTemperature() {
  return bme.temperature;
}

float getPressure() {
  return bme.pressure / 100.0;
}

float getHumidity() {
  return bme.humidity;
}

float getGas() {
  return bme.gas_resistance / 1000.0;
}

float getAltitude() {
  return bme.readAltitude(SEALEVELPRESSURE_HPA);
}
