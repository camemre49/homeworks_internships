#include "typelist_utils.h"
#include "typelist.h"
#include <type_traits>
#include <iostream>

int main() { 
    using typelist = List<int, float, double>;
    using affected = typename Append<char, typelist>::type;
    using expected = List<int, float, double, char>;
    static_assert(std::is_same<affected, expected>::value);
    
    // Test case 1: Append a type to a 3-element list
    using typelist1 = List<int, float, double>;
    using affected1 = typename Append<char, typelist1>::type;
    using expected1 = List<int, float, double, char>;
    static_assert(std::is_same<affected1, expected1>::value, "Test case 1 failed");

}