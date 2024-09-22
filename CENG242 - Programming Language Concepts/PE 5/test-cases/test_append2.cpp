#include "typelist_utils.h"
#include "typelist.h"
#include <type_traits>
#include <iostream>

using namespace std;

int main() { 
    using typelist = List<>;
    using affected = typename Append<double, typelist>::type;
    using expected = List<double>;
    static_assert(std::is_same<affected, expected>::value);

    // Test case 2: Append a type to a single-element list
    using single_element_list = List<int>;
    using affected2 = typename Append<float, single_element_list>::type;
    using expected2 = List<int, float>;
    static_assert(std::is_same<affected2, expected2>::value, "Test case 2 failed");
}