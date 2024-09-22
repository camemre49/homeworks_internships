#include <iostream>
#include "nfa.hpp"

using namespace std;

int main(){
    NFA M("dfa2.inp");

    M.update_state_name("q0", "q30");
    cout << M.starting_state << endl;
    for (string state : M.all_states)
        cout << state << " ";
    cout << endl;
    for (Rule rule : M.transitions.rules)
        cout << "(" << rule.initial_state << ", " << rule.symbol << ", " << rule.final_state <<")\n";
    for (string state : M.final_states)
        cout << state << " ";
    cout << endl << endl;

    NFA M2("nfa1.inp");

    M2.update_state_name("q1", "q5");
    cout << M2.starting_state << endl;
    for (string state : M2.all_states)
        cout << state << " ";
    cout << endl;
    for (Rule rule : M2.transitions.rules)
        cout << "(" << rule.initial_state << ", " << rule.symbol << ", " << rule.final_state <<")\n";
    for (string state : M2.final_states)
        cout << state << " ";
    cout << endl;

    return 0;
}