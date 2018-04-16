//
// Created by Killian on 20/01/18.
//

#ifndef COEDIT_CORE_BASIC_LINE_HPP
#define COEDIT_CORE_BASIC_LINE_HPP

#include <memory>

#include <kboost/kboost.hpp>

#include "basic_character_buffer_cache.hpp"
#include "basic_line_cache.hpp"
#include "fundamental_types.hpp"
#include "newline_format.hpp"


namespace coedit {
namespace core {


template<
        typename TpChar,
        std::size_t CHARACTER_BUFFER_SIZE,
        std::size_t CHARACTER_BUFFER_CACHE_SIZE,
        std::size_t CHARACTER_BUFFER_ID_BUFFER_SIZE,
        std::size_t CHARACTER_BUFFER_ID_BUFFER_CACHE_SIZE,
        std::size_t LINE_CACHE_SIZE,
        std::size_t LINE_ID_BUFFER_SIZE,
        std::size_t LINE_ID_BUFFER_CACHE_SIZE,
        typename TpAllocator
>
class basic_line_cache;


template<
        typename TpChar,
        std::size_t CHARACTER_BUFFER_SIZE,
        std::size_t CHARACTER_BUFFER_CACHE_SIZE,
        std::size_t CHARACTER_BUFFER_ID_BUFFER_SIZE,
        std::size_t CHARACTER_BUFFER_ID_BUFFER_CACHE_SIZE,
        std::size_t LINE_CACHE_SIZE,
        std::size_t LINE_ID_BUFFER_SIZE,
        std::size_t LINE_ID_BUFFER_CACHE_SIZE,
        typename TpAllocator
>
class basic_line
{
public:
    using char_type = TpChar;
    
    template<typename T>
    using allocator_type = typename TpAllocator::template rebind<T>::other;
    
    using character_buffer_type = basic_character_buffer<
            TpChar,
            CHARACTER_BUFFER_SIZE,
            CHARACTER_BUFFER_CACHE_SIZE,
            CHARACTER_BUFFER_ID_BUFFER_SIZE,
            CHARACTER_BUFFER_ID_BUFFER_CACHE_SIZE
    >;
    
    using character_buffer_cache_type = basic_character_buffer_cache<
            TpChar,
            CHARACTER_BUFFER_SIZE,
            CHARACTER_BUFFER_CACHE_SIZE,
            CHARACTER_BUFFER_ID_BUFFER_SIZE,
            CHARACTER_BUFFER_ID_BUFFER_CACHE_SIZE
    >;
    
    using line_type = basic_line<
            TpChar,
            CHARACTER_BUFFER_SIZE,
            CHARACTER_BUFFER_CACHE_SIZE,
            CHARACTER_BUFFER_ID_BUFFER_SIZE,
            CHARACTER_BUFFER_ID_BUFFER_CACHE_SIZE,
            LINE_CACHE_SIZE,
            LINE_ID_BUFFER_SIZE,
            LINE_ID_BUFFER_CACHE_SIZE,
            TpAllocator
    >;
    
    using line_cache_type = basic_line_cache<
            TpChar,
            CHARACTER_BUFFER_SIZE,
            CHARACTER_BUFFER_CACHE_SIZE,
            CHARACTER_BUFFER_ID_BUFFER_SIZE,
            CHARACTER_BUFFER_ID_BUFFER_CACHE_SIZE,
            LINE_CACHE_SIZE,
            LINE_ID_BUFFER_SIZE,
            LINE_ID_BUFFER_CACHE_SIZE,
            TpAllocator
    >;
    
    class iterator : public kcontain::i_mutable_iterator<char_type, iterator>
    {
    public:
        using self_type = iterator;
        
        using value_type = char_type;
        
        using node_type = std::pair<cbid_t, cboffset_t>;
    
        iterator() noexcept
                : cur_({EMPTY, 0})
                , char_buf_cache_(nullptr)
        {
        }
    
        iterator(
                node_type cur,
                character_buffer_cache_type* chars_buf_cache
        ) noexcept
                : cur_(std::move(cur))
                , char_buf_cache_(chars_buf_cache)
        {
        }
        
        self_type& operator ++() noexcept override
        {
            character_buffer_type& current_cb =
                    char_buf_cache_->get_character_buffer(cur_.first);
            
            if (cur_.second + 1 >= CHARACTER_BUFFER_SIZE ||
                cur_.second + 1 >= current_cb.get_size())
            {
                cur_.second = 0;
                cur_.first = current_cb.get_nxt();
            }
            else
            {
                ++cur_.second;
            }
    
            if (current_cb[cur_.second] == LF || current_cb[cur_.second] == CR)
            {
                cur_.first = EMPTY;
                cur_.second = 0;
            }
            
            return *this;
        }
        
        self_type& operator --() noexcept override
        {
            // todo : implement the operator --
            return *this;
        }
        
        bool operator ==(const self_type& rhs) const noexcept override
        {
            return cur_ == rhs.cur_;
        }
        
        bool end() const noexcept override
        {
            return cur_.first == EMPTY && cur_.second == 0;
        }
        
        value_type& operator *() noexcept override
        {
            character_buffer_type& current_cb =
                    char_buf_cache_->get_character_buffer(cur_.first);
            
            return current_cb[cur_.second];
        }
        
        value_type* operator ->() noexcept override
        {
            character_buffer_type& current_cb =
                    char_buf_cache_->get_character_buffer(cur_.first);
            
            return &(current_cb[cur_.second]);
        }
        
        template<
                typename TpChar__,
                std::size_t CHARACTER_BUFFER_SIZE__,
                std::size_t CHARACTER_BUFFER_CACHE_SIZE__,
                std::size_t CHARACTER_BUFFER_ID_BUFFER_SIZE__,
                std::size_t CHARACTER_BUFFER_ID_BUFFER_CACHE_SIZE__,
                std::size_t LINE_CACHE_SIZE__,
                std::size_t LINE_ID_BUFFER_SIZE__,
                std::size_t LINE_ID_BUFFER_CACHE_SIZE__,
                typename TpAllocator__
        >
        friend class basic_file_editor;
    
    protected:
        node_type cur_;
    
        character_buffer_cache_type* char_buf_cache_;
    };
    
    basic_line()
            : lid_(EMPTY)
            , prev_(EMPTY)
            , nxt_(EMPTY)
            , cbid_(EMPTY)
            , cboffset_(0)
            , n_chars_(0)
            , l_cache_(nullptr)
            , cb_cache_(nullptr)
    {
    }
    
    basic_line(
            lid_t lid,
            lid_t prev,
            lid_t nxt,
            character_buffer_cache_type* cb_cache,
            line_cache_type* l_cache
    )
            : lid_(lid)
            , prev_(prev)
            , nxt_(nxt)
            , cbid_(EMPTY)
            , cboffset_(0)
            , n_chars_(0)
            , cb_cache_(cb_cache)
            , l_cache_(l_cache)
    {
        if (prev_ == EMPTY)
        {
            auto it_cb = cb_cache->insert_first_character_buffer();
            cbid_ = it_cb->get_cbid();
        }
        else
        {
            line_type& prev_lne = l_cache_->get_line(prev_);
            character_buffer_type& prev_cb =
                    cb_cache_->get_character_buffer(prev_lne.cbid_);
            
            cbid_ = prev_cb.get_cbid();
            cboffset_ = prev_lne.cboffset_ + prev_lne.n_chars_;
            
            if (nxt_ == EMPTY && prev_cb.get_size() - 1 < cboffset_)
            {
                n_chars_ = 0;
            }
            else
            {
                n_chars_ = prev_cb.get_line_length(cboffset_);
            }
        }
    }
    
    basic_line(
            const stdfs::path& l_path,
            character_buffer_cache_type* cb_cache,
            line_cache_type* l_cache
    )
            : cb_cache_(cb_cache)
            , l_cache_(l_cache)
    {
        std::ifstream ifs;
        
        ifs.exceptions(std::ofstream::failbit | std::ofstream::badbit);
        ifs.open(l_path, std::ios::in | std::ios::binary);
        
        ifs.read((char*)&lid_, sizeof(lid_));
        ifs.read((char*)&prev_, sizeof(prev_));
        ifs.read((char*)&nxt_, sizeof(nxt_));
        ifs.read((char*)&cbid_, sizeof(cbid_));
        ifs.read((char*)&cboffset_, sizeof(cboffset_));
        ifs.read((char*)&n_chars_, sizeof(n_chars_));
        
        ifs.close();
    }
    
    inline iterator begin() noexcept
    {
        character_buffer_type& current_cb =
                cb_cache_->get_character_buffer(cbid_);
    
        // todo : when the sharing buffers will be implemented, fix that shit :)
        try
        {
            current_cb[cboffset_];
        }
        catch (...)
        {
            return end();
        }
        // end_shit
    
        if (current_cb[cboffset_] == LF || current_cb[cboffset_] == CR)
        {
            return end();
        }
        
        return iterator({cbid_, cboffset_}, cb_cache_);
    }
    
    inline iterator end() noexcept
    {
        return iterator({EMPTY, 0}, cb_cache_);
    }
    
    void insert_character(char_type ch, loffset_t loffset) // todo : get the return of the buffer insertion
    {
        character_buffer_type& current_cb = cb_cache_->get_character_buffer(cbid_);
        lid_t current_nxt_lid = nxt_;
        line_type* nxt_lne;
    
        current_cb.insert_character(ch, cboffset_ + loffset);
        ++n_chars_;
    
        while (current_nxt_lid != EMPTY)
        {
            nxt_lne = &(l_cache_->get_line(current_nxt_lid));
            if (nxt_lne->cbid_ != cbid_)
            {
                break;
            }
        
            nxt_lne->cboffset_ += 1;
            current_nxt_lid = nxt_lne->nxt_;
        }
    }
    
    void erase_character(loffset_t loffset)
    {
        character_buffer_type& current_cb = cb_cache_->get_character_buffer(cbid_);
        lid_t current_nxt_lid = nxt_;
        line_type* nxt_lne;
        
        current_cb.erase_character(cboffset_ + loffset);
        --n_chars_;
    
        while (current_nxt_lid != EMPTY)
        {
            nxt_lne = &(l_cache_->get_line(current_nxt_lid));
            if (nxt_lne->cbid_ != cbid_)
            {
                break;
            }
            
            nxt_lne->cboffset_ += ~(lid_t)0;
            current_nxt_lid = nxt_lne->nxt_;
        }
    }
    
    void merge_with_next_line()
    {
        if (nxt_ == EMPTY)
        {
            throw invalid_operation_exception();
        }
    
        line_type* nxt_lne = &(l_cache_->get_line(nxt_));
        character_buffer_type& current_cb = cb_cache_->get_character_buffer(cbid_);
        
        // Erase the endline characters.
        if (n_chars_ - 1 > 0 && current_cb[cboffset_ + n_chars_ - 2] == CR)
        {
            erase_character(n_chars_ - 1);
        }
        erase_character(n_chars_ - 1);
        
        // Assigning the pointers to the new target.
        nxt_ = nxt_lne->nxt_;
        n_chars_ += nxt_lne->n_chars_;
        if (nxt_ != EMPTY)
        {
            nxt_lne = &(l_cache_->get_line(nxt_));
            nxt_lne->prev_ = lid_;
        }
    }
    
    bool can_go_left(loffset_t loffset)
    {
        return loffset > 0;
    }
    
    bool can_go_right(loffset_t loffset)
    {
        character_buffer_type& current_cb = cb_cache_->get_character_buffer(cbid_);
        auto val = current_cb[cboffset_ + loffset];
        
        return val != LF && val != CR && (nxt_ != EMPTY || loffset < n_chars_);
    }
    
    std::size_t get_line_length()
    {
        character_buffer_type& current_cb = cb_cache_->get_character_buffer(cbid_);
        
        if (n_chars_ > 0)
        {
            auto val = current_cb[cboffset_ + n_chars_ - 1];
            
            if (n_chars_ - 1 > 0 && current_cb[cboffset_ + n_chars_ - 2] == CR)
            {
                return n_chars_ - 2;
            }
            else if (current_cb[cboffset_ + n_chars_ - 1] == LF)
            {
                return n_chars_ - 1;
            }
        }
        
        return n_chars_;
    }
    
    void store(const stdfs::path& l_path) const
    {
        std::ofstream ofs;
        
        ofs.exceptions(std::ofstream::failbit | std::ofstream::badbit);
        ofs.open(l_path, std::ios::out | std::ios::binary);
    
        ofs.write((char*)&lid_, sizeof(lid_));
        ofs.write((char*)&prev_, sizeof(prev_));
        ofs.write((char*)&nxt_, sizeof(nxt_));
        ofs.write((char*)&cbid_, sizeof(cbid_));
        ofs.write((char*)&cboffset_, sizeof(cboffset_));
        ofs.write((char*)&n_chars_, sizeof(n_chars_));
        
        ofs.close();
    }
    
    char_type& operator [](loffset_t i)
    {
        character_buffer_type& current_cb = cb_cache_->get_character_buffer(cbid_);
        
        return current_cb[cboffset_ + i];
    }
    
    lid_t get_lid() const
    {
        return lid_;
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
    
    void set_n_chars(size_t n_chars)
    {
        n_chars_ = n_chars;
    }

private:
    lid_t lid_;
    
    lid_t prev_;
    
    lid_t nxt_;
    
    cbid_t cbid_;
    
    cboffset_t cboffset_;
    
    std::size_t n_chars_;
    
    character_buffer_cache_type* cb_cache_;
    
    line_cache_type* l_cache_;
};


}
}


#endif
