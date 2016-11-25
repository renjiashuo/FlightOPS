/*************************************************
版权: 任家硕
作者: 任家硕
日期: 2016-10-16
功能: 机场停机位资源分配优化_最优解类
修改历史:

**************************************************/

/*<remark>=========================================================
/// <summary>
/// 机场停机位资源分配优化_最优解类
/// </summary>
===========================================================</remark>*/

#pragma once

#include "basic.h"
#include "flight.h"

#pragma region 新建的类
////////////////////////////////////////////////////////////////////////
class BestSolution;// 最优解类
////////////////////////////////////////////////////////////////////////
#pragma endregion

class BestSolution
{
#pragma region Variables

public:// 最优解参数
	static vector<string>			m_flightNo;						// 航班号
	static vector<string>			m_inTime;							// 进机位时间，字符串
	static vector<string>			m_outTime;						// 出机位时间，字符串
	static vector<string>			m_gatePositionNo;					// 所选机位代码
	static double					m_objValue;						// 最优目标函数值
	static int					m_distributionFlightCount;			// 获得机位的航班量
	static int					m_distanceGatePositionCount;		// 靠桥航班量
	static int					m_distanceGatePositionNumberCount;	// 靠桥旅客量
	static int					m_temporaryMeasuresSum;			// 执行临时措施航班量
	static int					m_conflictCount;					// 滑行通道冲突航班量
	static vector<vector<int>>		m_solution;						// cplex最优解
	static int					m_deleteNum;						// 检查时删除冲突航班数量
	static int					m_heuristicFlightNum;				// 启发式安排航班数量

#pragma endregion

#pragma region Functions

public:
	//
	// 摘要:
	//     根据cplex的解更新数据。
	static void updateValueByCplexSolver();
	//
	// 摘要:
	//     删除分段cplex的坏解。
	static void deleteWrongValue();
	//
	// 摘要:
	//     启发式给未分配的航班分配停机位。
	static void heuristicForUndistributed();
	//
	// 摘要:
	//     如果解优于目前最优解，则更新最优解。
	static void updateBestValue();
	//
	// 摘要：
	//     打印结果。
	static void printBestResult();
	//
	// 摘要：
	//     计算目标函数。
	static double getObjectiveFunction();
	//
	// 摘要：
	//     排序函数。
	static bool less_first_time_t(const pair<time_t, Flight *> & m1, const pair<time_t, Flight *> & m2);

#pragma endregion
};