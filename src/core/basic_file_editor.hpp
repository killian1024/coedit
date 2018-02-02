//
// Created by Killian on 19/01/18.
//

#ifndef COEDIT_CORE_BASIC_FILE_EDITOR_HPP
#define COEDIT_CORE_BASIC_FILE_EDITOR_HPP

#include <memory>

#include <kboost/kboost.hpp>

#include "basic_characters_buffer_cache.hpp"
#include "basic_lines_cache.hpp"
#include "core_exception.hpp"
#include "file_editor_command.hpp"
#include "fundamental_types.hpp"


namespace coedit {
namespace core {


template<
        typename TpChar,
        std::size_t LINES_CACHE_SIZE,
        std::size_t CHARACTERS_BUFFER_CACHE_SIZE,
        std::size_t CHARACTERS_BUFFER_SIZE,
        typename TpAllocator
>
class basic_file_editor
{
public:
    using char_type = TpChar;
    
    template<typename T>
    using allocator_type = typename TpAllocator::template rebind<T>::other;
    
    using characters_buffer_cache_type = basic_characters_buffer_cache<
            TpChar,
            CHARACTERS_BUFFER_CACHE_SIZE,
            CHARACTERS_BUFFER_SIZE
    >;
    
    using lines_cache_type = basic_lines_cache<
            TpChar,
            LINES_CACHE_SIZE,
            CHARACTERS_BUFFER_CACHE_SIZE,
            CHARACTERS_BUFFER_SIZE,
            TpAllocator
    >;
    
    using line_type = basic_line<TpChar, CHARACTERS_BUFFER_CACHE_SIZE, CHARACTERS_BUFFER_SIZE>;
    
    basic_file_editor()
            : lnes_cache_()
            , first_lne_(EMPTY)
            , current_lne_(EMPTY)
            , first_display_lne_(EMPTY)
            , loffset_(0)
    {
        lnes_cache_.load_line(&current_lne_);
        first_lne_ = current_lne_;
        first_display_lne_ = current_lne_;
    }
    
    void handle_command(file_editor_command cmd, void* dat)
    {
        switch (cmd)
        {
            case file_editor_command::INSERT:
                lnes_cache_.load_line(&current_lne_);
                lnes_cache_.insert_character(*static_cast<char_type*>(dat), current_lne_, loffset_);
                ++loffset_;
                break;
        }
    }

private:
    lines_cache_type lnes_cache_;
    
    lid_t first_lne_;
    
    lid_t current_lne_;
    
    lid_t first_display_lne_;
    
    loffset_t loffset_;
};


using file_editor = basic_file_editor<char16_t, 8192, 160, 4096, std::allocator<int>>;


}
}


#endif
