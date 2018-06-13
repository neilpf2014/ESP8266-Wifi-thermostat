
#include "SerialInput.h"

//Fixed serial readln function
String SerialInput::get()
{
	// Read String input handling: thanks Rick
	// "String" is now returned as a ptr to char assay
	
		unsigned int i = 0;
		char Ch = 'a';
		char buffer[50];
		Serial.flush();
		Serial.print(">");
		while ((Ch != '\n') && (i < 50))
		{
			while ((Serial.available() > 0))
			{
				Ch = Serial.read();
				if ((Ch != '\n') && (Ch != '\r')) //need to trap both <CR> & <LF> !
				{
					buffer[i] = Ch;
					i++;
				}
				Serial.print(Ch);
			}
		}
		char* somearray = new char[i + 1];
		for (int j = 0; j < i; j++)
			somearray[j] = buffer[j];
		somearray[i] = '\0'; // null termination
		return somearray;
}
