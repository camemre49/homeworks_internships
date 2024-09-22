#include "typelist_utils.h"
#include "typelist.h"
#include <type_traits>
#include <iostream>

int main() { 
    using typelist = List<double, double, double>;
    using affected = typename Append<double, typelist>::type;
    using expected = List<double, double, double, double>;
    static_assert(std::is_same<affected, expected>::value);
    
    // Test case 3: Append a type to an empty list
    using empty_list = List<>;
    using affected3 = typename Append<int, empty_list>::type;
    using expected3 = List<int>;
    static_assert(std::is_same<affected3, expected3>::value, "Test case 3 failed");
}