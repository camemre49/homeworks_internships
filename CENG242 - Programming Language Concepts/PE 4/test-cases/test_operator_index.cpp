#include <iostream>
#include "components.hpp"

using namespace std;

int main(){
    TransitionTable tt;

    Rule rule1;
    rule1.initial_state = "qq0"; rule1.symbol = 'a'; rule1.final_state = "qq1";
    Rule rule2;
    rule2.initial_state = "qq0"; rule2.symbol = 'a'; rule2.final_state = "qq2";

    tt.rules.push_back(rule1);
    tt.rules.push_back(rule2);

    for (string s : tt("qq0", 'a'))
        cout << s << " ";
    cout << endl;
    cout << tt("qq0", 'b').size() << endl;

    return 0;
}