// thermostat.h

#ifndef _THERMOSTAT_h
#define _THERMOSTAT_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

//class ThermostatClass
class Thermostat
{
#define HEATON 1
#define HEATOFF 0
#define MINUTE 60*1000 // min in mils

 protected:
	 double currTemp;
	 int TempSP; // temp setpoint
	 int state;
	 int ctlpin;
	 long MinRT; // min run / off time
	 long Cmils;
	 long Pmils;
	 void toggleState();

 public:
	Thermostat();
	Thermostat(int pin);
	void Setpoint(int temp);
	int  getTempSP();
	void RunTime(int R_mins);
	int  getRuntime();
	void SendCurTemp(double temp);
	int  getState();
};

// extern ThermostatClass Thermostat;

#endif

