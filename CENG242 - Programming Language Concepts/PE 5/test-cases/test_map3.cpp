#include "typelist_utils.h"
#include "typelist.h"
#include <type_traits>
#include <iostream>

int main() { 
    using typelist = List<int, float, double>;
    using affected = typename Map<std::add_lvalue_reference, List<>, typelist>::type;
    using expected = List<int&, float&, double&>;
    static_assert(std::is_same<affected, expected>::value);
    
    // Test case 4: Apply std::remove_const to each element in a list of const types
    using typelist4 = List<const int, const float, const double>;
    using affected4 = typename Map<std::remove_const, List<>, typelist4>::type;
    using expected4 = List<int, float, double>;
    static_assert(std::is_same<affected4, expected4>::value, "Test case 4 failed");
}