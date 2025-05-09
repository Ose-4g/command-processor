#include <iostream>
#include "command-processor.h"
#include <vector>

int main()
{
    std::cout << "Hello World!" << std::endl;
    ose4g::CommandProcessor processor("MyApplication");
    processor.add("send", [](ose4g::Args args)
                  { std::cout << "sending " << args[0]; }, "description for sending file");
    processor.run();
    return 0;
}