// lab3.cpp: определяет точку входа для консольного приложения.
//

#include "stdafx.h"
#include<iostream>
#include<time.h>
#include <Windows.h>
#include <vector>

using namespace std;

void argsParser(int argc, wchar_t * argv[]);
void help();
void middleMul(); // прототипфункциигенерациипсевдослучайныхчисел
void separateToIntervals();
unsigned int r2 = 0;
#define NUMBERS_COUNT 2000
#define INTERVAL_WIDTH 1000
vector<int> valuesVector;
unsigned int r0 = 15431, r1 = 1543;

bool isKey(wchar_t* arg) {
	bool flag = false;
	flag = lstrcmpW(arg, L"-s") ? false : true;
	flag = lstrcmpW(arg, L"--seed") ? false : true;
	flag = lstrcmpW(arg, L"-h") ? false : true;
	flag = lstrcmpW(arg, L"--help") ? false : true;
	return flag;
}

bool isPresent(wchar_t* arg) {
	if (arg != NULL) {
		return true; //проверка наличия аргумента
	}
	wcout << L"Не передан аргумент. Справка -h|--help" << endl;
	exit(666);
}

bool isKeyValid(wchar_t* arg[], int paramsCount) {
	isPresent(arg[0]);
	for (int i = 1; i <= paramsCount; i++) {
		if (isPresent(arg[i])) { //аргумент команды должен быть представлен и не должен являться ключем
			isKey(arg[i]);
		}
	}
	return true;
}

void argsParser(int argc, wchar_t* argv[]) {
	for (int i = 0; i < argc; i++) {
		if (lstrcmpW(argv[i], L"-h") == 0 || lstrcmpW(argv[i], L"--help") == 0) {
			help();
		}
		if (lstrcmpW(argv[i], L"-s") == 0 || lstrcmpW(argv[i], L"--seed") == 0) {
			if (isKeyValid(&argv[i], 2)) {
				int len = lstrlenW(argv[i + 1]);
				char* temp = (char*)malloc(sizeof(char) * len);
				wcstombs(temp, argv[i + 1], len);
				int val = atoi(temp);
				if (val) {
					r0 = val;
				}
				else
				{
					free(temp);
					printf("Неверный формат числа 1");
					exit(666);
				}
				free(temp);

				len = lstrlenW(argv[i + 2]);
				temp = (char*)malloc(sizeof(char) * len);
				wcstombs(temp, argv[i + 2], len);
				val = atoi(temp);
				if (val) {
					r1 = val;
				}
				else
				{
					free(temp);
					printf("Неверный формат числа 2");
					exit(666);
				}
				free(temp);

			}
		}
	}
}

void help() {
	printf("Для управления используются следующие ключи:\n");
	printf("\t\t -h --help вызов помощи\n");
	printf("\t\t -s --seed [seed1] [seed2] начальные значениия\n");
	exit(1);
}

int _tmain(int argc, wchar_t* argv[])
{
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);

	argsParser(argc, argv);

	const int number_numbers = NUMBERS_COUNT; // количество псевдослучайных чисел
	for (int i = 0; i <= number_numbers; i++) {
		middleMul();
		cout << r2 << "\n"; // генерация i-го числа
		valuesVector.push_back(r2);
	}
	separateToIntervals();
	system("pause");
	return 0;
}

void middleMul() // функция генерации псевдослучайных чисел
{
	r2 = r1 * r0;
	r2 = (r2 >> 8 & 65535);
	r0 = r1;
	r1 = r2;
}

void separateToIntervals()
{
	double percent = 0.0;
	for (int interval = 0; interval < 66000; interval += INTERVAL_WIDTH)
	{
		int count = 0;
		for each (int val in valuesVector)
		{
			if (val >= interval && val < (interval + INTERVAL_WIDTH)) {
				count++;
			}
		}
		double curPercent = (double)count / NUMBERS_COUNT;
		percent += curPercent;
		printf("\nfrom %5d to %5d: %3d {%.2f\%%} Total%%: %.2f",
			interval,
			interval + INTERVAL_WIDTH,
			count,
			curPercent * 100,
			percent);
		count = 0;
	}
}


