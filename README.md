# Context-Free Grammar (CFG) Parser

## Objective
This project processes a **Context-Free Grammar (CFG)** and applies transformations (left-factoring and left-revursion) to prepare it for LL(1) parsing. The key functionalities include:

- **Parsing the CFG from a file**
- **Left Factoring** to remove common prefixes in production rules
- **Left Recursion Removal** to eliminate direct left recursion
- **Computing FIRST & FOLLOW Sets**
- **Constructing an LL(1) Parsing Table**

## Code Approach

### 1. Parsing the CFG
- Reads a CFG from a file and stores it as a **map**.
- Each **non-terminal** is mapped to a list of **production rules**.

### 2. Left Factoring
- Identifies common prefixes in production rules.
- Introduces new non-terminals to factor out shared prefixes.

#### Example Before Left Factoring
```plaintext
S -> A a | A b  
A -> c d | c e | f  
```
#### After Left Factoring
```plaintext
S -> A a | A b  
A -> c A' | f  
A' -> d | e  
```

### 3. Left Recursion Removal
- Detects **direct left recursion** in production rules.
- Rewrites rules to eliminate left recursion using new non-terminals.

#### Example Before Left Recursion Removal
```plaintext
A -> A c | d  
```
#### After Left Recursion Removal
```plaintext
A -> d A'  
A' -> c A' | ε  
```

### 4. Computing FIRST & FOLLOW Sets
- Computes **FIRST sets**, which contain the first symbols of derivations.
- Computes **FOLLOW sets**, which track symbols that can appear after a non-terminal.

### 5. LL(1) Parsing Table
- Constructs an **LL(1) table** based on the CFG.
- Identifies conflicts and ensures the grammar is **LL(1) compatible**.

## How to Compile and Run

### Compilation (Using `g++`)
```sh
g++ -o parser main.cpp
```

### Running the Program
```sh
./parser
```


