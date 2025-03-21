#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <set>
#include <vector>
#include <iomanip>
using namespace std;

class CFG {
private:
    map<string, vector<vector<string>>> productions;  // grammar rules
    map<string, set<string>> firstSets;  // FIRST sets
    map<string, set<string>> followSets; // FOLLOW sets
    map<pair<string, string>, vector<string>> parsingTable; // LL(1) Parsing Table
public:
    CFG(const string& filename) {
        read_from_file(filename);
    }

    void read_from_file(const string& filename) {
        ifstream file(filename);
        string line;

        while (getline(file, line)) {
            istringstream read(line);
            string nonTerminal, arrow, production;
            read >> nonTerminal >> arrow; // read the shuro ka "S ->"

            vector<vector<string>> rules;
            vector<string> currentRule;
            while (read >> production) {
                if (production == "|") {
                    rules.push_back(currentRule);
                    currentRule.clear();
                } else {
                    currentRule.push_back(production);
                }
            }
            if (!currentRule.empty()) {
                rules.push_back(currentRule);
            }

            productions[nonTerminal] = rules;
        }

        file.close();
    }

    void print() const {
        for (const auto& rule : productions) {
            cout << rule.first << " -> ";
            for (size_t i = 0; i < rule.second.size(); i++) {
                for (const auto& symbol : rule.second[i]) {
                    cout << symbol << " ";
                }
                if (i != rule.second.size() - 1) {
                    cout << "| ";
                }
            }
            cout << endl;
        }
    }

    void LeftRecursion() {
        map<string, vector<vector<string>>> newCFG;

        for (auto& rule : productions) {
            string nonTerminal = rule.first;
            vector<vector<string>> alpha, beta;

            for (auto& prod : rule.second) {
                if (!prod.empty() && prod[0] == nonTerminal) {
                    alpha.push_back(vector<string>(prod.begin() + 1, prod.end()));
                } else {
                    beta.push_back(prod);
                }
            }

            if (alpha.empty()) {
                newCFG[nonTerminal] = rule.second;
            } else {
                string newNonTerminal = nonTerminal + "'";

                for (auto& prod : beta) {
                    prod.push_back(newNonTerminal);
                    newCFG[nonTerminal].push_back(prod);
                }

                for (auto& prod : alpha) {
                    prod.push_back(newNonTerminal);
                    newCFG[newNonTerminal].push_back(prod);
                }
                newCFG[newNonTerminal].push_back({"ε"});
            }
        }

        productions = newCFG;
    }

    void LeftFactoring() {
        map<string, vector<vector<string>>> updatedCFG;
        int newNonTerminalCount = 1;

        for (const auto& rule : productions) {
            string nonTerminal = rule.first;
            const vector<vector<string>>& prods = rule.second;
            map<string, vector<vector<string>>> groupedProds;

            for (size_t i = 0; i < prods.size(); i++) {
                string prefix = prods[i][0];
                for (size_t j = i + 1; j < prods.size(); j++) {
                    if (prods[j][0] == prefix) {
                        groupedProds[prefix].push_back(prods[j]);
                    }
                }
                groupedProds[prefix].push_back(prods[i]);
            }

            for (const auto& group : groupedProds) {
                string prefix = group.first;
                const vector<vector<string>>& groupProds = group.second;

                if (groupProds.size() == 1) {
                    updatedCFG[nonTerminal].push_back(groupProds[0]);
                } else {
                    string newNonTerminal = nonTerminal + "_F" + to_string(newNonTerminalCount++);
                    updatedCFG[nonTerminal].push_back({prefix, newNonTerminal});

                    for (const vector<string>& prod : groupProds) {
                        vector<string> suffix(prod.begin() + 1, prod.end());
                        if (suffix.empty()) {
                            suffix.push_back("ε");
                        }
                        updatedCFG[newNonTerminal].push_back(suffix);
                    }
                }
            }
        }

        productions = updatedCFG;
    }

    void computeFirstSets() {
        bool changed = true;

        while (changed) {
            changed = false;

            for (const auto& rule : productions) {
                string nonTerminal = rule.first;

                for (const auto& production : rule.second) {
                    string firstSymbol = production[0];

                    if (isTerminal(firstSymbol)) {
                        if (firstSets[nonTerminal].insert(firstSymbol).second)
                            changed = true;
                    } else {
                        for (const string& symbol : production) {
                            if (!isTerminal(symbol)) {
                                size_t oldSize = firstSets[nonTerminal].size();
                                firstSets[nonTerminal].insert(firstSets[symbol].begin(), firstSets[symbol].end());

                                if (firstSets[symbol].count("ε") == 0)
                                    break;

                                if (firstSets[nonTerminal].size() > oldSize)
                                    changed = true;
                            } else {
                                if (firstSets[nonTerminal].insert(symbol).second)
                                    changed = true;
                                break;
                            }
                        }
                    }
                }
            }
        }
    }

    void computeFollowSets() {
        followSets[begin(productions)->first].insert("$"); // Start symbol gets '$'

        bool changed = true;
        while (changed) {
            changed = false;

            for (const auto& rule : productions) {
                string nonTerminal = rule.first;

                for (const auto& production : rule.second) {
                    for (size_t i = 0; i < production.size(); i++) {
                        string symbol = production[i];

                        if (!isTerminal(symbol)) {
                            set<string> followToAdd;

                            if (i + 1 < production.size()) {
                                string nextSymbol = production[i + 1];

                                if (isTerminal(nextSymbol)) {
                                    followToAdd.insert(nextSymbol);
                                } else {
                                    followToAdd.insert(firstSets[nextSymbol].begin(), firstSets[nextSymbol].end());
                                    followToAdd.erase("ε");

                                    if (firstSets[nextSymbol].count("ε")) {
                                        followToAdd.insert(followSets[nonTerminal].begin(), followSets[nonTerminal].end());
                                    }
                                }
                            } else {
                                followToAdd.insert(followSets[nonTerminal].begin(), followSets[nonTerminal].end());
                            }

                            size_t oldSize = followSets[symbol].size();
                            followSets[symbol].insert(followToAdd.begin(), followToAdd.end());

                            if (followSets[symbol].size() > oldSize)
                                changed = true;
                        }
                    }
                }
            }
        }
    }

    void constructParsingTable() {
        for (const auto& rule : productions) {
            string nonTerminal = rule.first;

            for (const auto& production : rule.second) {
                set<string> firstSetForProduction = computeFirstForProduction(production);

                for (const string& terminal : firstSetForProduction) {
                    if (terminal != "ε") {
                        parsingTable[{nonTerminal, terminal}] = production;
                    }
                }

                if (firstSetForProduction.count("ε")) {
                    for (const string& terminal : followSets[nonTerminal]) {
                        parsingTable[{nonTerminal, terminal}] = production;
                    }
                }
            }
        }
    }

    void printFirstSets() const {
        cout << "\nFIRST Sets:\n";
        for (const auto& entry : firstSets) {
            cout << "FIRST(" << entry.first << ") = { ";
            for (const auto& symbol : entry.second) {
                cout << symbol << " ";
            }
            cout << "}\n";
        }
    }

    void printFollowSets() const {
        cout << "\nFOLLOW Sets:\n";
        for (const auto& entry : followSets) {
            cout << "FOLLOW(" << entry.first << ") = { ";
            for (const auto& symbol : entry.second) {
                cout << symbol << " ";
            }
            cout << "}\n";
        }
    }

    /*
    void printParsingTable() const {
        cout << "\nLL(1) Parsing Table:\n";
        for (const auto& entry : parsingTable) {
            cout << "M[" << entry.first.first << ", " << entry.first.second << "] = ";
            for (const string& symbol : entry.second) {
                cout << symbol << " ";
            }
            cout << "\n";
        }
    }*/

    void printParsingTable() const {
        cout << "\n=========================== LL(1) Parsing Table ==================================\n";
    
        // Extract unique terminals for table columns
        set<string> terminals;
        for (const auto& entry : parsingTable) {
            terminals.insert(entry.first.second);
        }
    
        // Print table header
        cout << setw(10) << left << "NT \\ T";  // Header for non-terminals
        for (const string& terminal : terminals) {
            cout << setw(10) << left << terminal;
        }
        cout << "\n----------------------------------------------------------------------------------\n";
    
        // Print each row for non-terminals
        set<string> nonTerminals;
        for (const auto& entry : parsingTable) {
            nonTerminals.insert(entry.first.first);
        }
    
        for (const string& nonTerminal : nonTerminals) {
            cout << setw(10) << left << nonTerminal;  // Print non-terminal
    
            for (const string& terminal : terminals) {
                auto key = make_pair(nonTerminal, terminal);
                if (parsingTable.find(key) != parsingTable.end()) {
                    cout << setw(10) << left;
                    for (const string& symbol : parsingTable.at(key)) {
                        cout << symbol << " ";
                    }
                } else {
                    cout << setw(10) << left << "-";  // Empty cell
                }
            }
            cout << "\n";
        }
    
        cout << "==================================================================================\n";
    }
 

    private:
    bool isTerminal(const string& symbol) const {
        return !(productions.count(symbol));
    }

    set<string> computeFirstForProduction(const vector<string>& production) {
        set<string> result;

        for (const string& symbol : production) {
            if (isTerminal(symbol)) {
                result.insert(symbol);
                break;
            } else {
                result.insert(firstSets[symbol].begin(), firstSets[symbol].end());

                if (firstSets[symbol].count("ε") == 0)
                    break;
            }
        }

        return result;
    }
};

