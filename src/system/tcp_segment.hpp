//
// Created by Killian on 30/04/18.
//

#ifndef COEDIT_SYSTEM_TCP_SEGMENT_HPP
#define COEDIT_SYSTEM_TCP_SEGMENT_HPP

#include <cstdint>
#include <filesystem>

#include "../core/file_editor_command.hpp"
#include "../core/fundamental_types.hpp"


namespace coedit {
namespace system {


enum class tcp_segment_type : std::uint8_t
{
    SESSION_REQUEST,
    SESSION_REPLY,
    LEAVE_REQUEST,
    LEAVE_REPLY,
    INSERSION_REQUEST,
    INSERSION_REPLY,
    INSERSION_END
};


struct tcp_segment_data
{
    tcp_segment_data()
    {
        memset(this, 0, sizeof(tcp_segment_data));
    }
    
    tcp_segment_data(tcp_segment_type tcp_seg_typ)
    {
        memset(this, 0, sizeof(tcp_segment_data));
        typ = tcp_seg_typ;
    }
    
    tcp_segment_type typ;
    
    union data
    {
        std::uint8_t raw[4096];
        
        bool ok;
    
        std::size_t fle_sz;
        
        struct editor_command
        {
            core::lid_t lid;
    
            core::loffset_t loffset;
    
            core::file_editor_command cmd;
            
        } editr_cmd;
        
    } dat;
};


}
}


#endif
