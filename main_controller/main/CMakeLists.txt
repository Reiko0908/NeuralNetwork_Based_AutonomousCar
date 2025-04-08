file(GLOB SOURCES src/*.cpp)
idf_component_register(
  SRCS
    "main_controller.cpp"
    ${SOURCES}
  INCLUDE_DIRS "include")
