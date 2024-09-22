#include <iostream>
#include "nfa.hpp"

using namespace std;

int main(){
    NFA M1("dfa3.inp");
    NFA M2("nfa3.inp");
    NFA M = !M1 + M2;

    M.process("aabbbbaaaaab");
    M.process("0b010101a");
    M.process("abababa");
    M.process("00110101");

    return 0;
}