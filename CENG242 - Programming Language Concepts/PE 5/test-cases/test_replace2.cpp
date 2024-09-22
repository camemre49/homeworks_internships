#include "typelist_utils.h"
#include "typelist.h"
#include <type_traits>
#include <iostream>

int main() { 
    using typelist = List<float>;
    using affected = typename Replace<double, 0, typelist>::type;
    using expected = List<double>;
    
    static_assert(std::is_same<affected, expected>::value);
    
    // Test case 2: Replace an element in a single-element list
    using single_element_list = List<int>;
    using affected3 = typename Replace<char, 0, single_element_list>::type;
    using expected3 = List<char>;
    static_assert(std::is_same<affected3, expected3>::value, "Test case 3 failed");
}