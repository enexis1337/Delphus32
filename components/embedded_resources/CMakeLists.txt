idf_component_register(
    SRCS "embedded_resources.c"
    INCLUDE_DIRS "."
    REQUIRES storage furi
    EMBED_TXTFILES
        "${PROJECT_DIR}/applications/main/infrared/resources/infrared/assets/tv.ir"
        "${PROJECT_DIR}/applications/main/infrared/resources/infrared/assets/ac.ir"
        "${PROJECT_DIR}/applications/main/infrared/resources/infrared/assets/audio.ir"
        "${PROJECT_DIR}/applications/main/infrared/resources/infrared/assets/fans.ir"
        "${PROJECT_DIR}/applications/main/infrared/resources/infrared/assets/leds.ir"
        "${PROJECT_DIR}/applications/main/infrared/resources/infrared/assets/projectors.ir"
)
