#include <iostream>
#include "components.hpp"

using namespace std;

int main(){
    string line1("a b c");
    string line2("d e f");
    string line3("a b d");
    Alphabet A, B, C, test;
    A.init_from_line(line1);
    B.init_from_line(line2);
    C.init_from_line(line3);

    test = A;
    test += B;
    cout << (test.get_symbols() == test.symbols) << endl;

    test = A;
    test += C;
    cout << (test.get_symbols() == test.symbols) << endl;
    
    return 0;
}