

/**
 * @file   atech_process_severity.h
 * @brief  Atech process severity
**/

#pragma once

/**
 * @brief  Process severity 
**/
enum scd_Process_Severity
{
  proc_sev_unknown   =  0,  /*!<  0 == condition unknown     */
  proc_sev_healthy   =  1,  /*!<  1 == process healthy       */
  proc_sev_warning   =  2,  /*!<  2 == process warning level */
  proc_sev_critical  =  3,  /*!<  3 == process critical      */
  proc_sev_failed    =  4,  /*!<  4 == process failed        */
};



