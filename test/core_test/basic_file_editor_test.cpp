//
// Created by Killian on 25/02/18.
//

#include <gtest/gtest.h>
#include <kboost/kboost.hpp>

#include "core/basic_file_editor.hpp"

namespace cc = coedit::core;


TEST(basic_file_editor, insert_character)
{
    cc::file_editor file_editr("", cc::newline_format::UNIX);
    cc::file_editor::char_type data = 65;
    
    file_editr.insert_character(data++);
    file_editr.insert_character(data++);
}


TEST(basic_file_editor, handle_command)
{
    cc::file_editor file_editr("", cc::newline_format::UNIX);
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


TEST(basic_file_editor, handle_newline_1)
{
    cc::file_editor file_editr("", cc::newline_format::UNIX);
    cc::file_editor::char_type data = 65;
    
    file_editr.set_terminal_size(4, 80);
    
    file_editr.insert_character(data++);
    file_editr.insert_character(data++);
    file_editr.insert_character(data++);
    file_editr.insert_character(data++);
    file_editr.insert_character(data++);
    file_editr.insert_character(data++);
    file_editr.insert_character(data++);
    file_editr.insert_character(data++);
    file_editr.insert_character(data++);
    file_editr.insert_character(data++);
    file_editr.insert_character(data++);
    file_editr.insert_character(data++);
    file_editr.insert_character(data++);
    EXPECT_NO_THROW(file_editr.handle_command(cc::file_editor_command::GO_LEFT));
    EXPECT_NO_THROW(file_editr.handle_command(cc::file_editor_command::NEWLINE));
    file_editr.insert_character(data++);
    file_editr.insert_character(data++);
    file_editr.insert_character(data++);
    file_editr.insert_character(data++);
    EXPECT_NO_THROW(file_editr.handle_command(cc::file_editor_command::GO_UP));
    EXPECT_NO_THROW(file_editr.handle_command(cc::file_editor_command::HOME));
    EXPECT_NO_THROW(file_editr.handle_command(cc::file_editor_command::GO_RIGHT));
    EXPECT_NO_THROW(file_editr.handle_command(cc::file_editor_command::NEWLINE));
    file_editr.insert_character(data++);
    file_editr.insert_character(data++);
    
    std::size_t n_lnes = 0;
    for (auto lne = file_editr.begin_terminal();
         lne != file_editr.end_terminal();
         ++lne)
    {
        ++n_lnes;
        for (auto ch = lne->begin_terminal();
             ch != lne->end_terminal();
             ++ch)
        {
            *ch;
        }
    }
    
    EXPECT_TRUE(n_lnes == 3);
}


TEST(basic_file_editor, handle_newline_2)
{
    cc::file_editor file_editr("", cc::newline_format::UNIX);
    cc::file_editor::char_type data = 65;
    
    file_editr.set_terminal_size(4, 80);
    
    file_editr.insert_character(data++);
    file_editr.insert_character(data++);
    file_editr.insert_character(data++);
    file_editr.insert_character(data++);
    EXPECT_NO_THROW(file_editr.handle_command(cc::file_editor_command::GO_LEFT));
    EXPECT_NO_THROW(file_editr.handle_command(cc::file_editor_command::GO_LEFT));
    EXPECT_NO_THROW(file_editr.handle_command(cc::file_editor_command::NEWLINE));
    
    std::size_t n_cols = 0;
    bool found = false;
    
    for (auto lne = file_editr.begin_lazy_terminal();
         lne != file_editr.end_lazy_terminal();
         ++lne)
    {
        for (auto ch = lne->begin_lazy_terminal();
             ch != lne->end_lazy_terminal();
             ++ch)
        {
            ++n_cols;
            
            if (n_cols == 4)
            {
                found = true;
            }
        }
    }
    
    EXPECT_TRUE(found);
}


TEST(basic_file_editor, handle_end)
{
    cc::file_editor file_editr("", cc::newline_format::UNIX);
    cc::file_editor::char_type data = 65;
    
    file_editr.insert_character(data++);
    file_editr.insert_character(data++);
    file_editr.insert_character(data++);
    file_editr.insert_character(data++);
    EXPECT_NO_THROW(file_editr.handle_command(cc::file_editor_command::END));
}


TEST(basic_file_editor, handle_load_file)
{
    cc::file_editor file_editr("./file.txt", cc::newline_format::UNIX);
    file_editr.set_terminal_size(80, 80);
    
    for (auto it_lne = file_editr.begin_lazy_terminal();
         it_lne != file_editr.end_lazy_terminal();
         ++it_lne)
    {
        for (auto it_ch = it_lne->begin_lazy_terminal();
             it_ch != it_lne->end_lazy_terminal();
             ++it_ch)
        {
            *it_ch;
        }
    }
}


TEST(basic_file_editor, iterator)
{
    using file_editor = cc::file_editor;
    
    file_editor file_editr("", cc::newline_format::UNIX);
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
