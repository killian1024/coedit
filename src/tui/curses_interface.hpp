//
// Created by Killian on 19/01/18.
//

#ifndef COEDIT_TUI_CURSES_INTERFACE_HPP
#define COEDIT_TUI_CURSES_INTERFACE_HPP

#include <curses.h>

#include "../core/basic_file_editor.hpp"
#include "curses_colors.hpp"


#ifndef CTRL
#define CTRL(c) ((c) & 037)
#endif


namespace coedit {
namespace tui {


class curses_interface
{
public:
    using file_editor_type = core::file_editor;
    
    using file_editor_command_type = core::file_editor_command;
    
    using cursor_position_type = core::cursor_position;
    
    curses_interface(file_editor_type* file_editr);

    void init();
    
    void free();
    
    file_editor_command_type get_command();
    
    void print();

private:
    void print_line_number(typename file_editor_type::lazy_terminal_iterator& it_lne);
    
    void update_cursor();
    
private:
    file_editor_type* file_editr_;
    
    std::size_t left_margin_;
    
    bool colors_enabled_;
    
    WINDOW* win_;
};


}
}


#endif
