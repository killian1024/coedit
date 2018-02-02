#include "core/basic_file_editor.hpp"
#include "tui/curses_interface.hpp"


int main(int argc, char* argv[])
{
    coedit::core::file_editor file_editr;
    coedit::tui::curses_interface curses_interf(&file_editr, 60);
    
    int data = 65;
    file_editr.handle_command(coedit::core::file_editor_command::INSERT, (data++, &data));
    file_editr.handle_command(coedit::core::file_editor_command::INSERT, (data++, &data));
    file_editr.handle_command(coedit::core::file_editor_command::INSERT, (data++, &data));
    
    return curses_interf.execute();
}