#include "command-processor.h"

namespace ose4g{ 
    CommandProcessor::CommandProcessor(const std::string &name){}

    void CommandProcessor::help(){}
    bool add(const Command &command, std::function<void(Args)> processor, const std::string &description){}
    void run(){}
    bool parseStatement(const std::string &input, Command &command, Args &args){
        return false;
    }
    bool process(const Command &command, Args args){}
}
