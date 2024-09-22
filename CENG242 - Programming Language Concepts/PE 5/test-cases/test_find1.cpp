#include "typelist_utils.h"
#include "typelist.h"
#include <type_traits>
#include <iostream>

int main() { 

    using L3 = List<int, double, char>;
    static_assert(Find<double, 0, L3>::value == 1);
    
    // Test case 2: Find the first occurrence of int starting from index 3
    using L2 = List<float, float, float, int, float, int, int, int, float>;
    static_assert(Find<int, 0, L2>::value == 3, "Test case 2 failed");
}