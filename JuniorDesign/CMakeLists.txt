cmake_minimum_required(VERSION 3.5.1)

project(JuniorDesign)

set(CMAKE_CXX_STANDARD 14)

include_directories(${CMAKE_SOURCE_DIR}/libs)

file(GLOB_RECURSE allCodeFiles RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.hpp" "*.cpp" "*.h")
# copies json files from source directory to build directory
file(GLOB_RECURSE jsonFiles RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.json")

foreach (jsonFile ${jsonFiles})
  file(COPY ${jsonFile} DESTINATION "${CMAKE_BINARY_DIR}")
endforeach()

file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/sample-team.txt" DESTINATION "${CMAKE_BINARY_DIR}")

add_executable(JuniorDesign ${allCodeFiles})

#Visual studio settings
if(WIN32)
  #Warning level 4
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
  #CWD
  set_target_properties(JuniorDesign PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")
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
