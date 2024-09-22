#include "typelist_utils.h"
#include "typelist.h"
#include <type_traits>
#include <iostream>

int main() { 
    using typelist = List<int, float, double>;
    using affected = typename Prepend<char, typelist>::type;
    using expected = List<char, int, float, double>;
    static_assert(std::is_same<affected, expected>::value);
    
    // Test case 1: Prepend a complex type to a 4-element list
    using typelist4 = List<int, float, double, char>;
    using complex_type = std::pair<int, double>;
    using affected4 = typename Prepend<complex_type, typelist4>::type;
    using expected4 = List<complex_type, int, float, double, char>;
    static_assert(std::is_same<affected4, expected4>::value, "Test case 4 failed");
}