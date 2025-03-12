file(GLOB SOURCES src/*.cpp)
set(COMPONENTS main)
idf_component_register(
  SRCS
    "battery_fuel_gauge.cpp"
    ${SOURCES}
  INCLUDE_DIRS "include")
