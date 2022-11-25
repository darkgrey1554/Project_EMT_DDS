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

#include "scd_global_accessors.h"


namespace atech
{

  CGlobals*                     g_globals_ctx(nullptr);
  std::atomic<int>              g_globals_ctx_ref_cnt;
  std::atomic<int>              g_shutdown;

  std::string                   g_host_name;
  int                           g_host_id(0);
  std::string                   g_unit_name;
  std::vector<std::string>      g_task_parameter;

  std::string                   g_process_name;
  std::string                   g_process_par;
  int                           g_process_id(0);
  std::string                   g_process_id_s;
  std::string                   g_process_info;
  
  scd_Process_Severity        g_process_severity(proc_sev_unknown);
   

}