#include <iostream>
#include "components.hpp"

using namespace std;

int main(){
     ComputationBranch cb;
     cb.push_config("q0", "abc");
     
     pair<string, string> p1 = make_pair("q0", "abc");
     pair<string, string> p2 = make_pair("q1", "abc");
     pair<string, string> p3 = make_pair("q2", "bc");
     pair<string, string> p4 = make_pair("q3", "c");
     cout << (find(cb.config_history.begin(), cb.config_history.end(), p1) != cb.config_history.end()) << " "
          << (find(cb.config_history.begin(), cb.config_history.end(), p2) != cb.config_history.end()) << " "
          << (find(cb.config_history.begin(), cb.config_history.end(), p3) != cb.config_history.end()) << " "
          << (find(cb.config_history.begin(), cb.config_history.end(), p4) != cb.config_history.end())
          << endl;

     cb.push_config("q1", "abc");
     cout << (find(cb.config_history.begin(), cb.config_history.end(), p1) != cb.config_history.end()) << " "
          << (find(cb.config_history.begin(), cb.config_history.end(), p2) != cb.config_history.end()) << " "
          << (find(cb.config_history.begin(), cb.config_history.end(), p3) != cb.config_history.end()) << " "
          << (find(cb.config_history.begin(), cb.config_history.end(), p4) != cb.config_history.end())
          << endl;

     return 0;
}