#include "FastLED.h"
#define NUM_LEDS 7
CRGB leds[NUM_LEDS];
#define LED_PIN 6 
uint8_t gBrightness = 255;

#include "SD.h"
#define SD_ChipSelectPin 4
#include "TMRpcm.h"
#include "SPI.h"
TMRpcm audio;
File root;
char toPlay[20]; 
int indexToWrite = 0;

#define SPEAKER_PIN 9
#define DISABLE_SPEAKER2

//ACCELEROMETER 
int Zval;
int accPin = A1;

void setup() {  
  audio.speakerPin = 9;
  FastLED.addLeds<WS2811, LED_PIN, RGB>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
  Serial.begin(9600);
  pinMode(accPin, INPUT); 
  pinMode(SPEAKER_PIN, OUTPUT); 
  digitalWrite(SPEAKER_PIN, LOW);
  pinMode(LED_PIN, OUTPUT);
  Zval = analogRead(accPin);
  
  if(!SD.begin(SD_ChipSelectPin))
   {
      Serial.println("SD fail");
      return;
    }
   Serial.println("OK!");
   root = SD.open("/");      // open SD card main root
   audio.setVolume(0);
}

void loop() {
  
Zval = analogRead(accPin);
Serial.print(analogRead(accPin));
Serial.println();
          
//only the z-axis is needed to detect it's been flipped over
   if (Zval > 460) { 
      digitalWrite(SPEAKER_PIN, LOW);
      audio.pause(); 
      audio.setVolume(0);   
      fill_solid(leds, NUM_LEDS, CRGB::Black);
      FastLED[1].showLeds(gBrightness);    
    }
      
  else if (Zval < 460) {          
    //SOUNDS
    digitalWrite(SPEAKER_PIN, HIGH);
    randomSeed(analogRead(0));
    int randNumber = random(20); // random number between 0 and 18
    makeName(randNumber, 0); // generate file name
    audio.play(toPlay);
    audio.quality(10); 
    audio.setVolume(4);  //Volume below 5 causes lights to whine 

    while ( audio.isPlaying() ) {
    delay(500); 
    //LIGHTS
      TwinkleRandom(5, 200, false);
    }   
  };

}

void TwinkleRandom(int Count, int SpeedDelay, boolean OnlyOne) {
  setAll(0,0,0);
  
  for (int i=0; i<Count; i++) {
     setPixel(random(NUM_LEDS),random(0,55),random(0,0),random(0,255));
     showStrip();
     delay(SpeedDelay);
     if(OnlyOne) { 
       setAll(0,0,0); 
     }
   }
  
  delay(SpeedDelay);
}


// ******* sd card and sound *******
void makeName(int number, int depth){  // generates a file name 0.WAV to 9999.WAV suppressing leading zeros
  if(number > 9) {
    makeName(number / 10, ++depth); // recursion
    depth--;
    number = number % 10;   // only have to deal with the next significant digit of the number
     }
  toPlay[indexToWrite] = (number & 0xf) | 0x30;
  indexToWrite++;
  if(depth > 0) return; // return if we have more levels of recursion to go
  else {  // finish off the string with the wave extesion
     toPlay[indexToWrite] = '.';
     toPlay[1+indexToWrite] = 'W';
     toPlay[2+indexToWrite] = 'A';
     toPlay[3+indexToWrite] = 'V';
     toPlay[4+indexToWrite] = '\0'; // terminator
     indexToWrite = 0; // reset pointer for next time we enter
  }
}

// LIGHTS LIGHTS LIGHTS *************************************//

void Twinkle(byte red, byte green, byte blue, int Count, int SpeedDelay, boolean OnlyOne) {
  setAll(0,0,0);
  
  for (int i=0; i<Count; i++) {
     setPixel(random(NUM_LEDS),red,green,blue);
     showStrip();
     delay(SpeedDelay);
     if(OnlyOne) { 
       setAll(0,0,0); 
     }
   }

  delay(SpeedDelay);
}


// *** REPLACE TO HERE ***

void showStrip() {
 #ifdef ADAFRUIT_NEOPIXEL_H 
   // NeoPixel
   strip.show();
 #endif
 #ifndef ADAFRUIT_NEOPIXEL_H
   // FastLED
   FastLED.show();
 #endif
}

void setPixel(int Pixel, byte red, byte green, byte blue) {
 #ifdef ADAFRUIT_NEOPIXEL_H 
   // NeoPixel
   strip.setPixelColor(Pixel, strip.Color(red, green, blue));
 #endif
 #ifndef ADAFRUIT_NEOPIXEL_H 
   // FastLED
   leds[Pixel].r = red;
   leds[Pixel].g = green;
   leds[Pixel].b = blue;
 #endif
}

void setAll(byte red, byte green, byte blue) {
  for(int i = 0; i < NUM_LEDS; i++ ) {
    setPixel(i, red, green, blue); 
  }
  showStrip();
}
