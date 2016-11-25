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

#include "ga.h"
#include "random.h"
#include "bestSolution.h"

//生成随机个体
vector<pair<Flight *, vector<GatePosition*>>> GeneticAlgorithm::GenerateRandomTour()
{
	vector<pair<Flight *, vector<GatePosition*>>> oneRandomTour;
	for (map<string, Flight *>::iterator iter = Flight::s_mapSetOfFlight.begin(); iter != Flight::s_mapSetOfFlight.end(); iter++)
	{
		Flight *flight = iter->second;
		vector<GatePosition*> possibleGatePositionVector(flight->possibleGatePosition.size(), NULL);
		vector<GatePosition*> possibleGatePositionVectorTemp;
		vector<int> possibleGatePositionCountNo = Random::GetRandomSequence(flight->possibleGatePosition.size());
		for (map<string, GatePosition*>::iterator iter2 = flight->possibleGatePosition.begin(); iter2 != flight->possibleGatePosition.end(); iter2++)
			possibleGatePositionVectorTemp.push_back(iter2->second);
		for (int i = 0; i < possibleGatePositionVectorTemp.size(); i++)
			possibleGatePositionVector[possibleGatePositionCountNo[i]] = possibleGatePositionVectorTemp[i];
		oneRandomTour.push_back(make_pair(flight, possibleGatePositionVector));
	}
	return oneRandomTour;
};

vector<int> GeneticAlgorithm::GenerateRandomTour2()
{
	vector<int> tour;
	int  numUnVisitedFlights = Flight::s_flightCount - 1;				//未加入解中航班的计数器
	vector<int> unVisitedFlights;									//未加入解中航班列表		

	//未加入的解中的航班列表初始化
	for (int i = 0; i < Flight::s_flightCount; i++)
	{
		tour.push_back(0);
		unVisitedFlights.push_back(i);
	}

	for (int i = 0; i < Flight::s_flightCount; i++)						//作总航班个数次循环
	{
		int index = Random::GetRandom(0, numUnVisitedFlights--);	//随机生成一个航班
		tour[i] = unVisitedFlights[index];					//将该航班添加到当前解的当前位置
		for (int j = index; j <= numUnVisitedFlights; j++)	//在未加入的航班列表中删除该航班
			unVisitedFlights[j] = unVisitedFlights[j + 1];
	}
	return tour;
}

//计算目标函数值
void GeneticAlgorithm::ComputeFitness()
{
	for (int i = 0; i < PopSize; i++)
	{
		GatePosition::clearResult();
		Road::clearResult();
		Flight::clearResult();
		for (int j = 0; j < individual[i].size(); j++)
		{
			Flight *flight = individual[i][j].first;
			time_t flightInTime_t = flight->m_inTime_t;		// 进机位时间，时间格式
			time_t flightOutTime_t = flight->m_outTime_t;		// 出机位时间，时间格式
			vector<GatePosition*> &possibleGatePositionVector = individual[i][j].second;
			for (vector<GatePosition*>::iterator iter2 = possibleGatePositionVector.begin(); iter2 != possibleGatePositionVector.end(); iter2++)
			{
				GatePosition *gatePosition = *iter2;
				if (gatePosition->m_mapOfStartTimeToFinishTime.find(flightInTime_t) != gatePosition->m_mapOfStartTimeToFinishTime.end())
					continue;
				gatePosition->m_mapOfStartTimeToFinishTime.insert(make_pair(flightInTime_t, flightOutTime_t));
				map<time_t, time_t>::iterator preiter3 = --gatePosition->m_mapOfStartTimeToFinishTime.find(flightInTime_t);
				map<time_t, time_t>::iterator iter3 = gatePosition->m_mapOfStartTimeToFinishTime.find(flightInTime_t);
				map<time_t, time_t>::iterator nextiter3 = ++gatePosition->m_mapOfStartTimeToFinishTime.find(flightInTime_t);
				if (preiter3 != --gatePosition->m_mapOfStartTimeToFinishTime.begin() && flightInTime_t < preiter3->second + 8 * 60)
				{
					gatePosition->m_mapOfStartTimeToFinishTime.erase(iter3);
					continue;
				}
				if (nextiter3 != gatePosition->m_mapOfStartTimeToFinishTime.end() && flightOutTime_t > nextiter3->first - 8 * 60)
				{
					gatePosition->m_mapOfStartTimeToFinishTime.erase(iter3);
					continue;
				}
				gatePosition->m_mapOfStartTimeToFlight.insert(make_pair(flightInTime_t, flight));
				Road *road = gatePosition->m_road;
				road->m_vectorOfStartTimeToFinishTime.push_back(make_pair(flightInTime_t - 5 * 60, flightInTime_t));
				road->m_vectorOfStartTimeToFinishTime.push_back(make_pair(flightOutTime_t, flightOutTime_t + 5 * 60));
				road->m_vectorOfStartTimeToFlight.push_back(make_pair(flightInTime_t - 5 * 60, flight));
				road->m_vectorOfStartTimeToFlight.push_back(make_pair(flightOutTime_t, flight));
				flight->gatePosition = gatePosition;
				break;
			}
		}
		fitness[i] = BestSolution::getObjectiveFunction();
		if (fitness[i] > BestSolution::m_objValue)
		{
			cout << "最优目标函数值已更新：" << BestSolution::m_objValue << " -> " << fitness[i] << endl;
			BestSolution::m_objValue = fitness[i];
			BestSolution::m_flightNo.clear();
			BestSolution::m_inTime.clear();
			BestSolution::m_outTime.clear();
			BestSolution::m_gatePositionNo.clear();
			for (map<string, Flight *>::iterator iter = Flight::s_mapSetOfFlight.begin(); iter != Flight::s_mapSetOfFlight.end(); iter++)
			{
				Flight *flight = iter->second;
				if (flight->gatePosition != NULL)
				{
					BestSolution::m_flightNo.push_back(flight->m_flightNo);
					BestSolution::m_inTime.push_back(flight->m_inTime);
					BestSolution::m_outTime.push_back(flight->m_outTime);
					BestSolution::m_gatePositionNo.push_back(flight->gatePosition->m_gatePositionNo);
				}
				//else
				//{
				//	BestSolution::m_flightNo.push_back(flight->m_flightNo);
				//	BestSolution::m_inTime.push_back(flight->m_inTime);
				//	BestSolution::m_outTime.push_back(flight->m_outTime);
				//	BestSolution::m_gatePositionNo.push_back("未分配");
				//}
			}
		}
	}
};

//计算目标函数值2
void GeneticAlgorithm::ComputeFitness2()
{
	for (int i = 0; i < PopSize; i++)
	{
		GatePosition::clearResult();
		Road::clearResult();
		Flight::clearResult();
		for (int k = 0; k < individual[i].size(); k++)
		{
			int j = individual2[i][k];
			Flight *flight = individual[i][j].first;
			time_t flightInTime_t = flight->m_inTime_t;		// 进机位时间，时间格式
			time_t flightOutTime_t = flight->m_outTime_t;		// 出机位时间，时间格式
			vector<GatePosition*> &possibleGatePositionVector = individual[i][j].second;
			for (vector<GatePosition*>::iterator iter2 = possibleGatePositionVector.begin(); iter2 != possibleGatePositionVector.end(); iter2++)
			{
				GatePosition *gatePosition = *iter2;
				if (gatePosition->m_mapOfStartTimeToFinishTime.find(flightInTime_t) != gatePosition->m_mapOfStartTimeToFinishTime.end())
					continue;
				gatePosition->m_mapOfStartTimeToFinishTime.insert(make_pair(flightInTime_t, flightOutTime_t));
				map<time_t, time_t>::iterator preiter3 = --gatePosition->m_mapOfStartTimeToFinishTime.find(flightInTime_t);
				map<time_t, time_t>::iterator iter3 = gatePosition->m_mapOfStartTimeToFinishTime.find(flightInTime_t);
				map<time_t, time_t>::iterator nextiter3 = ++gatePosition->m_mapOfStartTimeToFinishTime.find(flightInTime_t);
				if (preiter3 != --gatePosition->m_mapOfStartTimeToFinishTime.begin() && flightInTime_t < preiter3->second + 8 * 60)
				{
					gatePosition->m_mapOfStartTimeToFinishTime.erase(iter3);
					continue;
				}
				if (nextiter3 != gatePosition->m_mapOfStartTimeToFinishTime.end() && flightOutTime_t > nextiter3->first - 8 * 60)
				{
					gatePosition->m_mapOfStartTimeToFinishTime.erase(iter3);
					continue;
				}
				gatePosition->m_mapOfStartTimeToFlight.insert(make_pair(flightInTime_t, flight));
				Road *road = gatePosition->m_road;
				road->m_vectorOfStartTimeToFinishTime.push_back(make_pair(flightInTime_t - 5 * 60, flightInTime_t));
				road->m_vectorOfStartTimeToFinishTime.push_back(make_pair(flightOutTime_t, flightOutTime_t + 5 * 60));
				road->m_vectorOfStartTimeToFlight.push_back(make_pair(flightInTime_t - 5 * 60, flight));
				road->m_vectorOfStartTimeToFlight.push_back(make_pair(flightOutTime_t, flight));
				flight->gatePosition = gatePosition;
				break;
			}
		}
		fitness[i] = BestSolution::getObjectiveFunction();
		if (fitness[i] > BestSolution::m_objValue)
		{
			cout << "最优目标函数值已更新：" << BestSolution::m_objValue << " -> " << fitness[i] << endl;
			BestSolution::m_objValue = fitness[i];
			BestSolution::m_flightNo.clear();
			BestSolution::m_inTime.clear();
			BestSolution::m_outTime.clear();
			BestSolution::m_gatePositionNo.clear();
			for (map<string, Flight *>::iterator iter = Flight::s_mapSetOfFlight.begin(); iter != Flight::s_mapSetOfFlight.end(); iter++)
			{
				Flight *flight = iter->second;
				if (flight->gatePosition != NULL)
				{
					BestSolution::m_flightNo.push_back(flight->m_flightNo);
					BestSolution::m_inTime.push_back(flight->m_inTime);
					BestSolution::m_outTime.push_back(flight->m_outTime);
					BestSolution::m_gatePositionNo.push_back(flight->gatePosition->m_gatePositionNo);
				}
				//else
				//{
				//	BestSolution::m_flightNo.push_back(flight->m_flightNo);
				//	BestSolution::m_inTime.push_back(flight->m_inTime);
				//	BestSolution::m_outTime.push_back(flight->m_outTime);
				//	BestSolution::m_gatePositionNo.push_back("未分配");
				//}
			}
		}
	}
};

//制作轮盘赌的轮盘
void GeneticAlgorithm::BuildWheel()
{
	double sum = 0;
	for (int i = 0; i < PopSize; i++)						//计算种群每个个体目标函数值之和
	{
		sum += fitness[i];
		prob.push_back(0);
	}
	prob[0] = (double)fitness[0] / (double)sum;				//第一个个体占总和的分数
	for( int i = 1 ; i < PopSize ; i++ )					//计算每个个体占总和的累计分数
		prob[i] = prob[i-1] + fitness[i] / sum;
};

//轮盘赌方式选取一个个体
int  GeneticAlgorithm::SelectIndividual()
{
	double p = Random::GetRandom();
	if( p < prob[0] )
		return 0;
	if( p >= prob[PopSize - 1] )
		return (PopSize - 1);
	for( int i = 1 ; i < PopSize ; i++ )
		if( (p >= prob[i-1]) && (p < prob[i]) )
			return i;
	return -1;
};

//从lo到hi做交叉
void GeneticAlgorithm::PMX(int lo, int hi, vector<pair<Flight *, vector<GatePosition*>>> *tour1, vector<pair<Flight *, vector<GatePosition*>>> *tour2, vector<pair<Flight *, vector<GatePosition*>>> *newtour1, vector<pair<Flight *, vector<GatePosition*>>> *newtour2)
{
	int j;
	CopySolution(tour1, newtour1);
	CopySolution(tour2, newtour2);

	j = lo;
	while( j <= hi )
	{
		pair<Flight *, vector<GatePosition*>> temp = (*newtour1)[j];
		(*newtour1)[j] = (*newtour2)[j];
		(*newtour2)[j] = temp;
		j++;
	}
};

//从lo到hi做交叉2
void GeneticAlgorithm::PMX2(int lo, int hi, vector<int> *tour1, vector<int> *tour2, vector<int> *newtour1, vector<int> *newtour2)
{
	int j, pos1, pos2;
	CopySolution2(tour1, newtour1);
	CopySolution2(tour2, newtour2);

	j = lo;
	while (j <= hi)
	{
		pos1 = FindFlight((*tour2)[j], newtour1);
		pos2 = FindFlight((*tour1)[j], newtour2);
		SwapOperator(j, pos1, newtour1);
		SwapOperator(j, pos2, newtour2);
		j++;
	}
};

//变异
void GeneticAlgorithm::Mutation()
{
	for(int i = 0; i < PopSize; i++)					//对种群中每个个体做循环
	{
		vector<pair<Flight *, vector<GatePosition*>>>::iterator iter3 = individual[i].begin();
		if( Random::GetRandom() < Pm )							//变异概率
		{
			int lo = Random::GetRandom(0, Flight::s_flightCount - 1);	//随机两个数作为变异段的起始点和终止点
			int hi = Random::GetRandom(0, Flight::s_flightCount - 1);
			while (lo == hi)
				hi = Random::GetRandom(0, Flight::s_flightCount - 1);
			int min = lo < hi ? lo : hi;
			int max = lo < hi ? hi : lo;
			lo = min;
			hi = max;
			vector<pair<Flight *, vector<GatePosition*>>> oneRandomTour;
			int j = 0;
			for (map<string, Flight *>::iterator iter = Flight::s_mapSetOfFlight.begin(); iter != Flight::s_mapSetOfFlight.end(); iter++)
			{
				if (j >= lo && j <= hi)
				{
					Flight *flight = iter->second;
					vector<GatePosition*> possibleGatePositionVector(flight->possibleGatePosition.size(), NULL);
					vector<GatePosition*> possibleGatePositionVectorTemp;
					vector<int> possibleGatePositionCountNo = Random::GetRandomSequence(flight->possibleGatePosition.size());
					for (map<string, GatePosition*>::iterator iter2 = flight->possibleGatePosition.begin(); iter2 != flight->possibleGatePosition.end(); iter2++)
						possibleGatePositionVectorTemp.push_back(iter2->second);
					for (int k = 0; k < possibleGatePositionVectorTemp.size(); k++)
						possibleGatePositionVector[possibleGatePositionCountNo[k]] = possibleGatePositionVectorTemp[k];
					oneRandomTour.push_back(make_pair(flight, possibleGatePositionVector));
				}
				else
				{
					oneRandomTour.push_back(iter3[j]);
				}
				j++;
			}
			individual[i] = oneRandomTour;
		}
	}
};

//变异2
void GeneticAlgorithm::Mutation2()
{
	for (int i = 0; i < PopSize; i++)					//对种群中每个个体做循环
	{
		if (Random::GetRandom() < Pm)							//变异概率
		{
			if (Random::GetRandom() < Pm1or2)		// 第1种变异 or 第2种变异
			{
				vector<pair<Flight *, vector<GatePosition*>>>::iterator iter3 = individual[i].begin();
				int lo = Random::GetRandom(0, Flight::s_flightCount - 1);	//随机两个数作为变异段的起始点和终止点
				int hi = Random::GetRandom(0, Flight::s_flightCount - 1);
				while (lo == hi)
					hi = Random::GetRandom(0, Flight::s_flightCount - 1);
				int min = lo < hi ? lo : hi;
				int max = lo < hi ? hi : lo;
				lo = min;
				hi = max;
				vector<pair<Flight *, vector<GatePosition*>>> oneRandomTour;
				int j = 0;
				for (map<string, Flight *>::iterator iter = Flight::s_mapSetOfFlight.begin(); iter != Flight::s_mapSetOfFlight.end(); iter++)
				{
					if (j >= lo && j <= hi)
					{
						Flight *flight = iter->second;
						vector<GatePosition*> possibleGatePositionVector(flight->possibleGatePosition.size(), NULL);
						vector<GatePosition*> possibleGatePositionVectorTemp;
						vector<int> possibleGatePositionCountNo = Random::GetRandomSequence(flight->possibleGatePosition.size());
						for (map<string, GatePosition*>::iterator iter2 = flight->possibleGatePosition.begin(); iter2 != flight->possibleGatePosition.end(); iter2++)
							possibleGatePositionVectorTemp.push_back(iter2->second);
						for (int k = 0; k < possibleGatePositionVectorTemp.size(); k++)
							possibleGatePositionVector[possibleGatePositionCountNo[k]] = possibleGatePositionVectorTemp[k];
						oneRandomTour.push_back(make_pair(flight, possibleGatePositionVector));
					}
					else
					{
						oneRandomTour.push_back(iter3[j]);
					}
					j++;
				}
				individual[i] = oneRandomTour;
			}
			else
			{
				int i1 = Random::GetRandom(0, Flight::s_flightCount - 1);	//随机生成两个数
				int i2 = Random::GetRandom(0, Flight::s_flightCount - 1);
				while (i1 == i2)
					i2 = Random::GetRandom(0, Flight::s_flightCount - 1);

				SwapOperator(i1, i2, &individual2[i]);		//对解中这两个位置的数交换
			}
		}
	}
};

//初始化
void GeneticAlgorithm::Initialize()
{
	for (int i = 0; i < PopSize / 2; i++)
	{
		individual.push_back(Flight::initPossibleGatePosition);
		fitness.push_back(0);
	}
	for (int i = PopSize / 2; i < PopSize; i++)		//给初始种群生成PopSize个个体
	{
		individual.push_back(GenerateRandomTour());
		fitness.push_back(0);
	}
	for (int i = PopSize; i < 2 * PopSize; i++)		//给初始种群生成2 * PopSize个个体
	{
		individual.push_back(individual[i - PopSize]);
		fitness.push_back(0);
	}
};

//初始化2
void GeneticAlgorithm::Initialize2()
{
	for (int i = 0; i < PopSize / 2; i++)
	{
		individual.push_back(Flight::initPossibleGatePosition);
		individual2.push_back(GenerateRandomTour2());
		fitness.push_back(0);
	}
	for (int i = PopSize / 2; i < PopSize; i++)		//给初始种群生成PopSize个个体
	{
		individual.push_back(GenerateRandomTour());
		individual2.push_back(GenerateRandomTour2());
		fitness.push_back(0);
	}
	for (int i = PopSize; i < 2 * PopSize; i++)		//给初始种群生成2 * PopSize个个体
	{
		individual.push_back(individual[i - PopSize]);
		individual2.push_back(individual2[i - PopSize]);
		fitness.push_back(0);
	}
};

//选择和交叉
void GeneticAlgorithm::SelectionAndCrossover()
{
	BuildWheel();										//制作轮盘赌的轮盘
	for(int i = PopSize; i < 2 * PopSize; i += 2)		
	{
		int father = SelectIndividual();				//轮盘赌方式选取一个个体
		int mother = SelectIndividual();				//轮盘赌方式选取另一个个体
		
		if (Random::GetRandom() < Pc)							//如果满足交叉概率
		{
			int lo = Random::GetRandom(0, Flight::s_flightCount - 1);	//随机两个数作为交叉段的起始点和终止点
			int hi = Random::GetRandom(0, Flight::s_flightCount - 1);
			while( lo == hi)
				hi = Random::GetRandom(0, Flight::s_flightCount - 1);
			int min = lo < hi ? lo : hi;
			int max = lo < hi ? hi : lo;
			lo = min;
			hi = max;
			//从lo到hi这段长度对father和mother做交叉然后存到indevidual中
			PMX(lo, hi, &individual[father], &individual[mother], &individual[i], &individual[i+1]);
		}
		else		//如果不满足交叉概率
		{
			CopySolution(&individual[father], &individual[i]  );
			CopySolution(&individual[mother], &individual[i+1]);
		}
	}

	for(int i = 0; i < PopSize; i++)					//更新整个种群
		CopySolution(&individual[PopSize+i], &individual[i]);
}

//选择和交叉2
void GeneticAlgorithm::SelectionAndCrossover2()
{
	BuildWheel();										//制作轮盘赌的轮盘
	for (int i = PopSize; i < 2 * PopSize; i += 2)
	{
		int father = SelectIndividual();				//轮盘赌方式选取一个个体
		int mother = SelectIndividual();				//轮盘赌方式选取另一个个体

		if (Random::GetRandom() < Pc)							//如果满足交叉概率
		{
			int lo = Random::GetRandom(0, Flight::s_flightCount - 1);	//随机两个数作为交叉段的起始点和终止点
			int hi = Random::GetRandom(0, Flight::s_flightCount - 1);
			while (lo == hi)
				hi = Random::GetRandom(0, Flight::s_flightCount - 1);
			int min = lo < hi ? lo : hi;
			int max = lo < hi ? hi : lo;
			lo = min;
			hi = max;
			//从lo到hi这段长度对father和mother做交叉然后存到indevidual中
			if (Random::GetRandom() < Pc1or2)
				PMX(lo, hi, &individual[father], &individual[mother], &individual[i], &individual[i + 1]);
			else
				PMX2(lo, hi, &individual2[father], &individual2[mother], &individual2[i], &individual2[i + 1]);
		}
		else		//如果不满足交叉概率
		{
			CopySolution(&individual[father], &individual[i]);
			CopySolution2(&individual2[father], &individual2[i]);
			CopySolution(&individual[mother], &individual[i + 1]);
			CopySolution2(&individual2[mother], &individual2[i + 1]);
		}
	}

	for (int i = 0; i < PopSize; i++)					//更新整个种群
	{
		CopySolution(&individual[PopSize + i], &individual[i]);
		CopySolution2(&individual2[PopSize + i], &individual2[i]);
	}
}

void GeneticAlgorithm::CopySolution(vector<pair<Flight *, vector<GatePosition*>>> *from, vector<pair<Flight *, vector<GatePosition*>>> *to)//将解from复制到to中
{
	for (int i = 0; i < Flight::s_flightCount; i++)
		(*to)[i] = (*from)[i];
}

void GeneticAlgorithm::CopySolution2(vector<int> *from, vector<int> *to)//将解from复制到to中
{
	for (int i = 0; i < Flight::s_flightCount; i++)
		(*to)[i] = (*from)[i];
}

int GeneticAlgorithm::FindFlight(int flight, vector<int> *tour)//根据城市编号在解中寻找城市的位置
{
	int j = 0;
	while ((*tour)[j] != flight)
		j++;
	return j;
};

void GeneticAlgorithm::SwapOperator(int city1, int city2, vector<int> *tour)//邻域交换
{
	int temp = (*tour)[city1];
	(*tour)[city1] = (*tour)[city2];
	(*tour)[city2] = temp;
};

//遗传算法主函数
void GeneticAlgorithm::GeneticAlgorithmMain()
{
	Initialize2();				//初始化
	ComputeFitness2();				//计算初始化所得的种群的目标函数值

	for(int g = 1; g <= Gen; g++)	//循环总次数
	{
		cout << "遗传算法第 " << g << " 次迭代中……" << endl;
		SelectionAndCrossover2();	//选择和交叉
		Mutation2();				//变异
		ComputeFitness2();			//更新新种群对应每个个体的目标函数值
	}
}

int		GeneticAlgorithm::PopSize = 80;
int		GeneticAlgorithm::Gen = 300;
double	GeneticAlgorithm::Pc = 0.5;
double	GeneticAlgorithm::Pm = 0.2;

double	GeneticAlgorithm::Pc1or2 = 0.5;
double	GeneticAlgorithm::Pm1or2 = 0.5;