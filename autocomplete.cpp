#include "autocomplete.h"

namespace ose4g
{
    void AutoComplete::add(const std::string& s){
        auto temp = root.get();
        for(char c: s)
        {
            if(!temp->children[c])
            {
                temp->children[c] = std::make_unique<Node>();
            }
            temp = temp->children[c].get();
        }
        temp->isWord = true;
    }

    void AutoComplete::dfs(AutoComplete::Node* root, std::vector<std::string>& suggestions, std::string path)
    {
        if(root->isWord)
        {
            suggestions.push_back(path);
        }
        for(auto& node: root->children)
        {
            dfs(node.second.get(), suggestions, path + node.first);
        }
    }
    std::vector<std::string> AutoComplete::getSuggestions(const std::string& s){
        auto temp = root.get();
        std::vector<std::string> suggestions;
        for(char c: s)
        {
            if(temp->children.find(c) == temp->children.end())
            {
                return suggestions;
            }
            temp = temp->children[c].get();
        }
        dfs(temp, suggestions, s);
        return suggestions;
    }   
}