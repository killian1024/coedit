//
// Created by Killian on 16/03/18.
//

#include <gtest/gtest.h>
#include <kboost/kboost.hpp>

#include "core/basic_line_cache.hpp"

namespace cc = coedit::core;


TEST(basic_line, constructor_with_parameters)
{
    using chatacter_buffer_cache = cc::basic_character_buffer_cache<char, 4, 8, 4, 8,
                                                                    std::allocator<int>>;
    using chatacter_buffer = cc::basic_character_buffer<char, 4, 8, 4, 8, std::allocator<int>>;
    using line_cache = cc::basic_line_cache<char, 4, 8, 4, 8, 4, 4, 8, std::allocator<int>>;
    using line = cc::basic_line<char, 4, 8, 4, 8, 4, 4, 8, std::allocator<int>>;
    
    chatacter_buffer_cache cb_cache = chatacter_buffer_cache(0);
    line_cache lne_cache = line_cache(&cb_cache, 0);
    
    auto it_lne = lne_cache.insert_first_line();
}


TEST(basic_line, insert_character)
{
    using chatacter_buffer_cache = cc::basic_character_buffer_cache<char, 4, 8, 4, 8,
                                                                    std::allocator<int>>;
    using chatacter_buffer = cc::basic_character_buffer<char, 4, 8, 4, 8, std::allocator<int>>;
    using line_cache = cc::basic_line_cache<char, 4, 8, 4, 8, 4, 4, 8, std::allocator<int>>;
    using line = cc::basic_line<char, 4, 8, 4, 8, 4, 4, 8, std::allocator<int>>;
    
    chatacter_buffer_cache cb_cache = chatacter_buffer_cache(0);
    line_cache lne_cache = line_cache(&cb_cache, 0);
    cc::loffset_t loffset = 0;
    line::char_type ch = 65;
    line::char_type orig_ch = ch;
    
    auto it_lne1 = lne_cache.insert_first_line();
    it_lne1->insert_character(ch++, loffset++);
    it_lne1->insert_character(ch++, loffset++);
    it_lne1->insert_character(ch++, loffset++);
    it_lne1->insert_character(ch++, loffset++);
    
    ASSERT_TRUE((*it_lne1)[0] == orig_ch + 0);
    ASSERT_TRUE((*it_lne1)[1] == orig_ch + 1);
    ASSERT_TRUE((*it_lne1)[2] == orig_ch + 2);
    ASSERT_TRUE((*it_lne1)[3] == orig_ch + 3);
    
    auto it_lne2 = lne_cache.insert_line_after(it_lne1->get_lid(), loffset,
                                               cc::newline_format::UNIX);
    loffset = 0;
    it_lne2->insert_character(ch++, loffset++);
    it_lne2->insert_character(ch++, loffset++);
    it_lne2->insert_character(ch++, loffset++);
    it_lne2->insert_character(ch++, loffset++);
    
    ASSERT_TRUE((*it_lne1)[0] == orig_ch + 0);
    ASSERT_TRUE((*it_lne1)[1] == orig_ch + 1);
    ASSERT_TRUE((*it_lne1)[2] == orig_ch + 2);
    ASSERT_TRUE((*it_lne1)[3] == orig_ch + 3);
    
    ASSERT_TRUE((*it_lne2)[0] == orig_ch + 4);
    ASSERT_TRUE((*it_lne2)[1] == orig_ch + 5);
    ASSERT_TRUE((*it_lne2)[2] == orig_ch + 6);
    ASSERT_TRUE((*it_lne2)[3] == orig_ch + 7);
}


TEST(basic_line, erase_character)
{
    using chatacter_buffer_cache = cc::basic_character_buffer_cache<char, 4, 8, 4, 8,
                                                                    std::allocator<int>>;
    using chatacter_buffer = cc::basic_character_buffer<char, 4, 8, 4, 8, std::allocator<int>>;
    using line_cache = cc::basic_line_cache<char, 4, 8, 4, 8, 4, 4, 8, std::allocator<int>>;
    using line = cc::basic_line<char, 4, 8, 4, 8, 4, 4, 8, std::allocator<int>>;
    
    chatacter_buffer_cache cb_cache = chatacter_buffer_cache(0);
    line_cache lne_cache = line_cache(&cb_cache, 0);
    cc::loffset_t loffset = 0;
    line::char_type ch = 65;
    
    auto it_lne1 = lne_cache.insert_first_line();
    it_lne1->insert_character(ch++, loffset++);
    it_lne1->insert_character(ch++, loffset++);
    it_lne1->insert_character(ch++, loffset++);
    
    auto it_lne2 = lne_cache.insert_line_after(it_lne1->get_lid(), loffset,
                                               cc::newline_format::UNIX);
    loffset = 0;
    it_lne2->insert_character(ch++, loffset++);
    it_lne2->insert_character(ch++, loffset++);
    it_lne2->insert_character(ch++, loffset++);
    it_lne2->insert_character(ch++, loffset++);
    it_lne2->insert_character(ch++, loffset++);
    it_lne2->erase_character(loffset - 1);
    it_lne2->erase_character(loffset - 4);
    
    ASSERT_TRUE((*it_lne2)[0] == ch - 5);
    ASSERT_TRUE((*it_lne2)[1] == ch - 3);
    ASSERT_TRUE((*it_lne2)[2] == ch - 2);
}

