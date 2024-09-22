#include <iostream>
#include "nfa.hpp"

using namespace std;

int main(){
    NFA M("dfa2.inp");
    cout << M;

    return 0;
}