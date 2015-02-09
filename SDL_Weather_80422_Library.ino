/*
  SDL_Weather_80422.cpp - Library for SwitchDoc Labs WeatherRack.
  SparkFun Weather Station Meters
  Argent Data Systems
  Created by SwitchDoc Labs July 27, 2014.
  Released into the public domain.
  Version 1.1 - updated constants to suppport 3.3V
  
  Supports WeatherPiArduino Board www.switchdoc.com
*/
#include <Wire.h>
#include <Time.h>

#include "SDL_Weather_80422.h"



#define pinLED     13   // LED connected to digital pin 13
#define pinAnem    18  // Anenometer connected to pin 18 - Int 5 - Mega   / Uno pin 2
#define pinRain    2  // Anenometer connected to pin 2 - Int 0 - Mega   / Uno Pin 3 
#define intAnem    5  // int 0 (check for Uno)
#define intRain    1  // int 1

// for mega, have to use Port B - only Port B works.
/*
 Arduino Pins         PORT
 ------------         ----
 Digital 0-7          D
 Digital 8-13         B
 Analog  0-5          C
*/


// initialize SDL_Weather_80422 library
SDL_Weather_80422 weatherStation(pinAnem, pinRain, intAnem, intRain, A0, SDL_MODE_INTERNAL_AD);


uint8_t i;




float currentWindSpeed;
float currentWindGust;
float totalRain;
void setup()
{ 
  Serial.begin(57600);  
  

 Serial.println("-----------");
 Serial.println("WeatherArduino SDL_Weather_80422 Class Test");
 Serial.println("Version 1.1");
 Serial.println("-----------");
      
      
      weatherStation.setWindMode(SDL_MODE_SAMPLE, 5.0);
      //weatherStation.setWindMode(SDL_MODE_DELAY, 5.0);
      totalRain = 0.0;
}


void loop()
{
  Serial.println("----------------");

  currentWindSpeed = weatherStation.current_wind_speed()/1.6;
  currentWindGust = weatherStation.get_wind_gust()/1.6;
  totalRain = totalRain + weatherStation.get_current_rain_total()/25.4;
  Serial.print("rain_total=");
  Serial.print(totalRain);
  Serial.print(""" wind_speed=");
  Serial.print(currentWindSpeed);
    Serial.print("MPH wind_gust=");
  Serial.print(currentWindGust);
  Serial.print("MPH wind_direction=");
  Serial.println(weatherStation.current_wind_direction());
  

  
  delay(1000);
  
  
}

