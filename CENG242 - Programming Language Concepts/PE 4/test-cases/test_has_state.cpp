#include <iostream>
#include "nfa.hpp"

using namespace std;

int main(){
     NFA M("dfa1.inp");

     cout << M.has_state("q0") << " " << M.has_state("q") << " " << M.has_state("q1") << endl;
     NFA M2("nfa2.inp");

     cout << M2.has_state("q0") << " " << M2.has_state("q") << " " << M2.has_state("q9") << " " << M2.has_state("q3") << endl;
     return 0;
}