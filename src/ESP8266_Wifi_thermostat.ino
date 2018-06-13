/*
* Wifi Theromstat project
* Implent a simple connected theromostat using ESP8266 with
* DS temp sensor IC and SSD1306 OLED
* Right now this is just put together with Demo code for NTP time
* DS temp and SS1306 OLED
* Required external libraries for this 
* ESP8266 core https://github.com/esp8266/Arduino
* OLED Library use latest version https://github.com/ThingPulse/esp8266-oled-ssd1306
* Time https://github.com/PaulStoffregen/Time
* Wifi captive portal https://github.com/tzapu/WiFiManager
* DS temp library https://github.com/milesburton/Arduino-Temperature-Control-Library
*
* ToDo add code to send back temp logging & code to control hrs till set back
*/
// ESP8266 Time & Temp display on SSD1306 OLED
// Neil Flynn 5-28-2018
// 
#include "PushButton.h"  // my own simple button handler
#include "thermostat.h"  // my own theromstat
#include <TimeLib.h> 
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>


// for wifi config see https://github.com/tzapu/WiFiManager
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include "WiFiManager.h"

#include "SSD1306Wire.h" // alias for `#include "SSD1306Wire.h"`
#include <OneWire.h>
#include <DallasTemperature.h>


// NTP Server don't hardcode IP:
const char TimeSvr[] = "time.nist.gov";
IPAddress timeServer;
// timezone info
const int timeZone = -4; // EDT 
//const int timeZone = -5;  // Eastern Standard Time (USA)
//const int timeZone = -4;  // Eastern Daylight Time (USA)
//const int timeZone = -8;  // Pacific Standard Time (USA)
//const int timeZone = -7;  // Pacific Daylight Time (USA)

WiFiUDP Udp;
unsigned int localPort = 8888;  // local port to listen for UDP packets

#define ONE_WIRE_BUS D6

								// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(D6);
// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);
// OLED init
SSD1306Wire  display(0x3c, D5, D3);
Thermostat Thm;
// ensure we've picked proper GPIO's for the buttons
PushButton BtnUp(D2);
PushButton BtnDown(D1);
float temp; // temp from sensor

void configModeCallback(WiFiManager *myWiFiManager)
{
	Serial.println("Entered config mode");
	Serial.println(WiFi.softAPIP());
	//if you used auto generated SSID, print it
	Serial.println(myWiFiManager->getConfigPortalSSID());
}

void setup()
{
	// debug via serial remove later
	Serial.begin(115200);
	// new wifi code - captive portal type config
	// see https://github.com/tzapu/WiFiManager
	WiFiManager wifiManager;
	wifiManager.setAPCallback(configModeCallback);
	if (!wifiManager.autoConnect())
	{
		Serial.println("failed to connect and hit timeout");
		//reset and try again, or maybe put it to deep sleep
		ESP.reset();
		delay(1000);
	}
	display.init();
	display.setBrightness(126);
	display.flipScreenVertically();
	display.setFont(ArialMT_Plain_10);

	Serial.println("Time & Temp test with NTP"); // more test code
	// echo connection info to display
	display.drawString(0, 12, "connected to " + WiFi.SSID());
	String S = "Local IP " + WiFi.localIP().toString();
	display.drawString(0, 24, S);
	display.display();
	//display.clear();

	Serial.print("IP number assigned by DHCP is ");
	Serial.println(WiFi.localIP().toString());
	Serial.println(" set temp is " + Thm.getTempSP());
	Serial.println("Starting UDP");
	Udp.begin(localPort);
	Serial.print("Local port: ");
	Serial.println(Udp.localPort());
	WiFi.hostByName(TimeSvr, timeServer);
	S = ". ";
	display.drawString(0, 48, "waiting for sync");
	display.display();
	do {
		Serial.println("waiting for sync");
		setSyncProvider(getNtpTime);
		delay(5000); // don't poll ntp more then every 5sec !!
	} while (timeStatus() == timeNotSet);
	display.clear();
	sensors.begin();
}


// main
void loop()
{	
	static const long period = 3000; // time in millsec the display cycles
	static unsigned long preMills;
	static unsigned long curMills;
	static int count;
	static int Semph; // state semiphore

	if (timeStatus() != timeNotSet)// first make sure the time is set
	{
		// loop to display date / temp on oled & update cur temp
		// to thermostat control
		curMills = millis();
		BtnUp.update();
		BtnDown.update();// update btn state
		if ((curMills - preMills >= period))
		{
			preMills = curMills;
			switch (Semph)
			{
			case 0:
				OledTemp();
				Thm.SendCurTemp(temp);
				break;
			case 1:
				digitalClockDisplay();
				OLEDClock();
				break;
			default:
				break;
			}
			if (Semph > 1)
				Semph = 0;
			else
				Semph++;
		}
		ReadButtons();
	}
}

// Display temp from sensor to OLED
void OledTemp()
{
	Serial.print("Requesting temperatures...");
	sensors.requestTemperatures(); // Send the command to get temperatures
	Serial.println("DONE");
	temp = sensors.getTempFByIndex(0);
	Serial.println(temp);
	display.clear();
	display.setFont(ArialMT_Plain_24);
	display.setTextAlignment(TEXT_ALIGN_CENTER);
	display.drawString(64, 22, String(temp) + " F");
	display.display();
}

// Display time / date to serial console
// remove when done testing
void digitalClockDisplay() {
	Serial.print(hour());
	printDigits(minute());
	printDigits(second());
	Serial.print(" ");
	Serial.print(day());
	Serial.print(".");
	Serial.print(month());
	Serial.print(".");
	Serial.print(year());
	Serial.println();
}
// goes with above
void printDigits(int digits) {
	// utility for digital clock display: prints preceding colon and leading 0
	Serial.print(":");
	if (digits < 10)
		Serial.print('0');
	Serial.print(digits);
}

// Display Clock to OLED module
void OLEDClock() {
	String HrDis;
	String MinDis;
	String AMPM;

	HrDis = String(hourFormat12());
	// below is for 24hr clock
	// pad with zeros
	//if (HrDis.length() < 2)
	//	HrDis = "0" + String(hour());

	MinDis = String(minute());
	// pad min with zeros
	if (MinDis.length() < 2)
		MinDis = "0" + String(minute());
	if (isPM())
		AMPM = "PM";
	else
		AMPM = "AM";
	display.clear();
	//display.setFont(SansSerif_plain_36);
	display.setFont(ArialMT_Plain_24);
	display.setTextAlignment(TEXT_ALIGN_CENTER);
	display.drawString(64, 12, HrDis + ":" + MinDis + " " + AMPM);
	display.drawString(64, 40, String(month()) + "/" + String(day()) + "/" + String(year()));
	display.display();
}

// update the stored temp setting
void ReadButtons()
{
	int c_setting;
	c_setting = Thm.getTempSP();
	if (BtnUp.isCycled())
		c_setting = c_setting + BtnUp.cycleCount();
	if(BtnDown.isCycled())
		c_setting = c_setting - BtnDown.cycleCount();
	Thm.Setpoint(c_setting);
}

/*-------- NTP code ----------*/
// right from time lib demo

const int NTP_PACKET_SIZE = 48; // NTP time is in the first 48 bytes of message
byte packetBuffer[NTP_PACKET_SIZE]; //buffer to hold incoming & outgoing packets

time_t getNtpTime()
{
	while (Udp.parsePacket() > 0); // discard any previously received packets
	Serial.println("Transmit NTP Request");
	sendNTPpacket(timeServer);
	uint32_t beginWait = millis();
	while (millis() - beginWait < 1500) {
		int size = Udp.parsePacket();
		if (size >= NTP_PACKET_SIZE) {
			Serial.println("Receive NTP Response");
			Udp.read(packetBuffer, NTP_PACKET_SIZE);  // read packet into the buffer
			unsigned long secsSince1900;
			// convert four bytes starting at location 40 to a long integer
			secsSince1900 = (unsigned long)packetBuffer[40] << 24;
			secsSince1900 |= (unsigned long)packetBuffer[41] << 16;
			secsSince1900 |= (unsigned long)packetBuffer[42] << 8;
			secsSince1900 |= (unsigned long)packetBuffer[43];
			return secsSince1900 - 2208988800UL + timeZone * SECS_PER_HOUR;
		}
	}
	Serial.println("No NTP Response :-(");
	return 0; // return 0 if unable to get the time
}

// send an NTP request to the time server at the given address
void sendNTPpacket(IPAddress &address)
{
	// set all bytes in the buffer to 0
	memset(packetBuffer, 0, NTP_PACKET_SIZE);
	// Initialize values needed to form NTP request
	// (see URL above for details on the packets)
	packetBuffer[0] = 0b11100011;   // LI, Version, Mode
	packetBuffer[1] = 0;     // Stratum, or type of clock
	packetBuffer[2] = 6;     // Polling Interval
	packetBuffer[3] = 0xEC;  // Peer Clock Precision
							 // 8 bytes of zero for Root Delay & Root Dispersion
	packetBuffer[12] = 49;
	packetBuffer[13] = 0x4E;
	packetBuffer[14] = 49;
	packetBuffer[15] = 52;
	// all NTP fields have been given values, now
	// you can send a packet requesting a timestamp:                 
	Udp.beginPacket(address, 123); //NTP requests are to port 123
	Udp.write(packetBuffer, NTP_PACKET_SIZE);
	Udp.endPacket();
}