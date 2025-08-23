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
|            ++++++++++++++++++++++++++++++                                                           v1.0.1          |
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
| v1.0.1 (24 Aug 2025)                                                                                                |
| * Fixed a few gamemode designer limits and numeric input widget interactions.                                       |
| v1.0.0 (23 Aug 2025)                                                                                                |
| * Initial release.                                                                                                  |
|                                                                                                                     |
|-----------------------------------------------------HOW TO PLAY-----------------------------------------------------|
|                                                                                                                     |
| 1.) Move the player character with your mouse.                                                                      |
| 2.) Avoid the balls as long as possible.                                                                            |
|                                                                                                                     |
|--------------------------------------------------KEYBOARD CONTROLS--------------------------------------------------|
|                                                                                                                     |
| Arrow keys can be used in menus to select buttons, which can then be pressed with enter.                            |
| You can also usually use the top row numbers and escape/enter as shortcuts, as well as other screen-specific ones.  |
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
|                                         © 2025 TRDario, all rights reserved                                         |
-----------------------------------------------------------------------------------------------------------------------