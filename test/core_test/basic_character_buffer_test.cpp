//
// Created by Killian on 26/02/18.
//

#include <gtest/gtest.h>
#include <kboost/kboost.hpp>

#include "core/basic_file_editor.hpp"

namespace cc = coedit::core;


TEST(basic_characters_buffer, insert_character)
{
    using chatacter_buffer_cache = cc::basic_character_buffer_cache<char, 4, 8, 4, 8>;
    using chatacter_buffer = cc::basic_character_buffer<char, 4, 8, 4, 8>;
    
    chatacter_buffer_cache cb_cache(0);
    cc::cboffset_t cboffst = 0;
    chatacter_buffer::char_type ch = 65;
    
    cb_cache.insert(0, chatacter_buffer(0, cc::EMPTY, cc::EMPTY, &cb_cache));
    auto& cb = cb_cache.get_character_buffer_and_lock(0);
    
    cb.insert_character(ch++, cboffst++);
    cb.insert_character(ch++, cboffst++);
    cb.insert_character(ch++, cboffst++);
    cb.insert_character(ch++, cboffst++);
    cb.insert_character(ch++, cboffst++);
    cb.insert_character('\n', cboffst++);
    cb.insert_character('\n', cboffst++);
    cb.insert_character(ch++, cboffst++);
    cb.insert_character(ch++, cboffst++);
    cb.insert_character(ch++, cboffst++);
    cb.insert_character(ch++, cboffst++);
    cb.insert_character(ch++, cboffst++);
    
    ASSERT_TRUE(cb[6] == '\n');
    ASSERT_TRUE(cb[11] == ch - 1);
}


TEST(basic_characters_buffer, erase_character)
{
    using chatacter_buffer_cache = cc::basic_character_buffer_cache<char, 4, 8, 4, 8>;
    using chatacter_buffer = cc::basic_character_buffer<char, 4, 8, 4, 8>;
    
    chatacter_buffer_cache cb_cache(0);
    cc::cboffset_t cboffst = 0;
    chatacter_buffer::char_type ch = 65;
    
    cb_cache.insert(0, chatacter_buffer(0, cc::EMPTY, cc::EMPTY, &cb_cache));
    auto& cb = cb_cache.get_character_buffer_and_lock(0);
    
    cb.insert_character(ch++, cboffst++);
    cb.insert_character(ch++, cboffst++);
    cb.insert_character(ch++, cboffst++);
    cb.insert_character(ch++, cboffst++);
    cb.insert_character(ch++, cboffst++);
    cb.insert_character('\n', cboffst++);
    cb.insert_character('\n', cboffst++);
    cb.insert_character(ch++, cboffst++);
    cb.insert_character(ch++, cboffst++);
    cb.insert_character(ch++, cboffst++);
    cb.insert_character(ch++, cboffst++);
    cb.insert_character(ch++, cboffst++);
    
    cb.erase_character(4);
    cb.erase_character(9);
    
    ASSERT_TRUE(cb[4] == '\n');
    ASSERT_TRUE(cb[9] == ch - 1);
}


TEST(basic_characters_buffer, get_line_length)
{
    using chatacter_buffer_cache = cc::basic_character_buffer_cache<char, 4, 8, 4, 8>;
    using chatacter_buffer = cc::basic_character_buffer<char, 4, 8, 4, 8>;
    
    chatacter_buffer_cache cb_cache(0);
    cc::cboffset_t cboffst = 0;
    chatacter_buffer::char_type ch = 65;
    
    cb_cache.insert(0, chatacter_buffer(0, cc::EMPTY, cc::EMPTY, &cb_cache));
    auto& cb = cb_cache.get_character_buffer_and_lock(0);
    
    cb.insert_character(ch++, cboffst++);
    cb.insert_character(ch++, cboffst++);
    cb.insert_character(ch++, cboffst++);
    cb.insert_character(ch++, cboffst++);
    cb.insert_character(ch++, cboffst++);
    cb.insert_character('\n', cboffst++);
    cb.insert_character('\n', cboffst++);
    cb.insert_character(ch++, cboffst++);
    cb.insert_character(ch++, cboffst++);
    cb.insert_character(ch++, cboffst++);
    cb.insert_character(ch++, cboffst++);
    cb.insert_character(ch++, cboffst++);
    
    ASSERT_TRUE(cb.get_line_length(0) == 6);
    ASSERT_TRUE(cb.get_line_length(6) == 1);
    ASSERT_TRUE(cb.get_line_length(7) == 5);
}


TEST(basic_characters_buffer, operator_subscript)
{
    using chatacter_buffer_cache = cc::basic_character_buffer_cache<char, 4, 8, 4, 8>;
    using chatacter_buffer = cc::basic_character_buffer<char, 4, 8, 4, 8>;
    
    cc::cboffset_t cboffst;
    chatacter_buffer::char_type ch;
    chatacter_buffer_cache cb_cache(0);
    std::size_t i;
    
    cb_cache.insert(0, chatacter_buffer(0, cc::EMPTY, cc::EMPTY, &cb_cache));
    auto& cb = cb_cache.get_character_buffer_and_lock(0);
    
    for (ch = 33, cboffst = 0;
         ch < 127;
         ++ch, ++cboffst)
    {
        cb.insert_character(ch, cboffst);
    }
    
    for (i = 0, ch = 33;
         ch < 127;
         ++i, ++ch)
    {
        ASSERT_TRUE(cb[i] == ch);
    }
    
    cb_cache.unlock_character_buffer(0);
}
