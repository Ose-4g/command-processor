#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "autocomplete.h"

using namespace ::testing;

TEST(AutoCompleteTest, shouldNotGetAnyWords){
    ose4g::AutoComplete autocomplete;
    auto suggestions = autocomplete.getSuggestions("ose");
    
    ASSERT_EQ(suggestions.size(), 0);
}

TEST(AutoCompleteTest, shouldGetAllMatchingWords){
    ose4g::AutoComplete autocomplete;
    autocomplete.add("ose");
    autocomplete.add("ose4g");
    autocomplete.add("osemudiamen");
    autocomplete.add("os4ge");
    auto suggestions = autocomplete.getSuggestions("ose");
    ASSERT_THAT(suggestions, UnorderedElementsAre("ose", "ose4g", "osemudiamen"));
}