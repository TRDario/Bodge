-----------------------------------------------------------------------------------------------------------------------
|                                                                                                                     |
|        +++++++++++++++++          +######+     ++++++++++++++++       +++++++++++++++++++++++++++++++++++++         |
|        +#.............##++    ++##+++++++###++++#.............#++   ++##.............#+#.................#+         |
|        +#...............##+++##++++######++++#++#..............-#+++##...............#+#.................#+         |
|        +#...#++++++++#....++#++++##++++++##+++##+..-#+++++++#-...#+#....##+++++++++++++#...#+++++++++++++++         |
|        +#---#+      ++#--++#+++#+++++++++++##+++#+--#+      +#---#+#---#++            +#---#+                       |
|        +#---#+++++++##-+++#+++#++++++++++++++#++##++#+      +#---#+#---#+  ++++++++++++#---#++++++++++++            |
|        +#-------------++#+#++#+++++++++++++++#+++#++#+      +#---#+#---#+  +#--------#+#--------------#+            |
|        +#-----------+++#+#+++#+++++++++++++++#+++#++#+      +#---#+#---#+  +#--------#+#--------------#+            |
|        +#---#+++++++#+++++#++##++++++++++++++#+++#++#+      +#+--#+#---#+  ++++++#---#+#---#++++++++++++            |
|        +#+++#+   +++++#+++#+++#+++++++++++++#+++#+++#+      +#+++#+#+++#+       +#+++#+#+++#+                       |
|        +#+++#+ +++++++#++++#++++#+++++++++##+++#++++#+     ++#+++#+#+++##++     +#+++#+#+++#+                       |
|        +#+++##########+++##+##+++#########++++#+#+++#########++++#+##++++#########+++#+#+++###############+         |
|        +#++++++++++++++##+++++##++++++++++++#+++#++++++++++++++#++ ++##++++++++++++++#+#+++++++++++++++++#+         |
|        +###############++++++++++#########++++++###############++    ++###############+###################+         |
|         ++++++++++++++++++++++++++++++++++++   +++++++++++++++         +++++++++++++++++++++++++++++++++++          |
|            +++++++++++++++++++++++++++++++                                                                          |
|            ++++++++++++++++++++++++++++++                                                           v1.3.0          |
|            ++++++++++++++++++++++++++++                                                                             |
|             ++++++++++++++++++++++++++                                                                              |
|             ++++++++++++++++++++++++                                                                                |
|               +++++++++++++++++++++                                                                                 |
|                ++++++++++++++++++                                                                                   |
|                   ++++++++++++                                                                                      |
|                                                                                                                     |
|------------------------------------------------------FOREWORD-------------------------------------------------------|
|                                                                                                                     |
|                                           Thank you for downloading Bodge.                                          |
|                                                                                                                     |
|          This is my first completed game project, and though it's not really anything to write home about,          |
|                     I'm still happy to have finally broken the curse of not finishing anything.                     |
| I did sort of make it more difficult than necessary, since I insisted on writing it all in C++ and OpenGL directly. |
|                                                (please don't do this)                                               |
|                                         (unless you really want to, I guess)                                        |
|                                                                                                                     |
|                   The game is inspired by a flash game I played a long time ago named "Particles",                  |
|                 though it's not like the concept is particularly earthshatteringly original anyway.                 |
|                         So, shoutout to Matteo Guarnieri of Rag Doll Software for that one.                         |
|                                                                                                                     |
| I'd also like to thank my playtesters, toweli and starsurge, for helping me figure out bugs, both mine and Intel's. |
|                                      (why is their OpenGL driver so buggy man)                                      |
|                                                                                                                     |
|                                      I hope you enjoy your time dodging balls.                                      |
|          Feel free to contact me at therealdario123@gmail.com, or @trdario on Discord if you find any bugs.         |
|                                       (or just want to say hi, I don't bite)                                        |
|                                                                                                                     |
|                                                                                                           - TRDario |
|-----------------------------------------------------PATCH NOTES-----------------------------------------------------|
|                                                                                                                     |
| v1.3.0 (?? Feb 2026)                                                                                                |
| * Made it easier to earn lives: after failing to collect fragments, only the ones missed will spawn next time.      |
| * Adjusted the opacity of the lives, timer and score and made them fade out if a life fragment is behind them.      |
| * Adjusted ball collisions to make small balls less prone to supersonic speeds after hitting much larger balls.     |
| * Made a few quality-of-life adjustments to the gamemode designer.                                                  |
| * Fixed balls occasionally reflecting in mid-air due to floating-point imprecision.                                 |
| * Fixed balls rendering incorrectly when pausing and exiting a game while the screen is shaking after getting hit.  |
| * Fixed some settings not applying immediately.                                                                     |
| v1.2.1 (01 Feb 2026)                                                                                                |
| * Fixed the name entry screen not appearing at first startup.                                                       |
| * Fixed menu music not playing after exiting a game in some cases.                                                  |
| * Fixed missing sound when pressing enter for some input widgets.                                                   |
| * Fixed a crash when trying to use a custom user directory that doesn't exist.                                      |
| * Fixed missing Wayland support in Linux builds.                                                                    |
| v1.2.0 (07 Dec 2025)                                                                                                |
| * Rewrote state changes to be multithreaded, reducing lag between screen transitions.                               |
| * Introduced a new circle renderer, improving quality at lower resolutions and improving big ball performance.      |
| * Added mouse sensitivity to settings.                                                                              |
| * Tried to make frame times more stable when playing on higher framerates on Windows.                               |
| * Dropped support for v1.0 scorefiles. If you still have one, launch it with Bodge v1.1 to convert it first.        |
| * Clarified the effect of Shift and Ctrl on the various settings and gamemode parameters.                           |
| * Fixed the system cursor getting stuck in the middle of the screen on Windows.                                     |
| * Fixed hovering over the score text not hiding it.                                                                 |
| * Fixed the score being able to go negative.                                                                        |
| * Fixed a localization problem in the pause screens.                                                                |
| v1.1.1 (06 Sep 2025)                                                                                                |
| * Added vertical synchronization to settings.                                                                       |
| * Made the credits appear a bit faster.                                                                             |
| * Fixed scoreboards not switching pages correctly.                                                                  |
| * Fixed a graphical bug in the gamemode designer.                                                                   |
| v1.1.0 (05 Sep 2025)                                                                                                |
| * Introduced new systems for gaining lives and scoring.                                                             |
| * Updated most file formats (barring the settings), making v1.0 files unusable (besides the scorefile, partially).  |
| * Scores now automatically save (using the save feature is still useful for adding a description).                  |
| * Rebalanced the difficulties to be reasonable for a longer period of time.                                         |
| * Tweaked and improved some visual elements.                                                                        |
| * Miscellaneous bug fixes.                                                                                          |
| v1.0.1 (24 Aug 2025)                                                                                                |
| * Fixed a few gamemode designer limits and numeric input widget interactions.                                       |
| v1.0.0 (23 Aug 2025)                                                                                                |
| * Initial release.                                                                                                  |
|                                                                                                                     |
|-----------------------------------------------------HOW TO PLAY-----------------------------------------------------|
|                                                                                                                     |
| Survival:                                                                                                           |
| * Move the player character with your mouse.                                                                        |
| * Avoid the balls as long as possible.                                                                              |
| * Collect fragments to earn extra lives.                                                                            |
|                                                                                                                     |
| Scoring:                                                                                                            |
| * Survival gives you tick points every second. The value of ticks can be affected by hovering over certain regions  |
|   for a few seconds.                                                                                                |
|   * Being centered multiplies the value of ticks by 2.5x.                                                           |
|   * Hugging an edge multiplies the value of ticks by 0.5x.                                                          |
|   * Hugging a corner is penalized by giving you negative ticks.                                                     |
| * Standing in the path of oncoming balls gives you style points.                                                    |
|   * The cooldown for style points is very short, so surfing right in front of a ball can give you a lot of score.   |
|   * The value of style points depends somewhat on the size and particularly on the velocity of a ball.              |
|     * The exact formula is '√(size / 10) * (velocity / 250)^1.5'.                                                   |
| * Collecting life fragments and getting lives gives you some points too.                                            |
|                                                                                                                     |
|--------------------------------------------------KEYBOARD CONTROLS--------------------------------------------------|
|                                                                                                                     |
| Arrow keys can be used in menus to select buttons, which can then be pressed with enter.                            |
| You can also usually use the top row numbers and escape/enter as shortcuts, as well as other screen-specific ones.  |
|                                                                                                                     |
| Numeric settings can be directly inputted by pressing on them, or you can use shift/ctrl to change the rate that    |
| the arrows modify them.                                                                                             |
|                                                                                                                     |
| Replays can be sped up with ctrl and slowed down with shift.                                                        |
|                                                                                                                     |
|---------------------------------------------------USER DIRECTORY----------------------------------------------------|
|                                                                                                                     |
| Your saved data, as well as any custom gamemodes, languages, and music tracks are stored in the user directory.     |
| By default, this is in %appdata% on Windows, or /home/.local/share/ on Linux.                                       |
|                                                                                                                     |
| If you wish to redirect it, you can do that by running the game with '--userdir <path to directory>' passed to the  |
| command line.                                                                                                       |
|                                                                                                                     |
|-------------------------------------------------CUSTOM LOCALIZATION-------------------------------------------------|
|                                                                                                                     |
| Custom localization files may be created and used by placing them in the 'localization' subdirectory of the user    |
| directory.                                                                                                          |
| The file must be a .txt file named with a two-letter code (e.g. 'en.txt') in order to be recognised.                |
|                                                                                                                     |
| Localization file format:                                                                                           |
| * The file consists of a list of key-value pairs written as such:                                                   |
|    * key = "value"                                                                                                  |
| * Only one key-value pair may be defined per line.                                                                  |
| * The value may not span multiple lines, newlines in a value may be written with '\n'.                              |
| * Duplicate key definitions are ignored.                                                                            |
| * Empty lines and extraneous whitespace are ignored.                                                                |
| * Comments (lines starting with '#') are ignored.                                                                   |
|                                                                                                                     |
| In case the language is not covered by the default font, a custom font may be defined by setting the 'font' key to  |
| the name of your font file, which must then be placed in the 'fonts' subdirectory within the user directory.        |
|                                                                                                                     |
| The default english localization defines all key-value pairs except 'font', so it may be used as a template for     |
| what you should define. Any keys left undefined will be displayed without localization.                             |
|                                                                                                                     |
|----------------------------------------------------CUSTOM MUSIC-----------------------------------------------------|
|                                                                                                                     |
| Custom music tracks may be used in custom gamemodes by placing them in the 'music' subdirectory of the user         |
| directory.                                                                                                          |
| Only files in the Ogg Vorbis format (.ogg) are supported.                                                           |
|                                                                                                                     |
| By default, tracks are not looped. Looping is enabled and controlled through comments in the music file:            |
| * LOOPBEGIN=[offset in samples] sets the beginning loop point.                                                      |
|   * If not defined, it is assumed to be at the start of the track.                                                  |
| * LOOPEND=[offset in samples] sets the end loop point.                                                              |
|   * If not defined, it is assumed to be at the end of the track.                                                    |
| * Setting either LOOPBEGIN or LOOPEND is enough to automatically enable looping, but in the case where both points  |
|   should be defaulted, LOOP=true can instead be set to enable looping.                                              |
|                                                                                                                     |
| These comments can be set when exporting in software like Audacity.                                                 |
|                                                                                                                     |
|---------------------------------------------------------------------------------------------------------------------|
|                             © 2025-2026 TRDario, see copyright.txt for more information.                            |
-----------------------------------------------------------------------------------------------------------------------