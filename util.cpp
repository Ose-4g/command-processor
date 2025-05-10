#include "util.h"
#include <sstream>

std::string ose4g::addColor(const std::string &s, Color color)
{
    std::stringstream ss;
    ss << "\033[1;" << std::to_string(color) << "m" << s << "\033[0m";
    return ss.str();
}