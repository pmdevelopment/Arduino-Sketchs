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

#define COLOR_RED            139   /* Roter Anteil der Farbe */
#define COLOR_GREEN            0   /* Grüner Anteil der Farbe */
#define COLOR_BLUE           139   /* Blauer Anteil der Farbe */

#define FADE_DURATION       3500   /* Zeit die Farbübergänge dauern (ms) */

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
        int colorNew[3] = {0,0, 0};
        pixelsFade(colorNew, FADE_DURATION);
      }
    }
  } else {
    if(false == PIXELS_ON){
      PIXELS_ON = true;
      int colorNew[3] = {COLOR_RED,COLOR_GREEN, COLOR_BLUE};
      pixelsFade(colorNew, FADE_DURATION);
    }

    TIME_MOVEMENT = millis();
  }
}

void pixelsReset(){
  uint32_t colorBlack = pixels.Color(0, 0, 0);

  for(int pixel=0;pixel<PIXELS_CONNECTED;pixel++){
    pixels.setPixelColor(pixel,colorBlack);
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


void pixelsLightUpColor(int colorRed, int colorGreen, int colorBlue){
  uint32_t color = pixels.Color(colorRed, colorGreen, colorBlue);
  for(int pixel=0;pixel<PIXELS_USE;pixel++){
    pixels.setPixelColor(pixel, color);
  }
  pixels.show();
}

void pixelsFade(int colorTarget[], int duration){
  uint32_t colorCurrentInt = pixels.getPixelColor(0);
  
  int colorCurrentRed = (colorCurrentInt >> 16) & 0xFF;
  int colorCurrentGreen = (colorCurrentInt >> 8) & 0xFF;
  int colorCurrentBlue = colorCurrentInt & 0xFF;
  
  int colorCurrent[] = {colorCurrentRed,colorCurrentGreen,colorCurrentBlue};
  
  int dimmDelay = int(duration/getStepCount(colorCurrent, colorTarget));
  while(colorCurrent[0] != colorTarget[0] || colorCurrent[1] != colorTarget[1] || colorCurrent[2] != colorTarget[2]){
    for(int index = 0;index < 3;index++){
      if(colorCurrent[index] == colorTarget[index]){
        continue;
      }

      if(colorCurrent[index] > colorTarget[index]){
        colorCurrent[index]--;
      } else {
        colorCurrent[index]++;
      }
    }

    pixelsLightUpColor(colorCurrent[0], colorCurrent[1], colorCurrent[2]);
    delay(dimmDelay);
  }
}

int getStepCount(int from[], int to[]){
  int maxInteger = 0;

  for(int index = 0;index < 3;index++){
    int diff = to[index] - from[index];
    if(0 > diff){
      diff = diff * -1;
    }
    
    maxInteger = max(maxInteger, diff);
  }

  return maxInteger;
}

