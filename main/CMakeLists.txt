set(
    SOURCES     
        esp_board_driver.c
        tcp_client.c
        main.c

        #main_vad_vol.c
)

idf_component_register(
    SRCS ${SOURCES}
    INCLUDE_DIRS include
)
