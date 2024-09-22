#include <iostream>
#include "nfa.hpp"

using namespace std;

int main(){
    NFA M1("dfa2.inp");
    NFA M(!M1);
    M.process("baabbabb");
    M.process("bbbaa");
    M.process("aabbbab");
    M.process("bababaaabb");

    return 0;
}