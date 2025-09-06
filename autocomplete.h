#ifndef AUTOCOMPLETE_H
#define AUTOCOMPLETE_H

#include <string>
#include <vector>
#include <map>
#include <memory>



namespace ose4g
{
    class AutoComplete{
        public:
            struct Node{
                std::map<char,std::unique_ptr<Node>> children;
                bool isWord = false;
            };
        private:
            // Trie Node
            std::unique_ptr<Node> root;

            // depth first search for Trie Node
            static void dfs(Node* node, std::vector<std::string>& suggestions, std::string path);
        public:
            AutoComplete():root(std::make_unique<Node>()){}
            /**
            * @brief gets suggestions for the given prefix
            */
            std::vector<std::string> getSuggestions(const std::string& prefix);

            /**
            * @brief adds string to a particular suggestion
            */
            void add(const std::string&);
    };
}

#endif