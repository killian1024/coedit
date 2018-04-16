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
    
    auto it_cb = cb_cache.insert_first_character_buffer();
    cb_cache.lock(it_cb);
    
    it_cb->insert_character(ch++, cboffst++);
    it_cb->insert_character(ch++, cboffst++);
    it_cb->insert_character(ch++, cboffst++);
    it_cb->insert_character(ch++, cboffst++);
    it_cb->insert_character(ch++, cboffst++);
    it_cb->insert_character('\n', cboffst++);
    it_cb->insert_character('\n', cboffst++);
    it_cb->insert_character(ch++, cboffst++);
    it_cb->insert_character(ch++, cboffst++);
    it_cb->insert_character(ch++, cboffst++);
    it_cb->insert_character(ch++, cboffst++);
    it_cb->insert_character(ch++, cboffst++);
    
    ASSERT_TRUE((*it_cb)[6] == '\n');
    ASSERT_TRUE((*it_cb)[11] == ch - 1);
}


TEST(basic_characters_buffer, erase_character)
{
    using chatacter_buffer_cache = cc::basic_character_buffer_cache<char, 4, 8, 4, 8>;
    using chatacter_buffer = cc::basic_character_buffer<char, 4, 8, 4, 8>;
    
    chatacter_buffer_cache cb_cache(0);
    cc::cboffset_t cboffst = 0;
    chatacter_buffer::char_type ch = 65;
    
    auto it_cb = cb_cache.insert_first_character_buffer();
    cb_cache.lock(it_cb);
    
    it_cb->insert_character(ch++, cboffst++);
    it_cb->insert_character(ch++, cboffst++);
    it_cb->insert_character(ch++, cboffst++);
    it_cb->insert_character(ch++, cboffst++);
    it_cb->insert_character(ch++, cboffst++);
    it_cb->insert_character('\n', cboffst++);
    it_cb->insert_character('\n', cboffst++);
    it_cb->insert_character(ch++, cboffst++);
    it_cb->insert_character(ch++, cboffst++);
    it_cb->insert_character(ch++, cboffst++);
    it_cb->insert_character(ch++, cboffst++);
    it_cb->insert_character(ch++, cboffst++);
    
    it_cb->erase_character(4);
    it_cb->erase_character(9);
    
    ASSERT_TRUE((*it_cb)[4] == '\n');
    ASSERT_TRUE((*it_cb)[9] == ch - 1);
}


TEST(basic_characters_buffer, get_line_length)
{
    using chatacter_buffer_cache = cc::basic_character_buffer_cache<char, 4, 8, 4, 8>;
    using chatacter_buffer = cc::basic_character_buffer<char, 4, 8, 4, 8>;
    
    chatacter_buffer_cache cb_cache(0);
    cc::cboffset_t cboffst = 0;
    chatacter_buffer::char_type ch = 65;
    
    auto it_cb = cb_cache.insert_first_character_buffer();
    cb_cache.lock(it_cb);
    
    it_cb->insert_character(ch++, cboffst++);
    it_cb->insert_character(ch++, cboffst++);
    it_cb->insert_character(ch++, cboffst++);
    it_cb->insert_character(ch++, cboffst++);
    it_cb->insert_character(ch++, cboffst++);
    it_cb->insert_character('\n', cboffst++);
    it_cb->insert_character('\n', cboffst++);
    it_cb->insert_character(ch++, cboffst++);
    it_cb->insert_character(ch++, cboffst++);
    it_cb->insert_character(ch++, cboffst++);
    it_cb->insert_character(ch++, cboffst++);
    it_cb->insert_character(ch++, cboffst++);
    
    ASSERT_TRUE(it_cb->get_line_length(0) == 6);
    ASSERT_TRUE(it_cb->get_line_length(6) == 1);
    ASSERT_TRUE(it_cb->get_line_length(7) == 5);
}


TEST(basic_characters_buffer, operator_subscript)
{
    using chatacter_buffer_cache = cc::basic_character_buffer_cache<char, 4, 8, 4, 8>;
    using chatacter_buffer = cc::basic_character_buffer<char, 4, 8, 4, 8>;
    
    cc::cboffset_t cboffst;
    chatacter_buffer::char_type ch;
    chatacter_buffer_cache cb_cache(0);
    std::size_t i;
    
    auto it_cb = cb_cache.insert_first_character_buffer();
    cb_cache.lock(it_cb);
    
    for (ch = 33, cboffst = 0;
         ch < 127;
         ++ch, ++cboffst)
    {
        it_cb->insert_character(ch, cboffst);
    }
    
    for (i = 0, ch = 33;
         ch < 127;
         ++i, ++ch)
    {
        ASSERT_TRUE((*it_cb)[i] == ch);
    }
    
    cb_cache.unlock(it_cb);
}
