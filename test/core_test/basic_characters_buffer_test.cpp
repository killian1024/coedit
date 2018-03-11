//
// Created by Killian on 26/02/18.
//

#include <gtest/gtest.h>
#include <kboost/kboost.hpp>

#include "core/basic_file_editor.hpp"

namespace cc = coedit::core;


TEST(basic_characters_buffer, operator_subscript)
{
    using chatacters_buffer_cache = cc::basic_characters_buffer_cache<char, 4, 8, 4, 8>;
    using chatacters_buffer = cc::basic_characters_buffer<char, 4, 8, 4, 8>;
    
    cc::cboffset_t cboffst;
    chatacters_buffer::char_type ch;
    chatacters_buffer_cache cb_cache(0);
    std::size_t i;
    
    cb_cache.insert(0, chatacters_buffer(0, EMPTY, EMPTY, &cb_cache));
    auto& cb = cb_cache.get_characters_buffer_and_lock(0);
    
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
    
    cb_cache.unlock_characters_buffer(0);
}
