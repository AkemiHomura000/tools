# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles/NodeSim_autogen.dir/AutogenUsed.txt"
  "CMakeFiles/NodeSim_autogen.dir/ParseCache.txt"
  "NodeSim_autogen"
  )
endif()
