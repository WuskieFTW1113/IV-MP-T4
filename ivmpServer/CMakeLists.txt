cmake_minimum_required(VERSION 3.12.2)

project(ivmpServer)

set(CMAKE_CONFIGURATION_TYPES "Release"
    CACHE STRING "Configuration types" FORCE)

add_library(ivmpServer SHARED
  ../SharedDefines/packetsIds.h
  API/apiBlips.h
  API/apiCheckPoints.h
  API/apiColoredChat.h
  API/apiDialog.h
  API/apiMath.h
  API/apiNpc.h
  API/apiObjects.h
  API/apiParamHelper.h
  API/apiPlayer.h
  API/apiPlayerEvents.h
  API/apiPlayerInputs.h
  API/apiVehicle.h
  API/apiVehicleEvents.h
  API/apiWorld.h
  API/playerList.h
  API/vehicleList.h
  apiBlips.cpp
  apiCheckPoints.cpp
  apiColoredChat.cpp
  apiDebugTask.cpp
  apiDebugTask.h
  apiDialog.cpp
  apiMath.cpp
  apiNpc.cpp
  apiObjects.cpp
  apiParamHelper.cpp
  apiPlayerInputs.cpp
  apiPlayerManager.cpp
  apiPlayerManager.h
  apiPlayers.cpp
  apiVehicle.cpp
  apiWorld.cpp
  blipController.h
  chatFunctions.cpp
  checkPointsController.cpp
  checkPointsController.h
  dialogManager.h
  easylogging++.h
  entitySpawned.cpp
  entitySpawned.h
  inVehicleInputs.h
  mainNetwork.cpp
  mainNetwork.h
  networkManager.cpp
  networkManager.h
  npcChar.cpp
  npcChar.h
  npcManager.cpp
  npcManager.h
  objectsController.cpp
  objectsController.h
  onFootInputs.h
  playerConnectionState.cpp
  playerConnectionState.h
  playerFunctions.h
  playerList.cpp
  players.cpp
  players.h
  playerVehicleEntrance.cpp
  receiveClientCredentials.cpp
  receiveClientCredentials.h
  receiveFootSync.cpp
  receiveFootSync.h
  receivePlayerChat.cpp
  receivePlayerChat.h
  receiveVehicleHps.cpp
  receiveVehicleSync.cpp
  sendClientRequestedData.cpp
  sendClientRequestedData.h
  stdParamHelper.cpp
  stdParamHelper.h
  streamCheckPoint.cpp
  streamData.cpp
  streamFunctions.h
  streamObject.cpp
  streamPlayer.cpp
  streamPlayerList.cpp
  streamPlayerList.h
  streamVehicle.cpp
  vehicleDataRequested.cpp
  vehicleList.cpp
  vehicles.cpp
  vehicles.h
  vehicleSyncDeclarations.h
  vehStreamConfirmation.cpp
  vehStreamConfirmation.h
  worldsController.cpp
  worldsController.h
)
# Additional include directories
set_property(TARGET ivmpServer
  APPEND PROPERTY INCLUDE_DIRECTORIES
  $<$<CONFIG:Release>:
    ..\Raknet2\;
    ./API;
    ../SharedDefines>
)

# Preprocessor definitions
target_compile_definitions(ivmpServer PRIVATE
  $<$<CONFIG:Release>:WIN32;_WINDOWS;_USRDLL;IVMPSERVER_EXPORTS>
)

# Minimal rebuild
if (MSVC)
  target_compile_options(ivmpServer PRIVATE
    "$<$<CONFIG:Release>:/Gm->"
  )
endif ()

# Precompiled header files
if (MSVC)
  target_compile_options(ivmpServer PRIVATE
    ""
  )
endif ()

# Link libraries
set_property(TARGET ivmpServer
  APPEND PROPERTY LINK_LIBRARIES
  "$<$<CONFIG:Release>:../Release/RakNet2.lib;ws2_32.lib;../Release/ivmpServerMasterCon.lib>"
)
