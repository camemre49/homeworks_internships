#include <iostream>
#include "nfa.hpp"

using namespace std;

int main(){
    NFA M1("nfa2.inp");
    NFA M2("dfa1.inp");
    NFA M = M1 + M2;

    M.process("0000010100");
    M.process("10011011");
    M.process("001001");
    M.process("aabba");

    return 0;
}