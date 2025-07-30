#include <Adafruit_NeoPixel.h>
#include <LiquidCrystal_I2C.h>

#define NEOPIXEL_PIN 6
#define SINGLE_LED_PIN 5
#define NUMPIXELS 16
#define BUTTON_POWER 2
#define BUTTON_COLOR 4
#define POT_PIN A0

Adafruit_NeoPixel ring(NUMPIXELS, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel singleLED(1, SINGLE_LED_PIN, NEO_GRB + NEO_KHZ800); // NEW

LiquidCrystal_I2C lcd(32, 16, 2);

int colorIndex = 0;
bool reactorOn = false;

bool lastPowerButton = HIGH;
bool lastColorButton = HIGH;

unsigned long debounceDelay = 50;
unsigned long lastPowerDebounce = 0;
unsigned long lastColorDebounce = 0;

const uint32_t colors[] = {
  ring.Color(255, 0, 0),     // Red
  ring.Color(0, 255, 0),     // Green
  ring.Color(0, 0, 255),     // Blue
  ring.Color(255, 255, 0),   // Yellow
  ring.Color(255, 0, 255),   // Magenta
  ring.Color(0, 255, 255),   // Cyan
  ring.Color(255, 255, 255)  // White
};
const int numColors = sizeof(colors) / sizeof(colors[0]);

void setup() {
  pinMode(BUTTON_POWER, INPUT_PULLUP);
  pinMode(BUTTON_COLOR, INPUT_PULLUP);

  ring.begin();
  singleLED.begin(); // NEW
  ring.show();
  singleLED.show();  // NEW

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Reactor OFF");
}

void loop() {
  bool powerButton = digitalRead(BUTTON_POWER);
  bool colorButton = digitalRead(BUTTON_COLOR);
  int potVal = analogRead(POT_PIN);
  int brightness = map(potVal, 0, 1023, 0, 255);
  int brightnessPercent = map(potVal, 0, 1023, 0, 100);

  // Handle power button
  if (powerButton == LOW && lastPowerButton == HIGH && (millis() - lastPowerDebounce) > debounceDelay) {
    reactorOn = !reactorOn;
    lastPowerDebounce = millis();

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(reactorOn ? "Reactor ON " : "Reactor OFF");
  }
  lastPowerButton = powerButton;

  // Handle color button
  if (colorButton == LOW && lastColorButton == HIGH && (millis() - lastColorDebounce) > debounceDelay) {
    if (reactorOn) {
      colorIndex = (colorIndex + 1) % numColors;
      lastColorDebounce = millis();
    }
  }
  lastColorButton = colorButton;

  // Update LEDs
  if (reactorOn) {
    ring.setBrightness(brightness);
    singleLED.setBrightness(brightness); // NEW

    for (int i = 0; i < NUMPIXELS; i++) {
      ring.setPixelColor(i, colors[colorIndex]);
    }
    singleLED.setPixelColor(0, colors[colorIndex]); // NEW

    ring.show();
    singleLED.show(); // NEW

    // Update LCD brightness
    lcd.setCursor(0, 1);
    lcd.print("Brightness: ");
    lcd.print(brightnessPercent);
    lcd.print("%   ");
  } else {
    ring.clear();
    ring.show();

    singleLED.clear();     // NEW
    singleLED.show();      // NEW

    lcd.setCursor(0, 1);
    lcd.print("                ");
  }

  delay(10);
}