/* 
This code will control a NeoPixel-style addressable LED strip by pressing a button to select preconfigured effects. Wire the strip as follow:

RED -> Arduino (or power) 5 Volts
GREEN -> Arduino Pin 6 (configurable below)
WHITE -> Arduino (or power) Ground

If your LED strip is longer than about 1 foot, you will need to power your LEDs using the power adapter. The Arduino is not powerful enough to power multiple LEDs.
NOTE THAT THE LEDs HAVE POLARITY AND MUST HAVE THEIR DATA CONNECTED TO THE MALE CABLE (or Din on the strip itself)

Find a virtual version of this project (and see wiring information) here: https://www.tinkercad.com/things/9MeaKMZq6zi-button-activated-led-strip
*/

#include <Adafruit_NeoPixel.h> // You will need to ensure that the Adafruit NeoPixel library is installed via the Arduino IDE

// ---------------- SETTINGS ----------------
#define PIN 6          // Arduino pin connected to the NeoPixel strip
#define NUMPIXELS 300  // Number of NeoPixels in the strip. Set to 300 for a full length
#define BUTTON_PIN 2   // Pin connected to the button. Wire other side of button to Arduino ground

// Create the NeoPixel strip object.
Adafruit_NeoPixel strip(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

// Variables for mode handling
int mode = 0;            // Current lighting mode (0–10)
const int maxMode = 10;  // Maximum mode number

// ---------------- SETUP ----------------
void setup() {
  pinMode(BUTTON_PIN, INPUT_PULLUP);  // Button uses internal pull-up resistor
  strip.begin();                      // Start the NeoPixel strip
  strip.show();                       // Turn all pixels off at startup
  Serial.begin(9600);                 // Start Serial Monitor for debug messages
  Serial.println("Starting NeoPixel controller...");
  printMode(mode);  // Show initial mode (OFF)
}

// ---------------- MAIN LOOP ----------------
void loop() {
  handleButton();  // Check if button was pressed and update mode
  runMode();       // Run the current mode effect
}

// ---------------- BUTTON HANDLING ----------------
void handleButton() {
  static bool buttonPressed = false;      // Keeps track of button state
  int reading = digitalRead(BUTTON_PIN);  // Read the state of the button on BUTTON_PIN

  // Button is pressed (LOW because of INPUT_PULLUP)
  if (reading == LOW && !buttonPressed) {
    buttonPressed = true;          // Remember the button is pressed
    mode++;                        // Go to the next mode
    if (mode > maxMode) mode = 0;  // Wrap around to 0 if past max
    printMode(mode);               // Print new mode to Serial Monitor
    delay(200);                    // Simple debounce: wait before next press
  }

  // Reset button state when released
  if (reading == HIGH) {
    buttonPressed = false;
  }
}

// ---------------- MODE SELECTION ----------------
void runMode() {
  switch (mode) {
    case 0:  // Mode 0 = OFF
      strip.clear();
      strip.show();
      break;
    case 1: fillColor(strip.Color(255, 0, 0)); break;             // Red
    case 2: fillColor(strip.Color(0, 255, 0)); break;             // Green
    case 3: fillColor(strip.Color(0, 0, 255)); break;             // Blue
    case 4: fadeEffect(); break;                                  // Fade
    case 5: flameEffect(); break;                                 // Flame
    case 6: rainbowEffect(); break;                               // Rainbow
    case 7: discoEffect(); break;                                 // Random Disco Colors
    case 8: fireworkEffect(); break;                              // Firework effect
    case 9: theaterChase(strip.Color(127, 127, 127), 50); break;  //Marquee lights
    case 10: colorWipe(); break;                                  //Fill strip with color
  }
}

// ---------------- PRINT MODE NAME ----------------
void printMode(int m) {
  Serial.print("Mode ");
  Serial.print(m);
  Serial.print(": ");
  switch (m) {
    case 0: Serial.println("OFF"); break;
    case 1: Serial.println("All Red"); break;
    case 2: Serial.println("All Green"); break;
    case 3: Serial.println("All Blue"); break;
    case 4: Serial.println("Fade Effect"); break;
    case 5: Serial.println("Flame Effect"); break;
    case 6: Serial.println("Rainbow Effect"); break;
    case 7: Serial.println("Disco Effect"); break;
    case 8: Serial.println("Firework Effect"); break;
    case 9: Serial.println("Theater Chase"); break;
    case 10: Serial.println("Color Wipe"); break;
  }
}

// ---------------- EFFECT FUNCTIONS ----------------

// Fill all pixels with the same color
void fillColor(uint32_t c) {
  for (int i = 0; i < NUMPIXELS; i++) {
    strip.setPixelColor(i, c);
  }
  strip.show();
}

// Fade effect (smoothly changes brightness up and down)
void fadeEffect() {
  static int brightness = 0;  // Current brightness (0–255)
  static int fadeAmount = 5;  // How much to change brightness each step

  brightness += fadeAmount;
  if (brightness <= 0 || brightness >= 255) fadeAmount = -fadeAmount;

  fillColor(strip.Color(brightness, 0, brightness));  // Purple fading in/out
  delay(20);
}

// Flame effect (random flickering orange)
void flameEffect() {
  for (int i = 0; i < NUMPIXELS; i++) {
    int flicker = random(160, 255);  // Random brightness for realism
    // More red, less green, no blue → looks orange
    strip.setPixelColor(i, strip.Color(flicker, (int)(flicker * 0.2), 0));
  }
  strip.show();
  delay(50);
}

// Rainbow effect (colors cycle smoothly across strip)
void rainbowEffect() {
  static uint16_t j = 0;
  for (int i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, Wheel((i + j) & 255));
  }
  strip.show();
  j++;
  delay(20);
}

// Flashy random disco effect
void discoEffect() {
  for (int i = 0; i < NUMPIXELS; i++) {
    int r = random(0, 256);
    int g = random(0, 256);
    int b = random(0, 256);
    strip.setPixelColor(i, strip.Color(r, g, b));
  }
  strip.show();
  delay(50);  // speed of the disco effect
}

// Theater chase effect (moving dots like Marquee lights)
void theaterChase(uint32_t c, uint8_t wait) {
  static int q = 0;
  strip.clear();
  for (int i = 0; i < strip.numPixels(); i += 3) {
    strip.setPixelColor(i + q, c);
  }
  strip.show();
  q++;
  if (q >= 3) q = 0;
  delay(wait);
}

// Firework effect: bursts of color that fade out
void fireworkEffect() {
  static int step = 0;
  static int center = 0;
  static int r = 0, g = 0, b = 0;

  if (step == 0) {
    // Pick a random firework start
    center = random(0, NUMPIXELS);
    r = random(50, 256);
    g = random(50, 256);
    b = random(50, 256);
    step = 10;  // smaller step count = faster bursts
  }

  // Fade out previous pixels slightly (trail effect)
  for (int i = 0; i < NUMPIXELS; i++) {
    uint32_t c = strip.getPixelColor(i);
    int red = (c >> 16) & 0xFF;
    int green = (c >> 8) & 0xFF;
    int blue = c & 0xFF;
    strip.setPixelColor(i, strip.Color(red * 0.7, green * 0.7, blue * 0.7));
  }

  // Draw current "burst"
  int radius = (10 - step);
  for (int i = -radius; i <= radius; i++) {
    int pos = center + i;
    if (pos >= 0 && pos < NUMPIXELS) {
      int fade = max(0, 255 - abs(i) * 60);  // sharper falloff
      strip.setPixelColor(pos, strip.Color(r * fade / 255, g * fade / 255, b * fade / 255));
    }
  }

  strip.show();
  delay(40);  // refresh

  step--;

  // When finished, immediately start a new firework
  if (step <= 0) {
    step = 0;  // reset for new firework
  }
}

// Color wipe effect (lights up one by one, cycles through different colors)
void colorWipe() {
  static int i = 0;          // current pixel
  static int colorStep = 0;  // which color we're using

  // Define colors to cycle through
  int colors[5][3] = {
    { 255, 0, 0 },    // red
    { 0, 255, 0 },    // green
    { 0, 0, 255 },    // blue
    { 255, 255, 0 },  // yellow
    { 0, 255, 255 }   // cyan
  };

  // Set pixel to current color
  strip.setPixelColor(i, strip.Color(colors[colorStep][0], colors[colorStep][1], colors[colorStep][2]));
  strip.show();

  i++;  // move to next pixel
  if (i >= NUMPIXELS) {
    i = 0;        // reset pixel counter
    colorStep++;  // next color
  }

  delay(50);  // controls speed of wipe
}

// Helper function: converts a number (0–255) into a rainbow color
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if (WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if (WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}
