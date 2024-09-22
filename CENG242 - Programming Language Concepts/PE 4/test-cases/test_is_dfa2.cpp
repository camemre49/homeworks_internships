#include <iostream>
#include "nfa.hpp"

using namespace std;

int main(){
    NFA M("nfa1.inp");
    cout << M.is_DFA() << endl;

    return 0;
}