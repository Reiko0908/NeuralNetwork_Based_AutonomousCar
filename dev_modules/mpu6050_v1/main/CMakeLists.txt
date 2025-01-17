file(GLOB SOURCES src/*.cpp)
set(COMPONENTS main)
idf_component_register(
  SRCS
    "mpu6050_v1.cpp"
    ${SOURCES}
  INCLUDE_DIRS "include")

idf_build_set_property(COMPILE_OPTIONS "-O2" APPEND)
