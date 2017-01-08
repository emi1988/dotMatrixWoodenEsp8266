#pragma once

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

class numberStorage
{
public:
	numberStorage();
	~numberStorage();
	
	void init(Print &print);
	

	int readBinaryString(char *s);


	int *getNumber(int number, int rotateAngle);

private:

	Print* m_printer;

	int m_numberArray[10][5];
	int m_numberArrayRot90[10][3];
	int m_numberArrayRot180[10][5];

	void initNumbers();
	void rotateFont90();
	void rotateFont180();
};

