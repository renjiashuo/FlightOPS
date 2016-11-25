/*************************************************
版权: 任家硕
作者: 任家硕
日期: 2016-10-15
功能: 机场停机位资源分配优化_时间类
修改历史:

**************************************************/

/*<remark>=========================================================
/// <summary>
/// 机场停机位资源分配优化_时间类
/// </summary>
===========================================================</remark>*/

#pragma once

#include "time.h"

time_t Time::StringToDatetime(string str)
{
	char *cha = (char*)str.data();
	tm tm_;
	int year, month, day, hour, minute, second;
	sscanf_s(cha, "%d-%d-%d %d:%d:%d", &year, &month, &day, &hour, &minute, &second);
	tm_.tm_year = year - 1900;
	tm_.tm_mon = month - 1;
	tm_.tm_mday = day;
	tm_.tm_hour = hour;
	tm_.tm_min = minute;
	tm_.tm_sec = second;
	tm_.tm_isdst = 0;
	time_t t_ = mktime(&tm_); //已经减了8个时区  
	return t_; //秒时间  
}
string Time::DatetimeToString(time_t time)
{
	tm *tm_ = localtime(&time);
	int year, month, day, hour, minute, second;
	year = tm_->tm_year + 1900;
	month = tm_->tm_mon + 1;
	day = tm_->tm_mday;
	hour = tm_->tm_hour;
	minute = tm_->tm_min;
	second = tm_->tm_sec;
	char yearStr[5], monthStr[3], dayStr[3], hourStr[3], minuteStr[3], secondStr[3];
	_itoa(year, yearStr, 10);
	_itoa(month, monthStr, 10);
	_itoa(day, dayStr, 10);
	_itoa(hour, hourStr, 10);
	_itoa(minute, minuteStr, 10);
	if (minuteStr[1] == '\0')
	{
		minuteStr[2] = '\0';
		minuteStr[1] = minuteStr[0];
		minuteStr[0] = '0';
	}
	_itoa(second, secondStr, 10);
	if (secondStr[1] == '\0')
	{
		secondStr[2] = '\0';
		secondStr[1] = secondStr[0];
		secondStr[0] = '0';
	}
	char s[20];
	sprintf(s, "%s-%s-%s %s:%s:%s", yearStr, monthStr, dayStr, hourStr, minuteStr, secondStr);
	string str(s);
	return str;
}