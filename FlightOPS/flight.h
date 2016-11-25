/*************************************************
版权: 任家硕
作者: 任家硕
日期: 2016-10-15
功能: 机场停机位资源分配优化_航班类
修改历史:

**************************************************/

/*<remark>=========================================================
/// <summary>
/// 机场停机位资源分配优化_航班类
/// </summary>
===========================================================</remark>*/

#pragma once

#include "basic.h"
#include "gatePosition.h"
#include "time.h"

#pragma region 新建的类
////////////////////////////////////////////////////////////////////////
class Flight;// 航班类
class GatePosition;	// 停机位类
////////////////////////////////////////////////////////////////////////
#pragma endregion

class Flight
{

#pragma region Variables

public:// 航班参数
	string					m_companyNo;						// 航空公司
	string					m_flightNo;						// 航班号
	string					m_inTime;							// 进机位时间，字符串
	string					m_outTime;						// 出机位时间，字符串
	time_t					m_inTime_t;						// 进机位时间，时间格式
	time_t					m_outTime_t;						// 出机位时间，时间格式
	string					m_internationalOrDomestic;			// 国际国内属性
	int						m_internationalOrDomesticInt;		// 国际国内属性，1为国内，2位国际
	string					m_flyingCommission;				// 飞行任务
	string					m_model;							// 机型
	int						m_number;							// 最大旅客数
	map<string, GatePosition*>	possibleGatePosition;				// 当前航班可选机位的map集合，first为机位代码，second为机位
	GatePosition *			gatePosition;						// 所选机位

public:// 航班集合
	static map<string, Flight *>		s_mapSetOfFlight;				// 航班的map集合，key.first为航班号，key.value为进机位时间，value为航班
	static int						s_flightCount;				// 航班个数
	static map<string, Flight *>		s_mapSetOfwrongFlight;			// 错误航班的map集合，key为航班号，value为航班
	static int						s_wrongFlightCount;			// 进机位时间比出机位时间大的航班个数
	static int						s_nemberCount;				// 旅客总人数
	static vector<pair<Flight *, vector<GatePosition*>>>	initPossibleGatePosition;	// 初始航班可选机位的map集合，first为机位代码，second为机位

public:// 属性集合
	static vector<string>		s_vectorOfCompanyNo;					// 所有航空公司集合
	static vector<string>		s_vectorOfFlyingCommission;				// 所有飞行任务集合

#pragma endregion

#pragma region Functions

public:

	#pragma region 辅助函数
	//////////////////////////////////////////////////////////////////////////

	//
	// 摘要:
	//     构造函数。
	Flight(string	 companyNo, string flightNo, string inTime, string outTime, string internationalOrDomestic, string flyingCommission, string model, int number);
	//
	// 摘要：
	//     读取数据。
	static void readData();
	//
	// 摘要：
	//     统计旅客总人数。
	static void initNumberCount();
	//
	// 摘要：
	//     打印航班时间。
	static void printTime();
	//
	// 摘要：
	//     打印航班占用滑道时间。
	static void printRoadTime();
	//
	// 摘要:
	//     打印可能滑道冲突航班(N*N，不考虑实际使用滑道，若冲突为1)，分别打印航班A与航班B滑入滑道冲突、推出滑道冲突、滑入与推出、推出与滑入。
	static void printConflictFlight();
	//
	// 摘要:
	//     打印每架航班的旅客数量。
	static void printNumberOfFlight();
	//
	// 摘要：
	//     打印结果。
	static void printResult();
	//																											
	// 摘要：																									 
	//     清除结果。
	static void clearResult();
	//																											
	// 摘要：																									 
	//     清除一个航班的计划信息。
	static void clearOneFlight(Flight *flight);

	////////////////////////////////////////////////////////////////////////
	#pragma endregion

	#pragma region 集合相关
	//////////////////////////////////////////////////////////////////////////

	//
	// 摘要：
	//     获得航班可以使用的所有机位。
	static void GetFeasibleGatePosition();

	////////////////////////////////////////////////////////////////////////
	#pragma endregion

#pragma endregion

};