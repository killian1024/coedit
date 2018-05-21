//
// Created by Killian on 29/04/18.
//

#include "simple.hpp"


namespace coedit {
namespace system {


simple::simple(std::filesystem::path fle_path)
        : file_editr_(std::move(fle_path), coedit::core::newline_format::UNIX)
        , interf_(&file_editr_)
{
}


int simple::execute()
{
    file_editor_command_type cmd;
    char_type ch;
    bool execution_finish = false;
    
    interf_.init();
    
    while (!execution_finish)
    {
        interf_.print();
        
        if (interf_.get_command(&cmd, &ch), cmd != file_editor_command_type::NIL)
        {
            if (cmd == file_editor_command_type::EXIT)
            {
                execution_finish = true;
            }
            else
            {
                file_editr_.handle_command(cmd, ch);
            }
        }
    }
    
    interf_.free();
    
    return 0;
}


}
}
