
/**
 * @file   atech_process_mode.h
 * @brief  aTech process start mode
**/

#pragma once

/**
 * @brief  Process start mode (StartProcess) 
**/
enum scd_Process_StartMode
{
  proc_smode_normal     =  0,  /*!<  0 == start mode normal     */
  proc_smode_hidden     =  1,  /*!<  1 == start mode hidden     */
  proc_smode_minimized  =  2,  /*!<  2 == start mode minimized  */
  proc_smode_maximized  =  3,  /*!<  3 == start mode maximized  */
};
