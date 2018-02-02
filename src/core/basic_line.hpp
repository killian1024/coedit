//
// Created by Killian on 20/01/18.
//

#ifndef COEDIT_CORE_BASIC_LINE_HPP
#define COEDIT_CORE_BASIC_LINE_HPP

#include <memory>

#include <kboost/kboost.hpp>

#include "basic_characters_buffer_cache.hpp"
#include "fundamental_types.hpp"
#include "line_flags.hpp"


namespace coedit {
namespace core {


template<
        typename TpChar,
        std::size_t CHARACTERS_BUFFER_CACHE_SIZE,
        std::size_t CHARACTERS_BUFFER_SIZE
>
class basic_line
{
public:
    using char_type = TpChar;
    
    using characters_buffer_cache_type = basic_characters_buffer_cache<
            TpChar,
            CHARACTERS_BUFFER_CACHE_SIZE,
            CHARACTERS_BUFFER_SIZE
    >;
    
    using flags_type = kcontain::flags<line_flags>;
    
    basic_line()
            : lid_(EMPTY)
            , prev_(EMPTY)
            , nxt_(EMPTY)
            , cbid_(EMPTY)
            , cboffset_(0)
            , chars_buf_cache_(nullptr)
    {
    }
    
    basic_line(
            lid_t lid,
            lid_t prev,
            lid_t nxt,
            cbid_t bid,
            cboffset_t bid_offset,
            characters_buffer_cache_type* chars_cache
    )
            : lid_(lid)
            , prev_(prev)
            , nxt_(nxt)
            , cbid_(bid)
            , cboffset_(bid_offset)
            , chars_buf_cache_(chars_cache)
    {
    }
    
    lid_t get_lid() const
    {
        return lid_;
    }
    
    void set_lid(lid_t lid)
    {
        lid_ = lid;
    }
    
    lid_t get_prev() const
    {
        return prev_;
    }
    
    void set_prev(lid_t prev)
    {
        prev_ = prev;
    }
    
    lid_t get_nxt() const
    {
        return nxt_;
    }
    
    void set_nxt(lid_t nxt)
    {
        nxt_ = nxt;
    }
    
    cbid_t get_cbid() const
    {
        return cbid_;
    }
    
    void set_cbid(cbid_t cbid)
    {
        cbid_ = cbid;
    }
    
    cboffset_t get_cboffset() const
    {
        return cboffset_;
    }
    
    void set_cboffset(cboffset_t cboffset)
    {
        cboffset_ = cboffset;
    }
    
    void insert_character(char_type ch, loffset_t loffset)
    {
        chars_buf_cache_->load_characters_buffer(&cbid_, &cboffset_);
        chars_buf_cache_->insert_character(ch, cbid_, cboffset_, loffset);
    }

private:
    lid_t lid_;
    
    lid_t prev_;
    
    lid_t nxt_;
    
    cbid_t cbid_;
    
    cboffset_t cboffset_;
    
    characters_buffer_cache_type* chars_buf_cache_;
};


}
}


#endif
