#include "system/server.hpp"


int main(int argc, char* argv[])
{
    try
    {
        coedit::system::server serv(30050);
    
        return serv.execute();
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