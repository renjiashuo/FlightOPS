/*************************************************
版权: 任家硕
作者: 任家硕
日期: 2016-10-16
功能: 机场停机位资源分配优化_遗传算法类
修改历史:

**************************************************/

/*<remark>=========================================================
/// <summary>
/// 机场停机位资源分配优化_遗传算法类
/// </summary>
===========================================================</remark>*/

#pragma once

#include "basic.h"
#include "flight.h"

#pragma region 新建的类
////////////////////////////////////////////////////////////////////////
class GeneticAlgorithm;// 遗传算法类
////////////////////////////////////////////////////////////////////////
#pragma endregion

class GeneticAlgorithm
{

#pragma region Variables

public:// 算法常量
	static int		PopSize;									// 种群大小
	static int		Gen;										// 循环次数
	static double		Pc;										// 交叉概率
	static double		Pm;										// 变异概率

	static double		Pc1or2;									// 交叉种类概率
	static double		Pm1or2;									// 交叉种类概率

public:// 算法变量
	vector<vector<pair<Flight *, vector<GatePosition*>>>>		individual;	// 种群变量1
	vector<vector<int>>									individual2;	// 种群变量2
	vector<double>										fitness;		// 种群目标函数值
	vector<double>										prob;		// 轮盘赌转盘

	//int    individual[2 * PopSize][MaxNumOfCities];				// 种群
	//int    fitness[2 * PopSize];								// 种群目标函数值
	//double prob[PopSize];

#pragma endregion

#pragma region Functions

void BuildWheel();
void ComputeFitness();
void ComputeFitness2();
vector<pair<Flight *, vector<GatePosition*>>> GenerateRandomTour();
vector<int> GenerateRandomTour2();
int  SelectIndividual();
void PMX(int lo, int hi, vector<pair<Flight *, vector<GatePosition*>>> *tour1, vector<pair<Flight *, vector<GatePosition*>>> *tour2, vector<pair<Flight *, vector<GatePosition*>>> *newtour1, vector<pair<Flight *, vector<GatePosition*>>> *newtour2);
void PMX2(int lo, int hi, vector<int> *tour1, vector<int> *tour2, vector<int> *newtour1, vector<int> *newtour2);
void Mutation();
void Mutation2();

void Initialize();
void Initialize2();

void SelectionAndCrossover();
void SelectionAndCrossover2();

void CopySolution(vector<pair<Flight *, vector<GatePosition*>>> *from, vector<pair<Flight *, vector<GatePosition*>>> *to);
void CopySolution2(vector<int> *from, vector<int> *to);

int FindFlight(int flight, vector<int> *tour);
void SwapOperator(int city1, int city2, vector<int> *tour);

void GeneticAlgorithmMain();

#pragma endregion

};