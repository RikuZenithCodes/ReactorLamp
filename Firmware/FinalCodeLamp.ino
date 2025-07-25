#include <Adafruit_NeoPixel.h>
#include <LiquidCrystal_I2C.h>

#define NEOPIXEL_PIN 6
#define NUMPIXELS 16

#define BUTTON_POWER 2
#define BUTTON_COLOR 4
#define POT_PIN A0

Adafruit_NeoPixel ring(NUMPIXELS, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);
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
  ring.show();

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

  // Update LED ring
  if (reactorOn) {
    ring.setBrightness(brightness);
    for (int i = 0; i < NUMPIXELS; i++) {
      ring.setPixelColor(i, colors[colorIndex]);
    }
    ring.show();

    // Update brightness percentage display
    lcd.setCursor(0, 1);
    lcd.print("Brightness: ");
    lcd.print(brightnessPercent);
    lcd.print("%   "); // extra spaces to clear previous chars
  } else {
    ring.clear();
    ring.show();
    // Clear second row to avoid leftover text
    lcd.setCursor(0, 1);
    lcd.print("                ");  // 16 spaces to clear line
  }

  delay(10);
}