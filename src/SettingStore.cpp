// 
// 
// 

#include "SettingStore.h"

// read file into struct if not already in
unsigned int SettingStore::ReadFile()
{
	unsigned long fileSz;
	unsigned long filePos = 0;
	String TempBuf;
	if (FSStatus == FILE_OK_NR)
	{
		SF=SPIFFS.open(filename, "r");
		fileSz = SF.size();
		SetLen = 0;
		// skip header row
		TempBuf = SF.readStringUntil('\r');
		SF.seek(1, SeekCur);
		while (filePos < fileSz)
		{
			SetList[SetLen].key = SF.readStringUntil(':');
			SetList[SetLen].value = SF.readStringUntil('\r');
			SF.seek(1, SeekCur);
			filePos = SF.position();
			SetLen++;
		}
		SF.close();
		FSStatus = FILE_READ;
	}
	return FSStatus;
}
// write to file
unsigned int SettingStore::WriteFile()
{
	if ((FSStatus > 0) && (FSStatus < 4))
	{
		SF=SPIFFS.open(filename, "w");
		for (size_t i = 0; i < SetLen; i++)
		{
			SF.print(String(SetList[i].key) + ":");
			SF.println(String(SetList[i].value));
		}
		SF.close();
	}
	return FSStatus;
}
// write to console for testing
unsigned int SettingStore::WriteFileMock()
{
	// for testing will write output to serial
	Serial.println("test from Class");
	for (size_t i = 0; i < SetLen; i++)
	{
		Serial.print(String(SetList[i].key) + ":");
		Serial.println(String(SetList[i].value));
	}
	return SetLen;
}

// Helper function for writing new data to file
//int WriteHelper(parmstr _NS)
//{
//	String temps;  // a buffer
//	int retVal;
//	uint8_t found = false;
//	uint8_t status;
//	unsigned long fileSz;
//
//	if (FSerror != 1)
//	{
//		// first see if key exsist
//		SF = SPIFFS.open(filename, "r");
//		fileSz = SF.size();
//		while ((!found) && (SF.position() < fileSz))
//		{
//			temps = SF.readStringUntil(':');
//			// attempt to ensure we didn't hit EOF before the term char
//			if (SF.peek() != -1) {
//				status = 1;
//			}
//			else {
//				if (temps == _NS.key)
//					found = true;
//				status = 2;
//			}
//		}
//	return retVal;
//}
//

//Init class but don't pop data structure or modify files
SettingStore::SettingStore(String _filename)
{
	FSStatus = 0;
	filename = _filename;
	if (SPIFFS.begin())
	{
		if (SPIFFS.exists(filename))
		{
			// read first line into struc
			SF = SPIFFS.open(filename, "r");
			firstSetting.key = SF.readStringUntil(':');
			firstSetting.value = SF.readStringUntil('\r');
			SF.close();
			if ((firstSetting.key == "Key") && (firstSetting.value == "Value"))
				FSStatus = FILE_OK_NR;
			else
				FSStatus = FILE_INVALID;
		}
		else
		{
			FSStatus = FILE_NOT_FOUND;
		}
		SetList[0].key = "Key";
		SetList[0].value = "Value";
	}
	else
		FSStatus = NO_FILESYSTEM;
}
// lenght of array
unsigned int SettingStore::Length()
{
	return SetLen;
}
// get parameter given array index
String SettingStore::GetParmByIndex(unsigned int _index)
{
	String retVal;
	if (SetLen > 0)
		if (_index < SetLen)
			retVal = SetList[_index].key;
		else
			retVal = "#index out of range";
	else
		retVal = "#file not read yet";
	return retVal;
}
// get value given array index
String SettingStore::GetValByIndex(unsigned int _index)
{
	String retVal;
	if (SetLen > 0)
		if (_index < SetLen)
			retVal = SetList[_index].value;
		else
			retVal = "#index out of range";
	else
		retVal = "#file not read yet";
	return retVal;
}

// will search stuct until we find the value
String SettingStore::GetValbyParm(String _parm)
{
	String retVal = "not found";
	int cntr;
	for (size_t i = 0; i < SetLen; i++)
	{
		if (SetList[i].key == _parm)
			retVal = SetList[i].value;
	}
	return retVal;
}
// may revist, doing this the simple ineffecnt way first
//String SettingStore::GetValbyParm(String _parm)
//{
//	String temps;  // a buffer
//	String retVal;
//	uint8_t found = false;
//	uint8_t status;
//	unsigned long fileSz;
//
//	if (FSStatus < 2)
//	{
//		SF = SPIFFS.open(filename, "r");
//		fileSz = SF.size();
//		while ((!found) && (SF.position() < fileSz))
//		{
//			switch (status)
//			{
//				// first read a key value
//			case 0:
//			{
//				temps = SF.readStringUntil(':');
//				// attempt to ensure we didn't hit EOF before the term char
//				if (SF.peek() != -1) {
//					status = 1;
//				}
//				else
//					status = 4;
//				break;
//			}
//			// then a parameter
//			case 1:
//			{
//				CurSet.key = temps;
//				CurSet.value = SF.readStringUntil('\r');
//				// attempt to ensure we didn't hit EOF before the term char
//				if (SF.peek() != -1) {
//					status = 2;
//				}
//				else
//					status = 3; // hit EOF
//				break;
//			}
//			case 2:
//			{
//				if (temps == _parm)
//					found = true;
//				status = 0;
//				break;
//			}
//			// serial for debug only
//			case 3:
//			{
//				// CurSet.key = "";
//				CurSet.value = "";
//				Serial.println("Parameter found but hit EOF");
//				Serial.print("cursor at:" + String(SF.position()));
//				Serial.println(" of " + String(fileSz));
//				retVal = "Error 3: not found";
//				break;
//			}
//			case 4:
//			{
//				CurSet.key = "";
//				CurSet.value = "";
//				Serial.println("Parameter not found");
//				Serial.print("cursor at:" + String(SF.position()));
//				Serial.println(" of " + String(fileSz));
//				retVal = "Error 4: param not found";
//				break;
//			}
//
//			default:
//				break;
//			}
//		}
//		if (found)
//		{
//			CurSet.value = temps;
//			retVal = temps;
//		}
//		else
//			retVal = temps;
//	}
//	else
//		retVal = "error 1: FS error";
//	SF.close();
//	return retVal;
//}


uint8_t SettingStore::SetValbyParm(String _parm, String _value)
{
	bool retVal = false;
	for (size_t i = 0; i < SetLen; i++)
	{
		if (SetList[i].key == _parm)
		{
			SetList[i].value = _value;
			retVal = true;
		}
	}
	return retVal;
}

uint8_t SettingStore::AddNewSetting(String _parm, String _value)
{
	bool retVal = false;
	unsigned int oldSetlen;
	oldSetlen = SetLen;
	for (size_t i = 0; i < SetLen; i++)
	{
		if (SetList[i].key == _parm)
		{
			SetList[i].value = _value;
			retVal = true;
		}
	}
	if ((SetLen < 19) && (!retVal)) {
		SetList[SetLen].key = _parm;
		SetList[SetLen].value = _value;
		SetLen++;
		retVal = true;
	}
	return retVal;
}
