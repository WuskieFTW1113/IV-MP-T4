cmake_minimum_required(VERSION 3.12.2)

project(ivmpGameLauncher2)

set(CMAKE_CONFIGURATION_TYPES "Release"
    CACHE STRING "Configuration types" FORCE)

add_executable(ivmpGameLauncher2
  WIN32
  draw.cpp
  draw.h
  gameChecks.cpp
  gameChecks.h
  icon1.ico
  iv.ico
  ivmpGameLauncher2.rc
  ivTarget.h
  main.cpp
  memoryPipe.cpp
  memoryPipe.h
  resource.h
  streamHandler.cpp
  streamHandler.h
  version.cpp
  version.h
)
# Output file name
set_target_properties(ivmpGameLauncher2
  PROPERTIES
  OUTPUT_NAME_RELEASE ivmpGameLauncher
)

# Additional include directories
set_property(TARGET ivmpGameLauncher2
  APPEND PROPERTY INCLUDE_DIRECTORIES
  $<$<CONFIG:Release>:
    D:\RESTAURAÇAO\DxSDK\Include;
    D:\GWEN\include;
    ..\RakNet2;
    D:\lua-5.3.4\src>
)

# Preprocessor definitions
target_compile_definitions(ivmpGameLauncher2 PRIVATE
  $<$<CONFIG:Release>:WIN32;NDEBUG;_WINDOWS>
)

# Minimal rebuild
if (MSVC)
  target_compile_options(ivmpGameLauncher2 PRIVATE
    "$<$<CONFIG:Release>:/Gm->"
  )
endif ()

# Precompiled header files
if (MSVC)
  target_compile_options(ivmpGameLauncher2 PRIVATE
    ""
  )
endif ()

# Link libraries
set_property(TARGET ivmpGameLauncher2
  APPEND PROPERTY LINK_LIBRARIES
  "$<$<CONFIG:Release>:../Release/RakNet2.lib;ws2_32.lib;D:/RESTAURAÇAO/DxSDK/Lib/x86/d3d9.lib;D:/RESTAURAÇAO/DxSDK/Lib/x86/d3dx9.lib;D:/GWEN/lib/windows/vs2010/unittest.lib;D:/GWEN/lib/windows/vs2010/GWEN-Renderer-DirectX9.lib;D:/GWEN/lib/windows/vs2010/gwen_static.lib;version.lib>"
)
