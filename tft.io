#include <SPI.h>
#include <Adafruit_GFX.h>    // Adafruit Grafik-Bibliothek wird benötigt
#include <Adafruit_ST7735.h> // Adafruit ST7735-Bibliothek wird benötigt 
//MOSI 8
//CS 6
//SCLK 9
#define TFT_PIN_MOSI 8 //custom
#define TFT_PIN_SCLK 9 //custom
#define TFT_PIN_CS   6 // Arduino-Pin an Display CS
#define TFT_PIN_DC   1  // Arduino-Pin an oder 17
#define TFT_PIN_RST  15  // Arduino Reset-Pin
#define OUT_BUZZER 2 
#define OUT_LED_RED 3
#define OUT_LED_YELLOW 4
#define OUT_LED_GREEN 5
 
// ST7735-Bibliothek Setup
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_PIN_CS, TFT_PIN_DC, TFT_PIN_MOSI, TFT_PIN_SCLK, TFT_PIN_RST);
void setup() {
tft.initR(INITR_GREENTAB);     // Initialisierung der Bibliothek
tft.fillScreen(ST7735_GREEN);  // Färbt Hintergund Schwarz
pinMode(OUT_BUZZER, OUTPUT); //set Buzzer Pin as output
}  
 
void loop() {
  tft.setCursor(7,1);           //Setze Position
  tft.setTextSize(3);           //Schriftgröße einstellen
  tft.print("joy-IT");          //Text ausgeben
  tft.setCursor(15,30);         //Setze Position
  tft.setTextSize(1);           //Schriftgröße einstellen
  tft.print("1.8 TFT LCD");     //Text ausgeben
  tft.setCursor(7,45);          //Setze Position
  tft.setTextSize(2);           //Schriftgröße einstellen
  tft.print("ST7735");          //Text ausgeben 
  //digitalWrite(OUT_BUZZER, HIGH); // sets the digital pin OUT_BUZZER on
  delay(1000);            // waits for a second
  digitalWrite(OUT_BUZZER, LOW);  // sets the digital pin OUT_BUZZER off
  digitalWrite(OUT_LED_RED, HIGH);
  delay(1000);
  digitalWrite(OUT_LED_YELLOW, HIGH);
  digitalWrite(OUT_LED_RED, LOW);
  delay(250);                   //Wartet 0,25 Sekunden
  digitalWrite(OUT_LED_GREEN,HIGH);
  digitalWrite(OUT_LED_YELLOW, LOW);
  delay(250); 
  digitalWrite(OUT_LED_GREEN, LOW);
  delay(250);
  digitalWrite(OUT_LED_GREEN, HIGH);
  delay(250);
  digitalWrite(OUT_LED_GREEN, LOW);
} 
