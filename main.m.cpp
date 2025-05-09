#include <iostream>
#include "command-processor.h"


int main()
{
    std::cout<<"Hello World!"<<std::endl;
    ose4g::CommandProcessor processor("MyApplication");
    processor.run();
}