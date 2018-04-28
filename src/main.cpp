#include "core/basic_file_editor.hpp"
#include "tui/curses_interface.hpp"


int main(int argc, char* argv[])
{
    coedit::core::file_editor file_editr(coedit::core::newline_format::UNIX);
    coedit::tui::curses_interface curses_interf(&file_editr);
    
    return curses_interf.execute();
}
