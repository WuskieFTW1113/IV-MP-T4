cmake_minimum_required(VERSION 3.12.2)

project(Client)

set(CMAKE_CONFIGURATION_TYPES "Release"
    CACHE STRING "Configuration types" FORCE)

add_library(Client SHARED
  ../SharedDefines/paramHelper.cpp
  ../SharedDefines/paramHelper.h
  animList.cpp
  animList.h
  cameraMemory.cpp
  cameraMemory.h
  chatInput.cpp
  chatInput.h
  chatWindow.cpp
  chatWindow.h
  colorHelper.cpp
  colorHelper.h
  ControlState.h
  CPad.cpp
  CPad.h
  CPadData.h
  CPatcher.cpp
  CPatcher.h
  CustomFiberThread.cpp
  CustomFiberThread.h
  dialogList.cpp
  dialogList.h
  dummyVehicle.cpp
  dummyVehicles.h
  EntityMemory.cpp
  EntityMemory.h
  gameAudio.cpp
  gameAudio.h
  gameBlips.cpp
  gameBlips.h
  gameCheckPoints.cpp
  gameCheckPoints.h
  gameLibs.cpp
  gameLibs.h
  gameMemory.cpp
  gameMemory.h
  gameObjects.cpp
  gameObjects.h
  gameVectors.h
  gameWorld.cpp
  gameWorld.h
  guiManager.cpp
  guiManager.h
  guiMouse.cpp
  guiMouse.h
  inVehicleSync.cpp
  ivGUI.cpp
  ivGUI.h
  keyboard.cpp
  keyboard.h
  localPlayer.cpp
  localPlayerAnim.cpp
  localPlayerAnim.h
  localPlayerCam.cpp
  localPlayerCamera.cpp
  localPlayerCamera.h
  localPlayerInit.cpp
  localPlayerInputsHandler.cpp
  localPlayerInvent.cpp
  localPlayerInvent.h
  localVehicleEntry.cpp
  LuaVM.cpp
  LuaVM.h
  MainHook.cpp
  MainHook.h
  modelsHandler.cpp
  modelsHandler.h
  NetworkManager.cpp
  NetworkManager.h
  npcRecorder.cpp
  npcRecorder.h
  Offsets.h
  Offsetscpp.cpp
  openMpWindow.cpp
  openMpWindow.h
  padCallBack.cpp
  padCallBack.h
  padControl.cpp
  padControl.h
  padData.h
  padKeys.h
  parseConnectionStates.cpp
  parseConnectionStates.h
  parseInVehiclePlayerPacket.cpp
  parseInVehiclePlayerPacket.h
  parseOnFootPlayerPacket.cpp
  parseOnFootPlayerPacket.h
  PedStructureReference.cpp
  PhysicalMemory.h
  playerDisconnected.cpp
  playerDisconnected.h
  playerDriveBy.cpp
  playerDriveBy.h
  playerList.cpp
  playerList.h
  playerMemory.cpp
  playerMemory.h
  players.cpp
  players.h
  playerServerFunctions.cpp
  playerServerFunctions.h
  playerSkins.cpp
  playerSkins.h
  prepareClientToServerPacket.cpp
  prepareClientToServerPacket.h
  pulsePlayers.cpp
  pulseVehicles.cpp
  removeVehicle.cpp
  removeVehicle.h
  requestAndReceivePlayerData.cpp
  requestAndReceivePlayerData.h
  requestAndReceiveVehicleData.cpp
  rotationMatrix.cpp
  rotationMatrix.h
  simpleMath.cpp
  simpleMath.h
  spawnRemotePlayer.cpp
  spawnVehicle.cpp
  syncPlayer.cpp
  syncPlayer.h
  taskMemory.h
  teamSpeak.cpp
  teamSpeak.h
  testVehicleInputs.h
  vehicleMemory.cpp
  vehicleMemory.h
  vehicleRequestAndReceiveData.h
  vehicles.cpp
  vehicles.h
  vehiclesList.cpp
  vehiclesList.h
  vehicleSync.cpp
  vehicleSync.h
  vehicleTasks.cpp
  vehicleTasks.h
  weaponInfo.cpp
  weaponInfo.h
)
# Additional include directories
set_property(TARGET Client
  APPEND PROPERTY INCLUDE_DIRECTORIES
  $<$<CONFIG:Release>:
    ..\ArusHook\;
    ..\RakNet2;
    D:\RESTAURAÇAO\DxSDK\Include;
    D:\GWEN\include;
    D:\lua-5.3.4\src>
)

# Preprocessor definitions
target_compile_definitions(Client PRIVATE
  $<$<CONFIG:Release>:WIN32;NDEBUG;_WINDOWS;_USRDLL;CLIENT_EXPORTS>
)

# Minimal rebuild
if (MSVC)
  target_compile_options(Client PRIVATE
    "$<$<CONFIG:Release>:/Gm->"
  )
endif ()

# Precompiled header files
if (MSVC)
  target_compile_options(Client PRIVATE
    ""
  )
endif ()

# Link libraries
set_property(TARGET Client
  APPEND PROPERTY LINK_LIBRARIES
  "$<$<CONFIG:Release>:../Release/RakNet2.lib;ws2_32.lib;D:/RESTAURAÇAO/DxSDK/Lib/x86/d3d9.lib;D:/RESTAURAÇAO/DxSDK/Lib/x86/d3dx9.lib;D:/GWEN/lib/windows/vs2010/GWEN-Renderer-DirectX9.lib;D:/GWEN/lib/windows/vs2010/gwen_static.lib;../Release/clientLibs.lib;psapi.lib;../Client/bass.lib>"
)
