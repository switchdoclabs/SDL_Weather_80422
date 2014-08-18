/*
  SDL_Weather_80422.cpp - Library for Weather Sensor Assembly 80422.
  SparkFun Weather Station Meters - https://www.sparkfun.com/products/8942
  Imported by Argent Data Systems
  Created by SwitchDoc Labs July 27, 2014.
  Released into the public domain.
*/

#include "Arduino.h"

#include <Time.h>
#include "SDL_Weather_80422.h"



//#define NO_PORTC_PINCHANGES // to indicate that port c will not be used for pin change interrupts
//#define NO_PORTD_PINCHANGES // to indicate that port d will not be used for pin change interrupts
//
//#include "PinChangeInt.h"

// private functions


boolean fuzzyCompare(float compareValue, float value)
{
  #define VARYVALUE 0.05
  
   if ( (value > (compareValue * (1.0-VARYVALUE)))  && (value < (compareValue *(1.0+VARYVALUE))) )
   {
     
     return true;
     
   }
   
   return false;
  
  
  
}

float voltageToDegrees(float value, float defaultWindDirection)
{
  
  // Note:  The documenation for the wind vane says 16 positions.  Only recieve 8 positions.  And 370 degrees is wrong.
  
  if (fuzzyCompare(3.84, value))
      return 0.0;

  if (fuzzyCompare(1.98, value))
      return 22.5;

  if (fuzzyCompare(2.25, value))
      return 45;

  if (fuzzyCompare(0.41, value))
      return 67.5;

  if (fuzzyCompare(0.45, value))
      return 90.0;

  if (fuzzyCompare(0.32, value))
      return 112.5;

  if (fuzzyCompare(0.90, value))
      return 135.0;

  if (fuzzyCompare(0.62, value))
      return 157.5;

  if (fuzzyCompare(1.40, value))
      return 180;

  if (fuzzyCompare(1.19, value))
      return 202.5;

  if (fuzzyCompare(3.08, value))
      return 225;

  if (fuzzyCompare(2.93, value))
      return 247.5;

  if (fuzzyCompare(4.62, value))
      return 270.0;

  if (fuzzyCompare(4.04, value))
      return 292.5;

  if (fuzzyCompare(4.34, value))  // chart in documentation wrong
      return 315.0;

  if (fuzzyCompare(3.43, value))
      return 337.5;
      
  Serial.print(" FAIL WIND DIRECTION");
  return defaultWindDirection;  // return previous value if not found
  
  
}



// Interrupt routines
// for mega, have to use Port B - only Port B works.
/*
Interrupt 4 - Pin 19
Interrupt 5 - Pin 18
 
*/




unsigned long lastWindTime;


void serviceInterruptAnem()
{
  unsigned long currentTime= (unsigned long)(micros()-lastWindTime);

  lastWindTime=micros();
  if(currentTime>1000)   // debounce
  {
     SDL_Weather_80422::_currentWindCount++;
    if(currentTime<SDL_Weather_80422::_shortestWindTime)
    {
     SDL_Weather_80422::_shortestWindTime=currentTime;
    }
 
  }
 

  
}


unsigned long currentRainMin;
unsigned long lastRainTime;

void serviceInterruptRain()
{
  unsigned long currentTime=(unsigned long) (micros()-lastRainTime);

  lastRainTime=micros();
  if(currentTime>500)   // debounce
  {
       SDL_Weather_80422::_currentRainCount++;
//      interrupt_count[19]++;
    if(currentTime<currentRainMin)
    {
     currentRainMin=currentTime;
    }
 
  }

  
//  interrupt_count[18]++;

  
  
}

long SDL_Weather_80422::_currentWindCount =0;
long SDL_Weather_80422::_currentRainCount =0;
unsigned long SDL_Weather_80422::_shortestWindTime =0;
SDL_Weather_80422::SDL_Weather_80422(int pinAnem, int pinRain, int intAnem, int intRain, int ADChannel, int ADMode)
{
  //pinMode(pinAnem, INPUT);
  //pinMode(pinRain, INPUT);
  _pinAnem = pinAnem;
  _pinRain = pinRain;
  _intAnem = intAnem;
  _intRain = intRain;
  _ADChannel = ADChannel;
  _ADMode = ADMode;

  _currentRainCount = 0;
  _currentWindCount = 0;
  _currentWindSpeed = 0.0;
  _currentWindDirection = 0.0;

  lastWindTime = 0;
  _shortestWindTime = 0xffffffff;
  
  _sampleTime = 5.0;
  _selectedMode = SDL_MODE_SAMPLE;
  
  _startSampleTime = micros();
   
   // set up interrupts


  
  pinMode(pinAnem, INPUT);	   // pinAnem is input to which a switch is connected
  digitalWrite(pinAnem, HIGH);   // Configure internal pull-up resistor
  pinMode(pinRain, INPUT);	   // pinRain is input to which a switch is connected
  digitalWrite(pinRain, HIGH);   // Configure internal pull-up resistor
  attachInterrupt(_intAnem, serviceInterruptAnem, RISING);
  attachInterrupt(_intRain, serviceInterruptRain, RISING);
}




float SDL_Weather_80422::get_current_rain_total()
{
        float rain_amount = 0.2794 * _currentRainCount/2;  // mm of rain - we get two interrupts per bucket
        _currentRainCount = 0;
	return rain_amount;
}

#define WIND_FACTOR 2.400 
float SDL_Weather_80422::current_wind_speed() // in milliseconds
{
  
  if (_selectedMode == SDL_MODE_SAMPLE)
  {
    _currentWindSpeed = get_current_wind_speed_when_sampling();
  }
  else
  {
    // km/h * 1000 msec
    
    _currentWindCount = 0;
    delay(_sampleTime*1000);
    _currentWindSpeed = ((float)_currentWindCount/_sampleTime) * WIND_FACTOR;
   }
  return _currentWindSpeed;
}

float SDL_Weather_80422::get_wind_gust()
{
   
   
  unsigned long latestTime;
  latestTime =_shortestWindTime;
  _shortestWindTime=0xffffffff;
  double time=latestTime/1000000.0;  // in microseconds

  return (1/(time))*WIND_FACTOR/2;

}

float SDL_Weather_80422::current_wind_direction()
{
    float voltageValue;
    
    float Vcc = 5.00;

    if (_ADMode == SDL_MODE_I2C_ADS105)
    {

      voltageValue = 0.0;
    }
    else
    {
      // use internal A/D converter

      voltageValue = (analogRead(_ADChannel)/1023.0)*Vcc;
  
            
    }
    
    float direction = voltageToDegrees(voltageValue, _currentWindDirection);
    
    return direction;
}

void SDL_Weather_80422::reset_rain_total()
{
	_currentRainCount = 0;
}

float SDL_Weather_80422::accessInternalCurrentWindDirection()
{
   return _currentWindDirection;
}

void SDL_Weather_80422::reset_wind_gust()
{
   _shortestWindTime = 0xffffffff;
}

// ongoing samples in wind

void SDL_Weather_80422::startWindSample(float sampleTime)
{
  
      _startSampleTime = micros();
     
      _sampleTime = sampleTime;
      
}
    


float SDL_Weather_80422::get_current_wind_speed_when_sampling()
{

   unsigned long compareValue;
   compareValue = _sampleTime*1000000;
  
   if (micros() - _startSampleTime >= compareValue)
    {
      // sample time exceeded, calculate currentWindSpeed
      float _timeSpan;
 //     _timeSpan = (unsigned long)(micros() - _startSampleTime);
      _timeSpan = (micros() - _startSampleTime);
 
      _currentWindSpeed = ((float)_currentWindCount/(_timeSpan)) * WIND_FACTOR*1000000;

      _currentWindCount = 0;
      
      _startSampleTime = micros();


      }
      else
      {
        //Serial.println("NOT Triggered");
        //Serial.print("time = ");
        //Serial.println(micros() - _startSampleTime);
        // if not, then return last wind speed
  
      }

  
  
    return _currentWindSpeed;
}

void SDL_Weather_80422::setWindMode(int selectedMode, float sampleTime)  // time in seconds
{
  
  _sampleTime = sampleTime;
  _selectedMode = selectedMode;
  
  if (_selectedMode == SDL_MODE_SAMPLE)
  {
    startWindSample(_sampleTime);
  }
  

  
}
