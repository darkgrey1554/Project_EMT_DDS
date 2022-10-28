project(mon_lib)

find_package(Threads REQUIRED)

set (scd_mon_includes
  
  include/mon/scd_os.h
  include/mon/scd_process.h
  include/mon/scd_process_mode.h
  include/mon/scd_process_severity.h
)
set(scd_mon_src
    src/sys_usage.h
	src/sys_usage.cpp
	src/scd_win_main.h
	src/scd_process.cpp
	src/scd_global_accessors.h
	src/scd_global_accessors.cpp
)


add_library(${PROJECT_NAME}
       ${scd_mon_includes}
        ${scd_mon_src}
         )

add_library(eCAL::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME} PRIVATE src/)
target_include_directories(${PROJECT_NAME} PUBLIC
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
		$<INSTALL_INTERFACE:include>
		)
install(TARGETS ${TARGET_NAME}
		# IMPORTANT: Add the library to the "export-set"
		EXPORT eCALCoreTargets
		ARCHIVE       DESTINATION "${scd_install_archive_dir}" COMPONENT sdk
		LIBRARY       DESTINATION "${scd_install_lib_dir}"     COMPONENT sdk
		)
#target_link_libraries (${PROJECT_NAME} 	PRIVATE utils )

set_property(TARGET ${PROJECT_NAME} PROPERTY FOLDER lib)
