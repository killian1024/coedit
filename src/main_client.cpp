#include "system/client.hpp"


int main(int argc, char* argv[])
{
    coedit::system::client clint("./file.txt");
    
    return clint.execute();
}
