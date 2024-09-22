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

    cout << rule1.applies_to("q0", 'a') << " " << rule1.applies_to("q0", 'b') << " " << rule1.applies_to("q1", 'a') << endl;
    cout << rule2.applies_to("q000", '1') << " " << rule2.applies_to("q000", 'a') << " " << rule2.applies_to("sss", '1') << endl;

    return 0;
}