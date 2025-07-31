cmake_minimum_required(VERSION 3.30.0)
project(Bodge VERSION 0.3.0 LANGUAGES C CXX)

include (FetchContent)

FetchContent_Declare(
	tr
	GIT_REPOSITORY	https://TRDario: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@github.com/TRDario/tr.git
    GIT_TAG origin/main
) 
FetchContent_MakeAvailable(tr)

add_executable(
    Bodge
    src/main.cpp
    src/audio.cpp src/blur_renderer.cpp src/fonts.cpp src/graphics.cpp src/system.cpp src/tooltip_manager.cpp
    src/game/ball.cpp src/game/game.cpp src/game/player.cpp src/game/trail.cpp
    src/state/ball_settings_editor_state.cpp src/state/game_over_state.cpp src/state/game_state.cpp src/state/gamemode_designer_state.cpp
        src/state/name_entry_state.cpp src/state/pause_state.cpp src/state/player_settings_editor_state.cpp src/state/replays_state.cpp
        src/state/save_score_state.cpp src/state/save_replay_state.cpp src/state/scoreboards_state.cpp src/state/settings_state.cpp
        src/state/start_game_state.cpp src/state/title_state.cpp
    src/ui/interpolated.cpp src/ui/ui_manager.cpp src/ui/widget.cpp
    src/gamemode.cpp src/global.cpp src/localization.cpp src/replay.cpp src/score.cpp src/settings.cpp
)
target_compile_features(Bodge PUBLIC cxx_std_20)
if (TR_ENABLE_ASSERTS)
	target_compile_definitions(Bodge PRIVATE TR_ENABLE_ASSERTS)
endif ()
if ((CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND NOT (CMAKE_CXX_SIMULATE_ID STREQUAL "MSVC")) OR CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
	target_compile_options (Bodge PRIVATE -Wall -Wextra -Wpedantic -Wshadow)
	target_compile_options (Bodge PRIVATE -fno-trapping-math -fno-math-errno -fno-signed-zeros -fassociative-math -ffp-contract=fast)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND CMAKE_CXX_SIMULATE_ID STREQUAL "MSVC")
	target_compile_options (Bodge PRIVATE /clang:-Wall /clang:-Wextra /clang:-Wpedantic /clang:-Wshadow)
	target_compile_options (Bodge PRIVATE /clang:-fno-trapping-math /clang:-fno-math-errno /clang:-fno-signed-zeros /clang:-fassociative-math /clang:-ffp-contract=fast)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
	target_compile_options(Bodge PUBLIC /Zc:preprocessor /W3 /WX)
endif ()
target_link_libraries(Bodge tr::tr)
target_precompile_headers (Bodge PRIVATE <tr/utility.hpp>)