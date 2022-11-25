/* ========================= eCAL LICENSE =================================
 *
 * Copyright (C) 2016 - 2019 Continental Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *      http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * ========================= eCAL LICENSE =================================
*/

/**
 * @brief  System usage monitoring
**/
#include <iostream>
#include <chrono>
#include <thread>
#include "../include/mon/scd_os.h"

#ifdef ECAL_OS_WINDOWS

#include "scd_win_main.h"

class CpuUsage
{
public:
  CpuUsage(void);

  float  GetUsage();
  bool EnoughTimePassed();
private:
  ULONGLONG SubtractTimes(const FILETIME& ftA, const FILETIME& ftB);
  //bool EnoughTimePassed();
  inline bool IsFirstRun() const { return (m_dwLastRun == 0); }

  float         m_nCpuUsage;
  volatile LONG m_lRunCount;
  ULONGLONG     m_dwLastRun;

  // system total times
  FILETIME m_ftPrevSysKernel;
  FILETIME m_ftPrevSysUser;
  FILETIME m_ftPrevSysIdle;

  // process times
  FILETIME m_ftPrevProcKernel;
  FILETIME m_ftPrevProcUser;
};


CpuUsage::CpuUsage(void)
  :m_nCpuUsage(-1)
  ,m_dwLastRun(0)
  ,m_lRunCount(0)
{
  ZeroMemory(&m_ftPrevSysKernel, sizeof(FILETIME));
  ZeroMemory(&m_ftPrevSysUser, sizeof(FILETIME));

  ZeroMemory(&m_ftPrevProcKernel, sizeof(FILETIME));
  ZeroMemory(&m_ftPrevProcUser, sizeof(FILETIME));

}

/**********************************************
* CpuUsage::GetUsage
* returns the percent of the CPU that this process
* has used since the last time the method was called.
* If there is not enough information, -1 is returned.
* If the method is recalled to quickly, the previous value
* is returned.
***********************************************/
float CpuUsage::GetUsage()
{
  //create a local copy to protect against race conditions in setting the 
  //member variable
  float nCpuCopy = m_nCpuUsage;
  if (::InterlockedIncrement(&m_lRunCount) == 1)
  {
    /*
    If this is called too often, the measurement itself will greatly affect the
    results.
    */

    if (!EnoughTimePassed())
    {
      ::InterlockedDecrement(&m_lRunCount);
      return nCpuCopy;
    }

    FILETIME ftSysIdle, ftSysKernel, ftSysUser;
    FILETIME ftProcCreation, ftProcExit, ftProcKernel, ftProcUser;

    if (!GetSystemTimes(&ftSysIdle, &ftSysKernel, &ftSysUser) ||
      !GetProcessTimes(GetCurrentProcess(), &ftProcCreation, &ftProcExit, &ftProcKernel, &ftProcUser))
    {
      ::InterlockedDecrement(&m_lRunCount);
      return nCpuCopy;
    }

    if (!IsFirstRun())
    {
      /*
      CPU usage is calculated by getting the total amount of time the system has operated
      since the last measurement (made up of kernel + user) and the total
      amount of time the process has run (kernel + user).
      */
      ULONGLONG ftSysKernelDiff = SubtractTimes(ftSysKernel, m_ftPrevSysKernel);
      ULONGLONG ftSysUserDiff = SubtractTimes(ftSysUser, m_ftPrevSysUser);

      ULONGLONG ftProcKernelDiff = SubtractTimes(ftProcKernel, m_ftPrevProcKernel);
      ULONGLONG ftProcUserDiff = SubtractTimes(ftProcUser, m_ftPrevProcUser);

      ULONGLONG nTotalSys = ftSysKernelDiff + ftSysUserDiff;
      ULONGLONG nTotalProc = ftProcKernelDiff + ftProcUserDiff;
      

      if (nTotalSys > 0)
      {
        m_nCpuUsage = (float)((100.0* nTotalProc) / nTotalSys);
      }
    }

    m_ftPrevSysKernel = ftSysKernel;
    m_ftPrevSysUser = ftSysUser;

    m_ftPrevProcKernel = ftProcKernel;
    m_ftPrevProcUser = ftProcUser;

#if defined(__MINGW32__)
    m_dwLastRun = GetTickCount();
#else
  #if defined(_WIN64)
    m_dwLastRun = GetTickCount64();
  #else
    m_dwLastRun = GetTickCount();
  #endif
#endif

    nCpuCopy = m_nCpuUsage;
  }

  ::InterlockedDecrement(&m_lRunCount);

  return nCpuCopy;
}

ULONGLONG CpuUsage::SubtractTimes(const FILETIME& ftA, const FILETIME& ftB)
{
  LARGE_INTEGER a, b;
  a.LowPart = ftA.dwLowDateTime;
  a.HighPart = ftA.dwHighDateTime;

  b.LowPart = ftB.dwLowDateTime;
  b.HighPart = ftB.dwHighDateTime;

  return a.QuadPart - b.QuadPart;
}

bool CpuUsage::EnoughTimePassed()
{
  const int minElapsedMS = 250; //milliseconds

#if defined(__MINGW32__)
  ULONGLONG dwCurrentTickCount = GetTickCount();
#else
  #if defined(_WIN64)
  ULONGLONG dwCurrentTickCount = GetTickCount64();
  #else
  ULONGLONG dwCurrentTickCount = GetTickCount();
  #endif
#endif

  return (dwCurrentTickCount - m_dwLastRun) > minElapsedMS; 
}

static CpuUsage g_cpu_usage;

float GetCPULoad(size_t period_ms)
{
  static float usage = 0.0f;
  volatile float cpu_usage;
  g_cpu_usage.GetUsage();
  std::this_thread::sleep_for(std::chrono::milliseconds(period_ms));
  cpu_usage = g_cpu_usage.GetUsage(); 
 
      // -1 means could not be evaluated yet, do not use that value ..
    //  if (cpu_usage != -1) usage = 0.98f * usage + 0.02f * static_cast<float>(cpu_usage);
     if (cpu_usage != -1) usage = cpu_usage;
  return(usage);
}

double GetCpuTime() {
    FILETIME a, b, c, d;
    if (GetProcessTimes(GetCurrentProcess(), &a, &b, &c, &d) != 0) {
        //  Returns total user time.
        //  Can be tweaked to include kernel times as well.
        return
            (double)(d.dwLowDateTime |
                ((unsigned long long)d.dwHighDateTime << 32)) * 0.0000001;
    }
    else {
        //  Handle error
        return 0;
    }
}

#endif /* ECAL_OS_WINDOWS */

#ifdef ECAL_OS_LINUX

#include <cstring>
#include <unistd.h>


struct pstat {
    long unsigned int utime_ticks;
    long int cutime_ticks;
    long unsigned int stime_ticks;
    long int cstime_ticks;
    long unsigned int vsize; // virtual memory size in bytes
    long unsigned int rss; //Resident  Set  Size in bytes
    long unsigned int cpu_total_time;
};

/*
 * read /proc data into the passed struct pstat
 * returns 0 on success, -1 on error
*/
int get_usage(const pid_t pid, struct pstat* result) {
    //convert  pid to string
    char pid_s[20];
    snprintf(pid_s, sizeof(pid_s), "%d", pid);
    char stat_filepath[30] = "/proc/"; strncat(stat_filepath, pid_s,
        sizeof(stat_filepath) - strlen(stat_filepath) - 1);
    strncat(stat_filepath, "/stat", sizeof(stat_filepath) -
        strlen(stat_filepath) - 1);

    FILE* fpstat = fopen(stat_filepath, "r");
    if (fpstat == NULL) {
        perror("FOPEN ERROR ");
        return -1;
    }

    FILE* fstat = fopen("/proc/stat", "r");
    if (fstat == NULL) {
        perror("FOPEN ERROR ");
        fclose(fstat);
        return -1;
    }

    //read values from /proc/pid/stat
    bzero(result, sizeof(struct pstat));
    long int rss;
    if (fscanf(fpstat, "%*d %*s %*c %*d %*d %*d %*d %*d %*u %*u %*u %*u %*u %lu"
        "%lu %ld %ld %*d %*d %*d %*d %*u %lu %ld",
        &result->utime_ticks, &result->stime_ticks,
        &result->cutime_ticks, &result->cstime_ticks, &result->vsize,
        &rss) == EOF) {
        fclose(fpstat);
        return -1;
    }
    fclose(fpstat);
    result->rss = rss; //* getpagesize();

    //read+calc cpu total time from /proc/stat
    long unsigned int cpu_time[10];
    bzero(cpu_time, sizeof(cpu_time));
    if (fscanf(fstat, "%*s %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu",
        &cpu_time[0], &cpu_time[1], &cpu_time[2], &cpu_time[3],
        &cpu_time[4], &cpu_time[5], &cpu_time[6], &cpu_time[7],
        &cpu_time[8], &cpu_time[9]) == EOF) {
        fclose(fstat);
        return -1;
    }

    fclose(fstat);

    for (int i = 0; i < 10; i++)
        result->cpu_total_time += cpu_time[i];

    return 0;
}
/*
*
* calculates the elapsed CPU usage between 2 measuring points. in percent
*/
void calc_cpu_usage_pct(const struct pstat* cur_usage,
    const struct pstat* last_usage,
    float& ucpu_usage)
{
    unsigned long total_time_diff = (last_usage->cpu_total_time - cur_usage->cpu_total_time);
    unsigned long user_ticks = (last_usage->utime_ticks + last_usage->cutime_ticks) - (cur_usage->utime_ticks + cur_usage->cutime_ticks);
    ucpu_usage = (float)((100.0 * user_ticks) / total_time_diff);


}



float GetCPULoad(size_t period_ms)
{
    int pid = getpid();
    std::cout << "pid:" << pid << std::endl;
    std::shared_ptr<pstat> curr_stat = std::make_shared<pstat>();
    std::shared_ptr<pstat> last_stat = std::make_shared<pstat>();
    float ucpu_usage = 0.0f;
    if (get_usage(pid, curr_stat.get()) == 0)
        std::this_thread::sleep_for(std::chrono::milliseconds(period_ms));
    get_usage(pid, last_stat.get());
    calc_cpu_usage_pct(curr_stat.get(), last_stat.get(), ucpu_usage);



    return(ucpu_usage);
}

double GetCpuTime() {
    return 0.0;
}
#endif /* ECAL_OS_LINUX */
