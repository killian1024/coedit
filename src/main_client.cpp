#include <filesystem>
#include <iostream>

#include <kboost/kboost.hpp>

#include "system/client.hpp"
#include "system/system_exception.hpp"


int main(int argc, char* argv[])
{
    try
    {
        kap::arg_parser ap("coedit_client");
        ap.add_help_text("Options:");
        ap.add_key_value_arg({"--server-address", "-a"}, "The server address.",
                             {kap::avt_t::STRING}, 1, 1,
                             kap::af_t::ALLWAYS_REQUIRED | kap::af_t::DEFAULT_ARG_FLAGS);
        ap.add_key_value_arg({"--port-number", "-P"}, "The port to connect in the server.",
                             {kap::avt_t::UINT16}, 1, 1,
                             kap::af_t::ALLWAYS_REQUIRED | kap::af_t::DEFAULT_ARG_FLAGS);
        ap.add_key_value_arg({"--file-to-edit", "-f"}, "The file to edit in the server.",
                             {kap::avt_t::STRING}, 1, 1,
                             kap::af_t::ALLWAYS_REQUIRED | kap::af_t::DEFAULT_ARG_FLAGS);
        ap.add_help_arg({"--help"}, "Display this help and exit.");
        ap.add_gplv3_version_arg({"--version"}, "Output version information and exit", "1.0",
                                 "2018", "Killian");
        ap.parse_args((unsigned int)argc, argv);
        
        coedit::system::client clint(
                ap.get_front_arg_value_as<std::string>("--server-address").c_str(),
                ap.get_front_arg_value_as<std::uint16_t>("--port-number"),
                ap.get_front_arg_value_as<std::filesystem::path>("--file-to-edit")
        );
        
        return clint.execute();
    }
    catch (const coedit::system::system_exception& e)
    {
        std::cerr << kios::set_light_red_text << e.what() << kios::newl;
        return 1;
    }
    catch (const std::exception& e)
    {
        std::cerr << kios::set_default_text << e.what() << kios::newl;
        return -1;
    }
}
