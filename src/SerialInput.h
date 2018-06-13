// SerialInput.h
// Neil Flynn
// 2018-6-6

#ifndef _SERIALINPUT_h
#define _SERIALINPUT_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif
// fix for broken serial read line
class SerialInput
{
 protected:
	 
 public:
	 // fix for broken serial read line
	static String get();
};

#endif

