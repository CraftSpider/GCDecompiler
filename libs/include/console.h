#pragma once

#include <string>
#include <sstream>
#include "types.h"

namespace term {

std::string SET_ICON_TITLE(std::string name);
std::string SET_TITLE(std::string name);
std::string SCROLL_UP(ushort n = 1);
std::string SCROLL_DOWN(ushort n = 1);

constexpr char ALT_BUFFER[] = "\033[?1049h";
constexpr char MAIN_BUFFER[] = "\033[?1049l";

namespace curs {

std::string UP(ushort n = 1);
std::string DOWN(ushort n = 1);
std::string FORWARD(ushort n = 1);
std::string BACKWARD(ushort n = 1);
std::string DOWN_LINE(ushort n = 1);
std::string UP_LINE(ushort n = 1);
std::string SET_HORIZONTAL(ushort n = 1);
std::string SET_VERTICAL(ushort n = 1);
std::string SET_POS(ushort x = 1, ushort y = 1);

constexpr char SAVE[] = "\033[s";
constexpr char RESTORE[] = "\033[u";
constexpr char ENABLE_BLINK[] = "\033[?12h";
constexpr char DISABLE_BLINK[] = "\033[?12l";
constexpr char SHOW[] = "\033[?25h";
constexpr char HIDE[] = "\033[?25l";

}

namespace text {

std::string INSERT(ushort n = 1);
std::string DELETE(ushort n = 1);
std::string ERASE(ushort n = 1);
std::string INSERT_LINE(ushort n = 1);
std::string DELETE_LINE(ushort n = 1);

/**
 * Clear the screen. Option is one of three values:
 * 0: erases from beginning to cursor
 * 1: erases from cursor to end
 * 2: erases from beginning to end
 * 3: erases from beginning to end and clears the scrollback buffer
 * @param option Clear setting
 * @return Escape code
 */
std::string CLEAR(ushort option = 2);
std::string CLEAR_LINE(ushort option = 2);

}

namespace effect {

constexpr char BOLD[] = "\033[1m";
constexpr char DIM[] = "\033[2m";
constexpr char UNDERLINE[] = "\033[4m";
constexpr char BLINK[] = "\033[5m";
constexpr char INVERTED[] = "\033[7m";
constexpr char HIDDEN[] = "\033[8m";

constexpr char RESET[] = "\033[0m";
constexpr char RESET_BOLD[] = "\033[21m";
constexpr char RESET_DIM[] = "\033[22m";
constexpr char RESET_UNDERLINE[] = "\033[24m";
constexpr char RESET_BLINK[] = "\033[25m";
constexpr char RESET_INVERTED[] = "\033[27m";
constexpr char RESET_HIDDEN[] = "\033[28m";

}

namespace fore {

constexpr char DEFAULT[] = "\033[39m";
constexpr char BLACK[] = "\033[30m";
constexpr char RED[] = "\033[31m";
constexpr char GREEN[] = "\033[32m";
constexpr char YELLOW[] = "\033[33m";
constexpr char BLUE[] = "\033[34m";
constexpr char MAGENTA[] = "\033[35m";
constexpr char CYAN[] = "\033[36m";
constexpr char LIGHT_GRAY[] = "\033[37m";
constexpr char DARK_GRAY[] = "\033[90m";
constexpr char LIGHT_RED[] = "\033[91m";
constexpr char LIGHT_GREEN[] = "\033[92m";
constexpr char LIGHT_YELLOW[] = "\033[93m";
constexpr char LIGHT_BLUE[] = "\033[94m";
constexpr char LIGHT_MAGENTA[] = "\033[95m";
constexpr char LIGHT_CYAN[] = "\033[96m";
constexpr char WHITE[] = "\033[97m";

}

namespace back {

constexpr char DEFAULT[] = "\033[49m";
constexpr char BLACK[] = "\033[40m";
constexpr char RED[] = "\033[41m";
constexpr char GREEN[] = "\033[42m";
constexpr char YELLOW[] = "\033[43m";
constexpr char BLUE[] = "\033[44m";
constexpr char MAGENTA[] = "\033[45m";
constexpr char CYAN[] = "\033[46m";
constexpr char LIGHT_GRAY[] = "\033[47m";
constexpr char DARK_GRAY[] = "\033[100m";
constexpr char LIGHT_RED[] = "\033[101m";
constexpr char LIGHT_GREEN[] = "\033[102m";
constexpr char LIGHT_YELLOW[] = "\033[103m";
constexpr char LIGHT_BLUE[] = "\033[104m";
constexpr char LIGHT_MAGENTA[] = "\033[105m";
constexpr char LIGHT_CYAN[] = "\033[106m";
constexpr char WHITE[] = "\033[107m";

}

constexpr char RESET_ALL[] = "\033[0m\033[39m\033[49m";

}
