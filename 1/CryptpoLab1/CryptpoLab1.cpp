// CryptpoLab1.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//
#define _CRT_SECURE_NO_WARNINGS
#include "pch.h"
#include <iostream>
#include <vector>
#include<locale>
#include <Windows.h>
#include<fstream>
#include <conio.h>

using namespace std;
bool debug = false;

void encrypt(int key[], const wchar_t* toCryptFileName, const wchar_t* toDecryptFileName = L"Encrypt.txt");


void encrypt(int key[], const wchar_t* toCryptFileName, const wchar_t* toDecryptFileName) {
	char sourceChar;
	ifstream read(toCryptFileName, ios::binary);
	vector<char> vec1, vec2;

	while (read.get(sourceChar))
	{
		vec1.push_back(sourceChar);
	}
	read.close();

	int check = 10 - vec1.size() % 10, part = 0, encryptionKeyPosition = 0;
	//не все строки можно поделить на блоки по 10 элементов для шифрования
	if (check != 10) {
		for (int i = 0; i < check; i++) {
			vec1.push_back(' '); // дополняем пробелами
		}
	}

	//непосредственно перестановка
	for (int i = 0; i < vec1.size(); i++)
	{
		vec2.push_back(vec1[part + key[encryptionKeyPosition] - 1]); //отнимаем единицу т.к. индексы идут с 0..9
		if (encryptionKeyPosition > 8) { //>8, значит до 9
			encryptionKeyPosition = 0;
			part += 10;
		}
		else
		{
			encryptionKeyPosition++;
		}
	}
	ofstream write(toDecryptFileName, ios::trunc | ios::binary);
	for (int i = 0; i < vec2.size(); i++)
	{
		write << vec2[i]; //запись результата в файл
	}
	write.close();
	cout << "Выполнено.\n";
}



int getFileSize(const wchar_t* toCryptFileName);

char* cutString(char* str, int targetSize) {
	char* result = 0;
	int i = 0;
	result = (char*)malloc(sizeof(char*)*targetSize + 1);
	memset(result, 0, sizeof(result));
	for (i; i < targetSize; i++)
	{
		result[i] = str[i];
	}
	result[i] = '\0';
	return result;
}

void decrypt(int key[],
	const wchar_t* toDecryptFileName = L"Encrypted.txt",
	const wchar_t* toWriteFileName = L"Decrypted.txt",
	const wchar_t* originalFile = L"Source.txt") {

	char sourceChar;
	vector<char> vec1, vec2;

	ifstream read(toDecryptFileName, ios::binary);
	while (read.get(sourceChar))
	{
		vec1.push_back(sourceChar);
	}
	read.close();
	//доплняем пробелами, если нужно
	int check = 10 - vec1.size() % 10, part = 0, encryptionKeyPosition = 0;
	if (check != 10) {
		for (int i = 0; i < check; i++) {
			vec1.push_back(' ');
		}
	}
	//сразу расширяем второй, результирующий вектор до размеров первого
	//т.к. будем совершать перестановку и обращаться по индексу.
	vec2.resize(vec1.size());
	for (int i = 0; i < vec2.size(); i++)
	{
		vec2[part + key[encryptionKeyPosition] - 1] = vec1[i];
		if (encryptionKeyPosition > 8) {
			encryptionKeyPosition = 0;
			part += 10;
		}
		else
		{
			encryptionKeyPosition++;
		}
	}

	int fileSize = getFileSize(toDecryptFileName);
	ofstream write(toWriteFileName, ios::binary);
	for (int i = 0; i < fileSize; i++)
	{
		write << vec2[i];
	}
	write.close();
	cout << "Выполнено.\n";

	if (debug) {
		char* command = 0;
		wchar_t* fc = NULL;
		fc = (wchar_t*)malloc(sizeof(wchar_t) * 2);
		memset(fc, 0, sizeof(fc));
		lstrcatW(fc, L"FC ");
		lstrcatW(fc, toWriteFileName);
		lstrcatW(fc, L" ");
		lstrcatW(fc, originalFile);
		lstrcatW(fc, L"\0");
		command = (char*)malloc(sizeof(char) * lstrlenW(fc));
		memset(command, 0, sizeof(command));
		wcstombs(command, fc, lstrlenW(fc));
		strcat(command, "\0");
		command = cutString(command, lstrlenW(fc));
		cout << "Сравнение текстовых файлов...\n";
		system(command);
		cout << "Выполнено.\n";
		system("pause");
	}
}



int getFileSize(const wchar_t* toCryptFileName) {
	char sourceChar;
	int size = 0;
	ifstream read(toCryptFileName, ios::binary);
	while (read.get(sourceChar))
	{
		size++;
	}
	read.close();
	return size;
}

void menu() {

	char menu_word;
	int  c, key[10] = { 4, 1, 6, 2, 9, 3, 5, 7, 10, 8 };

	cout << "1) Зашифровать исходный файл.\n";
	cout << "2) Расшифровать исходный файл.\n";
	cout << "3) Сравнить исходный и результирующий файлы.\n";
	cout << "ESC) Выход.\n";
	switch ((menu_word = _getch()))
	{
	case '1':
		cout << "Шифрование исходного текста...\n";
		//encrypt(key);
		cout << "Выполнено.\n";
		system("pause");
		break;
	case '2':
		cout << "Расшифровка текста...\n";
		decrypt(key);
		cout << "Выполнено.\n";
		system("pause");
		break;
	case '3':
		cout << "Сравнение текстовых файлов...\n";
		system("FC Source.txt Decrypted.txt");
		cout << "Выполнено.\n";
		system("pause");
		break;
	case 27:
		cout << "Завершение процессов и закрытие программы....\n";
		exit(0);
		break;
	default:
		break;
	}
}

bool isDebugEnabled(int args, wchar_t *argv[]) {

	for (int i = 0; i < args; i++) {
		if (lstrcmpW(argv[i], L"-d") == 0 && ((bool)argv[i + 1] == true || (bool)argv[i + 1] == false)) {
			if (lstrcmpW(argv[i + 1], L"false") == 0) return false;
		}
	}
	return true;
}

wchar_t* getFilePath(int args, wchar_t *argv[], wchar_t* filePath = (wchar_t*)L"Source.txt") {
	for (int i = 0; i < args; i++) {
		if (lstrcmpW(argv[i], L"-p") == 0) {
			filePath = (wchar_t*)malloc(sizeof(wchar_t) * lstrlenW(argv[i + 1]));
			filePath = argv[i + 1];
		}
	}
	return filePath;
}

bool isHelpRequested(int args, wchar_t *argv[]) {
	for (int i = 0; i < args; i++) {
		if (args == 2 && lstrcmpW(argv[i], L"-h") == 0) {
			printf("Мануал:\n\t\t -h Вызов man\n\t\t -p указание пути к файлу или имени файла, \nесли он находится в текущей директории\n\t\t -d Режим Debug (Выведет подробный diff файлов)");
			printf("\n\t\t --encrypt filePathToEncrypt filePathToWrite вызов операции шифрования, если не указывать имена фалов, то будут использованы стандартные\n\t\t --decrypt filePathToDecrypt filePathToWrite originalFileToCompare вызов операции расшифровки");
			exit(0);
		}
	}
}

void isEncryptCalled(int argc, wchar_t *argv[]) {

	int key[10] = { 4, 1, 6, 2, 9, 3, 5, 7, 10, 8 };
	for (int i = 0; i < argc; i++) {
		if (lstrcmpW(argv[i], L"--encrypt") == 0) {
			if (argc < 4) {
				printf("Возможно формат команды неверен, проверьте мануал. Ключ \"-h\"\n");
				exit(-1);
			}
			encrypt(key, argv[i + 1], argv[i + 2]);
		}
	}
}

void isDecryptCalled(int argc, wchar_t *argv[]) {

	int key[10] = { 4, 1, 6, 2, 9, 3, 5, 7, 10, 8 };
	for (int i = 0; i < argc; i++) {
		if (lstrcmpW(argv[i], L"--decrypt") == 0) {
			if (argc < 5) {
				printf("Возможно формат команды неверен, проверьте мануал. Ключ \"-h\"\n");
				exit(-1);
			}
			decrypt(key, argv[i + 1], argv[i + 2], argv[i + 3]);
		}
	}
}

void printAllArgs(int argc, wchar_t *argv[]) {
	for (int i = 0; i < argc; i++)
	{
		wcout << argv[i]<< endl;
	}
}

int wmain(int argc, wchar_t *argv[], wchar_t *envp[])
{
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);
	setlocale(LC_ALL, "Russian");
	int  c, key[10] = { 4, 1, 6, 2, 9, 3, 5, 7, 10, 8 };
	isHelpRequested(argc, argv);
	debug = isDebugEnabled(argc, argv);
	isEncryptCalled(argc, argv);
	isDecryptCalled(argc, argv);
	system("pause");
}



