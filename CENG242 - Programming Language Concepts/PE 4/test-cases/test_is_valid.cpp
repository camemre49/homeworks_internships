#include <iostream>
#include "components.hpp"

using namespace std;

int main(){
    string line1("a b c");
    string line2("d e f");
    Alphabet A, B;
    A.init_from_line(line1);
    cout << A.is_valid("abc") << " " << A.is_valid("fbbacac") << " " << A.is_valid("ccdab") << " " << A.is_valid("a") << endl;
    B.init_from_line(line2);
    cout << B.is_valid("def") << " " << B.is_valid("ffef") << " " << B.is_valid("fa") << " " << B.is_valid("aacxa") << endl;
    
    return 0;
}