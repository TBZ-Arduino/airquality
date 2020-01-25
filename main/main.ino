/*

  TBZ HF - IG1
  ============

  Version: 21.01.2020

  Dependencies
  ==============

  Install in Arduino IDE at "Tools -> Manage Libraries"

  For the BME680 sensor:
  - Adafruit BME680 Library by Adafruit
  - Adafruit Unified Sensor by Adafruit

  For WiFi:
  - WiFiNNA by Arduino

  For Joy-IT 1.9" TFT Display:
  - TFT Built-in by Arduino, Adafruit
  - Adafruit GFX Library
  - Adafruit ST7735 and ST7789 Library

*/

#include <Wire.h>
#include <SPI.h>

// TFT: Libraries
#include <Adafruit_GFX.h>    // requires "Adafruit GFX Library"
#include <Adafruit_ST7735.h> // requires "Adafruit ST7735 and ST7789 Library"

// Sensor: Libraries
#include <Adafruit_Sensor.h>
#include "Adafruit_BME680.h"

//WiFi: Libraries
#include <WiFiNINA.h>
#include "arduino_secrets.h"

// TFT: Variables
#define TFT_PIN_CS 0         // Arduino-Pin on Display CS
#define TFT_PIN_DC 6         // Arduino-Pin
#define TFT_PIN_RST 1        // Arduino Reset-Pin

// Sensor: Variables
#define BME_SCK 9
#define BME_MISO 10
#define BME_MOSI 8
#define BME_CS 7

// Default value from documentation was 1013.25. According to https://kachelmannwetter.com/ch/messwerte/zuerich/luftdruck-qnh/20200119-1500z.html
// calibrated to Schweiz (Zürich) Adjust to your location.
#define SEALEVELPRESSURE_HPA (1036.5)

// TFT: Settings
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_PIN_CS, TFT_PIN_DC, TFT_PIN_RST);

// Sensor: Settings
Adafruit_BME680 bme(BME_CS); // hardware SPI

//WiFi: Settings, define these in arduino_secrets.h
char ssid[] = SECRET_SSID;        // your network SSID (name)
char pass[] = SECRET_PASS;    // your network password (use for WPA, or use as key for WEP)
int status = WL_IDLE_STATUS;     // the Wifi radio's status

WiFiClient client; //For HTTP requests to send data by POST request to influxDB
char buf[influxdbBufferSize] = {'\0'};
const String strInfluxDbServer = influxdbServer; //Convert to String
const String strInfluxDbPort = String(influxdbPort); //Convert to String

//Define globally used variables outside of scope
float floatTemperature;
float floatPressure;
float floatHumidity;
float floatGas;
float floatAltitude;

//For the output logic, these will be set to 0 for OK, 1 for warning and 2 for critical
int intTemperatureState;
int intPressureState;
int intHumidityState;
int intGasState;

void setup() {

  // Sets the Digital Pin as output
  pinMode(2, OUTPUT); // Buzzer
  pinMode(3, OUTPUT); // LED red
  pinMode(4, OUTPUT); // LED yellow
  pinMode(5, OUTPUT); // LED green

  // TFT
  tft.initR(INITR_GREENTAB); // Initialise TFT library
  tft.fillScreen(ST7735_BLACK); // Fill display with black

  // Sensor
  Serial.begin(9600);

  if (!bme.begin()) {
    Serial.println("Could not find a valid BME680 sensor, check wiring!");
    while (1);
  }

  bme.setTemperatureOversampling(BME680_OS_8X);
  bme.setHumidityOversampling(BME680_OS_2X);
  bme.setPressureOversampling(BME680_OS_4X);
  bme.setIIRFilterSize(BME680_FILTER_SIZE_3);
  bme.setGasHeater(320, 150); // 320*C for 150 ms

  setWifi(); //Call to function to set up WiFi connection
}

//Variables outside of loop
String line;

void loop() {
  setSensorData();
  setDisplay();
  setData();
  setOutput();

  delay(3000);
  //printCurrentNet();
}

// =========
// Void functions
// =========

// Status OK
float statusOk() {
  digitalWrite(5, HIGH); //LED green
}

// Status Warnung
float statusWarning() {
  digitalWrite(4, HIGH); //LED yellow
}

// Status Kritisch
float statusCritical() {
  digitalWrite(3, HIGH); // LED red
  digitalWrite(2, HIGH); // Buzzer
}

// Setzt Ausgaben zurück
float statusReset() {
  digitalWrite(5, LOW); // LED green
  digitalWrite(4, LOW); // LED yellow
  digitalWrite(3, LOW); // LED red
  digitalWrite(2, LOW); // Buzzer
}

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

void sendData(char* data, int influxdbBufferSize) {
  if (client.connect(influxdbServer, influxdbPort)) { //if connected to server
    //Print the buffer on the serial line to see how it looks
    Serial.print("Sending following dataset to InfluxDB: ");
    Serial.println(buf);
    client.println("POST /api/v2/write?org=" + influxdbOrgId + "&bucket=" + influxdbBucketName + "&precision=s HTTP/1.1"); //start POST request
    client.println("Host: " + strInfluxDbServer + ":" + strInfluxDbPort + ""); //set various Request Headers
    client.println("Authorization: Token " + influxdbAuthToken);
    client.println("Content-Type: application/x-www-form-urlencoded");
    client.println("Connection: close");
    client.print("Content-Length: ");
    client.println(influxdbBufferSize);
    client.println();
    client.println(data); //set request content
    client.stop();
  }
  else {
    Serial.println("Connection to server failed");
  }
}

void setData() {
  //this will be number of chars written to buffer, return value of sprintf
  int numChars = 0;
  //First of all we need to add the name of measurement to beginning of the buffer
  numChars = sprintf(buf, "sensor,");

  //tag should have an space at the end
  numChars += sprintf(&buf[numChars], "sensor-id=1 ");

  numChars += sprintf(&buf[numChars], "temperature=%.2f,", getTemperature());
  numChars += sprintf(&buf[numChars], "pressure=%.2f,", getPressure());
  numChars += sprintf(&buf[numChars], "humidity=%.2f,", getHumidity());
  numChars += sprintf(&buf[numChars], "gas=%.2f,", getGas());
  numChars += sprintf(&buf[numChars], "altitude=%.2f", getAltitude());

  //send to InfluxDB
  sendData(buf, numChars);

  //we have to reset the buffer at the end of loop
  memset(buf, '\0', influxdbBufferSize);
}

void setWifi() {
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

    // retry to connect every 5 seconds:
    delay(5000);
  }
  // you're connected now, so print out the data:
  Serial.print("You're connected to the network");
  //printCurrentNet();
  //printWifiData();
}

void setDisplay() {
  // =========
  // TFT
  // =========


  tft.fillScreen(ST7735_BLACK); // Fill display with black to not have strange behaviour when overwriting displayed text

  String Line1  = "Temperatur:";
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
  tft.setCursor(7, 0);            // Set position
  tft.setTextSize(1);             // Set font size
  tft.print(Line1);               // Output text

  tft.setCursor(7, 10);
  tft.setTextSize(2);
  tft.print(Line2);


  tft.setCursor(7, 30);
  tft.setTextSize(1);
  tft.print(Line3);

  tft.setCursor(7, 40);
  tft.setTextSize(2);
  tft.print(Line4);


  tft.setCursor(7, 60);
  tft.setTextSize(1);
  tft.print(Line5);

  tft.setCursor(7, 70);
  tft.setTextSize(2);
  tft.print(Line6);


  tft.setCursor(7, 90);
  tft.setTextSize(1);
  tft.print(Line7);

  tft.setCursor(7, 100);
  tft.setTextSize(2);
  tft.print(Line8);


  tft.setCursor(7, 120);
  tft.setTextSize(1);
  tft.print(Line9);

  tft.setCursor(7, 130);
  tft.setTextSize(2);
  tft.print(Line10);
}

void setSensorData() {
  if (! bme.performReading()) {
    Serial.println("Failed to perform reading on sensor:(");
    return;
  }
  setTemperature();
  setPressure();
  setHumidity();
  setGas();
  setAltitude();
}

void setTemperature() {
  floatTemperature = bme.temperature;
}

void setPressure() {
  floatPressure = bme.pressure / 100.0; // in hPa
}

void setHumidity() {
  floatHumidity = bme.humidity;
}

void setGas() {
  floatGas = bme.gas_resistance / 1000.0; // in kOhm
}

void setAltitude() {
  floatAltitude = bme.readAltitude(SEALEVELPRESSURE_HPA);
}

void setOutput() {
  setOutputTemperature();
  setOutputPressure();
  setOutputHumidity();
  setOutputGas();
  statusReset(); //set all output to LOW

  if ( intTemperatureState == 2 || intPressureState == 2 || intGasState == 2 ) { //if any value is critical
    statusCritical();
  }

  else if ( intTemperatureState == 0 && intPressureState == 0 && intHumidityState == 0 && intGasState == 0 ) { //if all values are ok
    statusOk();
  }

  else { //else show warning
    statusWarning();
  }
}

void setOutputTemperature() {
  if ( floatTemperature < 16 || floatTemperature > 29 ) { //set critical
    intTemperatureState = 2;
    Serial.println("Temperature critical");
  }

  else if ( floatTemperature >= 20 && floatTemperature <= 24) { //set ok
    intTemperatureState = 0;
    Serial.println("Temperature OK");
  }

  else { //set warning
    intTemperatureState = 1;
    Serial.println("Temperature warning");
  }
}

void setOutputPressure() {
  if ( floatPressure > 30000 ) { //set critical
    intPressureState = 2;
    Serial.println("Pressure critical");
  }

  else if ( floatPressure >= 533.3 && floatPressure <= 1026.3 ) { //set ok
    intPressureState = 0;
    Serial.println("Pressure OK");
  }

  else { //set warning
    intPressureState = 1;
    Serial.println("Pressure warning");
  }
}

void setOutputHumidity() {
  if ( floatHumidity <= 40 || floatHumidity >= 60 ) { //set warning
    intHumidityState = 1;
    Serial.println("Humidity warning");
  }

  else { //set ok
    intHumidityState = 0;
    Serial.println("Humidity OK");
  }
}

void setOutputGas() {
  if ( floatGas > 150) { //set ok
    intGasState = 0;
    Serial.println("Gas OK");
  }

  else if ( floatGas >= 100 && floatGas <= 150) {//set warning
    intGasState = 1;
    Serial.println("Gas warning");
  }

  else { //set critical
    intGasState = 2;
    Serial.println("Gas critical");
  }
}

// =========
// Float functions
// =========

float getTemperature() {
  return floatTemperature;
}

float getPressure() {
  return floatPressure;
}

float getHumidity() {
  return floatHumidity;
}

float getGas() {
  return floatGas;
}

float getAltitude() {
  return floatAltitude;
}
