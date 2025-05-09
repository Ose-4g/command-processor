#include <string>

namespace ose4g
{
    enum Color
    {
        NONE=0,
        RED=31,
        BLUE=34,
        GREEN=32
    };
    std::string addColor(const std::string &value, Color color);
}