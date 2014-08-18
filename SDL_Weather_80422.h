/*
  SDL_Weather_80422.h - Library for Weather Sensor Assembly 80422.
  Imported by Argent Data Systems
  SparkFun Weather Station Meters - https://www.sparkfun.com/products/8942
  Created by SwitchDoc Labs July 27, 2014.
  Released into the public domain.
*/
#ifndef SDL_Weather_80422_h
#define SDL_Weather_80422_h

#define SDL_MODE_INTERNAL_AD 0
#define SDL_MODE_I2C_ADS105 1

// sample mode means return immediately.  THe wind speed is averaged at sampleTime or when you ask, whichever is longer
#define SDL_MODE_SAMPLE 0
// Delay mode means to wait for sampleTime and the average after that time.
#define SDL_MODE_DELAY 1


#include "Arduino.h"
extern "C" void serviceInterruptAnem(void)  __attribute__ ((signal));
extern "C" void serviceInterruptRain(void)  __attribute__ ((signal));
class SDL_Weather_80422
{
  public:
    SDL_Weather_80422(int pinAnem, int pinRain, int intAnem, int intRain, int ADChannel, int ADMode);
  
    float get_current_rain_total();
    float current_wind_speed();
    float current_wind_direction();
    float get_wind_gust();
    void reset_rain_total();
    void reset_wind_gust();
  
    void setWindMode(int selectedMode, float sampleTime);  // time in seconds
    

    static unsigned long _shortestWindTime;
    static long _currentRainCount;
    static long _currentWindCount;
     
    float accessInternalCurrentWindDirection();

  friend void serviceInterruptAnem();
  friend void serviceInterruptRain(); 
  
  private:
  

    int _pinAnem;
    int _pinRain;    
    int _intAnem;
    int _intRain;
    int _ADChannel;
    int _ADMode;
    float _sampleTime;
    int _selectedMode;
    
    unsigned long _startSampleTime;
    

    float _currentWindSpeed;

    float _currentWindDirection;
    
    void startWindSample(float sampleTime);
    float get_current_wind_speed_when_sampling();
};

#endif

