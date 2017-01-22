#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

/*
 * Konfiguration
 */
#define PIN_OUT_PIXELS         6   /* Daten-Pin Out für WS2812(b) */
#define PIN_IN_PIR             8   /* Daten-Pin In für HC-SR501 */

#define PIXELS_CONNECTED      56   /* Anzahl Pixel, die angeschlossen sind */
#define PIXELS_USE            56   /* Anzahl Pixel, die verwendet werden sollen */

#define DELAY_KEEP_ALIVE   30000   /* Wartezeit nach letzter Bewegung (ms) */
#define DELAY_CHECK           20   /* Zeit die jede LED getestet wird (ms) */
#define DELAY_DIMMER          25   /* Wartezeit zwischen 2 Farben (ms) */

#define COLOR_RED            139   /* Roter Anteil der Farbe */
#define COLOR_GREEN            0   /* Grüner Anteil der Farbe */
#define COLOR_BLUE           139   /* Blauer Anteil der Farbe */


unsigned int TIME_MOVEMENT   = 0;   
bool PIXELS_ON               = false;


Adafruit_NeoPixel pixels = Adafruit_NeoPixel(PIXELS_CONNECTED, PIN_OUT_PIXELS, NEO_GRB + NEO_KHZ800);

void setup() {
  pixels.begin();
  /* Alle aus, falls noch welche an sind */
  pixelsReset();
  /* Alle Pixel einzeln testen */
  pixelsCheck();
}

void loop() {
  if(LOW == digitalRead(PIN_IN_PIR)){
    if(true == PIXELS_ON){
      int timeCurrent = millis();
  
      if(timeCurrent < TIME_MOVEMENT){
        /* 50 tage reset */
        TIME_MOVEMENT = 0;
      }

      if((timeCurrent - TIME_MOVEMENT) > DELAY_KEEP_ALIVE){
        PIXELS_ON = false;
        pixelsReset();
      }
    }
  } else {
    if(false == PIXELS_ON){
      PIXELS_ON = true;
      pixelsLightUpDimmed();
    }

    TIME_MOVEMENT = millis();
  }
}

void pixelsReset(){
  uint32_t COLOR_BLACK = pixels.Color(0, 0, 0);

  for(int pixel=0;pixel<PIXELS_CONNECTED;pixel++){
    pixels.setPixelColor(pixel,COLOR_BLACK);
  }
  pixels.show();
}

void pixelsCheck(){
  uint32_t colorBlack = pixels.Color(0, 0, 0);

  for(int color=0;color<3;color++){
    int colorArray[3] = {0,0,0};
    colorArray[color] = 255;

    uint32_t colorTest = pixels.Color(colorArray[0],colorArray[1],colorArray[2]);

    for(int pixel=0;pixel<PIXELS_CONNECTED;pixel++){
      pixels.setPixelColor(pixel,colorTest);
      if(0 < pixel){
        pixels.setPixelColor(pixel-1,colorBlack);
      }
      pixels.show();
      delay(DELAY_CHECK);
    }
    pixels.setPixelColor(PIXELS_CONNECTED-1,colorBlack);
    pixels.show();
  }
}

void pixelsLightUpDimmed(){
  int totalSteps = getMaxColorValue();

  int colorRedCurrent = 0;
  int colorGreenCurrent = 0;
  int colorBlueCurrent = 0;
  
  for(int colorStep = 0;colorStep <= totalSteps;colorStep++){
    if(colorRedCurrent < COLOR_RED){
      colorRedCurrent++;
    }
    if(colorGreenCurrent < COLOR_GREEN){
      colorGreenCurrent++;
    }
    if(colorBlueCurrent < COLOR_BLUE){
      colorBlueCurrent++;
    }
    
    uint32_t colorGenerated = pixels.Color(colorRedCurrent, colorGreenCurrent, colorBlueCurrent);
    for(int i=0;i<PIXELS_USE;i++){
      pixels.setPixelColor(i, colorGenerated);
    }  
    pixels.show();
    delay(DELAY_DIMMER);
  }
}

int getMaxColorValue(){
  int values[3] = {COLOR_RED,COLOR_GREEN,COLOR_BLUE};
  int maxInteger = 0;

  for(int index = 0;index < 3;index++){
    maxInteger = max(maxInteger, values[index]);
  }

  return maxInteger;
}


