cmake_minimum_required(VERSION 3.30.0)
project(Bodge VERSION 0.1.0 LANGUAGES C CXX)

find_package(tr REQUIRED COMPONENTS graphics system audio)

add_executable(
    Bodge
    src/main.cpp
    src/audio.cpp src/blur_renderer.cpp src/engine.cpp src/font_manager.cpp src/tooltip.cpp
    src/game/ball.cpp src/game/game.cpp src/game/player.cpp src/game/trail.cpp
    src/state/game_state.cpp src/state/pause_state.cpp src/state/replay_state.cpp src/state/replays_state.cpp src/state/settings_state.cpp src/state/start_game_state.cpp src/state/title_state.cpp
    src/ui/interpolated.cpp src/ui/ui_manager.cpp src/ui/widget.cpp
    src/gamemode.cpp src/global.cpp src/localization.cpp src/replay.cpp src/score.cpp src/settings.cpp
)
target_compile_features(Bodge PUBLIC cxx_std_23)
target_link_libraries(Bodge tr::system tr::graphics tr::audio)
target_precompile_headers (Bodge PRIVATE <tr/utility.hpp>)