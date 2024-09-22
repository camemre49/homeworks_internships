#include "typelist_utils.h"
#include "typelist.h"
#include <type_traits>
#include <iostream>

int main() { 
    using typelist = List<>;
    using affected = typename Prepend<char, typelist>::type;
    using expected = List<char>;
    static_assert(std::is_same<affected, expected>::value);
    
    // Test case 2: Prepend a type to a single-element list
    using single_element_list = List<int>;
    using affected2 = typename Prepend<float, single_element_list>::type;
    using expected2 = List<float, int>;
    static_assert(std::is_same<affected2, expected2>::value, "Test case 2 failed");
}