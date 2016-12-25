#pragma once
#include<vector>
#include<cstdlib>
#include<math.h>
#include<algorithm>
#include <ctime>

using std::vector;

namespace Prep
{
	//���������� ��������� ����������
	double distEuclid(vector<double> a, vector<double> b);

	//���������� ��������� �������
	double machineEps();
}

/*
���������� ��������� ��������������� ������
*/
namespace GSA {
	//�������������� �������� ������ (mode = 0 - ����������� �������, 1 - ������������)
	vector<double> GSA(double(*func)(vector<double>), int, bool(*restrict) (vector<double>), double, int, int, bool mode);

	//�������������� ��������� ������� � ������������ ������ (��������� �������)
	vector<vector<double>> init(int, int, bool(*restrict)(vector<double>));

	//���������� ��������� ������� � �������������� ����
	vector<vector<double>> calcAcceleration(vector<double>, vector<vector<double>>, double, int);

	//���������� �������� � ��������� �������
	void updateAgents(vector<vector<double>>&, vector<vector<double>>, vector<vector<double>>&);

	//���������� ����� ������� �������
	vector<double> calcMass(vector<double>, bool mode);

	//���������� �������������� ���������
	inline double calcGconst(int, double);

	//��������� ������� ������������ ������ ��� �������
	vector<vector<double>> spaceBound(vector<vector<double>>, bool(*restrict)(vector<double>));

	//������ �������
	vector<double> evaluate(vector<vector<double>>, double(*func)(vector<double>));
}