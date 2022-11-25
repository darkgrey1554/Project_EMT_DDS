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
 * @brief  eCAL core functions
**/

#pragma once
#include <atomic>
#include <string>
#include <vector>

#include "../include/mon/scd_process_severity.h"


// Forward declaration of global accessible classes
namespace atech
{
  class  CGlobals;
  
  // Declaration of getter functions for globally accessible variable instances
  CGlobals*              g_globals();
  

  // declaration of globally accessible variables
  extern CGlobals*                     g_globals_ctx;
  extern std::atomic<int>              g_globals_ctx_ref_cnt;
  extern std::atomic<int>              g_shutdown;

  extern std::string                   g_default_ini_file;

  extern std::string                   g_host_name;
  extern int                           g_host_id;
  extern std::string                   g_unit_name;
  extern std::vector<std::string>      g_task_parameter;

  extern std::string                   g_process_name;
  extern std::string                   g_process_par;
  extern int                           g_process_id;
  extern std::string                   g_process_id_s;
  extern std::string                   g_process_info;
 

  extern scd_Process_Severity          g_process_severity;
 
 
   
}
