file(GLOB SOURCES src/*.cpp)
idf_component_register(
  SRCS
    "tof_sensor.cpp"
    ${SOURCES}
  INCLUDE_DIRS "include"
)

idf_build_set_property(COMPILE_OPTIONS "-O2" APPEND)
