//
// Created by Killian on 26/02/18.
//

#include <gtest/gtest.h>
#include <kboost/kboost.hpp>

#include "core/basic_file_editor.hpp"

namespace cc = coedit::core;


TEST(basic_characters_buffer, operator_subscript)
{
    using chatacters_buffer_cache = cc::basic_characters_buffer_cache<char, 3, 8>;
    using chatacters_buffer = cc::basic_characters_buffer<char, 3, 8>;
    
    chatacters_buffer::cboffset_type cboffst = 0;
    chatacters_buffer::char_type ch = 'A';
    chatacters_buffer_cache cb_cache;
    cb_cache.insert(0, chatacters_buffer(0, EMPTY, EMPTY, &cb_cache));
    auto& cb = cb_cache.get_character_buffer(0);
    
    std::size_t i;
    for (i = 0; i < cb.get_real_size() * 3; ++i)
    {
        cb.insert_character(ch++, cboffst++);
    }
}

