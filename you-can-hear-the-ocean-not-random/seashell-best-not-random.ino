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
   printDirectory(root, 0);  // print all files names and sizes 
   audio.setVolume(4);
   audio.quality(10);  
}

void loop() {
  
Zval = analogRead(accPin);
Serial.print(analogRead(accPin));
Serial.println();
          
//only the z-axis is needed to detect it's been flipped over
   if (Zval > 460) { 
      digitalWrite(SPEAKER_PIN, LOW);
      audio.pause(); 
 
      fill_solid(leds, NUM_LEDS, CRGB::Black);
      FastLED[1].showLeds(gBrightness);    
    }
      
  else if (Zval < 460) {          
    //SOUNDS
    digitalWrite(SPEAKER_PIN, HIGH);

  if ( !audio.isPlaying() ) {
    // no audio file is playing
    File entry =  root.openNextFile();  // open next file
    if (! entry) {
      // no more files
      root.rewindDirectory();  // go to start of the folder
      return;
    }
 
    uint8_t nameSize = String(entry.name()).length();  // get file name size
    String str1 = String(entry.name()).substring( nameSize - 4 );  // save the last 4 characters (file extension)
  
    if ( str1.equalsIgnoreCase(".wav") ) {
      // the opened file has '.wav' extension
      audio.play( entry.name() );      // play the audio file
      Serial.print("Playing file: ");
      Serial.println( entry.name() );
    }
    else {
      // not '.wav' format file
      entry.close();
      return;
    }

  }

    while ( audio.isPlaying() ) {
    delay(500); 
    //LIGHTS
      TwinkleRandom(5, 200, false);
    }   
  };

}



void printDirectory(File dir, int numTabs) {
  while (true) {
 
    File entry =  dir.openNextFile();
    if (! entry) {
      // no more files
      break;
    }
    for (uint8_t i = 0; i < numTabs; i++) {
      Serial.print('\t');
    }
    Serial.print(entry.name());
    if (entry.isDirectory()) {
      Serial.println("/");
      printDirectory(entry, numTabs + 1);
    } else {
      // files have sizes, directories do not
      Serial.print("\t\t");
      Serial.println(entry.size(), DEC);
    }
    entry.close();
  }
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
