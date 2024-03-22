idf_component_register(SRCS "main.c"
                       INCLUDE_DIRS "")

target_include_directories(${COMPONENT_LIB} PUBLIC "/mnt/nfs/esp32/esp32/include")
target_link_directories(${COMPONENT_LIB} PUBLIC /mnt/nfs/esp32/esp32/lib/)
target_link_libraries(${COMPONENT_LIB} PRIVATE hy_os hy_log hy_os_type)
