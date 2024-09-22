#include <iostream>
#include "nfa.hpp"

using namespace std;

int main(){
    NFA M("nfa2.inp");
    M.process("10011011");
    M.process("11100");
    M.process("10100");
    M.process("0000010100");

    return 0;
}