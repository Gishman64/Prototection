// lab3.cpp: ���������� ����� ����� ��� ����������� ����������.
//

#include "stdafx.h"
#include<iostream>
#include<time.h>
#include <Windows.h>
#include <vector>

using namespace std;

void middleMul(); // ��������������������������������������������
void separateToIntervals();
unsigned int r2 = 0;
#define NUMBERS_COUNT 2000
#define INTERVAL_WIDTH 1000
vector<int> valuesVector;
unsigned int r0 = 0, r1 = 0;
int main()
{
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);
	const int number_numbers = NUMBERS_COUNT; // ���������� ��������������� �����
	printf("������� �1 � �0: ");
	scanf_s("%d %d", &r0, &r1);
	cout << "\n";
	for (int i = 0; i <= number_numbers; i++) {
		middleMul();
		cout << r2 << "\n"; // ��������� i-�� �����
		valuesVector.push_back(r2);
	}
	separateToIntervals();
	system("pause");
	return 0;
}

void middleMul() // ������� ��������� ��������������� �����
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
		printf("\nfrom %5d to %5d: %3d {%.3f\%%} Total%%: %.2f",
			interval, 
			interval + INTERVAL_WIDTH, 
			count, 
			curPercent,
			percent);
		count = 0;
	}
}


