#include <iostream>
#include "nfa.hpp"

using namespace std;

int main(){
    NFA M1("dfa1.inp");
    NFA M2("dfa2.inp");
    NFA M = M1 + M2;

    M.process("aabbbbaaaaab");
    M.process("baabbbaaaaab");
    M.process("bbbaa");
    M.process("aabbbab");

    return 0;
}