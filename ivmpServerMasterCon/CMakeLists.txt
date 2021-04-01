cmake_minimum_required(VERSION 3.12.2)

project(ivmpServerMasterCon)

set(CMAKE_CONFIGURATION_TYPES "Release"
    CACHE STRING "Configuration types" FORCE)

add_executable(ivmpServerMasterCon
  includes.h
  main.cpp
)
# Additional include directories
set_property(TARGET ivmpServerMasterCon
  APPEND PROPERTY INCLUDE_DIRECTORIES
  $<$<CONFIG:Release>:
    ..\RakNet2;
    ..\SharedDefines>
)

# Preprocessor definitions
target_compile_definitions(ivmpServerMasterCon PRIVATE
  $<$<CONFIG:Release>:WIN32;NDEBUG;_USRDLL;IVMPSERVERMASTERCON_EXPORTS>
)

# Minimal rebuild
if (MSVC)
  target_compile_options(ivmpServerMasterCon PRIVATE
    "$<$<CONFIG:Release>:/Gm->"
  )
endif ()

# Precompiled header files
if (MSVC)
  target_compile_options(ivmpServerMasterCon PRIVATE
    ""
  )
endif ()

# Link libraries
set_property(TARGET ivmpServerMasterCon
  APPEND PROPERTY LINK_LIBRARIES
  "$<$<CONFIG:Release>:../Release/RakNet2.lib;ws2_32.lib>"
)
