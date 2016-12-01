#pragma once
#include<vector>
/*
�������� ������� � ����������� ��� ��������� ��������������� ������
*/

double func1(std::vector<double> pos)
{
	double x = pos[0];
	double y = pos[1];
	return pow(x*x + y - 11, 2) + pow(y*y + x - 7,2);
}

double func2(std::vector<double> pos)
{
	double x = pos[0];
	double y = pos[1];
	return x+y;
}

bool restrict1(std::vector<double> point)
{
	return ((point[0]<4) && (point[0]>-4))&&(point[1]<4)&&(point[1]>-4);
}