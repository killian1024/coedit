#include "system/server.hpp"


int main(int argc, char* argv[])
{
    coedit::system::server serv(30050);
    
    return serv.execute();
}