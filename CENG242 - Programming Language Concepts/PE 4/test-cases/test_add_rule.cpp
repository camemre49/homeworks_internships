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

     TransitionTable TT;
     cout << (find(TT.rules.begin(), TT.rules.end(), rule1) != TT.rules.end()) << " "
          << (find(TT.rules.begin(), TT.rules.end(), rule2) != TT.rules.end())
          << endl;
     TT.add_rule("q0", 'a', "q1");
     cout << (find(TT.rules.begin(), TT.rules.end(), rule1) != TT.rules.end()) << " "
          << (find(TT.rules.begin(), TT.rules.end(), rule2) != TT.rules.end())
          << endl;
     TT.add_rule("q000", 'a', "sss");
     cout << (find(TT.rules.begin(), TT.rules.end(), rule1) != TT.rules.end()) << " "
          << (find(TT.rules.begin(), TT.rules.end(), rule2) != TT.rules.end())
          << endl;
     TT.add_rule("q000", '1', "q1");
     cout << (find(TT.rules.begin(), TT.rules.end(), rule1) != TT.rules.end()) << " "
          << (find(TT.rules.begin(), TT.rules.end(), rule2) != TT.rules.end())
          << endl;
     TT.add_rule("q", '1', "sss");
     cout << (find(TT.rules.begin(), TT.rules.end(), rule1) != TT.rules.end()) << " "
          << (find(TT.rules.begin(), TT.rules.end(), rule2) != TT.rules.end())
          << endl;
     TT.add_rule("q000", '1', "sss");
     cout << (find(TT.rules.begin(), TT.rules.end(), rule1) != TT.rules.end()) << " "
          << (find(TT.rules.begin(), TT.rules.end(), rule2) != TT.rules.end())
          << endl;

return 0;
}