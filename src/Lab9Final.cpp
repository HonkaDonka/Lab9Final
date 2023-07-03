/******************************************************/
//       THIS IS A GENERATED FILE - DO NOT EDIT       //
/******************************************************/

#include "Particle.h"
#line 1 "c:/Users/vinso/Documents/CTDIoT2023/Lab9Final/src/Lab9Final.ino"
#include "env.h"
#include "Blynk.h"
#include "oled-wing-adafruit.h"
#include "SparkFun_VCNL4040_Arduino_Library.h"

void setup();
void loop();
void resetDisplay();
void resetLEDs();
#line 6 "c:/Users/vinso/Documents/CTDIoT2023/Lab9Final/src/Lab9Final.ino"
#define FAHRENHEITPIN V0
#define CELSIUSPIN V3
#define LIGHTPIN V1
#define PANICPIN V2
#define TEMPPIN A1
#define PENTPIN A0
#define GREENPIN D7
#define BLUEPIN D6
#define ORANGEPIN D5
#define BUZZERPIN A5
#define BUTTONPIN D8
#define TRANSITIONNOTIF "lightnotif"

SYSTEM_THREAD(ENABLED);

bool showtemp = false;
bool previousState = false;
int transition1 = 0;
int transition2 = 0;
int setState = true;

OledWingAdafruit display;
VCNL4040 VCNL;

void setup()
{
  Serial.begin(9600);
  Wire.begin();

  VCNL.begin();
  VCNL.powerOnAmbient();
  VCNL.powerOffProximity();

  display.setup();
  display.clearDisplay();
  display.display();
  display.setTextSize(1);
  display.setTextColor(WHITE);

  Blynk.begin(BLYNK_AUTH_TOKEN);

  pinMode(TEMPPIN, INPUT);
  pinMode(PENTPIN, INPUT);
  pinMode(GREENPIN, OUTPUT);
  pinMode(BLUEPIN, OUTPUT);
  pinMode(ORANGEPIN, OUTPUT);
  pinMode(BUZZERPIN, OUTPUT);
  pinMode(BUTTONPIN, INPUT);
}

void loop()
{
  Blynk.run();
  display.loop();

  if (display.pressedA())
  {
    showtemp = false;
  }
  else if (display.pressedB())
  {
    showtemp = true;
  }

  uint16_t reading = analogRead(TEMPPIN);
  double voltage = (reading * 3.3) / 4095.0;
  double Ctemp = (voltage - 0.5) * 100;
  double Ftemp = (Ctemp * 9 / 5) + 32;

  Blynk.virtualWrite(FAHRENHEITPIN, Ftemp);
  Blynk.virtualWrite(CELSIUSPIN, Ctemp);

  uint16_t ambVal = VCNL.getAmbient();

  Blynk.virtualWrite(LIGHTPIN, ambVal);

  if (ambVal <= min(transition1, transition2))
  {
    resetLEDs();
    digitalWrite(GREENPIN, HIGH);
  }
  else if ((min(transition1, transition2) <= ambVal) && (ambVal <= max(transition1, transition2)))
  {
    resetLEDs();
    digitalWrite(BLUEPIN, HIGH);
  }
  else
  {
    resetLEDs();
    digitalWrite(ORANGEPIN, HIGH);
  }

  if ((((transition1 - 500) <= ambVal && ambVal <= (transition1 + 500)) || ((transition2 - 500) <= ambVal && ambVal <= (transition2 + 500))) && showtemp)
  {
    Blynk.logEvent(TRANSITIONNOTIF);
  } 

  if (!showtemp)
  {
    if (digitalRead(BUTTONPIN) && !previousState)
    {
      setState = !setState;
      previousState = true;
    }
    else if (!digitalRead(BUTTONPIN))
    {
      previousState = false;
    }

    if (setState)
    {
      transition1 = map(analogRead(PENTPIN), 0, 4095, 0, 65535);
    }
    else
    {
      transition2 = map(analogRead(PENTPIN), 0, 4095, 0, 65535);
    }

    resetDisplay();
    display.print("POINT 1: ");
    display.println(transition1);
    display.print("POINT 2: ");
    display.println(transition2);
    display.display();
  }
  else if (showtemp)
  {
    resetDisplay();
    display.print("FAHRENHEIT: ");
    display.println(Ftemp);
    display.print("CELSIUS: ");
    display.println(Ctemp);
    display.display();
  }
}

BLYNK_WRITE(PANICPIN) {
  if (param.asInt()) {
    digitalWrite(BUZZERPIN, HIGH);
  } else {
    digitalWrite(BUZZERPIN, LOW);
  }
}

void resetDisplay()
{
  display.clearDisplay();
  display.setCursor(0, 0);
}

void resetLEDs()
{
  digitalWrite(GREENPIN, LOW);
  digitalWrite(BLUEPIN, LOW);
  digitalWrite(ORANGEPIN, LOW);
}
