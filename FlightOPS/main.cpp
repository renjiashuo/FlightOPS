#include "flight.h"
#include "gatePosition.h"
#include "bestSolution.h"

#pragma once

void solveByCplex();

int main()
{
	#pragma region 调试使用
	//////////////////////////////////////////////////////////////////////////
	{
		Flight::s_mapSetOfFlight;
		GatePosition::s_mapSetOfGatePosition;
		Road::s_mapSetOfRoad;
		BestSolution::m_solution;
	}
	////////////////////////////////////////////////////////////////////////
	#pragma endregion

	#pragma region 初始化数据
	////////////////////////////////////////////////////////////////////////
	clock_t start, finish, time1, time2;
	double duration1, duration2, duration3, duration4, duration5, duration6, duration7;
	start = clock();							//开始计时(总)
	time1 = clock();
	Flight::readData();
	GatePosition::readData();
	Flight::initNumberCount();
	Road::initRoad();
	GatePosition::initBeConflict();
	time2 = clock();
	duration1 = (time2 - time1) / (double)1000;
	cout << "初始化数据用时 : " << duration1 << " 秒" << endl;	//显示花费时间
	////////////////////////////////////////////////////////////////////////
	#pragma endregion

	for (static map<string, Flight *>::iterator iter = Flight::s_mapSetOfFlight.begin(); iter != Flight::s_mapSetOfFlight.end(); iter++)
	{
		if (iter->second->m_inTime_t == -1 || iter->second->m_outTime_t == -1)
		{
			cout << "error";
		}
	}
	for (static map<string, Flight *>::iterator iter = Flight::s_mapSetOfwrongFlight.begin(); iter != Flight::s_mapSetOfwrongFlight.end(); iter++)
	{
		if (iter->second->m_inTime_t == -1 || iter->second->m_outTime_t == -1)
		{
			cout << "error";
		}
	}

	#pragma region 获得航班可以使用的所有机位
	////////////////////////////////////////////////////////////////////////
	time1 = clock();
	Flight::GetFeasibleGatePosition();
	time2 = clock();
	duration2 = (time2 - time1) / (double)1000;
	cout << "获得所有航班可用机位用时 : " << duration2 << " 秒" << endl;	//显示花费时间
	////////////////////////////////////////////////////////////////////////
	#pragma endregion

	#pragma region 获得机位可以停放的所有航班
	////////////////////////////////////////////////////////////////////////
	time1 = clock();
	GatePosition::GetFeasibleFlight();
	time2 = clock();
	duration3 = (time2 - time1) / (double)1000;
	cout << "获得机位可以停放的所有航班用时 : " << duration3 << " 秒" << endl;	//显示花费时间
	////////////////////////////////////////////////////////////////////////
	#pragma endregion

	#pragma region 输出cplex求解所需输入
	//////////////////////////////////////////////////////////////////////////;
	{
		time1 = clock();
		string str = "NMR.txt";
		ofstream NM(str);
		NM << Flight::s_flightCount << " " << GatePosition::s_gatePositionCount << " " << Road::s_roadCount << " " << Flight::s_nemberCount << endl;
		NM << "[";
		int i = 0;
		for (map<string, Road *>::iterator iter = Road::s_mapSetOfRoad.begin(); iter != Road::s_mapSetOfRoad.end(); iter++)
		{
			if (iter->first == "无限制" || iter->first == "")
				NM << 0;
			else
				NM << 1;
			i++;
			if (i == Road::s_roadCount)
				NM << "]";
			else
				NM << ",";
		}
		NM.close();
		Flight::printTime();
		GatePosition::PrintFeasibleFlight();
		GatePosition::PrintDistance();
		Flight::printRoadTime();
		Road::printGatePositionToRoad();
		Flight::printConflictFlight();
		Flight::printNumberOfFlight();
		GatePosition::PrintTemporaryMeasures();
		GatePosition::PrintConflictGatePosition();
		GatePosition::PrintInfluenceGatePosition();
		time2 = clock();
		duration4 = (time2 - time1) / (double)1000;
		cout << "输出cplex求解所需输入用时 : " << duration4 << " 秒" << endl;	//显示花费时间
	}
	////////////////////////////////////////////////////////////////////////
	#pragma endregion

	#pragma region CPLEX求解
	//////////////////////////////////////////////////////////////////////////
	time1 = clock();
	solveByCplex();
	time2 = clock();
	duration5 = (time2 - time1) / (double)1000;
	cout << "CPLEX求解用时 : " << duration5 << " 秒" << endl;	//显示花费时间
	////////////////////////////////////////////////////////////////////////
	#pragma endregion

	#pragma region 打印结果
	//////////////////////////////////////////////////////////////////////////
	time1 = clock();
	BestSolution::updateValueByCplexSolver();
	BestSolution::deleteWrongValue();
	BestSolution::heuristicForUndistributed();
	BestSolution::updateBestValue();
	BestSolution::printBestResult();
	string str = "";
	ostringstream oss;
	oss << BestSolution::m_objValue;
	str = oss.str();
	string result = "usedTimeWhenBestResult=" + str + ".txt";
	ofstream fout(result);
	time2 = clock();
	duration6 = (time2 - time1) / (double)1000;
	cout << "存储结果到csv文件用时 : " << duration6 << " 秒" << endl;	//显示花费时间
	//////////////////////////////////////////////////////////////////////////
	#pragma endregion

	#pragma region 统计花费时间
	//////////////////////////////////////////////////////////////////////////
	finish = clock();								//终止计时(总)
	duration7 = duration1 + duration2 + duration3 + duration4 + duration5 + duration6;
	cout << "程序总用时 : " << duration7 << " 秒" << endl;	//显示花费时间
	//////////////////////////////////////////////////////////////////////////
	#pragma endregion

	#pragma region 输出各段时间到文件
	//////////////////////////////////////////////////////////////////////////
	fout << "初始化数据用时 : " << duration1 << " 秒" << endl;	//显示花费时间
	fout << "获得所有航班可用机位用时 : " << duration2 << " 秒" << endl;	//显示花费时间
	fout << "获得机位可以停放的所有航班用时 : " << duration3 << " 秒" << endl;	//显示花费时间
	fout << "输出cplex求解所需输入用时 : " << duration4 << " 秒" << endl;	//显示花费时间
	fout << "CPLEX求解用时 : " << duration5 << " 秒" << endl;	//显示花费时间
	fout << "存储结果到csv文件用时 : " << duration6 << " 秒" << endl;	//显示花费时间
	fout << "程序总用时 : " << duration7 << " 秒" << endl;	//显示花费时间
	////////////////////////////////////////////////////////////////////////
	#pragma endregion

	system("pause");
	return 0;
}

void solveByCplex()
{
	// 定义CPLEX环境
	IloEnv env;

	try {
		int i, j, k;
		int parameter1 = 10;
		int parameter2 = 3;
		int parameter3 = 3;
		int parameter4 = 1;
		int parameter5 = 1;

		// 从文件中读取数据
		ifstream f("NMR.txt");									// 航班数量(N)、停机位数量(M)、滑道数量(R)、滑道是否为无限制(1*R)
		ifstream flightTime("flightTime.txt");					// 航班进港时间(1*N)、出港时间(1*N)
		ifstream feasibleFlightFile("feasibleFlight.txt");			// 航班-机位可否停放(N*M)
		ifstream distanceFile("distance.txt");					// 机位为近机位1或远机位0(1*M)
		ifstream flightRoadTime("flightRoadTime.txt");			// 航班进港时进入滑道时间(1*N)、进港时离开滑道时间(1*N)、出港时进入滑道时间(1*N)、出港时离开滑道时间(1*N)
		ifstream gatePositionToRoadFile("gatePositionToRoad.txt");	// 机位-滑入滑道对应(M*R)、机位-推出滑道对应(M*R)
		ifstream conflictFlightFile("conflictFlight.txt");			// 滑道时间上冲突的航班(N*N)，不考虑安排的滑道及机位。分别打印航班A与航班B，滑入滑道冲突、推出滑道冲突、滑入与推出、推出与滑入
		ifstream numberOfFlightFile("numberOfFlight.txt");			// 每架航班的旅客数量(1*N)
		ifstream temporaryMeasuresFile("temporaryMeasures.txt");	// 机位是否为临时措施，是临时措施为1，不是为0(1*M)
		ifstream conflictGatePositionFile("conflictGatePosition.txt");	// 不能同时使用的机位(M*M)，若不能同时使用则为1
		ifstream influenceGatePositionFile("influenceGatePosition.txt");	// 所影响滑入的机位(M*M)，所影响滑出的机位(M*M)，若影响为1

		// 定义已知常量
		IloInt N, M, R;										// 航班数量(N)、停机位数量(M)、滑道数量(R)
		IloInt allNumber;										// 总旅客人数
		IloIntArray _tin(env);									// 航班进港时间(1*N)
		IloIntArray _tout(env);								// 航班出港时间(1*N)
		IloArray<IloIntArray> _feasibleFlight(env);				// 航班-机位可否停放(N*M)
		IloIntArray _distance(env);								// 机位为近机位1或远机位0(1*M)
		IloIntArray _tComeBegin(env);							// 航班进港时进入滑道时间(1*N)
		IloIntArray _tComeEnd(env);								// 航班进港时离开滑道时间(1*N)
		IloIntArray _tGoBegin(env);								// 航班出港时进入滑道时间(1*N)
		IloIntArray _tGoEnd(env);								// 航班出港时离开滑道时间(1*N)
		IloArray<IloIntArray> _gatePositionToInRoad(env);			// 机位-滑入滑道对应(M*R)
		IloArray<IloIntArray> _gatePositionToOutRoad(env);			// 机位-推出滑道对应(M*R)
		IloArray<IloIntArray> _conflictFlightInIn(env);			// 滑道时间上冲突的航班(N*N)，不考虑安排的滑道及机位，滑入滑道冲突
		IloArray<IloIntArray> _conflictFlightOutOut(env);			// 滑道时间上冲突的航班(N*N)，不考虑安排的滑道及机位，推出滑道冲突
		IloArray<IloIntArray> _conflictFlightInOut(env);			// 滑道时间上冲突的航班(N*N)，不考虑安排的滑道及机位，滑入与推出冲突
		IloArray<IloIntArray> _conflictFlightOutIn(env);			// 滑道时间上冲突的航班(N*N)，不考虑安排的滑道及机位，推出与滑入冲突
		IloIntArray _noLimit(env);								// 滑道是否为无限制(1*R)
		IloIntArray _numberOfFlight(env);						// 每架航班的旅客数量(1*N)
		IloIntArray _temporaryMeasures(env);						// 机位是否为临时措施，是临时措施为1，不是为0(1*M)
		IloArray<IloIntArray> _conflictGatePosition(env);			// 不能同时使用的机位(M*M)，若不能同时使用则为1
		IloArray<IloIntArray> _influenceGatePositionIn(env);		// 所影响滑入的机位(M*M)，若影响则为1
		IloArray<IloIntArray> _influenceGatePositionOut(env);		// 所影响滑出的机位(M*M)，若影响则为1

		f >> N >> M >> R >> allNumber >> _noLimit;
		flightTime >> _tin >> _tout;
		feasibleFlightFile >> _feasibleFlight;
		distanceFile >> _distance;
		flightRoadTime >> _tComeBegin >> _tComeEnd >> _tGoBegin >> _tGoEnd;
		gatePositionToRoadFile >> _gatePositionToInRoad >> _gatePositionToOutRoad;
		conflictFlightFile >> _conflictFlightInIn >> _conflictFlightOutOut >> _conflictFlightInOut >> _conflictFlightOutIn;
		numberOfFlightFile >> _numberOfFlight;
		temporaryMeasuresFile >> _temporaryMeasures;
		conflictGatePositionFile >> _conflictGatePosition;
		influenceGatePositionFile >> _influenceGatePositionIn >> _influenceGatePositionOut;

		// 定义决策变量输出
		IloArray<IloBoolArray> X(env, N);
		for (i = 0; i < N; i++)
		{
			X[i] = IloBoolArray(env, M);
			for (j = 0; j < M; j++)
			{
				X[i][j] = 0;
			}
		}
		IloArray<IloBoolArray> YIN(env, N);
		for (i = 0; i < N; i++)
		{
			YIN[i] = IloBoolArray(env, R);
			for (k = 0; k < R; k++)
			{
				YIN[i][k] = 0;
			}
		}
		IloArray<IloBoolArray> YOUT(env, N);
		for (i = 0; i < N; i++)
		{
			YOUT[i] = IloBoolArray(env, R);
			for (k = 0; k < R; k++)
			{
				YOUT[i][k] = 0;
			}
		}

		int N_once = 700;
		int N_once_2 = 300;
		int N_once_1 = N_once - N_once_2;
		bool runTimes =  true;
		int N_min = 0, N_max = 0;

		while (runTimes)
		{
			IloEnv env2;
			N_max += N_once_1;
			if (N_max >= N)
			{
				N_max = N;
				runTimes = false;
			}
			N_min = N_max - N_once;
			if (N_min < 0)
				N_min = 0;

			cout << "开始计算第 " << N_min + 1 << " 架到第 " << N_max << " 架航班" << endl;

			// 定义已知常量
			IloIntArray tin(env2, N_max - N_min);									// 航班进港时间(1*N)
			IloIntArray tout(env2, N_max - N_min);								// 航班出港时间(1*N)
			IloArray<IloIntArray> feasibleFlight(env2, N_max - N_min);				// 航班-机位可否停放(N*M)
			IloIntArray distance(env2, M);								// 机位为近机位1或远机位0(1*M)
			IloIntArray tComeBegin(env2, N_max - N_min);							// 航班进港时进入滑道时间(1*N)
			IloIntArray tComeEnd(env2, N_max - N_min);								// 航班进港时离开滑道时间(1*N)
			IloIntArray tGoBegin(env2, N_max - N_min);								// 航班出港时进入滑道时间(1*N)
			IloIntArray tGoEnd(env2, N_max - N_min);								// 航班出港时离开滑道时间(1*N)
			IloArray<IloIntArray> gatePositionToInRoad(env2);			// 机位-滑入滑道对应(M*R)
			IloArray<IloIntArray> gatePositionToOutRoad(env2);			// 机位-推出滑道对应(M*R)
			IloArray<IloIntArray> conflictFlightInIn(env2, N_max - N_min);			// 滑道时间上冲突的航班(N*N)，不考虑安排的滑道及机位，滑入滑道冲突
			IloArray<IloIntArray> conflictFlightOutOut(env2, N_max - N_min);			// 滑道时间上冲突的航班(N*N)，不考虑安排的滑道及机位，推出滑道冲突
			IloArray<IloIntArray> conflictFlightInOut(env2, N_max - N_min);			// 滑道时间上冲突的航班(N*N)，不考虑安排的滑道及机位，滑入与推出冲突
			IloArray<IloIntArray> conflictFlightOutIn(env2, N_max - N_min);			// 滑道时间上冲突的航班(N*N)，不考虑安排的滑道及机位，推出与滑入冲突
			IloIntArray noLimit(env2);								// 滑道是否为无限制(1*R)
			IloIntArray numberOfFlight(env2, N_max - N_min);						// 每架航班的旅客数量(1*N)
			IloIntArray temporaryMeasures(env2);						// 机位是否为临时措施，是临时措施为1，不是为0(1*M)
			IloArray<IloIntArray> conflictGatePosition(env2);			// 不能同时使用的机位(M*M)，若不能同时使用则为1
			IloArray<IloIntArray> influenceGatePositionIn(env2);		// 所影响滑入的机位(M*M)，若影响则为1
			IloArray<IloIntArray> influenceGatePositionOut(env2);		// 所影响滑出的机位(M*M)，若影响则为1

			for (int i1 = 0; i1 < N_max - N_min; i1++)
			{
				tin[i1] = _tin[i1 + N_min];
				tout[i1] = _tout[i1 + N_min];
				tComeBegin[i1] = _tComeBegin[i1 + N_min];
				tComeEnd[i1] = _tComeEnd[i1 + N_min];
				tGoBegin[i1] = _tGoBegin[i1 + N_min];
				tGoEnd[i1] = _tGoEnd[i1 + N_min];
				numberOfFlight[i1] = _numberOfFlight[i1 + N_min];
				feasibleFlight[i1] = IloIntArray(env2, M);
				for (j = 0; j < M; j++)
				{
					feasibleFlight[i1][j] = _feasibleFlight[i1 + N_min][j];
				}
				conflictFlightInIn[i1] = IloIntArray(env2, N_max - N_min);
				conflictFlightOutOut[i1] = IloIntArray(env2, N_max - N_min);
				conflictFlightInOut[i1] = IloIntArray(env2, N_max - N_min);
				conflictFlightOutIn[i1] = IloIntArray(env2, N_max - N_min);
				for (int i2 = 0; i2 < N_max - N_min; i2++)
				{
					conflictFlightInIn[i1][i2] = _conflictFlightInIn[i1 + N_min][i2 + N_min];
					conflictFlightOutOut[i1][i2] = _conflictFlightOutOut[i1 + N_min][i2 + N_min];
					conflictFlightInOut[i1][i2] = _conflictFlightInOut[i1 + N_min][i2 + N_min];
					conflictFlightOutIn[i1][i2] = _conflictFlightOutIn[i1 + N_min][i2 + N_min];
				}
			}
			distance = _distance;
			gatePositionToInRoad = _gatePositionToInRoad;
			gatePositionToOutRoad = _gatePositionToOutRoad;
			noLimit = _noLimit;
			temporaryMeasures = _temporaryMeasures;
			conflictGatePosition = _conflictGatePosition;
			influenceGatePositionIn = _influenceGatePositionIn;
			influenceGatePositionOut = _influenceGatePositionOut;

			// 定义模型
			IloModel model(env2);

			// 定义决策变量
			IloArray<IloBoolVarArray> x(env2, N_max - N_min);	// 航班i占用停机位j
			for (i = 0; i < N_max - N_min; i++)
			{
				x[i] = IloBoolVarArray(env2, M);
			}
			IloArray<IloBoolVarArray> yin(env2, N_max - N_min);	// 航班i滑入使用滑道k
			for (i = 0; i < N_max - N_min; i++)
			{
				yin[i] = IloBoolVarArray(env2, R);
			}
			IloArray<IloBoolVarArray> yout(env2, N_max - N_min);	// 航班i滑出使用滑道k
			for (i = 0; i < N_max - N_min; i++)
			{
				yout[i] = IloBoolVarArray(env2, R);
			}
			IloBoolVarArray z2(env2, N_max - N_min);// 航班i是否与其他航班冲突

			if (N_max - N_min > N_once_1)
			{
				int N_readNum = 0;
				int N_middle = N_max - N_min;
				if (N_middle < N_once)
					N_readNum = N_max - N_once_1;
				else
					N_readNum = N_once_2;
				// 读取上一次的解
				for (i = 0; i < N_readNum; i++)
				{
					for (j = 0; j < M; j++)
					{
						model.add(x[i][j] == X[i + N_min][j]);
					}
				}
				for (i = 0; i < N_readNum; i++)
				{
					for (k = 0; k < R; k++)
					{
						model.add(yin[i][k] == YIN[i + N_min][k]);
					}
				}
				for (i = 0; i < N_readNum; i++)
				{
					for (k = 0; k < R; k++)
					{
						model.add(yout[i][k] == YOUT[i + N_min][k]);
					}
				}
			}

			// 目标函数
			// 第一部分
			IloNumExprArray distributionFlightCountTemp(env2, N_max - N_min);
			for (i = 0; i < N_max - N_min; i++)
			{
				distributionFlightCountTemp[i] = IloNumExpr(env2);
				for (j = 0; j < M; j++)
				{
					distributionFlightCountTemp[i] = distributionFlightCountTemp[i] + x[i][j];
				}
			}
			IloNumExpr distributionFlightCount = IloSum(distributionFlightCountTemp) / (double)(N_max - N_min);
			//IloNumExpr distributionFlightCount = IloSum(distributionFlightCountTemp) / (double)(Flight::s_flightCount + Flight::s_wrongFlightCount);
			// 第二部分
			IloNumExprArray distanceGatePositionCountTemp(env2, M);
			for (j = 0; j < M; j++)
			{
				distanceGatePositionCountTemp[j] = IloNumExpr(env2);
				if (distance[j] == 1)
				{
					for (i = 0; i < N_max - N_min; i++)
					{
						distanceGatePositionCountTemp[j] = distanceGatePositionCountTemp[j] + x[i][j];
					}
				}
			}
			IloNumExpr distanceGatePositionCount = IloSum(distanceGatePositionCountTemp) / (double)(N_max - N_min);
			//IloNumExpr distanceGatePositionCount = IloSum(distanceGatePositionCountTemp) / (double)(Flight::s_flightCount + Flight::s_wrongFlightCount);
			// 第三部分
			IloNumExprArray distanceGatePositionNumberCountTemp(env2, M);
			for (j = 0; j < M; j++)
			{
				distanceGatePositionNumberCountTemp[j] = IloNumExpr(env2);
				if (distance[j] == 1)
				{
					for (i = 0; i < N_max - N_min; i++)
					{
						distanceGatePositionNumberCountTemp[j] = distanceGatePositionNumberCountTemp[j] + x[i][j] * numberOfFlight[i];
					}
				}
			}
			IloNumExpr distanceGatePositionNumberCount = IloSum(distanceGatePositionNumberCountTemp) / (double)(IloSum(numberOfFlight));
			// 第四部分
			IloNumExprArray temporaryMeasuresTemp(env2, M);
			for (j = 0; j < M; j++)
			{
				temporaryMeasuresTemp[j] = IloNumExpr(env2);
				if (temporaryMeasures[j] == 1)
				{
					for (i = 0; i < N_max - N_min; i++)
					{
						temporaryMeasuresTemp[j] = temporaryMeasuresTemp[j] + x[i][j];
					}
				}
			}
			IloNumExpr temporaryMeasuresSum = IloSum(temporaryMeasuresTemp) / (double)(N_max - N_min);
			// 第五部分
			for (i = 0; i < N_max - N_min - 1; i++)
			{
				for (j = i + 1; j < N_max - N_min; j++)
				{
					if (conflictFlightInIn[i][j] == 1)
					{
						for (k = 0; k < R; k++)
						{
							if (noLimit[k] == 1)
							{
								// 精度修改
								model.add(z2[i] >= yin[i][k] + yin[j][k] - 1.1);
								model.add(z2[j] >= yin[i][k] + yin[j][k] - 1.1);
							}
						}
					}
					if (conflictFlightOutOut[i][j] == 1)
					{
						for (k = 0; k < R; k++)
						{
							if (noLimit[k] == 1)
							{
								// 精度修改
								model.add(z2[i] >= yout[i][k] + yout[j][k] - 1.1);
								model.add(z2[j] >= yout[i][k] + yout[j][k] - 1.1);
							}
						}
					}
					if (conflictFlightInOut[i][j] == 1)
					{
						for (k = 0; k < R; k++)
						{
							if (noLimit[k] == 1)
							{
								// 精度修改
								model.add(z2[i] >= yin[i][k] + yout[j][k] - 1.1);
								model.add(z2[j] >= yin[i][k] + yout[j][k] - 1.1);
							}
						}
					}
					if (conflictFlightOutIn[i][j] == 1)
					{
						for (k = 0; k < R; k++)
						{
							if (noLimit[k] == 1)
							{
								// 精度修改
								model.add(z2[i] >= yout[i][k] + yin[j][k] - 1.1);
								model.add(z2[j] >= yout[i][k] + yin[j][k] - 1.1);
							}
						}
					}
				}
			}
			IloNumExpr conflictCount = IloSum(z2) / (double)(N_max - N_min);

			// 添加约束条件
			model.add(IloMaximize(env2, parameter1 * distributionFlightCount + parameter2 * distanceGatePositionCount + parameter3 * distanceGatePositionNumberCount - parameter4 * temporaryMeasuresSum - parameter5 * conflictCount));

			// 约束条件1 每架航班只能选择一个机位
			IloNumExprArray xTemp1(env2, N_max - N_min);
			for (i = 0; i < N_max - N_min; i++)
			{
				xTemp1[i] = IloNumExpr(env2);
				for (j = 0; j < M; j++)
				{
					if (feasibleFlight[i][j] == 1)
						xTemp1[i] = xTemp1[i] + x[i][j];
					else
					{
						model.add(x[i][j] == 0);
					}
				}
				// 精度修改
				model.add(xTemp1[i] <= 1.1);
			}

			//for (i = 0; i < N_max - N_min; i++)
			//{
			//	for (j = 0; j < M; j++)
			//	{
			//		if (feasibleFlight[i][j] == 0)
			//			model.add(x[i][j] == 0);
			//	}
			//	// 精度修改
			//	model.add(IloSum(x[i]) <= 1.1);
			//}

			// 约束条件2 同一机位同一时刻只能停一架航班
			for (j = 0; j < M; j++)
			{
				for (int i1 = 0; i1 < N_max - N_min - 1; i1++)
				{
					for (int i2 = i1 + 1; i2 < N_max - N_min; i2++)
					{
						if ((feasibleFlight[i1][j] == 1) && (feasibleFlight[i2][j] == 1) && ((tin[i1] - tout[i2]) * (tin[i2] - tout[i1]) > 0))
						{
							// 精度修改
							model.add(x[i1][j] + x[i2][j] <= 1.1);
						}
					}
				}
			}

			// 约束条件3，航班所使用滑道的约束
			for (i = 0; i < N_max - N_min; i++)
			{
				if (IloSum(feasibleFlight[i]) == 0)
				{
					model.add(IloSum(yin[i]) == 0);
					model.add(IloSum(yout[i]) == 0);
					//for (k = 0; k < R; k++)
					//{
					//	model.add(yin[i][k] == 0);
					//	model.add(yout[i][k] == 0);
					//}
					continue;
				}
				model.add(IloSum(yin[i]) == IloSum(x[i]));
				model.add(IloSum(yout[i]) == IloSum(x[i]));
				for (j = 0; j < M; j++)
				{
					if (feasibleFlight[i][j] == 1)
					{
						for (k = 0; k < R; k++)
						{
							if (gatePositionToInRoad[j][k] == 1)
							{
								model.add(yin[i][k] >= x[i][j]);
							}
							if (gatePositionToOutRoad[j][k] == 1)
							{
								model.add(yout[i][k] >= x[i][j]);
							}
						}
					}
				}
			}

			// 约束条件4 冲突机位不能同时使用
			for (int j1 = 0; j1 < M; j1++)
			{
				for (int j2 = 0; j2 < M; j2++)
				{
					if (conflictGatePosition[j1][j2] == 1)
					{
						for (int i1 = 0; i1 < N_max - N_min; i1++)
						{
							for (int i2 = 0; i2 < N_max - N_min; i2++)
							{
								if ((feasibleFlight[i1][j1] == 1) && (feasibleFlight[i2][j2] == 1) && ((tin[i1] - tout[i2]) * (tin[i2] - tout[i1]) > 0))
								{
									// 精度修改
									model.add(x[i1][j1] + x[i2][j2] <= 1.1);
								}
							}
						}
					}
				}
			}

			// 约束条件5 特定机位会影响某些机位的滑入或滑出
			for (int j1 = 0; j1 < M; j1++)
			{
				for (int j2 = 0; j2 < M; j2++)
				{
					if (influenceGatePositionIn[j1][j2] == 1)
					{
						for (int i1 = 0; i1 < N_max - N_min; i1++)
						{
							for (int i2 = 0; i2 < N_max - N_min; i2++)
							{
								if ((feasibleFlight[i1][j1] == 1) && (feasibleFlight[i2][j2] == 1) && (tin[i1] < tin[i2]) && (tin[i2] < tout[i1]))
								{
									// 精度修改
									model.add(x[i1][j1] + x[i2][j2] <= 1.1);
								}
							}
						}
					}
					if (influenceGatePositionOut[j1][j2] == 1)
					{
						for (int i1 = 0; i1 < N_max - N_min; i1++)
						{
							for (int i2 = 0; i2 < N_max - N_min; i2++)
							{
								if ((feasibleFlight[i1][j1] == 1) && (feasibleFlight[i2][j2] == 1) && (tin[i1] < tout[i2]) && (tout[i2] < tout[i1]))
								{
									// 精度修改
									model.add(x[i1][j1] + x[i2][j2] <= 1.1);
								}
							}
						}
					}
				}
			}

			//将模型定义到CPLEX中
			IloCplex cplex2(model);

			//求解时间上限
			//cplex2.setParam(IloCplex::TiLim, 600);

			//设置Gap
			cplex2.setParam(IloCplex::EpGap, 0.002);
			
			//执行求解
			cplex2.solve();

			// 决策变量输出
			for (i = 0; i < N_max - N_min; i++)
			{
				for (j = 0; j < M; j++)
				{
					X[i + N_min][j] = (int)((double)cplex2.getValue(x[i][j]) + 0.5);
				}
			}
			for (i = 0; i < N_max - N_min; i++)
			{
				for (k = 0; k < R; k++)
				{
					YIN[i + N_min][k] = (int)((double)cplex2.getValue(yin[i][k]) + 0.5);
				}
			}
			for (i = 0; i < N_max - N_min; i++)
			{
				for (k = 0; k < R; k++)
				{
					YOUT[i + N_min][k] = (int)((double)cplex2.getValue(yout[i][k]) + 0.5);
				}
			}

			//// 调试用
			//char N_min_char[5];
			//_itoa(N_min, N_min_char, 10);
			//string N_min_str(N_min_char);
			//char N_max_char[5];
			//_itoa(N_max, N_max_char, 10);
			//string N_max_str(N_max_char);
			//string str = "X[" + N_min_str + "] -- X[" + N_max_str + "].dat";
			//ofstream Xfileout(str);
			//for (i = 0; i < N_max - N_min; i++)
			//{
			//	int Xsum = 0;
			//	for (j = 0; j < M; j++)
			//	{
			//		Xfileout << X[i + N_min][j] << "\t";
			//		Xsum += X[i + N_min][j];
			//	}
			//	Xfileout << "Xsum = " << Xsum << "\t";
			//	Xfileout << "xTemp1 = " << cplex2.getValue(IloSum(x[i])) << endl;
			//}


			cout << "获得机位航班量：" << cplex2.getValue(IloSum(distributionFlightCountTemp)) << endl;
			cout << "靠桥航班量：" << cplex2.getValue(IloSum(distanceGatePositionCountTemp)) << endl;
			cout << "靠桥旅客人数：" << cplex2.getValue(IloSum(distanceGatePositionNumberCountTemp)) << endl;
			cout << "执行临时措施的航班量：" << cplex2.getValue(IloSum(temporaryMeasuresTemp)) << endl;
			cout << "滑行通道冲突航班量：" << cplex2.getValue(IloSum(z2)) << endl;
			cout << "getObjValue：" << cplex2.getObjValue() << endl;
			cout << "getBestObjValue：" << cplex2.getBestObjValue() << endl;
			
			env2.end();
		}

		//cout << "获得机位航班量：" << cplex.getValue(IloSum(distributionFlightCountTemp)) << endl;
		//cout << "靠桥航班量：" << cplex.getValue(IloSum(distanceGatePositionCountTemp)) << endl;
		//cout << "靠桥旅客人数：" << cplex.getValue(IloSum(distanceGatePositionNumberCountTemp)) << endl;
		//cout << "执行临时措施的航班量：" << cplex.getValue(IloSum(temporaryMeasuresTemp)) << endl;
		//cout << "滑行通道冲突航班量：" << cplex.getValue(IloSum(z2)) << endl;
		//cout << "getObjValue：" << cplex.getObjValue() << endl;
		//cout << "getBestObjValue：" << cplex.getBestObjValue() << endl;

		IloInt X_SUM;
		IloIntArray X_SUMI(env,N);
		for (i = 0; i < N; i++)
		{
			X_SUMI[i] = IloSum(X[i]);
			for (j = 0; j < M; j++)
			{
				X_SUM = IloSum(X_SUMI);
			}
		}
		cout << "X_SUM：" << X_SUM << endl;

		for (i = 0; i < N; i++)
		{
			vector<int> resultTemp;
			for (j = 0; j < M; j++)
			{
				resultTemp.push_back(X[i][j]);
			}
			BestSolution::m_solution.push_back(resultTemp);
		}

		cout << endl;
	}

	catch (IloException& ex) {
		cerr << "Error: " << ex << endl;
	}
	catch (...) {
		cerr << "Error" << endl;
	}

	env.end();
}
