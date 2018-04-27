//
// Created by Killian on 27/04/18.
//

#ifndef COEDIT_TUI_CURSES_COLORS_HPP
#define COEDIT_TUI_CURSES_COLORS_HPP

#define COLOR_LIGHT_BLUE 9
#define COLOR_LIGHT_GREEN 10
#define COLOR_LIGHT_CYAN 11
#define COLOR_LIGHT_RED 12
#define COLOR_LIGHT_PINK 13
#define COLOR_LIGHT_YELLOW 14
#define COLOR_LIGHT_WHITE 15


namespace coedit {
namespace tui {


enum class curses_color
{
    DEFAULT = 1,
    LINE_NUMBER = 2,
};


}
}


#endif
