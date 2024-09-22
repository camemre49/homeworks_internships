#include <iostream>
#include "components.hpp"

using namespace std;

int main(){
    TransitionTable tt, tt2;

    Rule rule1;
    rule1.initial_state = "qq0"; rule1.symbol = 'a'; rule1.final_state = "qq1";
    Rule rule2;
    rule2.initial_state = "qq0"; rule2.symbol = 'a'; rule2.final_state = "qq2";
    Rule rule3;
    rule3.initial_state = "q0", rule3.symbol = 'a'; rule3.final_state = "q1";

    tt.rules.push_back(rule1);
    tt.rules.push_back(rule2);
    tt2.rules.push_back(rule3);

    tt2 += tt;
    for (Rule rule : tt2.rules)
        cout << "(" << rule.initial_state << ", " << rule.symbol << ", " << rule.final_state <<")\n";
    cout << endl;
    for (Rule rule : tt.rules)
        cout << "(" << rule.initial_state << ", " << rule.symbol << ", " << rule.final_state <<")\n";

    return 0;
}