#include "history.h"

namespace ose4g{
    void History::addBack(const std::string& record){
        d_historyDB.push_back(record);
        d_iterator = d_historyDB.end();
    }

    void History::addFront(const std::string& record){
        d_historyDB.push_front(record);
        d_iterator = d_historyDB.begin();
    }

    std::pair<bool, const std::string> History::getPrevious(){
        if(d_historyDB.size()==0)
        {
            return {false, ""};
        }
        if(d_iterator == d_historyDB.begin())
        {
            return {false, *d_iterator};
        }
        d_iterator--;
        return {true, *d_iterator};
    }

    std::pair<bool, const std::string> History::getNext(){
        if(d_historyDB.size() == 0)
        {
            return {false, ""};
        }

        // if this is the last element
        if(d_iterator == std::prev(d_historyDB.end()))
        {
            return {false, ""};
        }
        d_iterator++;
        return {true, *d_iterator};
    }

    void History::edit(const std::string& s){
        *d_iterator = s;
    }
}