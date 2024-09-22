#include <iostream>
#include "nfa.hpp"

using namespace std;

int main(){
    NFA M("nfa1.inp");
    M.process("abaababa");
    M.process("abaabababb");
    M.process("abaabab");
    M.process("abaababaaa");

    return 0;
}