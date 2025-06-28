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
        /// @brief get the previous value from history
        /// @return pair of bool of {success, value}
        std::pair<bool, const std::string> getPrevious();

        /// @brief get the next value from history
        /// @return pair of bool of {success, value}
        std::pair<bool, const std::string> getNext();

        /// @brief add record to bottom of history
        /// @param record 
        void addBack(const std::string &record);

        /// @brief add record to top of history
        /// @param record 
        void addFront(const std::string &record);

        /// @brief edit current position in history
        /// @param s
        void edit(const std::string &s);

        /// @brief get all values in history
        /// @return string of all values from history
        std::string getAllHistory();
    };
}

#endif