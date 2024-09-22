#include <iostream>
#include "nfa.hpp"

using namespace std;

int main(){
    NFA M("dfa2.inp");
    M.process("baabbabb");
    M.process("bbbaa");
    M.process("aabbbab");
    M.process("bababaaabb");

    return 0;
}