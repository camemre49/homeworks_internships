#include <iostream>
#include "nfa.hpp"

using namespace std;

int main(){
    NFA M1("dfa1.inp");
    NFA M = !M1;
    cout << M;

    return 0;
}