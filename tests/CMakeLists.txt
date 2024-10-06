find_package(GTest REQUIRED)

add_executable(audio_backend_tests
    test_jack_backend.cpp
    test_pipewire_backend.cpp
)

target_link_libraries(audio_backend_tests
    audio_backend
    GTest::GTest
    GTest::Main
)

add_test(NAME audio_backend_tests COMMAND audio_backend_tests)