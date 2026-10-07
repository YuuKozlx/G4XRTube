cmake_minimum_required(VERSION 3.16 FATAL_ERROR)

project(G4XRTube VERSION 1.0.0 LANGUAGES CXX)

find_package(Geant4 11.4 REQUIRED ui_all vis_all)
include(${Geant4_USE_FILE})

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

file(GLOB G4XRTUBE_SOURCES CONFIGURE_DEPENDS
  "${PROJECT_SOURCE_DIR}/src/*.cc"
)

add_executable(G4XRTube main.cc ${G4XRTUBE_SOURCES})
target_include_directories(G4XRTube PRIVATE "${PROJECT_SOURCE_DIR}/include")
target_link_libraries(G4XRTube PRIVATE ${Geant4_LIBRARIES})

# Copy run-time files next to the executable in the build tree. This mirrors
# the layout used by the official Geant4 examples and keeps batch commands
# such as `./G4XRTube H100.mac` simple.
set(G4XRTUBE_RUNTIME_FILES
  H100.mac
  L55.mac
  N100.mac
  W150.mac
  gui.mac
  init_vis.mac
  vis.mac
  plot.C
  geant4_gui_launcher.py
)

foreach(runtime_file IN LISTS G4XRTUBE_RUNTIME_FILES)
  configure_file(
    "${PROJECT_SOURCE_DIR}/${runtime_file}"
    "${PROJECT_BINARY_DIR}/${runtime_file}"
    COPYONLY
  )
endforeach()

install(TARGETS G4XRTube RUNTIME DESTINATION bin)
install(FILES ${G4XRTUBE_RUNTIME_FILES}
  DESTINATION share/G4XRTube
)
