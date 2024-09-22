#include "typelist_utils.h"
#include "typelist.h"
#include <type_traits>
#include <iostream>

int main() { 

    using L3 = List<int, int, int, int, float, int, int, int, float>;
    static_assert(Find<float, 0, L3>::value == 4);
    
    // Test case 3: Find the first occurrence of double starting from index 0
    using L4 = List<int, float, int, float, double, int, int, int, float>;
    static_assert(Find<double, 0, L4>::value == 4, "Test case 3 failed");
}