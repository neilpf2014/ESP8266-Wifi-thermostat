// Class for thermostat
// Simple thermostat functions assume sending in degree F
// 

#include "thermostat.h"

// constuctor demo mode
Thermostat::Thermostat()
{
	ctlpin = -1;
	TempSP = 68;
	currTemp = 69.0;
	state = HEATOFF;
	MinRT = 5 * MINUTE;
	Cmils = millis();
	Pmils = Cmils;
}

Thermostat::Thermostat(int pin)
{
	ctlpin = pin;
	TempSP = 68;
	currTemp = 69.0;
	state = HEATOFF;
	MinRT = 5 * MINUTE;
	Cmils = millis();
	Pmils = Cmils;
	pinMode(pin, OUTPUT);
}


//Set temp setpoint in deg F
void Thermostat::Setpoint(int temp)
{
	 TempSP = temp;
}
//Show temp setpoint in deg F
int Thermostat::getTempSP()
{
	return TempSP;
}
// set minimum runtime
void Thermostat::RunTime(int R_mins)
{
	MinRT = long(R_mins)*MINUTE;
}
int  Thermostat::getRuntime()
{
	return int(MinRT / MINUTE);
}
// send current temp reading from sensor in deg F
void Thermostat::SendCurTemp(double temp)
{
	// add the actions to change state
	currTemp = temp;
	if (1.0 + double(TempSP) > temp)
		state = HEATON;
	else
		state = HEATOFF;
	toggleState();
}
//Heat on or OFF 
int Thermostat::getState()
{
	return state;
}

// private member for cycling relay pin
void Thermostat::toggleState()
{
	Cmils = millis();
	if ((Cmils - Pmils) > MinRT)
	{
		if (ctlpin > 0)
		{
			if (state == HEATON)
				digitalWrite(ctlpin, HIGH);
			else
				digitalWrite(ctlpin, LOW);
		}
		Pmils = Cmils;
	}
}

// ThermostatClass Thermostat;

