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

    rule1.update_state_name("q0", "q3");
    cout << "(" << rule1.initial_state << ", " << rule1.symbol << ", " << rule1.final_state <<")\n";

    rule1.update_state_name("q0", "q3");
    cout << "(" << rule1.initial_state << ", " << rule1.symbol << ", " << rule1.final_state <<")\n";
    
    rule2.update_state_name("q0", "q3");
    cout << "(" << rule2.initial_state << ", " << rule2.symbol << ", " << rule2.final_state <<")\n";

    rule2.update_state_name("sss", "q3");
    cout << "(" << rule2.initial_state << ", " << rule2.symbol << ", " << rule2.final_state <<")\n";
    
    return 0;
}