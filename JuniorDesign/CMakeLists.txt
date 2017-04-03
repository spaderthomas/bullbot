cmake_minimum_required(VERSION 3.8)

project(JuniorDesign)

#Make sure we can include everything in project directory
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories("Dependencies/Include")

link_directories(${CMAKE_CURRENT_SOURCE_DIR})
link_directories("Dependencies/Lib")

#Find all source, header, and shader files to add to build 
file(GLOB_RECURSE allCodeFiles RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.hpp" "*.cpp" "*.h" "*.c")

#Compile them to an executable
if(WIN32)
  add_executable(JuniorDesign WIN32 ${allCodeFiles})
  #Warning level 4
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
  set_target_properties(JuniorDesign PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_BINARY_DIR}")
elseif(UNIX)
  add_executable(JuniorDesign ${allCodeFiles})
endif()

#Fix Visual studio source groups to correspond to the folder they're in
foreach(src ${allCodeFiles})
  get_filename_component(dir ${src} DIRECTORY)
  if(dir)
	string(REPLACE "/" "\\" fixedDir ${dir})
	source_group(${fixedDir} FILES ${src})
  else()
	source_group("\\\\" FILES ${src})
  endif()
endforeach()