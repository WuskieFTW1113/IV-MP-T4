cmake_minimum_required(VERSION 3.12.2)

project(clientLibs)

set(CMAKE_CONFIGURATION_TYPES "Release"
    CACHE STRING "Configuration types" FORCE)

add_library(clientLibs SHARED
  d3d.cpp
  d3d.h
  dumpManager.h
  EFLCScripts.cpp
  EFLCScripts.h
  execution.cpp
  execution.h
  gameWindow.cpp
  gameWindow.h
  keyHook.h
  Main.cpp
)
# Additional include directories
set_property(TARGET clientLibs
  APPEND PROPERTY INCLUDE_DIRECTORIES
  $<$<CONFIG:Release>:
    E:\DxSDK\Include;
    D:\GWEN\include>
)

# Preprocessor definitions
target_compile_definitions(clientLibs PRIVATE
  $<$<CONFIG:Release>:WIN32;NDEBUG;_WINDOWS;_USRDLL;CLIENTLIBS_EXPORTS>
)

# Minimal rebuild
if (MSVC)
  target_compile_options(clientLibs PRIVATE
    "$<$<CONFIG:Release>:/Gm->"
  )
endif ()

# Precompiled header files
if (MSVC)
  target_compile_options(clientLibs PRIVATE
    ""
  )
endif ()
