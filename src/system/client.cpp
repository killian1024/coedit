//
// Created by Killian on 29/04/18.
//

#include "client.hpp"


namespace coedit {
namespace system {


client::client(stdfs::path fle_path)
        : file_editr_(std::move(fle_path), coedit::core::newline_format::UNIX)
        , interf_(&file_editr_)
{
}


int client::execute()
{
    file_editor_command_type cmd;
    bool execution_finish = false;
    
    interf_.init();
    
    while (!execution_finish)
    {
        if ((cmd = interf_.get_command()) != file_editor_command_type::NIL)
        {
            if (cmd == file_editor_command_type::EXIT)
            {
                execution_finish = true;
            }
            if (cmd <= file_editor_command_type::MAX)
            {
                file_editr_.handle_command(cmd);
            }
            else
            {
                file_editr_.insert_character((char_type)cmd);
            }
            
            interf_.print();
        }
    }
    
    interf_.free();
    
    return 0;
}


}
}
