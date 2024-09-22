#include "typelist_utils.h"
#include "typelist.h"
#include <type_traits>
#include <iostream>

int main() { 
    using typelist = List<int, float, double>;
    using affected = typename Replace<char, 0, typelist>::type;
    using expected = List<char, float, double>;
    static_assert(std::is_same<affected, expected>::value);
    
    // Test case 4: Replace with a different complex type in a 5-element list
    using typelist4 = List<int, float, double, char, short>;
    using complex_type = std::pair<int, double>;
    using affected4 = typename Replace<complex_type, 2, typelist4>::type;
    using expected4 = List<int, float, complex_type, char, short>;
    static_assert(std::is_same<affected4, expected4>::value, "Test case 4 failed");
}