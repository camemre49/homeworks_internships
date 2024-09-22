#include <iostream>
#include "components.hpp"

using namespace std;

int main(){
    Rule rule1; // in case the constructor is not implemented
    rule1.initial_state = "q0";
    rule1.symbol = 'a';
    rule1.final_state = "q1";

    Rule rule2;
    rule2.initial_state = "q000";
    rule2.symbol = '1';
    rule2.final_state = "sss";

    Rule rule3;
    rule3.initial_state = "q0";
    rule3.symbol = 'a';
    rule3.final_state = "sss";

    TransitionTable tt;
    tt.rules.push_back(rule1);
    tt.rules.push_back(rule2);
    tt.rules.push_back(rule3);

    tt.update_state_name("q0", "q3");
    for (Rule rule : tt.rules)
        cout << "(" << rule.initial_state << ", " << rule.symbol << ", " << rule.final_state <<")\n";
    cout << endl;

    tt.update_state_name("q2", "q3");
    for (Rule rule : tt.rules)
        cout << "(" << rule.initial_state << ", " << rule.symbol << ", " << rule.final_state <<")\n";
    cout << endl;

    tt.update_state_name("sss", "q10");
    for (Rule rule : tt.rules)
        cout << "(" << rule.initial_state << ", " << rule.symbol << ", " << rule.final_state <<")\n";
    
    return 0;
}