#include <iostream>
#include "components.hpp"

using namespace std;

int main(){
     ComputationBranch cb;
     
     pair<string, string> p1 = make_pair("q0", "abc");
     cb.config_history.push_back(p1);
     cout << (cb.get_last_config() == p1) << endl;

     pair<string, string> p2 = make_pair("q1", "abc");
     cb.config_history.push_back(p2);
     cout << (cb.get_last_config() == p2) << endl;

     pair<string, string> p3 = make_pair("q0", "ab");
     cb.config_history.push_back(p3);
     cout << (cb.get_last_config() == p3) << endl;

     pair<string, string> p4 = make_pair("qq", "zxc");
     cb.config_history.push_back(p4);
     cout << (cb.get_last_config() == p4) << endl;
     
     return 0;
}