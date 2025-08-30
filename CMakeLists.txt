cmake_minimum_required(VERSION 3.30.0)
project(Bodge VERSION 1.1.0 LANGUAGES C CXX)

include(FetchContent)

FetchContent_Declare(
	tr
	GIT_REPOSITORY	https://TRDario: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@github.com/TRDario/tr.git
    GIT_TAG origin/main
) 
FetchContent_MakeAvailable(tr)
include(${tr_SOURCE_DIR}/check_compiler.cmake)

add_executable(
    Bodge
    src/main.cpp
    src/audio.cpp src/blur_renderer.cpp src/fonts.cpp src/graphics.cpp src/system.cpp src/tooltip_manager.cpp
    src/game/ball.cpp src/game/game.cpp src/game/life_fragment.cpp src/game/player.cpp src/game/trail.cpp
    src/state/ball_settings_editor_state.cpp src/state/credits_state.cpp src/state/game_over_state.cpp src/state/game_state.cpp
        src/state/gamemode_designer_state.cpp src/state/name_entry_state.cpp src/state/pause_state.cpp src/state/player_settings_editor_state.cpp
        src/state/replays_state.cpp src/state/save_score_state.cpp src/state/save_replay_state.cpp src/state/scoreboards_state.cpp src/state/settings_state.cpp
        src/state/state_base.cpp src/state/start_game_state.cpp src/state/title_state.cpp
    src/ui/arrow_widget.cpp src/ui/color_preview_widget.cpp src/ui/image_widget.cpp src/ui/label_widget.cpp src/ui/replay_playback_indicator_widget.cpp
        src/ui/replay_score_widget.cpp src/ui/text_button_widget.cpp src/ui/ui_manager.cpp src/ui/widget_base.cpp
    src/gamemode.cpp src/legacy_formats.cpp src/global.cpp src/localization.cpp src/replay.cpp src/score.cpp src/settings.cpp
)
tr_target_template(Bodge)
target_link_libraries(Bodge tr::tr)
target_precompile_headers(Bodge PRIVATE <tr/utility.hpp>)

# Post-build steps.

if(WIN32)
    target_sources(Bodge PRIVATE metadata/icon.rc)
elseif(TR_HAS_CLANG)
    target_link_options(Bodge PRIVATE $<IF:$<CONFIG:Release>,-s,>)
endif()

# Copying to 'final' if Release.

add_custom_command(TARGET Bodge POST_BUILD COMMAND $<IF:$<CONFIG:Release>,${CMAKE_COMMAND},true>
                   -E copy "$<TARGET_FILE:Bodge>" "${CMAKE_CURRENT_BINARY_DIR}/final/$<TARGET_FILE_NAME:Bodge>")
add_custom_command(TARGET Bodge POST_BUILD COMMAND $<IF:$<CONFIG:Release>,${CMAKE_COMMAND},true>
                   -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/data" "${CMAKE_CURRENT_BINARY_DIR}/final/data")
add_custom_command(TARGET Bodge POST_BUILD COMMAND $<IF:$<CONFIG:Release>,${CMAKE_COMMAND},true>
                   -E copy "${CMAKE_CURRENT_SOURCE_DIR}/metadata/readme.txt" "${CMAKE_CURRENT_BINARY_DIR}/final/readme.txt")