#include <iostream>
#include "nfa.hpp"

using namespace std;

int main(){
    NFA M1("nfa1.inp");
    NFA M(!M1);
    M.process("abaabababb");
    M.process("abaabab");
    M.process("abaababaaa");
    M.process("abaababa");

    return 0;
}