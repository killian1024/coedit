#include "core/basic_file_editor.hpp"
#include "tui/curses_interface.hpp"


int main(int argc, char* argv[])
{
    coedit::core::file_editor file_editr(coedit::core::newline_format::UNIX);
    coedit::tui::curses_interface curses_interf(&file_editr, 60);
    
    //coedit::core::file_editor::char_type data = 65;
    //file_editr.insert_character(data++);
    //file_editr.insert_character(data++);
    //file_editr.handle_command(coedit::core::file_editor_command::NEWLINE);
    //file_editr.insert_character(data++);
    //file_editr.insert_character(data++);
    //file_editr.handle_command(coedit::core::file_editor_command::NEWLINE);
    //file_editr.handle_command(coedit::core::file_editor_command::NEWLINE);
    //file_editr.insert_character(data++);
    //file_editr.handle_command(coedit::core::file_editor_command::NEWLINE);
    //file_editr.insert_character(data++);
    //file_editr.insert_character(data++);
    //file_editr.handle_command(coedit::core::file_editor_command::GO_LEFT);
    //file_editr.handle_command(coedit::core::file_editor_command::NEWLINE);
    //file_editr.insert_character(data++);
    //file_editr.handle_command(coedit::core::file_editor_command::GO_RIGHT);
    //file_editr.handle_command(coedit::core::file_editor_command::NEWLINE);
    //file_editr.handle_command(coedit::core::file_editor_command::GO_UP);
    //file_editr.handle_command(coedit::core::file_editor_command::GO_UP);
    //file_editr.insert_character(data++);
    //file_editr.handle_command(coedit::core::file_editor_command::GO_RIGHT);
    //file_editr.handle_command(coedit::core::file_editor_command::BACKSPACE);
    //
    //for (auto& line : file_editr)
    //{
    //    for (auto& ch : line)
    //    {
    //        std::cout << (char)ch;
    //    }
    //
    //    std::cout << kios::newl;
    //}
    
    return curses_interf.execute();
}