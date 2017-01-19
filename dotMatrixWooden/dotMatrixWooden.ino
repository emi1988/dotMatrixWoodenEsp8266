#include <SPI.h>
#include "LedMatrix.h"
#include "numberStorage.h"

#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <Adafruit_BMP085_U.h>
#include <TimeLib.h>
#include <WiFiUdp.h>
#include <ArduinoJson.h>



//WIFI-AccesDefinition
const char* ssid = "kartoffelsalat";
const char* password = "dosenfutterdosenfutter";

// NTP Servers:
IPAddress timeServer(132, 163, 4, 101); // time-a.timefreq.bldrdoc.gov
const int timeZone = 1;     // Central European Time

WiFiUDP Udp;
unsigned int localPort = 8888;  // local port to listen for UDP packets

//const char* ssid = "ASUS";
//const char* password = "5220468835767";

//END_WIFI-AccesDefinition

Adafruit_BMP085_Unified bmp = Adafruit_BMP085_Unified(10085);


String m_dayOfWeek;
int m_day;
String m_month;
int m_year;
int m_hour;
int m_minute;

long lastBlinkTime = 0;

DynamicJsonBuffer jsonBuffer;


bool preassureSensorAviable = false;
bool jsonDataAviable = false;

int m_oldFinalArray[4][8];


#ifdef DEBUG_ESP_PORT
#define DEBUG_MSG(...) DEBUG_ESP_PORT.printf( __VA_ARGS__ )
#else
#define DEBUG_MSG(...) 
#endif

#define NUMBER_OF_DEVICES 4
#define CS_PIN 15
LedMatrix ledMatrix = LedMatrix(NUMBER_OF_DEVICES, CS_PIN);
numberStorage m_numberStorage = numberStorage();

int x = 0;
int number = 0;

void setup() {

	m_oldFinalArray[0][0] = { -1 };
	m_oldFinalArray[1][0] = { -1 };
	m_oldFinalArray[2][0] = { -1 };
	m_oldFinalArray[3][0] = { -1 };

	ledMatrix.init();
	ledMatrix.setIntensity(15);
	//ledMatrix.setCharWidth(3);
	//ledMatrix.setText("1.12.16");

	ledMatrix.clear();
	ledMatrix.commit();

	Serial.begin(115200);

	m_numberStorage.init(Serial);

	setupWifi();

	setupBmp180();

	setupNTPSync();
	
}

void setupWifi()
{
	Serial.print("Connecting to ");
	Serial.print(ssid);

	WiFi.mode(WIFI_STA);

	bool retCode = WiFi.begin(ssid, password);

	Serial.print("\nWifi begin:" + retCode);


	while (WiFi.status() != WL_CONNECTED) {
		delay(500);
		Serial.print(".");
	}

	Serial.print("IP address assigned by DHCP: ");
	Serial.println(WiFi.localIP());

	//Serial.print(".");
	Serial.print("\nWiFi-Status: ");
	Serial.print(WiFi.status());
	
}

int setupBmp180()
{
	Serial.println("\ntry to init preassure sensor");
	/*
	Connect SCL to analog 5
	Connect SDA to analog 4
	Connect VDD to 3.3V DC
	Connect GROUND to common ground
	*/

	/* Initialise the sensor */
	if (!bmp.begin())
	{
		/* There was a problem detecting the BMP085 ... check your connections */
		Serial.print("Ooops, no BMP085 detected ... Check your wiring or I2C ADDR!");

		return -1;
	}
	preassureSensorAviable = true;
	return 0;
}

void setupNTPSync()
{

	Serial.println("Starting UDP");
	Udp.begin(localPort);
	Serial.print("Local port: ");
	Serial.println(Udp.localPort());
	Serial.println("waiting for sync");
	setSyncProvider(getNtpTime);
}

float readPreassureFromSensor()
{

	/* Get a new sensor event */
	sensors_event_t event;
	bmp.getEvent(&event);

	/* Display the results (barometric pressure is measure in hPa) */
	float preassure = event.pressure;
	Serial.print("\nread preassure from sensor:");
	Serial.print(preassure);

	return  preassure;
}

/*
void syncTimeFromWeb()
{
	WiFiClient client;
	while (!!!client.connect("google.com", 80))
	{
		Serial.println("connection failed, retrying...");
	}

	client.print("HEAD / HTTP/1.1\r\n\r\n");

	while (!!!client.available())
	{
		yield();
	}

	while (client.available())
	{
		String readHeader = client.readString();
		Serial.print(readHeader);

		int index1 = readHeader.indexOf("Date");

		Serial.print("index of Date: ");
		Serial.print(index1);

		String dateLine = readHeader.substring(index1);

		Serial.print("\ndateLine:");
		Serial.print(dateLine);

		//Date: Thu, 22 Dec 2016 21:10:43 

		int index2 = dateLine.indexOf(" ");

		String nextString = dateLine.substring(index2 + 1);
		index2 = nextString.indexOf(",");
		String dayOfWeek = nextString.substring(0, index2);
		nextString = nextString.substring(index2 + 2);
		//22 Dec 2016 21:10:43

		index2 = nextString.indexOf(" ");
		String day = nextString.substring(0, index2);
		nextString = nextString.substring(index2 + 1);
		//Dec 2016 21:10:43

		index2 = nextString.indexOf(" ");
		String month = nextString.substring(0, index2);
		nextString = nextString.substring(index2 + 1);
		//Dec 2016 21:10:43

		index2 = nextString.indexOf(" ");
		String year = nextString.substring(0, index2);
		nextString = nextString.substring(index2 + 1);
		//21:10:43
		index2 = nextString.indexOf(":");
		String hour = nextString.substring(0, index2);
		nextString = nextString.substring(index2 + 1);
		//10:43

		index2 = nextString.indexOf(":");
		String minute = nextString.substring(0, index2);
		nextString = dateLine.substring(index2 + 1);
		//:43

		Serial.print("\ndayOfWeek:" + dayOfWeek);
		Serial.print("\nday: " + day);
		Serial.print("\nmonth: " + month);
		Serial.print("\nyear: " + year);
		Serial.print("\nhour: " + hour);
		Serial.print("\nminute: " + minute);


		//setTime(hour, minute, )

		m_dayOfWeek = dayOfWeek;
		m_day = day.toInt();
		m_month = month;
		m_year = year.toInt();
		m_hour = hour.toInt() + 1;
		if (m_hour == 24)
		{
			m_hour = 0;
		}

		m_minute = minute.toInt();

	}

}

*/

void displayNumber(int number, int device, int horShift1, int vertShift1, int horShift2, int vertShift2, bool fillZeroes, int AddArray[8], bool animation)
{

	int *numberAsArray;
	int degree;

	int horShiftAdd;
	int vertShiftAdd;

	int finalArray[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };

	//-- 3  0
	//   2  1

	switch (device)
	{

	case 0:
	case 1:
	{
			  degree = 180;
			  horShiftAdd = 0;
			  vertShiftAdd = 3;

			  Serial.println("print number on device 0");
			  /*
			  ledMatrix.sendByte(device, 1 + vertShift1, numberAsArray[0] << horShift1);
			  ledMatrix.sendByte(device, 2 + vertShift1, numberAsArray[1] << horShift1);
			  ledMatrix.sendByte(device, 3 + vertShift1, numberAsArray[2] << horShift1);
			  ledMatrix.sendByte(device, 4 + vertShift1, numberAsArray[3] << horShift1);
			  ledMatrix.sendByte(device, 5 + vertShift1, numberAsArray[4] << horShift1);
			  */

			  if (number < 10 & fillZeroes == false)
			  {
				  numberAsArray = m_numberStorage.getNumber(number, degree);

				  Serial.println("print single number without zeros");

				  for (int i = 0; i < 5; i++)
				  {
					  finalArray[i + (vertShiftAdd - vertShift1)] = numberAsArray[i] << (horShift1);
				  }
			  }
			  else
			  {
				  String numberString = String(number);
				  String firstPart, secondPart;

				  //add a leading zero if number has only one digit
				  if (number < 10)
				  {
					  firstPart = "0";

					  secondPart = numberString.substring(0, 1);
					  //Serial.println("\nnumber last: ");
					  //Serial.println(secondPart);

				  }
				  else
				  {
					  firstPart = numberString.substring(0, 1);
					  //Serial.println("\nnumber first: ");
					  //Serial.println(firstPart);

					  secondPart = numberString.substring(1, 2);
					  //Serial.println("\nnumber last: ");
					  //Serial.println(secondPart);
				  }

				  
				  numberAsArray = m_numberStorage.getNumber(firstPart.toInt(), degree);
				  for (int i = 0; i < 5; i++)
				  {
					  finalArray[i + (vertShiftAdd - vertShift1)] = numberAsArray[i] << (horShift1);
				  }

				  
				  numberAsArray = m_numberStorage.getNumber(secondPart.toInt(), degree);
				  for (int i = 0; i < 5; i++)
				  {
					  finalArray[i + (vertShiftAdd - vertShift2)] += numberAsArray[i] << (horShift2);
				  }

			  }

			  break;
	}

	case 2:
	case 3:
	{
			  degree = 0;
			  horShiftAdd = 5;
			  //vertShiftAdd = 0;

			  //Serial.println("print number on device 2");
			  numberAsArray = m_numberStorage.getNumber(number, 0);

			  /*
			  ledMatrix.sendByte(device, 1 + (3 - vertShift1), numberAsArray[0] << (5 - horShift1));
			  ledMatrix.sendByte(device, 2 + (3 - vertShift1), numberAsArray[1] << (5 - horShift1));
			  ledMatrix.sendByte(device, 3 + (3 - vertShift1), numberAsArray[2] << (5 - horShift1));
			  ledMatrix.sendByte(device, 4 + (3 - vertShift1), numberAsArray[3] << (5 - horShift1));
			  ledMatrix.sendByte(device, 5 + (3 - vertShift1), numberAsArray[4] << (5 - horShift1));
			  */

			  if (number < 10 & fillZeroes == false)
			  {
				  numberAsArray = m_numberStorage.getNumber(number, degree);

				//  Serial.println("print single number without zeros");

				  for (int i = 0; i < 5; i++)
				  {
					  finalArray[i + (vertShift1)] = numberAsArray[i] << (horShiftAdd - horShift1);
				  }
			  }
			  else
			  {
				  String numberString = String(number);
				  String firstPart, secondPart;

				  //check if we have to add a leading zero
				  if (number < 10)
				  {
					  firstPart = "0";

					  secondPart = numberString.substring(0, 1);
					//  Serial.println("\nnumber last: ");
					 // Serial.println(secondPart);
				  }
				  else
				  {
					  firstPart = numberString.substring(0, 1);
					  //Serial.println("\nnumber first: ");
					 //Serial.println(firstPart);

					  secondPart = numberString.substring(1, 2);
					  //Serial.println("\nnumber last: ");
					  //Serial.println(secondPart);

				  }


				  numberAsArray = m_numberStorage.getNumber(firstPart.toInt(), degree);
				  for (int i = 0; i < 5; i++)
				  {
					  finalArray[i + (vertShift1)] = numberAsArray[i] << (horShiftAdd - horShift1);
				  }


				  numberAsArray = m_numberStorage.getNumber(secondPart.toInt(), degree);
				  for (int i = 0; i < 5; i++)
				  {
					  finalArray[i + (vertShift2)] += numberAsArray[i] << (horShiftAdd - horShift2);
				  }

			  }

			  break;
	}



	default:

		break;
	}


	if (AddArray != NULL)
	{
		for (int i = 0; i < 8; i++)
		{
			finalArray[i] += AddArray[i];
		}
	}

	if (animation == true & (m_oldFinalArray[device][0] != -1))
	{
		//check if array has changed
		bool equal = true;
		for (int i = 0; i < 8; i++)
		{
			if (m_oldFinalArray[device][i] != finalArray[i])
			{
				equal = false;
			}
		}
		if (equal == true)
		{
			return;
		}

		//clear column
		int currentColBit = 1;
		for (int j = 0; j < 8; j++)
		{
			for (int i = 0; i < 8; i++)
			{
				//ledMatrix.sendByte(device, i + 1, m_oldFinalArray[device][i] &= ~(1 << currentColBit));
				ledMatrix.sendByte(device, i + 1, m_oldFinalArray[device][i] &= ~(currentColBit));
				delay(25);
			}
			currentColBit = currentColBit << 1;
			currentColBit = currentColBit | 1;
			//Serial.println("\ncurrentColBit:");
			//Serial.println(currentColBit);
		}

		//disp new values
		for (int i = 0; i < 8; i++)
		{
			ledMatrix.sendByte(device, i + 1, finalArray[i]);
		}

	}
	else
	{
		for (int i = 0; i < 8; i++)
		{
			ledMatrix.sendByte(device, i + 1, finalArray[i]);
		}
	}

	//save the old final array for the next animation
	for (int i = 0; i < 8; i++)
	{
		m_oldFinalArray[device][i] = finalArray[i];
	}
}

//-- 3  0
//   2  1

void displayTime()
{

	int horShift1 = 0;
	int vertShift1 = 0;
	int horShift2 = 4;
	int vertShift2 = 1;
		
	int addAray[8] = { 0, 0, 1, 0, 1, 0, 0, 0 };

	displayNumber(hour(), 3, horShift1, vertShift1, horShift2, vertShift2, true, addAray, true);

	horShift1 = 1;
	vertShift1 = 0;
	horShift2 = 5;
	vertShift2 = 1;


	displayNumber(minute(), 0, horShift1, vertShift1, horShift2, vertShift2, true, NULL, true);

}

void displayDate()
{

	int horShift1 = 0;
	int vertShift1 = 0;
	int horShift2 = 4;
	int vertShift2 = 1;

	long blinkIntervall = 2;

	int addAray[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
	if (now() > lastBlinkTime + blinkIntervall)
	{
		lastBlinkTime = now();
		 addAray[5] = 1;
	}

	

	displayNumber(day(), 2, horShift1, vertShift1, horShift2, vertShift2, true, addAray, true);

	/*
	String monthArray[13] = { "null", "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };

	int currentMonth = 0;
	for (int i = 0; i < 13; i++)
	{
		if (m_month.compareTo(monthArray[i]) == 0)
		{
			break;
		}
		currentMonth++;
	}
	*/
	displayNumber(month(), 1, horShift1, vertShift1, horShift2, vertShift2, true, NULL, true);


}

void writePreassureToDatabase(float preassure)
{
	Serial.println("\ntry to write preassure to the database");
	
	//first check if time was already set
	if (timeStatus() != timeNotSet)
	{
		HTTPClient http;
		http.begin("http://web568.lenny.servertools24.de/ledMatrixWooden/preassureReceive.php");
		http.addHeader("Content-Type", "application/x-www-form-urlencoded");
		String htmlPostString;
		
		htmlPostString.concat("timeStamp=");
		htmlPostString.concat(now());
		htmlPostString.concat("&preassure=");
		htmlPostString.concat(String(preassure));

		http.POST(htmlPostString);
		http.writeToStream(&Serial);
		http.end();
	}
	else
	{
		Serial.println("\ndid not write to database, because time was not snced yet");

	}
}

void readPreassureFromDatabase()
{
	Serial.println("\ntry to read preassure from the database");

	//first check if time was already set
	if (timeStatus() != timeNotSet)
	{
		HTTPClient http;
		http.begin("http://web568.lenny.servertools24.de/ledMatrixWooden/preassureRead.php");
		http.addHeader("Content-Type", "application/x-www-form-urlencoded");
		
		//time intervall for diplaying preassure data
		int intervall = 5;

		//seconds for all intervalls
		int sinceTime = (intervall * 60) * 16;

		sinceTime = now() - sinceTime;
				
		String htmlPostString;

		htmlPostString.concat("sinceTime=");
		htmlPostString.concat(sinceTime);


		int httpCode = http.POST(htmlPostString);
		// httpCode will be negative on error
		if (httpCode > 0) {
			// HTTP header has been send and Server response header has been handled
			Serial.printf("[HTTP] GET... code: %d\n", httpCode);

			// file found at server
			if (httpCode == HTTP_CODE_OK) {
				Serial.println("read payload: ");
				//http.writeToStream(&Serial);
								
				String payload = http.getString();
				Serial.println(payload);

				JsonObject& rootJsonObject = jsonBuffer.parseObject(payload);

				// Test if parsing succeeds.
				if (!rootJsonObject.success()) {
					Serial.println("parse JSON-Object FAILED");
					return;
				}
				else
				{
					Serial.println("parse JSON-Object SUCCES");
					jsonDataAviable = true;
					double preassure = rootJsonObject["preassure"][0];

					Serial.println("read preassure value:");
					Serial.println(preassure);
				}

			}
		}
		else {
			Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
		}

		http.end();
	}
	else
	{
		Serial.println("\ndid not write to database, because time was not snced yet");

	}
}

void dispPreassureDiagramm()
{

	if (jsonDataAviable)
	{
		// first get min and max preassure in the data-set

		int count = 0;
		while (true)
		{
			

			delay(10);			
		}
	}
	else
	{
	}
}


/*-------- NTP code ----------*/

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

void loop() {

	Serial.println("\nMain Loop");


	float preassure;
	if (preassureSensorAviable == true)
	{
		preassure = readPreassureFromSensor();
	}
	else
	{
		preassure = -1;
	}

	writePreassureToDatabase(preassure);

	readPreassureFromDatabase();


	//syncTimeFromWeb();


	displayTime();
	displayDate();


	x++;

	if (x > 3)
	{
		x = 0;

	}

	if (number > 9)
	{
		number = 0;

	}

	delay(6000);

	//yield();



}
