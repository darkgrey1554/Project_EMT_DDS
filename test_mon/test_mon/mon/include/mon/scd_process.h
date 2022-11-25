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
 * @file   ecal_process.h
 * @brief  eCAL process interface
**/

#pragma once

#include <string>

#include "scd_os.h"
#include "scd_process_mode.h"
#include "scd_process_severity.h"


namespace atech
{
 
  namespace Process
  {
    /**
     * @brief  Dump configuration to console. 
    **/
     void DumpConfig();

    /**
     * @brief  Dump configuration to std::string.
     *
     * @param  cfg_s_  String to dump configuration to.
    **/
     void DumpConfig(std::string& cfg_s_);

    /**
     * @brief  Get current host name. 
     *
     * @return  Host name or empty string if failed. 
    **/
     std::string GetHostName();

    /**
     * @brief  Get unique host id.
     *
     * @return  Host id or zero if failed.
    **/
    int GetHostID();

    /**
     * @brief  Get current unit name (defined via eCAL::Initialize). 
     *
     * @return  Unit name or empty string if failed. 
    **/
     std::string GetUnitName();

    /**
     * @brief  Get current process parameter (defined via eCAL::Initialize(argc_, arg_v). 
     *
     * @param sep_  Separator. 
     *
     * @return  Task parameter separated by 'sep_' or zero string if failed. 
    **/
     std::string GetTaskParameter(const char* sep_);

    /**
     * @brief  Sleep current thread. 
     *
     * @param  time_ms_  Time to sleep in ms. 
    **/
     void SleepMS(long time_ms_);

    /**
     * @brief  Get current process id. 
     *
     * @return  The process id. 
    **/
     int GetProcessID();

    /**
     * @brief  Get current process id as string.
     *
     * @return  The process id.
    **/
     std::string GetProcessIDAsString();

    /**
     * @brief  Get current process name. 
     *
     * @return  Process name length or zero if failed. 
    **/
     std::string GetProcessName();

    /**
     * @brief  Get current process parameter. 
     *
     * @return  Process parameter or empty string if failed. 
    **/
    std::string GetProcessParameter();

    /**
     * @brief  Get CPU usage of current process. 
     *
     * @return  The CPU usage in percent. 
    **/
    float GetProcessCpuUsage();

    /**
     * @brief  Get memory usage of current process. 
     *
     * @return  The memory usage in bytes. 
    **/
     unsigned long GetProcessMemory();

    /**
     * @deprecated  Use the function GetWClock() instead
    **/
    [[deprecated("use GetWClock() instead")]]
     long long GetSClock();

    /**
     * @deprecated  Use the function GetWBytes() instead
    **/

    [[deprecated("use GetWBytes() instead")]]
    long long GetSBytes();

    /**
     * @brief  Get the write clock of the current process. 
     *
     * @return  The message write count per second. 
    **/
     long long GetWClock();

    /**
     * @brief  Get the write bytes of the current process. 
     *
     * @return  The message write bytes per second. 
    **/
     long long GetWBytes();

    /**
     * @brief  Get the read clock of the current process. 
     *
     * @return  The message read count per second. 
    **/
     long long GetRClock();

    /**
     * @brief  Get the read bytes of the current process. 
     *
     * @return  The message read bytes per second. 
    **/
    long long GetRBytes();

    /**
     * @brief  Set process state info. 
     *
     * @param severity_  Severity. 
     * @param level_     Severity level.
     * @param info_      Info message.
     *
    **/
     void SetState(scd_Process_eSeverity severity_, scd_Process_eSeverity_Level level_, const char* info_);

    /**
     * @brief  Add registration callback.
     *
     * @param event_          The type of registration.
     * @param callback_       The callback.
     *
     * @return  Zero if succeeded.
    **/
    //ECAL_API int AddRegistrationCallback(enum scd_Registration_Event event_, RegistrationCallbackT callback_);

    /**
    * @brief  Remove registration callback.
    *
    * @param event_          The type of registration.
    *
    * @return  Zero if succeeded.
    **/
    //ECAL_API int RemRegistrationCallback(enum scd_Registration_Event event_);

    /**
     * @brief  Start specified process (windows only). 
     *
     * @param proc_name_       Process name. 
     * @param proc_args_       Process argument string. 
     * @param working_dir_     Working directory. 
     * @param create_console_  Start process in own console window (Windows only). 
     * @param process_mode_    Start process normal, hidden, minimized, maximized (Windows only). 
     * @param block_           Block until process finished. 
     *
     * @return  Process id or zero if failed. 
    **/
     int StartProcess(const char* proc_name_, const char* proc_args_, const char* working_dir_, bool create_console_, scd_Process_eStartMode process_mode_, bool block_);

    /**
     * @brief  Stop specified process (windows only). 
     *
     * @param proc_name_  Process name. 
     *
     * @return  True if successful. 
    **/
    bool StopProcess(const char* proc_name_);

    /**
     * @brief  Stop specified process (windows only). 
     *
     * @param proc_id_    Process id. 
     *
     * @return  True if successful. 
    **/
     bool StopProcess(int proc_id_);
  }
  /** @example process.cpp
  * This is an example how functions from the eCAL::Process namespace may be used.
  */
}

