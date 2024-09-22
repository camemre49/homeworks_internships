#include "typelist_utils.h"
#include "typelist.h"
#include <type_traits>
#include <iostream>

int main() { 
    using typelist = List<int, float, double>;
    using affected = typename Map<std::add_pointer, List<>, typelist>::type;
    using expected = List<int*, float*, double*>;
    static_assert(std::is_same<affected, expected>::value);
    
    // Test case 2: Apply std::remove_pointer to each element in a list of pointer types
    using typelist2 = List<int*, float*, double*>;
    using affected2 = typename Map<std::remove_pointer, List<>, typelist2>::type;
    using expected2 = List<int, float, double>;
    static_assert(std::is_same<affected2, expected2>::value, "Test case 2 failed");
}