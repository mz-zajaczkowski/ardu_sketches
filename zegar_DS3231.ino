
#include <FastLED.h>
#include <DS3231.h>
#include <Wire.h>

#define DATA_PIN 3

#define NUM_LEDS 8
CRGB leds[NUM_LEDS];

DS3231 clock;
RTCDateTime dt;
static bool start = false;
void setup()
{
  clock.begin();
 
  // Ustawiamy date i godzine kompilacji szkicu
  if(!clock.isReady())
    clock.setDateTime(__DATE__, __TIME__);
  Serial.begin(9600);
  Serial.print(clock.getDateTime().hour);
  Serial.print(clock.getDateTime().minute);
//  clock.setBattery(true, true);
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
  clearAlarms();
  testAlarm();
}
 
void loop()
{
  Serial.print("second"); Serial.print(clock.getDateTime().second);Serial.print("\n");
  if(clock.isAlarm2())
  {
    Serial.print("alarm 2\n");
    start = true;
//    sunrise();
  }
   if(start) sunrise();

    FastLED.show();
    delay(10);
}

void sunrise() 
{
  Serial.print("sunrise\n");
  static uint8_t day = clock.getDateTime().dayOfWeek;
  
  // total sunrise length, in minutes
  static const uint8_t sunriseLength = 15;

  // how often (in seconds) should the heat color increase?
  // for the default of 30 minutes, this should be about every 7 seconds
  // 7 seconds x 256 gradient steps = 1,792 seconds = ~30 minutes
  static const float interval = ((float)(sunriseLength * 60) / 256)*1000;

  // current gradient palette color index
  static uint8_t heatIndex = 0; // start out at 0

  // HeatColors_p is a gradient palette built in to FastLED
  // that fades from black to red, orange, yellow, white
  // feel free to use another palette or define your own custom one
  CRGB color = ColorFromPalette(HeatColors_p, heatIndex);

  // fill the entire strip with the current color
  fill_solid(leds, NUM_LEDS, color);
  EVERY_N_MILLISECONDS(interval)
  {
//    Serial.print("alarm 1\n");
    day = (day < 8) ? ++day : 1;
    // stop incrementing at 255, we don't want to overflow back to 0
    if(heatIndex < 255) heatIndex++;
    else
    {
//      Serial.print("set alarm 2 for next day");Serial.print(day);
      //setAlarm2 for next day and reset heatIndex
      clock.setAlarm2(day, 6, 45, DS3231_MATCH_DY_H_M);
      heatIndex = 0;
      start = false;
      fill_solid(leds, NUM_LEDS, CRGB::Black);
    }
  }
}

void clearAlarms()
{  
  clock.armAlarm1(false);
  clock.armAlarm2(false);
  clock.clearAlarm1();
  clock.clearAlarm2();
}

void testAlarm()
{
  RTCDateTime now = clock.getDateTime();
    if(now.minute == 59)
    {
        now.minute = 0;
        if(now.hour == 23)
        {
            now.hour = 0;
            if(now.dayOfWeek == 7)
                now.dayOfWeek = 1;
//            else now.dayOfWeek++;
        }
//        else
//            now.hour++;
    }
    else
        now.minute++;
    clock.setAlarm2(now.dayOfWeek, now.hour, now.minute, DS3231_MATCH_DY_H_M);
}
