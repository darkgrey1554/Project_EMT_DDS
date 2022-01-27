#include <iostream>
#include "TimeConverter.hpp"

int main()
{

	auto now = TimeConverter::GetTime_SysClock();

	long long now_ll = TimeConverter::SysClockToLLmcs(now);

	auto now_res = TimeConverter::LLmcsToSysClock(now_ll);

	TimePoint now_tp= TimeConverter::SysClockToTimePoint(now);

	auto nn = TimeConverter::GetTime_LLmcs();

	auto n2 = TimeConverter::GetTime_TimePoint();

}
