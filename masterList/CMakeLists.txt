cmake_minimum_required(VERSION 3.12.2)

project(masterList)

set(CMAKE_CONFIGURATION_TYPES "Release"
    CACHE STRING "Configuration types" FORCE)

add_executable(masterList
  main.cpp
)
# Additional include directories
set_property(TARGET masterList
  APPEND PROPERTY INCLUDE_DIRECTORIES
  $<$<CONFIG:Release>:
    ..\RakNet2>
)

# Preprocessor definitions
target_compile_definitions(masterList PRIVATE
  $<$<CONFIG:Release>:WIN32;NDEBUG;_CONSOLE>
)

# Minimal rebuild
if (MSVC)
  target_compile_options(masterList PRIVATE
    "$<$<CONFIG:Release>:/Gm->"
  )
endif ()

# Precompiled header files
if (MSVC)
  target_compile_options(masterList PRIVATE
    ""
  )
endif ()

# Link libraries
set_property(TARGET masterList
  APPEND PROPERTY LINK_LIBRARIES
  "$<$<CONFIG:Release>:ws2_32.lib;../Release/RakNet2.lib>"
)
