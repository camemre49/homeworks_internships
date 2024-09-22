#include "typelist_utils.h"
#include "typelist.h"
#include <type_traits>
#include <iostream>

int main() { 
    using typelist = List<double, double, double, double, double, double, double, double, double, double, double, double>;
    using affected = typename Prepend<char, typelist>::type;
    using expected = List<char, double, double, double, double, double, double, double, double, double, double, double, double>;
    static_assert(std::is_same<affected, expected>::value);
    
    // Test case 3: Prepend a type to an empty list
    using empty_list = List<>;
    using affected3 = typename Prepend<int, empty_list>::type;
    using expected3 = List<int>;
    static_assert(std::is_same<affected3, expected3>::value, "Test case 3 failed");
}