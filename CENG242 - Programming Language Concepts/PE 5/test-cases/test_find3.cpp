#include "typelist_utils.h"
#include "typelist.h"
#include <type_traits>
#include <iostream>

int main() {
    static_assert(Find<char, 0, List<int, float, double>>::value == -1);
    
    // Test case 4: Find the first occurrence of char starting from index 5
    using L4 = List<int, float, double, char, float, int, char, int, float>;
    static_assert(Find<char, 0, L4>::value == 3, "Test case 4 failed");

}