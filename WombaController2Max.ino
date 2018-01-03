// NeoPixel Controller
// + Input Sender


// NeoPixel map based on LED map (1-69)
// "Touch" spots @ 8, 19, 28, 41, 54
// Animation effects:
// 1.) centered around each touch spot, ripple effect radiating outwards
//     -> using internal "rings" [ ] TBD
// 2.)
// 3.) radial lines from centre [ ] TBD
// 4.) "background"/idle mode (gentle fade, cycle colours) [ ] TBD

#include <Adafruit_NeoPixel.h>
#include "LedPatterns.h"


//Hardware specific variables
#define LED_PIN   14
#define N_LEDS    69
#define NUM_BUTTONS 5

#define FRAME_SKIP 3
#define FADE_LOOP 200

//effect config variables
const int DIMMING_FACTOR = 5; //because full brightness is blinding...
const int NUM_CONCURRENT_EFFECTS = NUM_BUTTONS + 1; //max effects: 1 + number of buttons
const int BTN_EFFECT_SPEED = 6;
const int BREATH_EFFECT_SPEED = 1;
int effectStack[NUM_CONCURRENT_EFFECTS];
int effectProgression[NUM_CONCURRENT_EFFECTS];
int effectSpeed[NUM_CONCURRENT_EFFECTS];
bool isEffectActive[NUM_CONCURRENT_EFFECTS];

bool showSensorSpots;
int frameSkipCounter;


//float breathPeriod_s = 5.0; //period of "breathing" effect in seconds
//float kickEffectDuration_s = 2.0;

uint32_t LED_BUFFER[N_LEDS];

bool buttonVals[NUM_BUTTONS];
int x;
Adafruit_NeoPixel strip = Adafruit_NeoPixel(N_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);
LEDEffectType myBackgroundEffect;

void setup()
{
  strip.begin(); //set up neopixels
  Serial.begin(115200);               // 115200 is the default Arduino Bluetooth speed

  //set background colours
  effectStack[0] = LEDEffectType::breath_gray;
  effectStack[1] = LEDEffectType::grow_orange;
  effectStack[2] = LEDEffectType::grow_pink;
  effectStack[3] = LEDEffectType::grow_blue;
  effectStack[4] = LEDEffectType::grow_green;
  effectStack[5] = LEDEffectType::grow_purple;

  //only background effect active at start
  isEffectActive[0] = true;
  for (int i = 0; i < NUM_CONCURRENT_EFFECTS; i++)
  {
    effectProgression[i] = 0;
    effectSpeed[i] = BTN_EFFECT_SPEED;
  }
  effectSpeed[0] = BREATH_EFFECT_SPEED; //breathing is slower
  //clear LED buffer
  for (int i = 0 ; i < N_LEDS; i++)
  {
    LED_BUFFER[i] = 0;
  }
  //set up pins 2-13 as digital in,
  // write a "High" to enable internal pullups
  // NOTE: pin 13 is actually used as an output for LED for debugging,
  // so don't use it as an input pin
  for (int pin = 2; pin <= 13; pin++) {
    pinMode(pin, INPUT);
    digitalWrite(pin, HIGH);
  }
  pinMode(13, OUTPUT);
  digitalWrite(13, HIGH);             ///startup blink
  //  setSensorLocationsRed(true);
  delay(600);
  digitalWrite(13, LOW);
  //  setSensorLocationsRed(false);

  showSensorSpots = false;
  frameSkipCounter = 0;

}



void loop()
{
  if (Serial.available() > 0) {        // Check serial buffer for characters
    char R = Serial.read();

    if (R == 'r') {       // If an 'r' is received then read the pins

      // pins 14-19 on teensy 3.x
      for (int pin = 0; pin <= 5; pin++) {  // Read and send analog pins 0-5
        x = analogRead(pin);
        sendValue (x);
      }
      // pins 2-13 on teensy 3.x
      for (int pin = 2; pin <= 13; pin++) { // Read and send digital pins 2-13
        x = digitalRead(pin);
        sendValue (x);
        if ( (pin < 5) && (pin > 1) ) //keep track of internal values
        {
          buttonVals[pin - 2] = x;
        }
      }

      //TODO: internal buttonVals check, if we want to trigger effects "locally"
      Serial.println();                 // Send a carriage returnt to mark end of pin data.

    }
    if (R == 'A') { //test toggle white on sensor locations
      showSensorSpots = true;
    }
    if (R == 'a') {
      showSensorSpots = false;
    }
    if (R == 'g') {
      effectStack[0] = LEDEffectType::breath_gray;
    }
    if (R == 'G') {
      effectStack[0] = LEDEffectType::breath_green;
    }
    if (R == 'B') {
      effectStack[0] = LEDEffectType::breath_blue;
    }
    if (R == '+') {
      effectSpeed[0]++;
      if (effectSpeed[0] > 10)
        effectSpeed[0] = 5;
    }
    if (R == '-') {
      effectSpeed[0]--;
      if (effectSpeed[0] < 1)
        effectSpeed[0] = 1;
    }
    if (R == '1') {
      isEffectActive[1] = true;
      //Serial.println("starting effect 1");
    }
    if (R == '2') {
      isEffectActive[2] = true;
    }
    if (R == '3') {
      isEffectActive[3] = true;
    }
    if (R == '4') {
      isEffectActive[4] = true;
    }
    if (R == '5') {
      isEffectActive[5] = true;
    }
  }
  frameSkipCounter++;
  if (frameSkipCounter >= FRAME_SKIP) {
    frameSkipCounter = 0;
    updateStrip();
  }
  delay (2);                        // add a delay to prevent crashing/overloading of the serial port
}

void updateStrip()
{
  //0.0: we clear the buffer
  for (int i = 0; i < strip.numPixels(); i++) {
    LED_BUFFER[i] = 0;
  }

  //1. go through all the active effects, and update the LED array.
  //there should always be at least one (background) effect
  for (int i = 0; i < NUM_CONCURRENT_EFFECTS; i++) {

    if (isEffectActive[i]) {

      effectProgression[i] += effectSpeed[i];
      if (effectProgression[i] >= FADE_LOOP)
      {
        effectProgression[i] = 0;
        if (i != 0) //for other effects, we only play once
        {
          isEffectActive[i] = false;
          //Serial.print("stopping effect ");
          //Serial.println(i);
        }
      }

    }


    //1.1
    if (i == 0) { //background effect
      uint32_t col = 0;
      int bri = FADE_LOOP - 2 * abs(effectProgression[i] - FADE_LOOP / 2);
      bri = map (bri, 0, FADE_LOOP, 0, 255) / DIMMING_FACTOR;

      switch (effectStack[i]) {
        case LEDEffectType::breath_blue:
          col = strip.Color(0, 0, bri);
          break;
        case LEDEffectType::breath_green:
          col = strip.Color(0, bri, 0);
          break;
        case LEDEffectType::breath_gray:
          col = strip.Color(bri, bri, bri);
          break;
        case LEDEffectType::breath_white:
          col = strip.Color(bri, bri, bri);
          break; //100-2*abs(x-50)
      }
      for (int i = 0; i < N_LEDS; i++) {
        LED_BUFFER[i] += col;
        for (int j = 0; j < 20; j++) { //TODO: remove these cycle wasters later when we need to do other effects
          LED_BUFFER[i] += 1;
          LED_BUFFER[i] -= 1;
        }
      }


    }
    else {
      //1.1: button effects, i=1 to 5 for the 5 buttons
      if (isEffectActive[i]) {
        uint32_t col;
        switch (effectStack[i]) {
          case LEDEffectType::grow_blue:
            col = strip.Color(0, 0, 255 / DIMMING_FACTOR);
            updateButtonEffect(i, col);
            break;
          case LEDEffectType::grow_green:
            col = strip.Color(0, 255 / DIMMING_FACTOR, 0);
            updateButtonEffect(i, col);
            break;
          case LEDEffectType::grow_pink:
            col = strip.Color(255 / DIMMING_FACTOR, 192 / DIMMING_FACTOR, 203 / DIMMING_FACTOR);
            updateButtonEffect(i, col);
            break;
          case LEDEffectType::grow_orange:
            col = strip.Color(255 / DIMMING_FACTOR, 122 / DIMMING_FACTOR, 0);
            updateButtonEffect(i, col);
            break;
          case LEDEffectType::grow_purple:
            col = strip.Color(255 / DIMMING_FACTOR, 40 / DIMMING_FACTOR, 240 / DIMMING_FACTOR);
            updateButtonEffect(i, col);
            break;
        }
        if (isEffectActive[i]) {
          updateButtonEffect(i, col);
        }
      }

    }

    setSensorLocationsRed(showSensorSpots); //NOTE: this is a "overwriting" and not "mixing" setting


    //finally, update the strip from the internal buffer:
    for (int i = 0; i < strip.numPixels(); i++) {
      //TODO: replace with updating of internal buffer
      strip.setPixelColor(i, LED_BUFFER[i]);
    }
    strip.show();



  }
  //next,
}

void sendValue (int x) {             // function to send the pin value followed by a "space".
  Serial.print(x);
  Serial.write(32);
}

static void chaseNoDelay(uint32_t c) {

}

static void chase(uint32_t c) {
  for (uint16_t i = 0; i < strip.numPixels() + 4; i++) {
    strip.setPixelColor(i  , c); // Draw new pixel
    strip.setPixelColor(i - 4, 0); // Erase pixel a few steps back
    strip.show();
    delay(25);
  }
}

static void startEffect(int effectNum)
{
  if ( (effectNum < 1) || (effectNum > NUM_CONCURRENT_EFFECTS) )
  {
    return;
  }
  isEffectActive[effectNum] = true;

}

void updateButtonEffect(int btnNumber, uint32_t c)
{ //kinda ugly hard coded effects...
  switch (btnNumber) {
    case 1:
      if (effectProgression[btnNumber] < FADE_LOOP / 2)
      {
        for (int i = 0; i < EFFECT_7_1_size; i++)
        {
          LED_BUFFER[EFFECT_7_1[i]] = c;
        }
      }
      else {
        for (int i = 0; i < EFFECT_7_2_size; i++)
        {
          LED_BUFFER[EFFECT_7_2[i]] = c;
        }
      }
      break;
    case 2:
      if (effectProgression[btnNumber] < FADE_LOOP / 2)
      {
        for (int i = 0; i < EFFECT_18_1_size; i++)
        {
          LED_BUFFER[EFFECT_18_1[i]] = c;
        }
      }
      else {
        for (int i = 0; i < EFFECT_18_2_size; i++)
        {
          LED_BUFFER[EFFECT_18_2[i]] = c;
        }
      }
      break;
    case 3:
      if (effectProgression[btnNumber] < FADE_LOOP / 2)
      {
        for (int i = 0; i < EFFECT_27_1_size; i++)
        {
          LED_BUFFER[EFFECT_27_1[i]] = c;
        }
      }
      else {
        for (int i = 0; i < EFFECT_18_2_size; i++)
        {
          LED_BUFFER[EFFECT_27_2[i]] = c;
        }
      }
      break;
    case 4:
      if (effectProgression[btnNumber] < FADE_LOOP / 2)
      {
        for (int i = 0; i < EFFECT_40_1_size; i++)
        {
          LED_BUFFER[EFFECT_40_1[i]] = c;
        }
      }
      else {
        for (int i = 0; i < EFFECT_40_2_size; i++)
        {
          LED_BUFFER[EFFECT_40_2[i]] = c;
        }
      }
      break;
    case 5:
      if (effectProgression[btnNumber] < FADE_LOOP / 2)
      {
        for (int i = 0; i < EFFECT_53_1_size; i++)
        {
          LED_BUFFER[EFFECT_53_1[i]] = c;
        }
      }
      else {
        for (int i = 0; i < EFFECT_53_2_size; i++)
        {
          LED_BUFFER[EFFECT_53_2[i]] = c;
        }
      }
      break;
  }
}

static void setSensorLocationsRed(bool isOn) {
  for (uint16_t i = 0; i < NUM_SENSORS; i++) {
    if (isOn)
      LED_BUFFER[SENSOR_SPOTS[i]] = strip.Color(255, 0, 0);
    //else
    //LED_BUFFER[SENSOR_SPOTS[i]] = strip.Color(0, 0, 0);
  }
}
