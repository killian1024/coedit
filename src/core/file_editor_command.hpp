//
// Created by Killian on 01/02/18.
//

#ifndef COEDIT_CORE_FILE_EDITOR_COMMAND_HPP
#define COEDIT_CORE_FILE_EDITOR_COMMAND_HPP


namespace coedit {
namespace core {


enum class file_editor_command : std::uint32_t
{
    NIL,
    INSERT,
    NEWLINE,
    BACKSPACE,
    GO_LEFT,
    GO_RIGHT,
    GO_UP,
    GO_DOWN,
    HOME,
    END,
    CTRL_HOME,
    CTRL_END,
    SAVE_FILE,
    EXIT,
};


using fec_t = file_editor_command;


}
}


#endif
