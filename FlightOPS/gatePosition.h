/*************************************************
版权: 任家硕
作者: 任家硕
日期: 2016-10-15
功能: 机场停机位资源分配优化_停机位类
修改历史:

**************************************************/

/*<remark>=========================================================
/// <summary>
/// 机场停机位资源分配优化_停机位类
/// </summary>
===========================================================</remark>*/

#pragma once

#include "basic.h"
#include "flight.h"

#pragma region 新建的类
////////////////////////////////////////////////////////////////////////
class GatePosition;	// 停机位类
class Road;			// 滑行道类
class Flight;			// 航班类
////////////////////////////////////////////////////////////////////////
#pragma endregion

class GatePosition
{

#pragma region Variables

public:// 停机位参数
	string					m_summary;						// 概述
	string					m_gatePositionNo;					// 停机位代码
	string					m_internationalOrDomestic;			// 国际国内属性
	string					m_distance;						// 远近机位属性
	string					m_inRoadNo;						// 滑入滑行道代码
	string					m_outRoadNo;						// 推出滑行道代码
	vector<string>			m_flyingCommission;				// 飞行任务
	vector<string>			m_companyNo;						// 航空公司
	vector<string>			m_model;							// 机型
	vector<string>			m_conflictGatePosition;			// 影响的机位代码集合（我影响别人）
	vector<string>			m_influenceGatePositionIn;			// 影响滑入机位代码集合（我影响别人）
	vector<string>			m_influenceGatePositionOut;			// 影响滑出机位代码集合（我影响别人）
	vector<GatePosition*>		m_beConflictGatePosition;			// 影响的机位集合（别人影响我）
	vector<GatePosition*>		m_beInfluenceGatePositionIn;		// 影响滑入机位集合（别人影响我）
	vector<GatePosition*>		m_beInfluenceGatePositionOut;		// 影响滑出机位集合（别人影响我）
	map<string, Flight*>		possibleFlight;					// 当前机位可选航班的map集合，first为航班代码，second为航班
	int						temporaryMeasures;					// 是否为临时措施，是为1，不是为0

public:// 停机位时间分配
	map<time_t, time_t>		m_mapOfStartTimeToFinishTime;		// 停机位停机时刻的集合，key为停机位停机开始时刻，value为停机位停机结束时刻
	map<time_t, Flight *>		m_mapOfStartTimeToFlight;			// 停机位停机的航班的集合，key为停机位停机开始时刻，value为航班号

public:// 国际国内属性
	int						m_internationalOrDomesticInt;		// 1为国内，2为国外，3为国内国外均可

public:// 所属滑行道
	Road	*					m_inRoad;							// 所属滑入滑行道
	Road	*					m_outRoad;						// 所属推出滑行道

public:// 停机位集合
	static map<string, GatePosition *>		s_mapSetOfGatePosition;	// 停机位的map集合，key为停机位代码，value为停机位
	static int							s_gatePositionCount;		// 停机位个数

#pragma endregion

#pragma region Functions

public:

#pragma region 辅助函数
	//////////////////////////////////////////////////////////////////////////

	//
	// 摘要:
	//     构造函数。
	GatePosition(string summary, string gatePositionNo, string internationalOrDomestic, string distance, string inRoadNo, string outRoadNo, vector<string> flyingCommission, vector<string> companyNo, vector<string> model, vector<string> conflictGatePosition, vector<string> influenceGatePositionIn, vector<string> influenceGatePositionOut);
	//																											
	// 摘要：																									 
	//     读取数据。
	static void readData();
	//																											
	// 摘要：																									 
	//     初始化(别人影响我)。
	static void initBeConflict();
	//																											
	// 摘要：																									 
	//     以，分割字符串，由string转化为vector<string>。
	static vector<string> stringToVector(string sourceStr);
	//																											
	// 摘要：																									 
	//     获得机位可以停放的所有航班。
	static void GetFeasibleFlight();
	//																											
	// 摘要：																									 
	//     打印机位可以停放的所有航班。
	static void PrintFeasibleFlight();
	//																											
	// 摘要：																									 
	//     打印机位靠桥情况，靠桥为1。
	static void PrintDistance();
	//																											
	// 摘要：																									 
	//     打印机位是否为临时措施，是临时措施为1，不是临时措施为0。
	static void PrintTemporaryMeasures();
	//																											
	// 摘要：																									 
	//     打印不能同时使用的机位(M*M)，若不能同时使用则为1
	static void PrintConflictGatePosition();
	//																											
	// 摘要：																									 
	//     打印所影响滑入的机位(M*M)，所影响滑出的机位(M*M)，若影响为1
	static void PrintInfluenceGatePosition();
	//																											
	// 摘要：																									 
	//     清除结果。
	static void clearResult();

	////////////////////////////////////////////////////////////////////////
#pragma endregion

#pragma endregion

};

class Road
{

#pragma region Variables

public:// 滑行道参数
	string					m_roadNo;							// 滑行道代码
	vector<string>			m_gatePositionOfRoadNo;			// 滑行道包含停机位

public:// 滑行道时间分配
	vector<pair<time_t, time_t>>		m_vectorOfStartTimeToFinishTime;		// 滑行道分配滑行时刻的集合，key为滑行道分配滑行开始时刻，value为滑行道分配滑行结束时刻
	vector<pair<time_t, Flight *>>		m_vectorOfStartTimeToFlight;			// 滑行道分配滑行的航班的集合，key为滑行道分配滑行开始时刻，value为航班号

public:// 滑行道集合
	static map<string, Road *>		s_mapSetOfRoad;				// 滑行道的map集合，key为滑行道代码，value为滑行道
	static int					s_roadCount;					// 滑行道个数

#pragma endregion

#pragma region Functions

public:

#pragma region 辅助函数
	//////////////////////////////////////////////////////////////////////////

	//
	// 摘要:
	//     构造函数。
	Road(string roadNo, string gatePositionOfRoadNo);
	//
	// 摘要:
	//     初始化滑行道。
	static void initRoad();
	//
	// 摘要:
	//     打印停机位与滑道对应关系。
	static void printGatePositionToRoad();
	//																											
	// 摘要：																									 
	//     清除结果。
	static void clearResult();

	////////////////////////////////////////////////////////////////////////
#pragma endregion

#pragma endregion

};