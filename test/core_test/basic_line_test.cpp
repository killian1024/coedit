//
// Created by Killian on 16/03/18.
//

#include <gtest/gtest.h>
#include <kboost/kboost.hpp>

#include "core/basic_line_cache.hpp"

namespace cc = coedit::core;


TEST(basic_line, constructor_with_parameters)
{
    //using chatacters_buffer_cache = cc::basic_character_buffer_cache<char, 4, 8, 4, 8>;
    //using line = cc::basic_line<char, 4, 4, 8, 4, 8, std::allocator<int>>;
    //using lines_cache = cc::basic_line_cache<char, 4, 4, 8, 4, 8, std::allocator<int>>;
    //
    //lines_cache lines_cche(0);
    //chatacters_buffer_cache characters_buffer_cche(0);
    //
    //lines_cche.insert(0, line(0, cc::EMPTY, cc::EMPTY, &lines_cche, &characters_buffer_cche));
    //auto& lne = lines_cche.get(0);
}

