idf_component_register(SRCS "partition_api.cpp"
                    INCLUDE_DIRS ".")

idf_build_set_property(COMPILE_OPTIONS "-O2" APPEND)
