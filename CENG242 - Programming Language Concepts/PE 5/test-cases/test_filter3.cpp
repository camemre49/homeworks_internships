#include "typelist_utils.h"
#include "typelist.h"
#include <type_traits>
#include <iostream>

int main() { 
    using typelist = List<int&, float, double&>;
    using affected = typename Filter<std::is_lvalue_reference, List<>, typelist>::type;
    using expected = List<int&, double&>;
    static_assert(std::is_same<affected, expected>::value);
    
    // Test case 4: Filter out integral types from a 4-element list
    using typelist4 = List<int, float, double, char>;
    using affected4 = typename Filter<std::is_integral, List<>, typelist4>::type;
    using expected4 = List<int, char>;
    static_assert(std::is_same<affected4, expected4>::value, "Test case 4 failed");
}