/* 
 *  Code created by Chris Evans @drumminhands
 *  Used to control Pumpkin light
 *  Use a button to chose between two programs:
 *    1) softly change the colors in a rainbow pattern
 *    2) map a potentiometer to color hue
 *    
*/

// sensor input from  http://www.arduino.cc/en/Tutorial/AnalogInput
// button state debounce from here: http://www.arduino.cc/en/Tutorial/Debounce
// NeoPixels from here: http://learn.adafruit.com/adafruit-neopixel-uberguide/overview

#include <Adafruit_NeoPixel.h>

const uint8_t neoPin = 6;      // Neopixels Pin
const uint8_t buttonPin = 4;   // the number of the pushbutton pin
const uint8_t pxls = 12;       // number of pixels in the neopixel ring
const uint8_t maxBright = 255; // the default brightness between 0 and 255

int sensorPin = A0;            // the input pin for the potentiometer
int sensorValue = 0;           // variable to store the value coming from the sensor
int state = HIGH;              // the current program state
int buttonState;               // the current reading from the input pin
int lastButtonState = LOW;     // the previous reading from the input pin
long lastDebounceTime = 0;     // the last time the output pin was toggled
long debounceDelay = 50;       // the debounce time; increase if the output flickers
long previousMillis = 0;       // will store last time LED was updated during rainbow
int neoPixelHue = 0;           // stores current hue of the neoPixels during the rainbow cycles

// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(pxls, neoPin, NEO_GRB + NEO_KHZ800);

// IMPORTANT: To reduce NeoPixel burnout risk, add 1000 uF capacitor across
// pixel power leads, add 300 - 500 Ohm resistor on first pixel's data input
// and minimize distance between Arduino and first pixel.  Avoid connecting
// on a live circuit...if you must, connect GND first.

void setup() {  
  pinMode(buttonPin, INPUT_PULLUP);
  strip.begin();
  strip.setBrightness(maxBright); // initialize brightness
  strip.show(); // Initialize all pixels to 'off'
}

void loop() {
//// first check if if the button has been pressed and if the state should be changed.
  
  // read the state of the switch into a local variable:
  int reading = digitalRead(buttonPin);

  // check to see if you just pressed the button
  // (i.e. the input went from LOW to HIGH),  and you've waited
  // long enough since the last press to ignore any noise:

  // If the switch changed, due to noise or pressing:
  if (reading != lastButtonState) {
    // reset the debouncing timer
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    // whatever the reading is at, it's been there for longer
    // than the debounce delay, so take it as the actual current state:

    // if the button state has changed:
    if (reading != buttonState) {
      buttonState = reading;

      // only toggle the LED if the new button state is HIGH
      if (buttonState == HIGH) {
        state = !state;
      }
    }
  }

//// then based on the state, either use the Potentiometer mapped to hue, or run the rainbow program
  if (state == HIGH){
    sensorValue = analogRead(sensorPin); // read the value from the sensor
    sensorValue = map(sensorValue, 0, 1023, 0, 255); // change value porportionally to be between 0 and 255
    colorSet(Wheel(sensorValue)); // set the color based on the pot
  } else {
    rainbow(20);
  }

  // save the reading.  Next time through the loop,
  // it'll be the lastButtonState:
  lastButtonState = reading;
}

// sets all pixels to one given color
void colorSet(uint32_t c){
  for(uint16_t i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, c);
  }
  strip.show();
}

// change the hue of the neopixels in a nice soft shift
void rainbow(uint8_t wait) {
    
  unsigned long currentMillis = millis(); // the current timestamp

  if(currentMillis - previousMillis > wait) { // have we waited long enough to debounce
    
    // save the last time you changed a NeoPixel 
    previousMillis = currentMillis; 

    //change the colors of the pixels
    uint16_t i;
    for(i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel((i+neoPixelHue) & 255));
    }
    strip.show();
    neoPixelHue = (neoPixelHue + 1) % 255; //increment until all colors are used, then start over
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
   return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else if(WheelPos < 170) {
    WheelPos -= 85;
   return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  } else {
   WheelPos -= 170;
   return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  }
}

