#ifndef HISTORY_H
#define HISTORY_H

#include <list>
#include <string>

namespace ose4g
{

    class History
    {
        std::list<std::string> d_historyDB;
        std::list<std::string>::iterator d_iterator;

    public:
        std::pair<bool, const std::string> getPrevious();
        std::pair<bool, const std::string> getNext();
        void addBack(const std::string &record);
        void addFront(const std::string &record);
        void edit(const std::string &s);
    };
}

#endif