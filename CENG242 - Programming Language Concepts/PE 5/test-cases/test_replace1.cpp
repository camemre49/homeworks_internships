#include "typelist_utils.h"
#include "typelist.h"
#include <type_traits>
#include <iostream>

int main() { 
    using typelist = List<int, float, double>;
    using affected = typename Replace<char, 1, typelist>::type;
    using expected = List<int, char, double>;
    static_assert(std::is_same<affected, expected>::value);
    
    // Test case 1: Replace the first element (index 0) in a 3-element list
    using typelist2 = List<int, float, double>;
    using affected2 = typename Replace<char, 0, typelist2>::type;
    using expected2 = List<char, float, double>;
    static_assert(std::is_same<affected2, expected2>::value, "Test case 2 failed");
}