/*************************************************
版权: 任家硕
作者: 任家硕
日期: 2015-06-20
功能: 机场停机位资源分配优化_随机数类
修改历史:

**************************************************/

/*<remark>=========================================================
/// <summary>
/// 机场停机位资源分配优化_随机数类
/// </summary>
===========================================================</remark>*/

#pragma once

#include "random.h"

void Random::Initialize()
{
	srand(unsigned(time(0)));//以当前时间作为随机数种子	
	GetRandom();//随机数生成的时候，第一个数为坏点，清除掉
}

int Random::GetRandom(int min, int max)
{
	//rand(）是函数库中的函数，作用是产生一个1到RAND_MAX之间的整数
	return (int)(min + (max - min + 1) * rand() / (RAND_MAX + 1.0));
}

double Random::GetRandom()
{
	return (rand() / (RAND_MAX + 0.0));
}

vector<int> Random::GetRandomSequence(int lengthOfSeq)
{
	vector<int> randomSeq(lengthOfSeq);

	//初始化
	for (int i = 0; i < lengthOfSeq; i++)
	{
		randomSeq[i] = i;
	}

	//两两交换
	for (int i = 0; i < lengthOfSeq; i++)
	{
		int firstPos = GetRandom(0, lengthOfSeq - 1);
		int secondPos = GetRandom(0, lengthOfSeq - 1);
		int temp = randomSeq[firstPos];
		randomSeq[firstPos] = randomSeq[secondPos];
		randomSeq[secondPos] = temp;
	}

	return randomSeq;
}