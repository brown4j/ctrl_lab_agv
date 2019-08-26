// SerialPort.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "SerialPort.h"
#include <windows.h>
#include <string.h>

#pragma warning (disable:4996)

int _tmain(int argc, _TCHAR* argv[])
{
	CSerialPort mySerial;

	int baudNum;
	char portName[10];
	
	printf("Comport: (ex:COM1)");
	scanf("%s", &portName);
	printf("Baudrate: ");
	scanf("%d", &baudNum);

	switch (baudNum)
	{
	default:
		break;

	case 110:
		mySerial.Open(portName, CBR_110, 8, ONESTOPBIT, NOPARITY);
		break;

	case 300:
		mySerial.Open(portName, CBR_300, 8, ONESTOPBIT, NOPARITY);
		break;

	case 600:
		mySerial.Open(portName, CBR_600, 8, ONESTOPBIT, NOPARITY);
		break;

	case 1200:
		mySerial.Open(portName, CBR_1200, 8, ONESTOPBIT, NOPARITY);
		break;

	case 2400:
		mySerial.Open(portName, CBR_2400, 8, ONESTOPBIT, NOPARITY);
		break;

	case 4800:
		mySerial.Open(portName, CBR_4800, 8, ONESTOPBIT, NOPARITY);
		break;

	case 9600:
		mySerial.Open(portName, CBR_9600, 8, ONESTOPBIT, NOPARITY);
		break;

	case 14400:
		mySerial.Open(portName, CBR_14400, 8, ONESTOPBIT, NOPARITY);
		break;

	case 19200:
		mySerial.Open(portName, CBR_19200, 8, ONESTOPBIT, NOPARITY);
		break;

	case 38400:
		mySerial.Open(portName, CBR_38400, 8, ONESTOPBIT, NOPARITY);
		break;

	case 56000:
		mySerial.Open(portName, CBR_56000, 8, ONESTOPBIT, NOPARITY);
		break;

	case 57600:
		mySerial.Open(portName, CBR_57600, 8, ONESTOPBIT, NOPARITY);
		break;

	case 115200:
		mySerial.Open(portName, CBR_115200, 8, ONESTOPBIT, NOPARITY);
		break;

	case 128000:
		mySerial.Open(portName, CBR_128000, 8, ONESTOPBIT, NOPARITY);
		break;

	case 256000:
		mySerial.Open(portName, CBR_256000, 8, ONESTOPBIT, NOPARITY);
		break;
	}

	mySerial.SetTimeout(10, 10, 1);

	unsigned char buff[24];

	while (1) {
		
		mySerial.Read(buff, 24);


		UINT32 data;
		
		data = ((byte)buff[10] << 24) | ((byte)buff[11] << 16) | ((byte)buff[12] << 8) | ((byte)buff[13]);
		float rdata = *((float*)&data);

		data = ((byte)buff[14] << 24) | ((byte)buff[15] << 16) | ((byte)buff[16] << 8) | ((byte)buff[17]);
		float pdata = *((float*)&data);

		data = ((byte)buff[18] << 24) | ((byte)buff[19] << 16) | ((byte)buff[20] << 8) | ((byte)buff[21]);
		float ydata = *((float*)&data);

		printf("Roll: %f , Pitch: %f , Yaw: %f\n", rdata, pdata, ydata);
	};
	 
	return 0;
}

