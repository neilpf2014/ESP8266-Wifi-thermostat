// SettingStore.h
// Neil Flynn
// 2018-6-6

#ifndef _SETTINGSTORE_h
#define _SETTINGSTORE_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif
#include <FS.h>

//class to handle config file
class SettingStore
{
#define FILE_OK_NR 0
#define FILE_READ 1
#define NO_FILESYSTEM 2
#define FILE_NOT_FOUND 3
#define FILE_INVALID 4

#define SET_SZ 21

 protected:
	 struct parmstr
	 {
	 	String key;
	 	String value;
	  };
	 parmstr firstSetting; // used for file validation
	 // will adjust if we need more then 20 settings
	 parmstr SetList[SET_SZ];
	 unsigned int SetLen; // actual amount of data in array
	 String filename;
	 File SF;
	 unsigned int FSStatus;
	 //int WriteHelper(parmstr _NS);

 public:
	 //class to handle config file
	 SettingStore(String _filename);
	 //call first, reads file contents into array
	 unsigned int ReadFile();
	 unsigned int WriteFile();
	 // write to console for testing
	 unsigned int WriteFileMock();
	 //number of Elements in array
	 unsigned int Length();
	 //get parameter given array index
	 String GetParmByIndex(unsigned int _index);
	 //get value given array index
	 String GetValByIndex(unsigned int _index);
	 //get value for parm
	 String GetValbyParm(String _parm);
	 uint8_t SetValbyParm(String _parm, String _value);
	 uint8_t AddNewSetting(String _parm, String _value);
};



#endif

