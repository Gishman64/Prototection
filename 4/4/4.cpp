// 4.cpp: определяет точку входа для консольного приложения.
//

#include "stdafx.h"
#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <Windows.h>
#include <locale.h>

using namespace std;
#define MAX_SIZE_MSG 64*1024
typedef unsigned char byte;

char crcString[2];
char msgLen[2];
wchar_t* imgFileName = L"5.bmp";
wchar_t* msgFileName = L"toCrypt.txt";

int absoluteCurPosition = 0;
int curBitPos = 0;
int curImagePos = 0;
int imgI = 0;
int imgJ = 0;
BITMAPINFOHEADER infoHeader;
BITMAPFILEHEADER fileHeader;
HANDLE bmp, msgFile;
char buffer[MAX_SIZE_MSG];
DWORD dwReaded;
int imageSize;

struct Pixel
{
	byte red;
	byte green;
	byte blue;
} CUR_PIXEL;
int imageCapacity(BITMAPINFOHEADER* fileInfo) {
	return fileInfo->biHeight*fileInfo->biWidth * 2 / 8;
}

int getBit(char block, int numBit)
{
	return ((block & (1 << numBit)) != 0);
}

unsigned short crc16(unsigned char* data_p, int length) {
	unsigned char x;
	unsigned short crc = 0xFFFF;

	while (length--) {
		x = crc >> 8 ^ *data_p++;
		x ^= x >> 4;
		crc = (crc << 8) ^ ((unsigned short)(x << 12)) ^ ((unsigned short)(x << 5)) ^ ((unsigned short)x);
	}
	return crc;
}

char* calcCrc(unsigned char* data_p, int length) {
	USHORT block = crc16(data_p, length);
	char* crc;
	crc = (char*)malloc(sizeof(char) * 3);
	crc[0] = (block >> 8) & 255;
	crc[1] = (block) & 255;
	crc[2] = '\0';
	return crc;
}

char* msgLenToChar(USHORT len) {
	char* msglen = (char*)malloc(sizeof(char) * 3);
	msglen[0] = (len >> 8) & 255;
	msglen[1] = (len) & 255;
	msglen[2] = '\0';
	return msglen;
}

USHORT getUSHORT(char buffer[2]) {
	unsigned short block = 0;
	block |= buffer[0] << 8 & 0xff00;
	block |= buffer[1] & 0x00ff;
	return block;
}

int writeCurrentPixel() {
	bmp = CreateFile(imgFileName, GENERIC_WRITE, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (bmp == INVALID_HANDLE_VALUE) {
		printf("cannot read or open image file");
		return 0;
	}
	SetFilePointer(bmp, absoluteCurPosition - 3, 0, FILE_BEGIN);
	if (!WriteFile(bmp, &CUR_PIXEL, sizeof(CUR_PIXEL), &dwReaded, 0)) {
		CloseHandle(bmp);
		return 0;
	}
	CloseHandle(bmp);
	return -1;
}

int readCurrentPixel() {
	bmp = CreateFile(imgFileName, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (bmp == INVALID_HANDLE_VALUE) {
		printf("\nCannot read or open image file");
		CloseHandle(bmp);
		return 0;
	}
	SetFilePointer(bmp, absoluteCurPosition, 0, FILE_BEGIN);
	if (!ReadFile(bmp, &CUR_PIXEL, sizeof(Pixel), &dwReaded, 0)) {
		printf("\nerror reading pixel");
		CloseHandle(bmp);
		return 0;
	}
	absoluteCurPosition += 3;
	if (++curImagePos > infoHeader.biWidth) {
		curImagePos = 0;
		//SetFilePointer(bmp, infoHeader.biWidth % 4, 0, FILE_CURRENT);
		//absoluteCurPosition += infoHeader.biWidth % 4;
	}
	CloseHandle(bmp);
	return 1;
}

int cryptAndRewriteChar(char ch) {
	for (int i = 0; i < 8; i += 2) {
		if (!readCurrentPixel()) {
			printf("\nerror reading pixel");
			return 0;
		}
		char value = getBit(ch, i);
		CUR_PIXEL.red = (CUR_PIXEL.red & 0xfe) | value;
		value = getBit(ch, i + 1);
		CUR_PIXEL.green = (CUR_PIXEL.green & 0xfe) | value;
		if (!writeCurrentPixel()) {
			printf("\nerror writing pixel");
			return 0;
		}
	}
	return 1;
}
void printMenu() {
	system("cls");
	printf("\t\t\tSTEGANOGRAPHY");
	printf("\n\t\t\1)Зашифровать собщение в файл");
	printf("\n\t\t\2)Прочитать сообщение из файла");
	printf("\n\t\t\3)Выход");
}
int cryptMessage(char *message) {
	int len = strlen(message);
	if (strlen(message) > imageCapacity(&infoHeader)) {
		printf("\nРазмер сообщения не соответствует размеру изображения.");
		return 0;
	}
	if (len == 0 && message[1] != 0) {
		for (int i = 0; i < 2; i++) {
			if (!cryptAndRewriteChar(message[i])) {
				printf("\n error crypt char");
				return 0;
			}
		}
	}
	else {
		for (int i = 0; i < len; i++) {
			if (!cryptAndRewriteChar(message[i])) {
				printf("\n error crypt char");
				return 0;
			}
		}
	}
	return 1;
}

char readChar() {
	char chToBuild = 0;
	for (int i = 0; i < 8; )
	{
		if (!readCurrentPixel()) {
			printf("Cannot read pixel while fetching char");
			return 0;
		}
		chToBuild |= (CUR_PIXEL.red & 0x01) << i++;
		chToBuild |= (CUR_PIXEL.green & 0x01) << i++;
	}
	return chToBuild;
}
char* readData(int len) {
	char *data = (char*)malloc(sizeof(char) * len + 1);
	memset(data, 0, len + 1);
	for (int i = 0; i < len; i++) {
		char readed = readChar();
		data[i] = readed;
	}
	return data;
}
int _tmain(int argc, _TCHAR* argv[])
{
#pragma region init
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);
	bmp = CreateFile(imgFileName, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (bmp == INVALID_HANDLE_VALUE) {
		printf("cannot read or open image file");
		return -1;
	}
	msgFile = CreateFile(msgFileName, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (bmp == INVALID_HANDLE_VALUE) {
		printf("cannot open msg file");
		return -1;
	}
	if (!ReadFile(msgFile, &buffer, MAX_SIZE_MSG, &dwReaded, 0)) {
		printf("cannot Read msg");
		return -1;
	}
	if (!ReadFile(bmp, &fileHeader, sizeof(BITMAPFILEHEADER), &dwReaded, 0)) {
		printf("cannot read or open image file");
		return -1;
	}
	if (!ReadFile(bmp, &infoHeader, sizeof(BITMAPINFOHEADER), &dwReaded, 0)) {
		printf("cannot read or open image file");
		return -1;
	}
	if (strlen(buffer) + 4 > imageCapacity(&infoHeader)) {
		printf("\nРазмер сообщения не соответствует размеру изображения.");
		return 0;
	}
	SetFilePointer(bmp, fileHeader.bfOffBits, 0, FILE_BEGIN);
	absoluteCurPosition = fileHeader.bfOffBits;
	CloseHandle(bmp);
#pragma endregion

	while (true) {

		printMenu();
		char key = _getch();
		switch (key) {
		case '1': {
#pragma region crypt
			bool hasErrors = false;
			if (!cryptMessage(calcCrc((unsigned char*)buffer, strlen(buffer)))) {
				printf("\nОшибка. Не удалось записать контрольную сумму.");
				hasErrors = true;
			};
			if (!cryptMessage(msgLenToChar(strlen(buffer))) && !hasErrors) {
				printf("\nОшибка. Не удалось записать длину сообщения.");
			};
			if (!cryptMessage(buffer) && !hasErrors) {
				printf("\nОшибка. Не удалось записать сообщение.");
			};
			absoluteCurPosition = fileHeader.bfOffBits;
			if (!hasErrors) {
				printf("\nУспех.");
			}
			_getch();
			break;
#pragma endregion
		}

		case '2': {
#pragma region decrypt
			bool hasErrors = false;
			char* crc = readData(2);
			char* msglen = readData(2);
			USHORT len = getUSHORT(msglen);
			if (len + 4 > imageCapacity(&infoHeader)) {
				printf("Файл не содержит сообщения. Длина %d явно превышает максимальную вместимость.");
				hasErrors = true;
			}
			char* message = readData(len);
			if (strcmp(calcCrc((unsigned char*)message, len), crc) && !hasErrors) {
				printf("Несовпадение контрольной суммы. Файл не содержит сообщения.");
				hasErrors = true;
			}
			if (!hasErrors) {
				printf("\n\t\tReaded message: %s", message);
				printf("\n\t\tУспех.");
			}
			_getch();
#pragma endregion
			break;
		}
		case '3':
			goto end;
		}
	}
end:
	return 0;
}