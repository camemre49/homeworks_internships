#include "typelist_utils.h"
#include "typelist.h"
#include <type_traits>
#include <iostream>

int main() { 
    using typelist = List<int*, float*, double*>;
    using affected = typename Map<std::remove_pointer, List<>, typelist>::type;
    using expected = List<int, float, double>;
    static_assert(std::is_same<affected, expected>::value);
    
    // Test case 3: Apply std::add_const to each element in a 3-element list
    using typelist3 = List<int, float, double>;
    using affected3 = typename Map<std::add_const, List<>, typelist3>::type;
    using expected3 = List<const int, const float, const double>;
    static_assert(std::is_same<affected3, expected3>::value, "Test case 3 failed");
}