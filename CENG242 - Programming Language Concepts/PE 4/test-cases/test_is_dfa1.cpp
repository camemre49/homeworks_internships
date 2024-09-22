#include <iostream>
#include "nfa.hpp"

using namespace std;

int main(){
    NFA M("nfa2.inp");
    cout << M.is_DFA() << endl;

    return 0;
}