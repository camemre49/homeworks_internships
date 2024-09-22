#include "typelist_utils.h"
#include "typelist.h"
#include <type_traits>
#include <iostream>

int main() { 
    using typelist = List<int*, float, double*>;
    using affected = typename Filter<std::is_pointer, List<>, typelist>::type;
    using expected = List<int*, double*>;
    static_assert(std::is_same<affected, expected>::value);
    
    // Test case 3: Filter out const types from a 4-element list
    using typelist3 = List<const int, float, const double, char>;
    using affected3 = typename Filter<std::is_const, List<>, typelist3>::type;
    using expected3 = List<const int, const double>;
    static_assert(std::is_same<affected3, expected3>::value, "Test case 3 failed");
}