#include <SPI.h>
#include <SdFat.h>
#include <SdFatUtil.h>
#include <SFEMP3Shield.h>
#include <SoftwareSerial.h>
#include "Adafruit_TCS34725.h"
#include "Flora_Pianoglove.h"

SdFat sd;
RgbColor rgb;
byte redH, greenH, blueH;
SFEMP3Shield MP3player;
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X);

typedef struct Hsv {
     double h;
     double s;
     double v;
} Hsv;

Hsv hsv;

void(* resetFunc) (void) = 0;   

void setup() {
  
  Serial.begin(9600);
  
  //start the sdcard
  sd.begin(SD_SEL, SPI_HALF_SPEED);

  //start the mp3
  MP3player.begin();
  MP3player.setVolume(10,10);
    
  //Check for color sensor
  if (!tcs.begin()) {
    Serial.println("No TCS34725 found ... check your connections");
    while (1); // halt!
  }
}


void loop() {
    while(true){
      uint16_t clear, red, green, blue;
         
      tcs.setInterrupt(false); delay(60);
      tcs.getRawData(&red, &green, &blue, &clear);

      rgb = colorNormalized(red, green, blue, clear);  
      hsv = rgbToHsv(rgb);

      if(hsv.h == 0.0  && hsv.v == 0.0  &&  hsv.s == 0.0) resetFunc();
      playSong( defineColor(hsv) );
        
      Serial.println(" "); Serial.println("----"); 
      Serial.print("\tH:\t"); Serial.print(hsv.h);
      Serial.print("\tS:\t"); Serial.print(hsv.s);
      Serial.print("\tV:\t"); Serial.print(hsv.v);
      Serial.println(" "); Serial.println("----");
      Serial.println(" "); 
      delay(300);
    } 
}


RgbColor colorNormalized( uint16_t red, uint16_t green, uint16_t blue,uint16_t clear ){

  RgbColor rgb;

  uint32_t sum = red;
  sum += green;
  sum += blue;
  sum = clear;
  
float r, g, b;
  r = red; r /= sum;
  g = green; g /= sum;
  b = blue; b /= sum;
  
  r *= 256; g *= 256; b *= 256;
  
  if (r > 255) r = 255;
  if (g > 255) g = 255;
  if (b > 255) b = 255;

  rgb.r = r;
  rgb.g = g;
  rgb.b = b;
  
  return rgb;
}


Hsv rgbToHsv(RgbColor rgb){
  Hsv hsv;
  double red = rgb.r;
  double green = rgb.g;
  double blue = rgb.b;
  double max =  max(red, max(green, blue));
  double min = min(red, min(green, blue));
  double h, s, v = max;
    
  double delta = max - min;
  s = max == 0 ? 0 : delta / max;


    if (max == min) { 
        h = 0; // achromatic
    } else {
        if (max == red) {
            h = (green - blue) / delta + (green < blue ? 6 : 0);
        } else if (max == green) {
            h = (blue - red) / delta + 2;
        } else if (max == blue) {
            h = (red - green) / delta + 4;
        }
        h *= 60;
    }

    hsv.h = h;
    hsv.s = s;
    hsv.v = v;

  return hsv;
}

int defineColor(Hsv hsv){
 int number = 0;
 double hue = hsv.h;
 double sat = hsv.s;

  if(hue > 351){
    Serial.print("Vermelho");
    number=4;
   }

  if(hue == 0.0) {
    Serial.print("Branco");
  } 
   
  if(hue > 0 && hue < 11 && hue <= 45) {
    Serial.print("Marrom");
    number=4;
  }
  
  if(hue > 45 && hue <= 64){
    Serial.print("Amarelo");
    number=5;
  }
  
  if(hue > 64 && hue < 180){
    Serial.print("Verde");
    
     if(hue > 159 && hue <=172) number=3;
     if(hue > 173) number=7;
     
   }
   
   if(hue > 180 && hue <= 255) {
      
      Serial.print("Azul ");
      if(sat <= 0.73){
        Serial.print("Claro - 1");
        number=1;
        }else{
           Serial.print("Escuro - 2");
          number=2;
          } 
   }
   
   if(hue > 255 && hue < 310) {
     Serial.print("Violeta");
     number=6;
   }

  return number;
  }

  void playSong(int number){
    if(number != 0)  MP3player.playTrack(number);    
    }
  
