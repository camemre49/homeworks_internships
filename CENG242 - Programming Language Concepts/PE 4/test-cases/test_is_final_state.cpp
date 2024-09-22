#include <iostream>
#include "nfa.hpp"

using namespace std;

int main(){
     NFA M("dfa1.inp");

     cout << M.is_final_state("q0") << " " << M.is_final_state("q") << " " << M.is_final_state("q1") << endl;
     NFA M2("nfa2.inp");

     cout << M2.is_final_state("q0") << " " << M2.is_final_state("q4") << " " << M2.is_final_state("q9") << " " << M2.is_final_state("q3") << endl;
     return 0;
}