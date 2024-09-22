#include <iostream>
#include "nfa.hpp"

using namespace std;

int main(){
    NFA M1("dfa2.inp");
    NFA M2("dfa1.inp");
    NFA M = !M1 + M2;

    M.process("aabbaaaaab");
    M.process("baabbbaaaaab");
    M.process("bbaa");
    M.process("aabbab");

    return 0;
}