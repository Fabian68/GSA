#pragma once
#include<vector>
#include<cstdlib>
#include<math.h>
#include<algorithm>
#include <ctime>

using std::vector;

namespace Prep
{
	double distEuclid(vector<double> a, vector<double> b)
	{
		if (a.size() != b.size()) return 0;
		double r = 0;
		for (size_t i = 0; i < a.size(); i++)
		{
			r += pow((a[i] - b[i]),2);
		}
		return sqrt(r);
	}
	double machineEps()
	{
		double e = 1.0;
		while (1.0 + e / 2.0 > 1.0)
		{
			e /= 2.0;
		}
		return e;
	}
}

/*
���������� ��������� ��������������� ������
*/

//�������������� �������� ������ (�� ��������� mode = 0 - ����������� �������, 1 - ������������)
vector<double> GSA(double(*func)(vector<double>), int, bool(*restrict) (vector<double>), double, int, int, bool mode = 0);

//�������������� ��������� ������� � ������������ ������ (��������� �������)
vector<vector<double>> init(int, int, bool (*restrict)(vector<double>));

//���������� ��������� ������� � �������������� ����
vector<vector<double>> calcAcceleration(vector<double>, vector<vector<double>>, double, int);

//���������� �������� � ��������� �������
void updateAgents(vector<vector<double>>&, vector<vector<double>>, vector<vector<double>>&);

//���������� ����� ������� �������
vector<double> calcMass(vector<double>, bool mode);

//���������� �������������� ���������
inline double calcGconst(int, double);

//��������� ������� ������������ ������ ��� �������
vector<vector<double>> spaceBound(vector<vector<double>>, bool (*restrict)(vector<double>));

//������ �������
vector<double> evaluate(vector<vector<double>>, double(*func)(vector<double>));



vector<double> GSA(double(*func)(vector<double>), int dim, bool(*restrict) (vector<double>), double G, int N, int max_it, bool mode)
{
	/*
	�������������� ��������� ������
	func - ��������� �� ������� �������
	restrict - ��������� �� ������� �����������
	G - �������������� ����������
	N - ����� �������
	mode - ����� ������ ������ ���������� (����������� = true(0), ������������ false(1))
	*/
	vector<double> result(dim);		//�������� ����������, ��������������� ���������� �� ������� ������
	vector<vector<double>> pos(N);	//������ ��������� ������� �������
	for (int i = 0; i < N; i++)
	{
		pos[i].resize(dim);
	}
	srand(time(NULL));
	pos = init(dim, N, restrict);	//������������� ������� ��������� �������
	vector<vector<double>> vel(N);			//������ ��������� �������
	for (int i = 0; i < N; i++)
	{
		vel[i].resize(dim);
	}
	vector<double> fitness(N);
	vector<double> mass(N);
	vector<vector<double>> accel(N);
	for (int i = 0; i < N; i++)
	{
		accel[i].resize(dim);
	}
	vector<double> tempBest(dim);	//�������� �������� ���������� (�������)
	double best = 0;		//������ ������� �������� ��� ������� �������
	double tempfunc = 0;			//�������� ������� �� ������� ����
	double Gi = 0;
	for (int i = 0; i < max_it; i++)
	{

		fitness = evaluate(pos, func);
		pos = spaceBound(pos, restrict);
		vector<double>::iterator ext = mode ? std::max_element(fitness.begin(), fitness.end()) : std::min_element(fitness.begin(), fitness.end());
		vector<double>::iterator bad = std::max_element(mass.begin(), mass.end());
		tempBest = pos[distance(fitness.begin(), ext)];
		
		tempfunc = func(tempBest);
		if (mode)
		{
			if (best < tempfunc) 
			{ 
				result = tempBest; 
				best = tempfunc; 
			}
		}
		else
		{
			if ((best > tempfunc)||(i == 0)) 
			{ 
				result = tempBest; 
				best = tempfunc; 
			}
		}
		mass = calcMass(fitness,mode);
		Gi = calcGconst(i, G);
		accel = calcAcceleration(mass, pos, Gi, i);
		updateAgents(pos, accel, vel);
		if (N < max_it)
		{
			pos.erase(pos.begin() + distance(fitness.begin(), bad), pos.begin() + distance(fitness.begin(), bad));
		}
	}
	return result;
}


vector<vector<double>> init(int dim, int N, bool(*restrict)(vector<double>))
{
	/*
		��������� ������������� �������
		dim - ����������� ������� (����� ����������)
		N - ����� �������
		restrict - ��������� �� ������� � �������� �����������
	*/	
	vector<vector<double>> pos(N);
	for (int i = 0; i < N; i++)
	{
		pos[i].resize(dim);
		do
		{
			for (int j = 0; j < dim; j++)
			{
				pos[i][j] = pow(-1.0, rand()*2/RAND_MAX)*rand()*100/RAND_MAX;
			}
		}while (!restrict(pos[i]));
	}
	return pos;
}


vector<vector<double>> spaceBound(vector<vector<double>> X, bool(*restrict)(vector<double>))
{
	/*
	�������� ������ 
	X - ���������� ������� �������
	restict - ������� �����������
	*/
	for (size_t i = 0; i < X.size(); i++)
	{
		while (!restrict(X[i]))
		{
			for (size_t j = 0; j < X[i].size(); j++)
			{
				X[i][j] = pow(-1.0,rand()*2/RAND_MAX)*rand()*100/RAND_MAX;
			}
		}
	}
	return X;
}

vector<vector<double>> calcAcceleration(vector<double> mass, vector<vector<double>> pos, double G, int it)
{
	/*
	�������� ��������� �������
	mass - ����� �������
	pos - ���������� ������� �������
	it - ������� ��������
	max_it - ������������ ����� ��������
	G - �������������� ���������� (��� i)
	*/
	vector<vector<double>> ac(pos.size());
	for (size_t i = 0; i < pos.size(); i++)
	{
		ac[i].resize(pos[i].size());
		for (size_t j = 0; j < pos.size(); j++)
		{
			if (j != i)
			{
				//��������� ���������� ����� ��������
				double R = Prep::distEuclid(pos[i], pos[j]);
				for (size_t k = 0; k < pos[j].size(); k++)
				{
					//��������� �� k-��� ���������
					ac[i][k] = (rand() / double(RAND_MAX)) * G * mass[j] * (pos[j][k] - pos[i][k]) / (R + Prep::machineEps());
				}
			}
		}
	}
	return ac;
}

vector<double> calcMass(vector<double> fitness, bool mode)
{
	/*
	�������� ����
	fitness - �������-�������
	mode - �����
	*/
	vector<double> mass(fitness.size());
	vector<double>::iterator fitMin = min_element(fitness.begin(), fitness.end());
	vector<double>::iterator fitMax = max_element(fitness.begin(), fitness.end());
	double max = mode == 0 ? *fitMin : *fitMax;
	double min = mode == 0 ? *fitMax : *fitMin;
	for (size_t i = 0; i < fitness.size(); i++)
	{
		mass[i] = (fitness[i] - min) / (max - min);
	}
	double sum = 0;
	for (size_t i = 0; i < mass.size(); i++)
	{
		sum += mass[i];
	}
	for (size_t i = 0; i < fitness.size(); i++)
	{
		mass[i] = mass[i] / sum;
	}
	return mass;
}

inline double calcGconst(int it, double G0)
{
	/*
	������� �������������� ����������
	it - ������� ��������
	G0 - ��������� �������� �������������� ����������
	*/
	double d = 0.1; //��������� �������� ���������
	return G0*std::pow((1 / (it + 1)), d);
}

void updateAgents(vector<vector<double>>& pos, vector<vector<double>> acc, vector<vector<double>>& vel)
{
	/*
	�������� ��������� � ��������� ������� �������
	pos - ������ ��������� ������� ������� 
	acc -  ������ ��������� �������
	vel - ������ ��������� �������
	*/
	for (size_t i = 0; i < vel.size(); i++)
	{
		for (size_t j = 0; j < pos[i].size(); j++)
		{
			vel[i][j] = rand() / RAND_MAX * vel[i][j] + acc[i][j];
			pos[i][j] = pos[i][j] + vel[i][j];
		}
	}
}

vector<double> evaluate(vector<vector<double>> pos, double(*func)(vector<double>))
{
	/*
	������ �������
	pos - ������ ��������� ������� �������
	func - ��������� �� ������� �������
	*/
	vector<double> fitness(pos.size());		//������-�������
	for (size_t i = 0; i < pos.size(); i++)
	{
		vector<double> agent(pos[i].size());
		for (size_t j = 0; j < pos[i].size(); j++)
		{
			agent[j] = pos[i][j];
		}
		fitness[i] = func(agent);
	}
	return fitness;
}