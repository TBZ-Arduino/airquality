#include <SPI.h>
#include <Adafruit_GFX.h> // Adafruit Grafik-Bibliothek wird benötigt
#include <Adafruit_ST7735.h> // Adafruit ST7735-Bibliothek wird benötigt
#define TFT_PIN_CS 0 // Arduino-Pin an Display CS
#define TFT_PIN_DC 6 // Arduino-Pin an
#define TFT_PIN_RST 1 // Arduino Reset-Pin
// ST7735-Bibliothek Setup
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_PIN_CS, TFT_PIN_DC, TFT_PIN_RST);
void setup()
{
 tft.initR(INITR_GREENTAB); // Initialisierung der Bibliothek
 tft.fillScreen(ST7735_BLACK); // Färbt Hintergund Schwarz
}
void loop()
{
 tft.setCursor(7,1); //Setze Position
 tft.setTextSize(3); //Schriftgröße einstellen
 tft.print("Test!"); //Text ausgeben

 tft.setCursor(15,30); //Setze Position
 tft.setTextSize(1); //Schriftgröße einstellen
 tft.print("1.8 TFT LCD"); //Text ausgeben

 tft.setCursor(7,45); //Setze Position
 tft.setTextSize(2); //Schriftgröße einstellen
 tft.print("ST7735"); //Text ausgeben
 delay(250); //Wartet 0,25 Sekunden
}
