#include "auxiliar_code/aux_functions.h"
#include "commands/commands.h"
#include <string>
#include <iostream>

int main(int argc, char **argv)
{
    std::string DSIP, DSport;
    
    ParseArgs(argc, argv,DSIP,DSport);
    Client(DSIP,DSport);
    exit(EXIT_SUCCESS);
}