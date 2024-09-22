#include <iostream>
#include "nfa.hpp"

using namespace std;

int main(){
    NFA M("dfa1.inp");
    cout << M;

    return 0;
}