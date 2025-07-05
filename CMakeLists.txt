cmake_minimum_required(VERSION 3.30.0)
project(Bodge VERSION 0.1.0 LANGUAGES C CXX)

include (FetchContent)

FetchContent_Declare(
	tr-sysgfx
	GIT_REPOSITORY	https://TRDario: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@github.com/TRDario/tr-sysgfx.git
	GIT_TAG			0144ed1446d763767fa72e6fd30c52bbb7e5dca0
)
FetchContent_MakeAvailable(tr-sysgfx)

FetchContent_Declare(
	tr-audio
	GIT_REPOSITORY	https://TRDario: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@github.com/TRDario/tr-audio.git
	GIT_TAG			d43057eaae61fd0df95da094d0c25a590cd35866
)
FetchContent_MakeAvailable(tr-audio)

add_executable(
    Bodge
    src/main.cpp
    src/audio.cpp src/blur_renderer.cpp src/engine.cpp src/font_manager.cpp src/tooltip.cpp
    src/game/ball.cpp src/game/game.cpp src/game/player.cpp src/game/trail.cpp
    src/state/ball_settings_editor_state.cpp src/state/game_over_state.cpp src/state/game_state.cpp src/state/gamemode_designer_state.cpp
        src/state/name_entry_state.cpp src/state/pause_state.cpp src/state/player_settings_editor_state.cpp src/state/replays_state.cpp
        src/state/save_score_state.cpp src/state/save_replay_state.cpp src/state/scoreboards_state.cpp src/state/settings_state.cpp
        src/state/start_game_state.cpp src/state/title_state.cpp
    src/ui/interpolated.cpp src/ui/ui_manager.cpp src/ui/widget.cpp
    src/gamemode.cpp src/global.cpp src/localization.cpp src/replay.cpp src/score.cpp src/settings.cpp
)
target_compile_features(Bodge PUBLIC cxx_std_20)
if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
	target_compile_options (Bodge PRIVATE -Wall -Wextra -Wpedantic)
	target_compile_options (Bodge PRIVATE -fno-trapping-math -fno-math-errno -fno-signed-zeros -fassociative-math -ffp-contract=fast)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
	target_compile_options(Bodge PUBLIC /Zc:preprocessor /W4 /WX)
endif ()
target_link_libraries(Bodge tr::sysgfx tr::audio)
target_precompile_headers (Bodge PRIVATE <tr/utility.hpp>)