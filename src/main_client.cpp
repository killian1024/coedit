#include <filesystem>
#include <iostream>

#include <kboost/kboost.hpp>

#include "system/client.hpp"
#include "system/system_exception.hpp"


int main(int argc, char* argv[])
{
    try
    {
        coedit::system::client clint("127.0.0.1", 30050, std::filesystem::path(
                "/media/HDD1/home/dragonfly/Desktop/file.c"));
        
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
