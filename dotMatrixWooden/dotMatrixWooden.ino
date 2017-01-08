#include <SPI.h>
#include "LedMatrix.h"
#include "numberStorage.h"


#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <WiFiClient.h>

//WIFI-AccesDefinition
//const char* ssid = "kartoffelsalat";
//const char* password = "dosenfutterdosenfutter";

//END_WIFI-AccesDefinition

const char* ssid = "ASUS";
const char* password = "5220468835767";

String m_dayOfWeek;
int m_day;
String m_month;
int m_year;
int m_hour;
int m_minute;

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

}

void setupWifi() 
{
	Serial.print("Connecting to ");
	Serial.print(ssid);

	WiFi.mode(WIFI_STA);

	bool retCode = WiFi.begin(ssid, password);
	
	Serial.print("\nWifi begin:" + retCode);


	// (WiFi.status() != WL_CONNECTED) 
	//{
		delay(500);
		
		//Serial.print(".");
		Serial.print("\nWiFi-Status: ");
		Serial.print(WiFi.status());
		Serial.print("\nWiFi connected, IP address: ");
		Serial.println(WiFi.localIP());
	//}
	Serial.print("\nWiFi connected, IP address: ");
	Serial.println(WiFi.localIP());


}

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
		
		String nextString = dateLine.substring(index2+1);
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
					  Serial.println("\nnumber last: ");
					  Serial.println(secondPart);

				  }
				  else
				  {
					  firstPart = numberString.substring(0, 1);
					  Serial.println("\nnumber first: ");
					  Serial.println(firstPart);

					  secondPart = numberString.substring(1, 2);
					  Serial.println("\nnumber last: ");
					  Serial.println(secondPart);
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

			  Serial.println("print number on device 2");
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

				  Serial.println("print single number without zeros");

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
					  Serial.println("\nnumber last: ");
					  Serial.println(secondPart);
				  }
				  else
				  {
					  firstPart = numberString.substring(0, 1);
					  Serial.println("\nnumber first: ");
					  Serial.println(firstPart);

					  secondPart = numberString.substring(1, 2);
					  Serial.println("\nnumber last: ");
					  Serial.println(secondPart);

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
	
	/*

	int finalArray[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
	//int firstArray[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
	//int secondArray[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };

	if (number < 9 & fillZeroes == false)
	{
		numberAsArray = m_numberStorage.getNumber(number, degree);

		for (int i = 0; i < 5; i++)
		{
			finalArray[i + (vertShiftAdd - vertShift1)] = numberAsArray[i] << (horShiftAdd - horShift1);
		}
	}
	else
	{
		String numberString = String(number);
		String firstPart, secondPart;

		if (degree == 0)
		{
			//check if we have to add a leading zero
			if (number < 9)
			{
				firstPart = "0";
			}
			else
			{
				firstPart = numberString.substring(0, 1);
				Serial.println("\nnumber first: ");
				Serial.println(firstPart);
			}
			

			secondPart = numberString.substring(1, 2);
			Serial.println("\nnumber last: ");
			Serial.println(secondPart);
		}
		else if (degree == 180)
		{
			//check if we have to add a leading zero
			if (number < 9)
			{
				firstPart = "0";
			}
			else
			{
				firstPart = numberString.substring(1, 2);
				Serial.println("\nnumber first: ");
				Serial.println(firstPart);
			}

			secondPart = numberString.substring(0, 1);
			Serial.println("\nnumber last: ");
			Serial.println(secondPart);
		}
	

		numberAsArray = m_numberStorage.getNumber(firstPart.toInt(), degree);
		for (int i = 0; i < 5; i++)
		{
			finalArray[i + (vertShiftAdd - vertShift1)] = numberAsArray[i] << (horShiftAdd - horShift1);
		}

		

		numberAsArray = m_numberStorage.getNumber(secondPart.toInt(), degree);
		for (int i = 0; i < 5; i++)
		{
			finalArray[i + (vertShiftAdd - vertShift2)] += numberAsArray[i] << (horShiftAdd - horShift2);
		}
				
	}
	*/

	if (AddArray != NULL)
	{
		for (int i = 0; i < 8; i++)
		{
			finalArray[i] += AddArray[i];
		}
	}

	if (animation == true & (m_oldFinalArray[device][0]!= -1))
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

	displayNumber(m_hour, 3, horShift1, vertShift1, horShift2, vertShift2, true, addAray, true);

	horShift1 = 1;
	vertShift1 = 0;
	horShift2 = 5;
	vertShift2 = 1;

	
	displayNumber(m_minute, 0, horShift1, vertShift1, horShift2, vertShift2, true, NULL, true);

}

void displayDate()
{

	int horShift1 = 0;
	int vertShift1 = 0;
	int horShift2 = 4;
	int vertShift2 = 1;

	int addAray[8] = { 0, 0, 0, 0, 0, 1, 0, 0 };

	displayNumber(m_day, 2, horShift1, vertShift1, horShift2, vertShift2, true, addAray, true);


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

	displayNumber(currentMonth, 1, horShift1, vertShift1, horShift2, vertShift2, true, NULL, true);


}


void loop() {
		
	syncTimeFromWeb();


//	ledMatrix.clear();
	//ledMatrix.commit();

	displayTime();
	displayDate();

	/*
	if (x == 0)
	{
		ledMatrix.clear();
		ledMatrix.commit();
		
		int test = m_numberStorage.readBinaryString("11001101");




		ledMatrix.sendByte(1, 1, p1);
		ledMatrix.sendByte(1, 2, p2);
		ledMatrix.sendByte(1, 3, p3);
		ledMatrix.sendByte(1, 4, p4);
		ledMatrix.sendByte(1, 5, p5);
		
		
	}
	*/
	
	Serial.println("\ntest print");
	/*
	displayNumber(0, 0, 3, 2);
	displayNumber(1, 1, 3, 2);
	displayNumber(2, 2, 0, 3);
	displayNumber(3, 3, 3, 2);
	*/

	/*
	int *numberAsArray= m_numberStorage.getNumber(number, 180);
	number++;

	Serial.println(numberAsArray[0]);
	Serial.println(numberAsArray[1]);
	Serial.println(numberAsArray[2]);
	Serial.println(numberAsArray[3]);
	Serial.println(numberAsArray[4]);
	
	
	for (int x = 0; x < 4; x++)
	{

	ledMatrix.sendByte(x, 1, numberAsArray[0]);
	ledMatrix.sendByte(x, 2, numberAsArray[1]);
	ledMatrix.sendByte(x, 3, numberAsArray[2]);
	ledMatrix.sendByte(x, 4, numberAsArray[3]);
	ledMatrix.sendByte(x, 5, numberAsArray[4]);

	}
	*/
	/*

	ledMatrix.sendByte(1, 1, 1);
	ledMatrix.sendByte(2, 2, 2);
	ledMatrix.sendByte(3, 3, 3);
	ledMatrix.sendByte(4, 4, 4);
	ledMatrix.sendByte(1, 5, 5);

	ledMatrix.commit();
	*/
	/*
	numberAsArray = m_numberStorage.getNumber(number, 90);
	number++;

	
	delay(3000);

	ledMatrix.clear();
	ledMatrix.commit();

	ledMatrix.sendByte(2, 1, numberAsArray[0]);
	ledMatrix.sendByte(2, 2, numberAsArray[1]);
	ledMatrix.sendByte(2, 3, numberAsArray[2]);

	*/
	
	/*
	ledMatrix.clear();
	ledMatrix.scrollTextLeft();
	ledMatrix.drawText();
	ledMatrix.commit();
	*/


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
