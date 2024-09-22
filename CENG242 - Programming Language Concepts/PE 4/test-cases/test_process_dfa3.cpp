#include <iostream>
#include "nfa.hpp"

using namespace std;

int main(){
    NFA M("dfa3.inp");
    M.process("a");
    M.process("ababababa");
    M.process("aa");
    M.process("ababab");

    return 0;
}