#include <iostream>
#include "CFG.h"

using namespace std;

int main() {
    CFG cfg("..\\input.txt");

    cout << "\nOriginal CFG:\n";
    cfg.print();

    cout << "\nApplying Left Factoring...\n";
    cfg.LeftFactoring();
    cfg.print();

    cout << "\nRemoving Left Recursion...\n";
    cfg.LeftRecursion();
    cfg.print();

    cout << "\nComputing FIRST Sets...\n";
    cfg.computeFirstSets();
    cfg.printFirstSets();

    cout << "\nComputing FOLLOW Sets...\n";
    cfg.computeFollowSets();
    cfg.printFollowSets();
    
    cout << "\nConstructing LL(1) Parsing Table...\n";
    cfg.constructParsingTable();
    cfg.printParsingTable();

    return 0;
}
