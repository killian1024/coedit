//
// Created by Killian on 19/01/18.
//

#ifndef COEDIT_CORE_BASIC_FILE_EDITOR_HPP
#define COEDIT_CORE_BASIC_FILE_EDITOR_HPP

#include <memory>

#include <kboost/kboost.hpp>

#include "basic_line.hpp"
#include "fundamental_types.hpp"
#include "basic_lines_cache.hpp"


namespace coedit {
namespace core {


template<typename TpChar, std::size_t LINES_CACHE_SIZE, typename TpAllocator = std::allocator<int>>
class basic_file_editor
{
public:
    using char_type = TpChar;
    
    template<typename T>
    using allocator_type = typename TpAllocator::template rebind<T>::other;
    
    using lines_cache_type = basic_lines_cache<TpChar, LINES_CACHE_SIZE, TpAllocator>;
    
    using line_type = basic_line<TpChar, TpAllocator>;
    
    basic_file_editor()
            : lnes_cache_()
    {
    }
    
    void handle_command(std::uint64_t cmd)
    {
    }

private:
    lines_cache_type lnes_cache_;
};


using file_editor = basic_file_editor<char16_t, 8192>;


}
}


#endif
