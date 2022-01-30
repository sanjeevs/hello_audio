if (WIN32)

ELSE()
    add_executable(lnx_microphone lnx_microphone.cpp)
    target_link_libraries(lnx_microphone asound)

    add_executable(lnx_speaker lnx_speaker.cpp)
    target_link_libraries(lnx_speaker asound)
ENDIF()