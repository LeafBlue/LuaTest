# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles\\LuaTest_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\LuaTest_autogen.dir\\ParseCache.txt"
  "LuaTest_autogen"
  )
endif()
