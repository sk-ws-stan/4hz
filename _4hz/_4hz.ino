#include "FastLED.h"

// how fast to move the dots, in frames per second, adjust this to 
// make the dots move faster or slower
#define SPEED 100

// Change this to adjust how long of a tail a dot will have
#define FADE_RATE 64

#define NUM_LEDS 48
#define NUM_ARMS 4

CRGB leds[NUM_ARMS][NUM_LEDS];

// These are the moving dots - we'll store a hue value in each of these
uint8_t dots[NUM_ARMS][NUM_LEDS];

void setup() { 
  // replace with your actual pin layouts - see https://github.com/FastLED/FastLED/wiki/SPI-Hardware-or-Bit-banging for advice
  FastLED.addLeds<APA102,7,8>(leds[0], NUM_LEDS);
  FastLED.addLeds<APA102,9,10>(leds[1], NUM_LEDS);
  FastLED.addLeds<APA102,11,12>(leds[2], NUM_LEDS);
  FastLED.addLeds<APA102,13,14>(leds[3], NUM_LEDS);

  // Keep power usage sane (at least while testing)
  setMaxPowerInVoltsAndMilliamps(5, 1000);
  
  // 0 out all the dots
  memset(dots, 0, NUM_ARMS * NUM_LEDS);
}

bool is_button_pressed(int button) { 
  // TODO: Fill this in to check whether a given button has been pressed
  // you may want some checks to make sure that holding the button down
  // only does a single dot still

  // return a random dot for now
  if(random8() < 25) return true; 
}

void loop() { 
  // 60 times a second, this will move dots down the line with fading tails
  EVERY_N_MILLISECONDS(1000/SPEED) { 
    // Fade anything currently on the leds
    fadeToBlackBy((CRGB*)leds, NUM_ARMS * NUM_LEDS, 64);

    // Draw all the currently defined dots
    for(int r = 0 ; r < NUM_ARMS; r++ ) {
      for(int i = 0; i < NUM_LEDS; i++) { 
        if(dots[r][i] != 0) { 
          leds[r][i] += CHSV(dots[r][i], 192, 255);
        }
      }
    }

    // Now, "move" all the dots down the line.  We're going to cheat and use memmove
    // to do all the work from us.  Basically, we're going to move every dot one forward.
    // of course, this would mean that dots[0][99] would be moved into dots[1][0], so we'll 
    // clean that up next so they don't wrap over the arms
    memmove(&dots[0][1], dots, (NUM_ARMS * NUM_LEDS) - 1);
    for(int r = 0; r < NUM_ARMS; r++) { dots[r][0] = 0; }

    // Now, check if any buttons are pressed, if so - put a new dot on the line!
    for(int r = 0; r < NUM_ARMS; r++) { 
      if(is_button_pressed(r)) { 
        // yay, button pressed!  New dot for this row!  Let's pick a random hue from 
        // 1-255 for the value
        dots[r][0] = 1 + random8(254);
      }
    }
    
    FastLED.show();
  }
