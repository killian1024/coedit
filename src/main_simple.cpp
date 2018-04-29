#include "system/simple.hpp"


int main(int argc, char* argv[])
{
    coedit::system::simple simpl("./file.txt");
    
    return simpl.execute();
}

