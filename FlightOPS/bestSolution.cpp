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

#include "bestSolution.h"

void BestSolution::updateValueByCplexSolver()
{
	int i = 0;
	for (map<string, Flight *>::iterator iter = Flight::s_mapSetOfFlight.begin(); iter != Flight::s_mapSetOfFlight.end(); iter++)
	{
		string flightNo = iter->first;
		Flight *flight = iter->second;
		time_t flightInTime_t = flight->m_inTime_t;		// 进机位时间，时间格式
		time_t flightOutTime_t = flight->m_outTime_t;		// 出机位时间，时间格式
		int j = 0;
		for (map<string, GatePosition *>::iterator iter2 = GatePosition::s_mapSetOfGatePosition.begin(); iter2 != GatePosition::s_mapSetOfGatePosition.end(); iter2++)
		{
			string gatePositionNo = iter2->first;
			GatePosition *gatePosition = iter2->second;
			if (m_solution[i][j] == 1)
			{
				if (gatePosition->m_mapOfStartTimeToFinishTime.find(flightInTime_t) == gatePosition->m_mapOfStartTimeToFinishTime.end())
				{
					gatePosition->m_mapOfStartTimeToFinishTime.insert(make_pair(flightInTime_t, flightOutTime_t));
					gatePosition->m_mapOfStartTimeToFlight.insert(make_pair(flightInTime_t, flight));
					Road *inRoad = gatePosition->m_inRoad;
					Road *outRoad = gatePosition->m_outRoad;
					inRoad->m_vectorOfStartTimeToFinishTime.push_back(make_pair(flightInTime_t - 5 * 60, flightInTime_t));
					outRoad->m_vectorOfStartTimeToFinishTime.push_back(make_pair(flightOutTime_t, flightOutTime_t + 5 * 60));
					inRoad->m_vectorOfStartTimeToFlight.push_back(make_pair(flightInTime_t - 5 * 60, flight));
					outRoad->m_vectorOfStartTimeToFlight.push_back(make_pair(flightOutTime_t, flight));
					flight->gatePosition = gatePosition;
				}
				else
					m_deleteNum++;
				break;
			}
			j++;
		}
		i++;
	}
}

void BestSolution::deleteWrongValue()
{
	// 第一个停机位特殊处理
	while (true)
	{
		bool mark = false;
		map<string, GatePosition *>::iterator iter = GatePosition::s_mapSetOfGatePosition.begin();
		GatePosition * gatePosition = iter->second;
		if (gatePosition->m_mapOfStartTimeToFinishTime.size() < 2)
			break;
		for (map<time_t, time_t>::iterator iter2 = gatePosition->m_mapOfStartTimeToFinishTime.begin(); iter2 != --gatePosition->m_mapOfStartTimeToFinishTime.end();)
		{
			time_t preInTime = iter2->first;
			time_t preOutTime = iter2->second;
			iter2++;
			time_t nextInTime = iter2->first;
			time_t nextOutTime = iter2->second;
			if (preOutTime + 10 * 60 > nextInTime)
			{
				m_deleteNum++;
				// 删除该航班的安排
				Flight *flight = gatePosition->m_mapOfStartTimeToFlight[nextInTime];
				Road *inRoad = gatePosition->m_inRoad;
				Road *outRoad = gatePosition->m_outRoad;
				int deleteInIter = find(inRoad->m_vectorOfStartTimeToFlight.begin(), inRoad->m_vectorOfStartTimeToFlight.end(), make_pair(nextInTime - 5 * 60, flight)) - inRoad->m_vectorOfStartTimeToFlight.begin();
				inRoad->m_vectorOfStartTimeToFinishTime.erase(inRoad->m_vectorOfStartTimeToFinishTime.begin() + deleteInIter);
				inRoad->m_vectorOfStartTimeToFlight.erase(remove(inRoad->m_vectorOfStartTimeToFlight.begin(), inRoad->m_vectorOfStartTimeToFlight.end(), make_pair(nextInTime - 5 * 60, flight)), inRoad->m_vectorOfStartTimeToFlight.end());
				int deleteOutIter = find(outRoad->m_vectorOfStartTimeToFlight.begin(), outRoad->m_vectorOfStartTimeToFlight.end(), make_pair(nextOutTime, flight)) - outRoad->m_vectorOfStartTimeToFlight.begin();
				outRoad->m_vectorOfStartTimeToFinishTime.erase(outRoad->m_vectorOfStartTimeToFinishTime.begin() + deleteOutIter);
				outRoad->m_vectorOfStartTimeToFlight.erase(remove(outRoad->m_vectorOfStartTimeToFlight.begin(), outRoad->m_vectorOfStartTimeToFlight.end(), make_pair(nextOutTime, flight)), outRoad->m_vectorOfStartTimeToFlight.end());
				gatePosition->m_mapOfStartTimeToFinishTime.erase(nextInTime);
				gatePosition->m_mapOfStartTimeToFlight.erase(nextInTime);
				flight->gatePosition = NULL;
				break;
			}
			if (iter2 == --gatePosition->m_mapOfStartTimeToFinishTime.end())
				mark = true;
		}
		if (mark == true)
			break;
	}
	// 其他停机位
	for (map<string, GatePosition *>::iterator iter = GatePosition::s_mapSetOfGatePosition.begin(); iter != GatePosition::s_mapSetOfGatePosition.end(); iter++)
	{
		GatePosition * gatePosition = iter->second;
		if (gatePosition->m_mapOfStartTimeToFinishTime.size() < 2)
			continue;
		for (map<time_t, time_t>::iterator iter2 = gatePosition->m_mapOfStartTimeToFinishTime.begin(); iter2 != --gatePosition->m_mapOfStartTimeToFinishTime.end();)
		{
			time_t preInTime = iter2->first;
			time_t preOutTime = iter2->second;
			iter2++;
			time_t nextInTime = iter2->first;
			time_t nextOutTime = iter2->second;
			if (preOutTime + 10 * 60 > nextInTime)
			{
				m_deleteNum++;
				// 删除该航班的安排
				Flight *flight = gatePosition->m_mapOfStartTimeToFlight[nextInTime];
				Road *inRoad = gatePosition->m_inRoad;
				Road *outRoad = gatePosition->m_outRoad;
				int deleteInIter = find(inRoad->m_vectorOfStartTimeToFlight.begin(), inRoad->m_vectorOfStartTimeToFlight.end(), make_pair(nextInTime - 5 * 60, flight)) - inRoad->m_vectorOfStartTimeToFlight.begin();
				inRoad->m_vectorOfStartTimeToFinishTime.erase(inRoad->m_vectorOfStartTimeToFinishTime.begin() + deleteInIter);
				inRoad->m_vectorOfStartTimeToFlight.erase(remove(inRoad->m_vectorOfStartTimeToFlight.begin(), inRoad->m_vectorOfStartTimeToFlight.end(), make_pair(nextInTime - 5 * 60, flight)), inRoad->m_vectorOfStartTimeToFlight.end());
				int deleteOutIter = find(outRoad->m_vectorOfStartTimeToFlight.begin(), outRoad->m_vectorOfStartTimeToFlight.end(), make_pair(nextOutTime, flight)) - outRoad->m_vectorOfStartTimeToFlight.begin();
				outRoad->m_vectorOfStartTimeToFinishTime.erase(outRoad->m_vectorOfStartTimeToFinishTime.begin() + deleteOutIter);
				outRoad->m_vectorOfStartTimeToFlight.erase(remove(outRoad->m_vectorOfStartTimeToFlight.begin(), outRoad->m_vectorOfStartTimeToFlight.end(), make_pair(nextOutTime, flight)), outRoad->m_vectorOfStartTimeToFlight.end());
				gatePosition->m_mapOfStartTimeToFinishTime.erase(nextInTime);
				gatePosition->m_mapOfStartTimeToFlight.erase(nextInTime);
				flight->gatePosition = NULL;
				iter--;
				break;
			}
		}
	}
	// 按照已安排航班查找冲突
	for (map<string, Flight *>::iterator iter = Flight::s_mapSetOfFlight.begin(); iter != Flight::s_mapSetOfFlight.end(); iter++)
	{
		string flightNo = iter->first;
		Flight *flight = iter->second;
		GatePosition *gatePosition = flight->gatePosition;
		if (gatePosition == NULL)
			continue;
		time_t inTime = flight->m_inTime_t;
		time_t outTime = flight->m_outTime_t;
		bool isTrue = true;
		// 影响冲突机位
		for (vector<GatePosition*>::iterator iter4 = gatePosition->m_beConflictGatePosition.begin(); iter4 != gatePosition->m_beConflictGatePosition.end(); )
		{
			if (isTrue == false)
			{
				isTrue = true;
				//if (iter4 == gatePosition->m_beConflictGatePosition.begin())
				//	isTrue = true;
				//else
				//{
				//	iter4--;
				//	isTrue = true;
				//}
			}
			GatePosition *conflictGatePosition = *iter4;
			if (conflictGatePosition->m_mapOfStartTimeToFlight.find(inTime) != conflictGatePosition->m_mapOfStartTimeToFlight.end())
			{
				m_deleteNum++;
				Flight *conflictFlight = conflictGatePosition->m_mapOfStartTimeToFlight.find(inTime)->second;
				Flight::clearOneFlight(conflictFlight);
				isTrue = false;
				continue;
			}
			conflictGatePosition->m_mapOfStartTimeToFinishTime.insert(make_pair(inTime, outTime));
			map<time_t, time_t>::iterator conflictPreiter3 = --conflictGatePosition->m_mapOfStartTimeToFinishTime.find(inTime);
			map<time_t, time_t>::iterator conflictIter3 = conflictGatePosition->m_mapOfStartTimeToFinishTime.find(inTime);
			map<time_t, time_t>::iterator conflictNextiter3 = ++conflictGatePosition->m_mapOfStartTimeToFinishTime.find(inTime);
			if (conflictPreiter3 != --conflictGatePosition->m_mapOfStartTimeToFinishTime.begin() && inTime < conflictPreiter3->second + 10 * 60)
			{
				m_deleteNum++;
				Flight *conflictFlight = conflictGatePosition->m_mapOfStartTimeToFlight.find(conflictPreiter3->first)->second;
				Flight::clearOneFlight(conflictFlight);
				isTrue = false;
			}
			if (conflictNextiter3 != conflictGatePosition->m_mapOfStartTimeToFinishTime.end() && outTime > conflictNextiter3->first - 10 * 60)
			{
				m_deleteNum++;
				Flight *conflictFlight = conflictGatePosition->m_mapOfStartTimeToFlight.find(conflictNextiter3->first)->second;
				Flight::clearOneFlight(conflictFlight);
				isTrue = false;
			}
			conflictGatePosition->m_mapOfStartTimeToFinishTime.erase(conflictIter3);
			if (isTrue == true)
				iter4++;
		}
		// 影响入机位
		for (vector<GatePosition*>::iterator iter4 = gatePosition->m_beInfluenceGatePositionIn.begin(); iter4 != gatePosition->m_beInfluenceGatePositionIn.end(); iter4++)
		{
			GatePosition *conflictGatePosition = *iter4;
			if (conflictGatePosition->m_mapOfStartTimeToFinishTime.empty())
				continue;
			map<time_t, time_t>::iterator iter5 = conflictGatePosition->m_mapOfStartTimeToFinishTime.begin();
			map<time_t, Flight *>::iterator iter6 = conflictGatePosition->m_mapOfStartTimeToFlight.begin();
			while (true)
			{
				if (iter5->first < inTime && inTime < iter5->second)
				{
					m_deleteNum++;
					Flight *conflictFlight = iter6->second;
					Flight::clearOneFlight(conflictFlight);
					iter5 = conflictGatePosition->m_mapOfStartTimeToFinishTime.begin();
					iter6 = conflictGatePosition->m_mapOfStartTimeToFlight.begin();
					if (conflictGatePosition->m_mapOfStartTimeToFinishTime.empty())
						break;
					else
						continue;
				}
				iter5++;
				iter6++;
				if (iter5 == conflictGatePosition->m_mapOfStartTimeToFinishTime.end())
					break;
			}
		}
		// 影响出机位
		for (vector<GatePosition*>::iterator iter4 = gatePosition->m_beInfluenceGatePositionOut.begin(); iter4 != gatePosition->m_beInfluenceGatePositionOut.end(); iter4++)
		{
			GatePosition *conflictGatePosition = *iter4;
			if (conflictGatePosition->m_mapOfStartTimeToFinishTime.empty())
				continue;
			map<time_t, time_t>::iterator iter5 = conflictGatePosition->m_mapOfStartTimeToFinishTime.begin();
			map<time_t, Flight *>::iterator iter6 = conflictGatePosition->m_mapOfStartTimeToFlight.begin();
			while (true)
			{
				if (iter5->first < outTime && outTime < iter5->second)
				{
					m_deleteNum++;
					Flight *conflictFlight = iter6->second;
					Flight::clearOneFlight(conflictFlight);
					iter5 = conflictGatePosition->m_mapOfStartTimeToFinishTime.begin();
					iter6 = conflictGatePosition->m_mapOfStartTimeToFlight.begin();
					if (conflictGatePosition->m_mapOfStartTimeToFinishTime.empty())
						break;
					else
						continue;
				}
				iter5++;
				iter6++;
				if (iter5 == conflictGatePosition->m_mapOfStartTimeToFinishTime.end())
					break;
			}
		}
	}
	cout << "删除了 " << m_deleteNum << " 架冲突的航班。" << endl;
}

void BestSolution::heuristicForUndistributed()
{
	for (map<string, Flight *>::iterator iter = Flight::s_mapSetOfFlight.begin(); iter != Flight::s_mapSetOfFlight.end(); iter++)
	{
		string flightNo = iter->first;
		Flight *flight = iter->second;
		if (flight->gatePosition != NULL)
			continue;
		time_t inTime = flight->m_inTime_t;
		time_t outTime = flight->m_outTime_t;

		#pragma region 安排近机位
		//////////////////////////////////////////////////////////////////////////
		for (map<string, GatePosition*>::iterator iter2 = flight->possibleGatePosition.begin(); iter2 != flight->possibleGatePosition.end(); iter2++)
		{
			string gatePositionNo = iter2->first;
			GatePosition *gatePosition = iter2->second;
			if (gatePosition->m_distance != "近机位")
				break;
			if (gatePosition->m_mapOfStartTimeToFinishTime.find(inTime) != gatePosition->m_mapOfStartTimeToFinishTime.end())
				continue;
			gatePosition->m_mapOfStartTimeToFinishTime.insert(make_pair(inTime, outTime));
			bool isTrue = true;
			map<time_t, time_t>::iterator preiter3 = --gatePosition->m_mapOfStartTimeToFinishTime.find(inTime);
			map<time_t, time_t>::iterator iter3 = gatePosition->m_mapOfStartTimeToFinishTime.find(inTime);
			map<time_t, time_t>::iterator nextiter3 = ++gatePosition->m_mapOfStartTimeToFinishTime.find(inTime);
			if (isTrue && preiter3 != --gatePosition->m_mapOfStartTimeToFinishTime.begin() && inTime < preiter3->second + 10 * 60)
				isTrue = false;
			if (isTrue && nextiter3 != gatePosition->m_mapOfStartTimeToFinishTime.end() && outTime > nextiter3->first - 10 * 60)
				isTrue = false;
			// 影响的冲突机位
			if (isTrue)
			{
				for (vector<GatePosition*>::iterator iter4 = gatePosition->m_beConflictGatePosition.begin(); iter4 != gatePosition->m_beConflictGatePosition.end(); iter4++)
				{
					GatePosition *conflictGatePosition = *iter4;
					if (conflictGatePosition->m_mapOfStartTimeToFinishTime.find(inTime) != conflictGatePosition->m_mapOfStartTimeToFinishTime.end())
					{
						isTrue = false;
						continue;
					}
					conflictGatePosition->m_mapOfStartTimeToFinishTime.insert(make_pair(inTime, outTime));
					map<time_t, time_t>::iterator conflictPreiter3 = --conflictGatePosition->m_mapOfStartTimeToFinishTime.find(inTime);
					map<time_t, time_t>::iterator conflictIter3 = conflictGatePosition->m_mapOfStartTimeToFinishTime.find(inTime);
					map<time_t, time_t>::iterator conflictNextiter3 = ++conflictGatePosition->m_mapOfStartTimeToFinishTime.find(inTime);
					if (isTrue && conflictPreiter3 != --conflictGatePosition->m_mapOfStartTimeToFinishTime.begin() && inTime < conflictPreiter3->second + 10 * 60)
						isTrue = false;
					if (isTrue && conflictNextiter3 != conflictGatePosition->m_mapOfStartTimeToFinishTime.end() && outTime > conflictNextiter3->first - 10 * 60)
						isTrue = false;
					conflictGatePosition->m_mapOfStartTimeToFinishTime.erase(conflictIter3);
					if (isTrue == false)
						break;
				}
			}
			// 影响的滑入机位
			if (isTrue)
			{
				for (vector<GatePosition*>::iterator iter4 = gatePosition->m_beInfluenceGatePositionIn.begin(); iter4 != gatePosition->m_beInfluenceGatePositionIn.end(); iter4++)
				{
					GatePosition *conflictGatePosition = *iter4;
					if (conflictGatePosition->m_mapOfStartTimeToFinishTime.empty())
						continue;
					map<time_t, time_t>::iterator iter5 = conflictGatePosition->m_mapOfStartTimeToFinishTime.begin();
					while (isTrue)
					{
						if (iter5->first < inTime && inTime < iter5->second)
						{
							isTrue = false;
							break;
						}
						iter5++;
						if (iter5 == conflictGatePosition->m_mapOfStartTimeToFinishTime.end())
							break;
					}
				}
			}
			// 影响的滑出机位
			if (isTrue)
			{
				for (vector<GatePosition*>::iterator iter4 = gatePosition->m_beInfluenceGatePositionOut.begin(); iter4 != gatePosition->m_beInfluenceGatePositionOut.end(); iter4++)
				{
					GatePosition *conflictGatePosition = *iter4;
					if (conflictGatePosition->m_mapOfStartTimeToFinishTime.empty())
						continue;
					map<time_t, time_t>::iterator iter5 = conflictGatePosition->m_mapOfStartTimeToFinishTime.begin();
					while (true)
					{
						if (iter5->first < outTime && outTime < iter5->second)
						{
							isTrue = false;
							break;
						}
						iter5++;
						if (iter5 == conflictGatePosition->m_mapOfStartTimeToFinishTime.end())
							break;
					}
				}
			}
			if (isTrue)
			{
				gatePosition->m_mapOfStartTimeToFlight.insert(make_pair(inTime, flight));
				Road *inRoad = gatePosition->m_inRoad;
				Road *outRoad = gatePosition->m_outRoad;
				inRoad->m_vectorOfStartTimeToFinishTime.push_back(make_pair(inTime - 5 * 60, inTime));
				outRoad->m_vectorOfStartTimeToFinishTime.push_back(make_pair(outTime, outTime + 5 * 60));
				inRoad->m_vectorOfStartTimeToFlight.push_back(make_pair(inTime - 5 * 60, flight));
				outRoad->m_vectorOfStartTimeToFlight.push_back(make_pair(outTime, flight));
				flight->gatePosition = gatePosition;
				break;
			}
			else
			{
				gatePosition->m_mapOfStartTimeToFinishTime.erase(iter3);
				continue;
			}
		}
		//////////////////////////////////////////////////////////////////////////
		#pragma endregion

		if (flight->gatePosition != NULL)
		{
			m_heuristicFlightNum++;
			continue;
		}

		#pragma region 安排远机位
		//////////////////////////////////////////////////////////////////////////
		for (map<string, GatePosition*>::iterator iter2 = flight->possibleGatePosition.begin(); iter2 != flight->possibleGatePosition.end(); iter2++)
		{
			string gatePositionNo = iter2->first;
			GatePosition *gatePosition = iter2->second;
			if (gatePosition->m_distance == "近机位" || gatePosition->m_summary == "西区父子公务机坪" || gatePosition->m_summary == "超远机坪" || gatePosition->m_summary == "临时机位")
				break;
			if (gatePosition->m_mapOfStartTimeToFinishTime.find(inTime) != gatePosition->m_mapOfStartTimeToFinishTime.end())
				continue;
			gatePosition->m_mapOfStartTimeToFinishTime.insert(make_pair(inTime, outTime));
			bool isTrue = true;
			map<time_t, time_t>::iterator preiter3 = --gatePosition->m_mapOfStartTimeToFinishTime.find(inTime);
			map<time_t, time_t>::iterator iter3 = gatePosition->m_mapOfStartTimeToFinishTime.find(inTime);
			map<time_t, time_t>::iterator nextiter3 = ++gatePosition->m_mapOfStartTimeToFinishTime.find(inTime);
			if (isTrue && preiter3 != --gatePosition->m_mapOfStartTimeToFinishTime.begin() && inTime < preiter3->second + 10 * 60)
				isTrue = false;
			if (isTrue && nextiter3 != gatePosition->m_mapOfStartTimeToFinishTime.end() && outTime > nextiter3->first - 10 * 60)
				isTrue = false;
			// 影响的冲突机位
			if (isTrue)
			{
				for (vector<GatePosition*>::iterator iter4 = gatePosition->m_beConflictGatePosition.begin(); iter4 != gatePosition->m_beConflictGatePosition.end(); iter4++)
				{
					GatePosition *conflictGatePosition = *iter4;
					if (conflictGatePosition->m_mapOfStartTimeToFinishTime.find(inTime) != conflictGatePosition->m_mapOfStartTimeToFinishTime.end())
					{
						isTrue = false;
						continue;
					}
					conflictGatePosition->m_mapOfStartTimeToFinishTime.insert(make_pair(inTime, outTime));
					map<time_t, time_t>::iterator conflictPreiter3 = --conflictGatePosition->m_mapOfStartTimeToFinishTime.find(inTime);
					map<time_t, time_t>::iterator conflictIter3 = conflictGatePosition->m_mapOfStartTimeToFinishTime.find(inTime);
					map<time_t, time_t>::iterator conflictNextiter3 = ++conflictGatePosition->m_mapOfStartTimeToFinishTime.find(inTime);
					if (isTrue && conflictPreiter3 != --conflictGatePosition->m_mapOfStartTimeToFinishTime.begin() && inTime < conflictPreiter3->second + 10 * 60)
						isTrue = false;
					if (isTrue && conflictNextiter3 != conflictGatePosition->m_mapOfStartTimeToFinishTime.end() && outTime > conflictNextiter3->first - 10 * 60)
						isTrue = false;
					conflictGatePosition->m_mapOfStartTimeToFinishTime.erase(conflictIter3);
					if (isTrue == false)
						break;
				}
			}
			// 影响的滑入机位
			if (isTrue)
			{
				for (vector<GatePosition*>::iterator iter4 = gatePosition->m_beInfluenceGatePositionIn.begin(); iter4 != gatePosition->m_beInfluenceGatePositionIn.end(); iter4++)
				{
					GatePosition *conflictGatePosition = *iter4;
					if (conflictGatePosition->m_mapOfStartTimeToFinishTime.empty())
						continue;
					map<time_t, time_t>::iterator iter5 = conflictGatePosition->m_mapOfStartTimeToFinishTime.begin();
					while (isTrue)
					{
						if (iter5->first < inTime && inTime < iter5->second)
						{
							isTrue = false;
							break;
						}
						iter5++;
						if (iter5 == conflictGatePosition->m_mapOfStartTimeToFinishTime.end())
							break;
					}
				}
			}
			// 影响的滑出机位
			if (isTrue)
			{
				for (vector<GatePosition*>::iterator iter4 = gatePosition->m_beInfluenceGatePositionOut.begin(); iter4 != gatePosition->m_beInfluenceGatePositionOut.end(); iter4++)
				{
					GatePosition *conflictGatePosition = *iter4;
					if (conflictGatePosition->m_mapOfStartTimeToFinishTime.empty())
						continue;
					map<time_t, time_t>::iterator iter5 = conflictGatePosition->m_mapOfStartTimeToFinishTime.begin();
					while (true)
					{
						if (iter5->first < outTime && outTime < iter5->second)
						{
							isTrue = false;
							break;
						}
						iter5++;
						if (iter5 == conflictGatePosition->m_mapOfStartTimeToFinishTime.end())
							break;
					}
				}
			}
			if (isTrue)
			{
				gatePosition->m_mapOfStartTimeToFlight.insert(make_pair(inTime, flight));
				Road *inRoad = gatePosition->m_inRoad;
				Road *outRoad = gatePosition->m_outRoad;
				inRoad->m_vectorOfStartTimeToFinishTime.push_back(make_pair(inTime - 5 * 60, inTime));
				outRoad->m_vectorOfStartTimeToFinishTime.push_back(make_pair(outTime, outTime + 5 * 60));
				inRoad->m_vectorOfStartTimeToFlight.push_back(make_pair(inTime - 5 * 60, flight));
				outRoad->m_vectorOfStartTimeToFlight.push_back(make_pair(outTime, flight));
				flight->gatePosition = gatePosition;
				break;
			}
			else
			{
				gatePosition->m_mapOfStartTimeToFinishTime.erase(iter3);
				continue;
			}
		}
		//////////////////////////////////////////////////////////////////////////
		#pragma endregion

		if (flight->gatePosition != NULL)
		{
			m_heuristicFlightNum++;
			continue;
		}

		#pragma region 安排临时机位
		//////////////////////////////////////////////////////////////////////////
		for (map<string, GatePosition*>::iterator iter2 = flight->possibleGatePosition.begin(); iter2 != flight->possibleGatePosition.end(); iter2++)
		{
			string gatePositionNo = iter2->first;
			GatePosition *gatePosition = iter2->second;
			if (gatePosition->m_summary != "西区父子公务机坪" && gatePosition->m_summary != "超远机坪" && gatePosition->m_summary != "临时机位")
				break;
			if (gatePosition->m_mapOfStartTimeToFinishTime.find(inTime) != gatePosition->m_mapOfStartTimeToFinishTime.end())
				continue;
			gatePosition->m_mapOfStartTimeToFinishTime.insert(make_pair(inTime, outTime));
			bool isTrue = true;
			map<time_t, time_t>::iterator preiter3 = --gatePosition->m_mapOfStartTimeToFinishTime.find(inTime);
			map<time_t, time_t>::iterator iter3 = gatePosition->m_mapOfStartTimeToFinishTime.find(inTime);
			map<time_t, time_t>::iterator nextiter3 = ++gatePosition->m_mapOfStartTimeToFinishTime.find(inTime);
			if (isTrue && preiter3 != --gatePosition->m_mapOfStartTimeToFinishTime.begin() && inTime < preiter3->second + 10 * 60)
				isTrue = false;
			if (isTrue && nextiter3 != gatePosition->m_mapOfStartTimeToFinishTime.end() && outTime > nextiter3->first - 10 * 60)
				isTrue = false;
			// 影响的冲突机位
			if (isTrue)
			{
				for (vector<GatePosition*>::iterator iter4 = gatePosition->m_beConflictGatePosition.begin(); iter4 != gatePosition->m_beConflictGatePosition.end(); iter4++)
				{
					GatePosition *conflictGatePosition = *iter4;
					if (conflictGatePosition->m_mapOfStartTimeToFinishTime.find(inTime) != conflictGatePosition->m_mapOfStartTimeToFinishTime.end())
					{
						isTrue = false;
						continue;
					}
					conflictGatePosition->m_mapOfStartTimeToFinishTime.insert(make_pair(inTime, outTime));
					map<time_t, time_t>::iterator conflictPreiter3 = --conflictGatePosition->m_mapOfStartTimeToFinishTime.find(inTime);
					map<time_t, time_t>::iterator conflictIter3 = conflictGatePosition->m_mapOfStartTimeToFinishTime.find(inTime);
					map<time_t, time_t>::iterator conflictNextiter3 = ++conflictGatePosition->m_mapOfStartTimeToFinishTime.find(inTime);
					if (isTrue && conflictPreiter3 != --conflictGatePosition->m_mapOfStartTimeToFinishTime.begin() && inTime < conflictPreiter3->second + 10 * 60)
						isTrue = false;
					if (isTrue && conflictNextiter3 != conflictGatePosition->m_mapOfStartTimeToFinishTime.end() && outTime > conflictNextiter3->first - 10 * 60)
						isTrue = false;
					conflictGatePosition->m_mapOfStartTimeToFinishTime.erase(conflictIter3);
					if (isTrue == false)
						break;
				}
			}
			// 影响的滑入机位
			if (isTrue)
			{
				for (vector<GatePosition*>::iterator iter4 = gatePosition->m_beInfluenceGatePositionIn.begin(); iter4 != gatePosition->m_beInfluenceGatePositionIn.end(); iter4++)
				{
					GatePosition *conflictGatePosition = *iter4;
					if (conflictGatePosition->m_mapOfStartTimeToFinishTime.empty())
						continue;
					map<time_t, time_t>::iterator iter5 = conflictGatePosition->m_mapOfStartTimeToFinishTime.begin();
					while (isTrue)
					{
						if (iter5->first < inTime && inTime < iter5->second)
						{
							isTrue = false;
							break;
						}
						iter5++;
						if (iter5 == conflictGatePosition->m_mapOfStartTimeToFinishTime.end())
							break;
					}
				}
			}
			// 影响的滑出机位
			if (isTrue)
			{
				for (vector<GatePosition*>::iterator iter4 = gatePosition->m_beInfluenceGatePositionOut.begin(); iter4 != gatePosition->m_beInfluenceGatePositionOut.end(); iter4++)
				{
					GatePosition *conflictGatePosition = *iter4;
					if (conflictGatePosition->m_mapOfStartTimeToFinishTime.empty())
						continue;
					map<time_t, time_t>::iterator iter5 = conflictGatePosition->m_mapOfStartTimeToFinishTime.begin();
					while (true)
					{
						if (iter5->first < outTime && outTime < iter5->second)
						{
							isTrue = false;
							break;
						}
						iter5++;
						if (iter5 == conflictGatePosition->m_mapOfStartTimeToFinishTime.end())
							break;
					}
				}
			}
			// 影响冲突的机位(影响别人)
			if (isTrue)
			{
				for (vector<string>::iterator iter4 = gatePosition->m_conflictGatePosition.begin(); iter4 != gatePosition->m_conflictGatePosition.end(); iter4++)
				{
					GatePosition *conflictGatePosition = GatePosition::s_mapSetOfGatePosition.find(*iter4)->second;
					if (conflictGatePosition->m_mapOfStartTimeToFinishTime.find(inTime) != conflictGatePosition->m_mapOfStartTimeToFinishTime.end())
					{
						isTrue = false;
						continue;
					}
					conflictGatePosition->m_mapOfStartTimeToFinishTime.insert(make_pair(inTime, outTime));
					map<time_t, time_t>::iterator conflictPreiter3 = --conflictGatePosition->m_mapOfStartTimeToFinishTime.find(inTime);
					map<time_t, time_t>::iterator conflictIter3 = conflictGatePosition->m_mapOfStartTimeToFinishTime.find(inTime);
					map<time_t, time_t>::iterator conflictNextiter3 = ++conflictGatePosition->m_mapOfStartTimeToFinishTime.find(inTime);
					if (isTrue && conflictPreiter3 != --conflictGatePosition->m_mapOfStartTimeToFinishTime.begin() && inTime < conflictPreiter3->second + 10 * 60)
						isTrue = false;
					if (isTrue && conflictNextiter3 != conflictGatePosition->m_mapOfStartTimeToFinishTime.end() && outTime > conflictNextiter3->first - 10 * 60)
						isTrue = false;
					conflictGatePosition->m_mapOfStartTimeToFinishTime.erase(conflictIter3);
					if (isTrue == false)
						break;
				}
			}
			// 影响滑入的机位(影响别人)
			if (isTrue)
			{
				for (vector<string>::iterator iter4 = gatePosition->m_influenceGatePositionIn.begin(); iter4 != gatePosition->m_influenceGatePositionIn.end(); iter4++)
				{
					GatePosition *conflictGatePosition = GatePosition::s_mapSetOfGatePosition.find(*iter4)->second;
					if (conflictGatePosition->m_mapOfStartTimeToFinishTime.empty())
						continue;
					map<time_t, time_t>::iterator iter5 = conflictGatePosition->m_mapOfStartTimeToFinishTime.begin();
					while (isTrue)
					{
						if (inTime < iter5->first && iter5->first < outTime)
						{
							isTrue = false;
							break;
						}
						iter5++;
						if (iter5 == conflictGatePosition->m_mapOfStartTimeToFinishTime.end())
							break;
					}
				}
			}
			// 影响滑出的机位(影响别人)
			if (isTrue)
			{
				for (vector<string>::iterator iter4 = gatePosition->m_influenceGatePositionOut.begin(); iter4 != gatePosition->m_influenceGatePositionOut.end(); iter4++)
				{
					GatePosition *conflictGatePosition = GatePosition::s_mapSetOfGatePosition.find(*iter4)->second;
					if (conflictGatePosition->m_mapOfStartTimeToFinishTime.empty())
						continue;
					map<time_t, time_t>::iterator iter5 = conflictGatePosition->m_mapOfStartTimeToFinishTime.begin();
					while (true)
					{
						if (inTime < iter5->second && iter5->second < outTime)
						{
							isTrue = false;
							break;
						}
						iter5++;
						if (iter5 == conflictGatePosition->m_mapOfStartTimeToFinishTime.end())
							break;
					}
				}
			}
			if (isTrue)
			{
				gatePosition->m_mapOfStartTimeToFlight.insert(make_pair(inTime, flight));
				Road *inRoad = gatePosition->m_inRoad;
				Road *outRoad = gatePosition->m_outRoad;
				inRoad->m_vectorOfStartTimeToFinishTime.push_back(make_pair(inTime - 5 * 60, inTime));
				outRoad->m_vectorOfStartTimeToFinishTime.push_back(make_pair(outTime, outTime + 5 * 60));
				inRoad->m_vectorOfStartTimeToFlight.push_back(make_pair(inTime - 5 * 60, flight));
				outRoad->m_vectorOfStartTimeToFlight.push_back(make_pair(outTime, flight));
				flight->gatePosition = gatePosition;
				break;
			}
			else
			{
				gatePosition->m_mapOfStartTimeToFinishTime.erase(iter3);
				continue;
			}
		}
		//////////////////////////////////////////////////////////////////////////
		#pragma endregion

		if (flight->gatePosition != NULL)
		{
			m_heuristicFlightNum++;
			continue;
		}
	}
}

void BestSolution::updateBestValue()
{
	double thisObjValue = BestSolution::getObjectiveFunction();
	if (thisObjValue > m_objValue)
	{
		cout << "最优目标函数值已更新：" << BestSolution::m_objValue << " -> " << thisObjValue << endl;
		m_objValue = thisObjValue;
		m_flightNo.clear();
		m_inTime.clear();
		m_outTime.clear();
		m_gatePositionNo.clear();
		for (map<string, Flight *>::iterator iter = Flight::s_mapSetOfFlight.begin(); iter != Flight::s_mapSetOfFlight.end(); iter++)
		{
			Flight *flight = iter->second;
			if (flight->gatePosition != NULL)
			{
				m_flightNo.push_back(flight->m_flightNo);
				m_inTime.push_back(flight->m_inTime);
				m_outTime.push_back(flight->m_outTime);
				m_gatePositionNo.push_back(flight->gatePosition->m_gatePositionNo);
			}
			//else
			//{
			//	m_flightNo.push_back(flight->m_flightNo);
			//	m_inTime.push_back(flight->m_inTime);
			//	m_outTime.push_back(flight->m_outTime);
			//	m_gatePositionNo.push_back("未分配");
			//}
		}
	}
}

void BestSolution::printBestResult()
{
	string str = "";
	ostringstream oss;
	oss << m_objValue;
	str = oss.str();

	string result1 = "bestResult=" + str + ".csv";
	ofstream fout1(result1);
	//fout1 << "进港航班号,进港时间,出港时间,停机位" << endl;
	for (int i = 0; i < m_flightNo.size(); i++)
		fout1 << m_flightNo[i] << "," << m_inTime[i] << "," << m_outTime[i] << "," << m_gatePositionNo[i] << endl;

	string result2 = "parameter=" + str + ".txt";
	ofstream fout2(result2);
	fout2 << "获得机位的航班量:" << m_distributionFlightCount << endl;
	fout2 << "靠桥航班量:" << m_distanceGatePositionCount << endl;
	fout2 << "靠桥航班的旅客人数:" << m_distanceGatePositionNumberCount << endl;
	fout2 << "执行临时措施的航班量:" << m_temporaryMeasuresSum << endl;
	fout2 << "滑行通道冲突航班量:" << m_conflictCount << endl;

	fout2 << endl << "删除冲突航班数量:" << m_deleteNum << endl;
	fout2 << "启发式安排航班数量:" << m_heuristicFlightNum << endl;
}

double BestSolution::getObjectiveFunction()
{
	int parameter1 = 10;
	int parameter2 = 3;
	int parameter3 = 3;
	int parameter4 = 1;
	int parameter5 = 1;
	double objvalue = 0, objvalue1 = 0, objvalue2 = 0, objvalue3 = 0, objvalue4 = 0, objvalue5 = 0, objvalue6 = 0;
	int distributionFlightCount = 0;
	int distanceGatePositionCount = 0;
	int distanceGatePositionNumberCount = 0;
	int conflictCount = 0;
	int temporaryMeasuresSum = 0;
	for (map<string, Flight *>::iterator iter = Flight::s_mapSetOfFlight.begin(); iter != Flight::s_mapSetOfFlight.end(); iter++)
	{
		Flight *flight = iter->second;
		if (flight->gatePosition != NULL)
			distributionFlightCount++;
		else 
			continue;
		if (flight->gatePosition->m_distance == "近机位")
		{
			distanceGatePositionCount++;
			distanceGatePositionNumberCount = distanceGatePositionNumberCount + flight->m_number;
		}
		if (flight->gatePosition->temporaryMeasures == 1)
			temporaryMeasuresSum++;
	}
	//string str = "";
	//ostringstream oss;
	//oss << BestSolution::m_objValue;
	//str = oss.str();
	//string result = "冲突航班=" + str + ".txt";
	//ofstream fout(result);
	map<string, int> conflictFlights = map<string, int>();
	for (map<string, Road *>::iterator iter = Road::s_mapSetOfRoad.begin(); iter != Road::s_mapSetOfRoad.end(); iter++)
	{
		Road *road = iter->second;
		int mapOfStartTimeToFlightCountNow = 0;
		int mapOfStartTimeToFlightCount = road->m_vectorOfStartTimeToFlight.size();
		if (mapOfStartTimeToFlightCount == 0 || road->m_roadNo == "无限制" || road->m_roadNo == "")
			continue;
		sort(road->m_vectorOfStartTimeToFlight.begin(), road->m_vectorOfStartTimeToFlight.end(), less_first_time_t);
		for (vector<pair<time_t, Flight *>>::iterator iter2 = road->m_vectorOfStartTimeToFlight.begin(); iter2 != --road->m_vectorOfStartTimeToFlight.end(); mapOfStartTimeToFlightCountNow++)
		{
			time_t t1 = iter2->first;
			Flight *flight1 = iter2->second;
			string flight1No = flight1->m_flightNo;
			iter2++;
			time_t t2 = iter2->first;
			Flight *flight2 = iter2->second;
			string flight2No = flight2->m_flightNo;
			if (t1 + 5 * 60 > t2)
			{
				conflictFlights[flight1No] = 1;
				conflictFlights[flight2No] = 1;
			}
		}
	}
	for (map<string, int>::iterator iter2 = conflictFlights.begin(); iter2 != conflictFlights.end(); iter2++)
	{
		if (iter2->second == 1)
		{
			//fout << iter2->first << endl;
			conflictCount++;
		}
	}
	// 6为惩罚项
	for (map<string, GatePosition *>::iterator iter = GatePosition::s_mapSetOfGatePosition.begin(); iter != GatePosition::s_mapSetOfGatePosition.end(); iter++)
	{
		bool mark = false;
		GatePosition * gatePosition = iter->second;
		if (gatePosition->m_mapOfStartTimeToFinishTime.size() < 2)
			continue;
		for (map<time_t, time_t>::iterator iter2 = gatePosition->m_mapOfStartTimeToFinishTime.begin(); iter2 != --gatePosition->m_mapOfStartTimeToFinishTime.end();)
		{
			time_t preInTime = iter2->first;
			time_t preOutTime = iter2->second;
			iter2++;
			time_t nextInTime = iter2->first;
			time_t nextOutTime = iter2->second;
			if (preOutTime + 10 * 60 > nextInTime)
			{
				objvalue6 = -100;// 惩罚项
				mark = true;
				break;
			}
		}
		if (mark == true)
			break;
	}
	objvalue1 = parameter1 * (distributionFlightCount / (double)(Flight::s_flightCount + Flight::s_wrongFlightCount));
	objvalue2 = parameter2 * (distanceGatePositionCount / (double)(Flight::s_flightCount + Flight::s_wrongFlightCount));
	objvalue3 = parameter3 * (distanceGatePositionNumberCount / (double)Flight::s_nemberCount);
	objvalue4 = parameter4 * (temporaryMeasuresSum / (double)(Flight::s_flightCount + Flight::s_wrongFlightCount));
	objvalue5 = parameter5 * (conflictCount / (double)(Flight::s_flightCount + Flight::s_wrongFlightCount));
	objvalue = objvalue1 + objvalue2 + objvalue3 - objvalue4 - objvalue5 + objvalue6;
	if (objvalue < 0)
		objvalue = 0;
	if (objvalue > BestSolution::m_objValue)
	{
		m_distributionFlightCount = distributionFlightCount;
		m_distanceGatePositionCount = distanceGatePositionCount;
		m_distanceGatePositionNumberCount = distanceGatePositionNumberCount;
		m_temporaryMeasuresSum = temporaryMeasuresSum;
		m_conflictCount = conflictCount;
	}
	return objvalue;
}

bool BestSolution::less_first_time_t(const pair<time_t, Flight *> & m1, const pair<time_t, Flight *> & m2)
{
	return m1.first < m2.first;
}

vector<string>	BestSolution::m_flightNo = vector<string>();
vector<string>	BestSolution::m_inTime = vector<string>();
vector<string>	BestSolution::m_outTime = vector<string>();
vector<string>	BestSolution::m_gatePositionNo = vector<string>();
double			BestSolution::m_objValue = 0;
int				BestSolution::m_distributionFlightCount = 0;
int				BestSolution::m_distanceGatePositionCount = 0;
int				BestSolution::m_distanceGatePositionNumberCount = 0;
int				BestSolution::m_temporaryMeasuresSum = 0;
int				BestSolution::m_conflictCount = 0;
vector<vector<int>>	BestSolution::m_solution = vector<vector<int>>();
int				BestSolution::m_deleteNum = 0;
int				BestSolution::m_heuristicFlightNum = 0;