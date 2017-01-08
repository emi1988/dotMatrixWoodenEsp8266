#include "numberStorage.h"


numberStorage::numberStorage()
{

}

numberStorage::~numberStorage()
{
}

void numberStorage::init(Print &print)
{
	m_printer = &print; //operate on the adress of print

	initNumbers();
	rotateFont90();
	rotateFont180();
}


int numberStorage::readBinaryString(char *s) {
	int result = 0;
	while (*s) {
		result <<= 1;
		if (*s++ == '1') result |= 1;
	}
	return result;
}

void numberStorage::rotateFont90()
{
	for (int i = 0; i < 10 ; i++)
	{
		int bitMask = 4;
		int bitShift = 2;	

		m_printer->println("rotate number 90: " + String(i));

		for (int j = 0; j < 3; j++)
		{
			String currentNumberBinary = "";			

			for (int k = 0; k < 5; k++)
			{
				int currentBinaryValue = (m_numberArray[i][k] & bitMask) >> bitShift;
				
				currentNumberBinary = currentNumberBinary + String(currentBinaryValue);

				m_printer->println(currentNumberBinary);				
			}

			bitMask = bitMask >> 1;			

			char *charNumberArray = &currentNumberBinary[0];
			int currentLineAsInt = readBinaryString(charNumberArray);

			m_printer->println("currentLineAsBinary:");
			m_printer->println(currentNumberBinary);

			m_numberArrayRot90[i][bitShift] = currentLineAsInt;

			bitShift--;
		}
	}
}

void numberStorage::rotateFont180()
{
	for (int i = 0; i < 10; i++)
	{
		int bitMask = 16;
		int bitShift = 4;

		m_printer->println("rotate number 180: " + String(i));

		for (int j = 0; j < 5; j++)
		{
			String currentNumberBinary = "";

			for (int k = 0; k < 3; k++)
			{
				int currentBinaryValue = (m_numberArrayRot90[i][k] & bitMask) >> bitShift;

				currentNumberBinary = currentNumberBinary + String(currentBinaryValue);

				m_printer->println(currentNumberBinary);
			}

			bitMask = bitMask >> 1;

			char *charNumberArray = &currentNumberBinary[0];
			int currentLineAsInt = readBinaryString(charNumberArray);

			m_printer->println("currentLineBinary:");
			m_printer->println(currentNumberBinary);

			m_numberArrayRot180[i][bitShift] = currentLineAsInt;

			bitShift--;
		}
	}
}

void numberStorage::initNumbers()
{		

	//0:
	m_numberArray[0][0] = readBinaryString("111");
	m_numberArray[0][1] = readBinaryString("101");
	m_numberArray[0][2] = readBinaryString("101");
	m_numberArray[0][3] = readBinaryString("101");
	m_numberArray[0][4] = readBinaryString("111");

	//1
	m_numberArray[1][0] = readBinaryString("010");
	m_numberArray[1][1] = readBinaryString("110");
	m_numberArray[1][2] = readBinaryString("010");
	m_numberArray[1][3] = readBinaryString("010");
	m_numberArray[1][4] = readBinaryString("010");

	//2:
	m_numberArray[2][0] = readBinaryString("010");
	m_numberArray[2][1] = readBinaryString("101");
	m_numberArray[2][2] = readBinaryString("001");
	m_numberArray[2][3] = readBinaryString("010");
	m_numberArray[2][4] = readBinaryString("111");


	//3:
	m_numberArray[3][0] = readBinaryString("111");
	m_numberArray[3][1] = readBinaryString("001");
	m_numberArray[3][2] = readBinaryString("011");
	m_numberArray[3][3] = readBinaryString("001");
	m_numberArray[3][4] = readBinaryString("111");

	//4:
	m_numberArray[4][0] = readBinaryString("101");
	m_numberArray[4][1] = readBinaryString("101");
	m_numberArray[4][2] = readBinaryString("111");
	m_numberArray[4][3] = readBinaryString("001");
	m_numberArray[4][4] = readBinaryString("001");

	//5:
	m_numberArray[5][0] = readBinaryString("111");
	m_numberArray[5][1] = readBinaryString("100");
	m_numberArray[5][2] = readBinaryString("111");
	m_numberArray[5][3] = readBinaryString("001");
	m_numberArray[5][4] = readBinaryString("111");

	//6:
	m_numberArray[6][0] = readBinaryString("111");
	m_numberArray[6][1] = readBinaryString("100");
	m_numberArray[6][2] = readBinaryString("111");
	m_numberArray[6][3] = readBinaryString("101");
	m_numberArray[6][4] = readBinaryString("111");

	//7:
	m_numberArray[7][0] = readBinaryString("111");
	m_numberArray[7][1] = readBinaryString("001");
	m_numberArray[7][2] = readBinaryString("010");
	m_numberArray[7][3] = readBinaryString("100");
	m_numberArray[7][4] = readBinaryString("100");

	//8:
	m_numberArray[8][0] = readBinaryString("111");
	m_numberArray[8][1] = readBinaryString("101");
	m_numberArray[8][2] = readBinaryString("111");
	m_numberArray[8][3] = readBinaryString("101");
	m_numberArray[8][4] = readBinaryString("111");

	//9:
	m_numberArray[9][0] = readBinaryString("111");
	m_numberArray[9][1] = readBinaryString("101");
	m_numberArray[9][2] = readBinaryString("111");
	m_numberArray[9][3] = readBinaryString("001");
	m_numberArray[9][4] = readBinaryString("111");
}


int * numberStorage::getNumber(int number, int rotateAngle)
{
	if (rotateAngle == 0)
	{
		static int outputArray[5];

		m_printer->println("get number with angle 0");

		for (int i = 0; i < 5; i++)
		{
			outputArray[i] = m_numberArray[number][i];
		}
		return outputArray;
	}
	else if (rotateAngle == 90)
	{
		static int outputArray[3];

		m_printer->println("get number with angle 90");

		for (int i = 0; i < 3; i++)
		{
			outputArray[i] = m_numberArrayRot90[number][i];
		}
		return outputArray;
	}
	else if (rotateAngle == 180)
	{
		static int outputArray[5];

		m_printer->println("get number with angle 180");

		for (int i = 0; i < 5; i++)
		{
			outputArray[i] = m_numberArrayRot180[number][i];
		}
		return outputArray;
	}

	
}

