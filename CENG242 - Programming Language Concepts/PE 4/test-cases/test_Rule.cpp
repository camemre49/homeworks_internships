#include <iostream>
#include "components.hpp"

using namespace std;

int main(){
    Rule rule1("q0", 'a', "q1");
    Rule rule2("q000", '1', "sss");

    cout << "(" << rule1.initial_state << ", " << rule1.symbol << ", " << rule1.final_state << ")\n";
    cout << "(" << rule2.initial_state << ", " << rule2.symbol << ", " << rule2.final_state << ")\n";

    return 0;
}