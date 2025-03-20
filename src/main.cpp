#include <iostream>
#include <map>
#include <vector>
#include "CFG.h"
using namespace std;

int main() {
    CFG cfg("..\\input.txt");  
    cout << "\nOriginal CFG:\n";
    cfg.print();

    cout << "\nCFG Applying Left Factoring:\n";
    cfg.LeftFactoring();
    cfg.print();

    cout << "\nCFG Removing Left Recursion:\n";
    cfg.LeftRecursion();
    cfg.print();

    return 0;
}

