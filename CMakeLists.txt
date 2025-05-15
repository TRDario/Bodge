cmake_minimum_required(VERSION 3.30.0)
project(Bodge VERSION 0.1.0 LANGUAGES C CXX)

find_package(tr REQUIRED COMPONENTS graphics system audio)

add_executable(
    Bodge
    src/main.cpp
    src/audio.cpp src/blur_renderer.cpp src/engine.cpp src/font_manager.cpp
    src/gamemode.cpp src/global.cpp src/replay.cpp src/score.cpp src/settings.cpp
)
target_compile_features(Bodge PUBLIC cxx_std_23)
target_link_libraries(Bodge tr::system tr::graphics tr::audio)
target_precompile_headers (Bodge PRIVATE <tr/utility.hpp>)