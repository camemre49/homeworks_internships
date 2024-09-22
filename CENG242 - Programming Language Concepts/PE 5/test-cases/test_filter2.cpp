#include "typelist_utils.h"
#include "typelist.h"
#include <type_traits>
#include <iostream>

int main() { 
    using typelist = List<int, float*, double>;
    using affected = typename Filter<std::is_floating_point, List<>, typelist>::type;
    using expected = List<double>;
    static_assert(std::is_same<affected, expected>::value);
    
    // Test case 1: Filter out pointer types from a 3-element list
    using typelist1 = List<int*, float, double*>;
    using affected1 = typename Filter<std::is_pointer, List<>, typelist1>::type;
    using expected1 = List<int*, double*>;
    static_assert(std::is_same<affected1, expected1>::value, "Test case 1 failed");
}