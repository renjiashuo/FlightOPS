/*************************************************
版权: 任家硕
作者: 任家硕
日期: 2016-10-15
功能: 机场停机位资源分配优化_常用工具
修改历史:

**************************************************/

/*<remark>=========================================================
/// <summary>
/// 机场停机位资源分配优化_常用工具
/// </summary>
===========================================================</remark>*/

#pragma once

#include "stdio.h"
#include "limits.h"
#include "math.h"
#include <ctime>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <map>
#include <list>
#include <queue>
#include <stack>
#include <vector>
#include <stdlib.h>
#include <time.h>
#include <string> 
#include <algorithm> 
#include <sstream>
#include <ilcplex/ilocplex.h>

#define accuracy 1.0e-6

using namespace std;

#define INT_LARGE		INT_MAX / 2;	// 大的整数
#define INT_SMALL		INT_MIN / 2;	// 小的整数