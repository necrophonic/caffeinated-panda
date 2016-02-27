/*
 * RGB waterfall and colour blend for 3 pin Gemma
 */

#include <Adafruit_NeoPixel.h>

#define ANP  Adafruit_NeoPixel

#define PIN0  0
#define PIN1  1
#define PIN2  2

#define STRIP_LENGTH      18 // Number of neopixels in a strip
#define NUM_STRIPS        3  // Designed to run in pairs
#define MAX_BRIGHTNESS    100
#define PULSE_SIZE        7  // Length of each lit pulse

#define SECONDS_PER_MINUTE 60

// Predefine some colours
#define C_OFF   ANP::Color(0,0,0)
#define C_RED   ANP::Color(200,0,0)
#define C_GREEN ANP::Color(0,200,0)
#define C_BLUE  ANP::Color(0,0,200)

// Define 3 strips.
// Assuming each is up to about 20 pixels each
struct Pixel_strip {
  ANP     strip;   // The strip itself
  uint8_t offset;  // Used for staggered effects
} strips[3] = {
  {
    ANP(STRIP_LENGTH, PIN0, NEO_GRB + NEO_KHZ800),
    0
  },
  {
    ANP(STRIP_LENGTH, PIN1, NEO_GRB + NEO_KHZ800),
    4
  },
  {
    ANP(STRIP_LENGTH, PIN2, NEO_GRB + NEO_KHZ800),
    8
  }
};


struct RGB {
  uint8_t  r;
  uint8_t  g;
  uint8_t  b; 
};
  
void setup() {
  // Initialise and clear down each strip
  for (uint8_t i=0; i<NUM_STRIPS; i++) {
    strips[i].strip.begin();
    strips[i].strip.show(); 
  }
}

void loop() {
    waterfall( C_RED, STRIP_LENGTH, strips, 5 * SECONDS_PER_MINUTE, 50 );
    pulse(     C_RED, STRIP_LENGTH, strips, 5 * SECONDS_PER_MINUTE, 50 );
}

/* === EFFECTS === */

/*
Waterfall

Waterfall down each strip continuously. Can specify a start delay
that will offset each strip starting.

color     Adafruit_Neopixel colour to display
length    length of the strips
strips[]  array of strips to waterfall down
duration  how long to run the effect for (seconds)
delay     offset delay between strip starts
*/
void waterfall(uint32_t colour, uint8_t length, struct Pixel_strip strips[], uint16_t duration, uint8_t start_delay) {

  const uint8_t drop_length = 7;
  const uint16_t delay_time  = 50;
  
  // Manage current offsets
  uint8_t offsets[NUM_STRIPS] = {0};
  
  // Initialise
  resetStrips(strips);
  setStripsBrightness(strips,MAX_BRIGHTNESS);

  // Work out how many iterations we need.  
  uint16_t max_iterations = (duration*1000) / delay_time;
  uint16_t iterations     = 0;
  
  for (;;) {
    for (uint8_t pix=0; pix<length+1+drop_length; pix++) {
      for (uint8_t sn=0; sn<NUM_STRIPS; sn++) {
        strips[sn].strip.setPixelColor(pix, colour);
        if (pix>drop_length-1) strips[sn].strip.setPixelColor(pix-drop_length-1, C_OFF);
        strips[sn].strip.show();
      }
      delay(delay_time);
      if (iterations++ >= max_iterations) return;
    }
  }
}

/*
Colour pulse

color     Adafruit_Neopixel colour to display
length    length of the strips
strips[]  array of strips to waterfall down
duration  how long to run the effect for (seconds)
delay     offset delay between strip starts
*/
void pulse(uint32_t color, uint8_t length, struct Pixel_strip strips[], uint16_t duration, uint8_t start_delay) {
  
  const uint16_t delay_time  = 20;
  
  // Work out how many iterations we need.  
  uint16_t max_iterations = (duration*1000) / delay_time;
  uint16_t iterations     = 0;
  
  for (uint8_t sn=0; sn<NUM_STRIPS; sn++) {
    for (uint8_t pix=0; pix<20; pix++) {
      strips[sn].strip.setPixelColor(pix, color);
      strips[sn].strip.setBrightness(20);
      strips[sn].strip.show();
    }
  }
    
  for (;;) {
    // Pulse in then out
    for (uint8_t i=20; i<=MAX_BRIGHTNESS; i+=2) {
      setStripsBrightness(strips,i);
      delay(20);
      if (iterations++ >= max_iterations) return;
    }
    for (uint8_t i=MAX_BRIGHTNESS; i>=20; i-=2) {
      setStripsBrightness(strips,i);
      delay(20);
      if (iterations++ >= max_iterations) return;
    }
  } // pulses
}

/* === UTILITIES === */

// Hard reset to "off"
void resetStrips(struct Pixel_strip strips[]) {
  for (uint8_t sn=0; sn<NUM_STRIPS; sn++) {
    for (uint8_t pix=0; pix<STRIP_LENGTH; pix++) {
      strips[sn].strip.setPixelColor(pix,C_OFF);
    }
    strips[sn].strip.show();
  }
}

void setStripsBrightness(struct Pixel_strip strips[],uint8_t bright) {
  for (uint8_t sn=0; sn<NUM_STRIPS; sn++) {
    strips[sn].strip.setBrightness(bright);
    strips[sn].strip.show();
  }
}

struct RGB parseColour(uint32_t colour) {
  struct RGB rgb = {
    (uint8_t)(colour >> 16),
    (uint8_t)(colour >> 8),
    (uint8_t)(colour)
  };
  return rgb;
}
