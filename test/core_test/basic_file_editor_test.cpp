//
// Created by Killian on 25/02/18.
//

#include <gtest/gtest.h>
#include <kboost/kboost.hpp>

#include "core/basic_file_editor.hpp"

namespace cc = coedit::core;


TEST(basic_file_editor, insert_character)
{
    cc::file_editor file_editr(cc::newline_format::UNIX);
    cc::file_editor::char_type data = 65;
    
    file_editr.insert_character(data++);
    file_editr.insert_character(data++);
}


TEST(basic_file_editor, handle_command)
{
    cc::file_editor file_editr(cc::newline_format::UNIX);
    cc::file_editor::char_type data = 65;
    
    file_editr.insert_character(data++);
    file_editr.insert_character(data++);
    file_editr.insert_character(data++);
    file_editr.insert_character(data++);
    file_editr.insert_character(data++);
    file_editr.insert_character(data++);
    file_editr.insert_character(data++);
    file_editr.insert_character(data++);
    file_editr.insert_character(data++);
    EXPECT_NO_THROW(file_editr.handle_command(cc::file_editor_command::NEWLINE));
    file_editr.insert_character(data++);
    file_editr.insert_character('\n');
}


TEST(basic_file_editor, handle_newline)
{
    cc::file_editor file_editr(cc::newline_format::UNIX);
    cc::file_editor::char_type data = 65;
    
    file_editr.insert_character(data++);
    file_editr.insert_character(data++);
    file_editr.insert_character(data++);
    file_editr.insert_character(data++);
    EXPECT_NO_THROW(file_editr.handle_command(cc::file_editor_command::NEWLINE));
    file_editr.insert_character(data++);
    file_editr.insert_character(data++);
    file_editr.insert_character(data++);
    file_editr.insert_character(data++);
    EXPECT_NO_THROW(file_editr.handle_command(cc::file_editor_command::GO_LEFT));
    EXPECT_NO_THROW(file_editr.handle_command(cc::file_editor_command::GO_LEFT));
    EXPECT_NO_THROW(file_editr.handle_command(cc::file_editor_command::NEWLINE));
    file_editr.insert_character(data++);
    file_editr.insert_character(data++);
    
    for (auto& lne : file_editr)
    {
        for (auto& ch : lne)
        {
        }
    }
}


TEST(basic_file_editor, handle_end)
{
    cc::file_editor file_editr(cc::newline_format::UNIX);
    cc::file_editor::char_type data = 65;
    
    file_editr.insert_character(data++);
    file_editr.insert_character(data++);
    file_editr.insert_character(data++);
    file_editr.insert_character(data++);
    EXPECT_NO_THROW(file_editr.handle_command(cc::file_editor_command::END));
}


TEST(basic_file_editor, iterator)
{
    using file_editor = cc::file_editor;
    
    file_editor file_editr(cc::newline_format::UNIX);
    file_editor::char_type data = 48;
    
    file_editr.insert_character(data++);
    file_editr.insert_character(data++);
    file_editr.insert_character(data++);
    file_editr.insert_character(data++);
    file_editr.insert_character(data++);
    file_editr.insert_character(data++);
    file_editr.insert_character(data++);
    file_editr.insert_character(data++);
    file_editr.insert_character(data++);
    EXPECT_NO_THROW(file_editr.handle_command(cc::file_editor_command::NEWLINE));
    file_editr.insert_character(data++);
    file_editr.insert_character(data++);
    EXPECT_NO_THROW(file_editr.handle_command(cc::file_editor_command::NEWLINE));
    
    kios::ios_redirect iosr(std::cout);
    iosr.redirect_to_embedded_stringstream();
    
    for (auto& line : file_editr)
    {
        for (auto& ch : line)
        {
            std::cout << (char)ch;
        }
        
        std::cout << kios::newl;
    }
    
    iosr.unredirect();
}
