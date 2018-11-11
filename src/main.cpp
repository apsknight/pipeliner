/**
 * Computer Organisation and Architecture (CS3L002) Assignment
 * @author: Aman Pratp Singh <aps10@iitbbs.ac.in>
 * Main Entrypoint module.
 */

#include <bits/stdc++.h>
using namespace std;

#define MEMORY_CYCLE 3

const vector<string> instructions{"MOV", "LOAD", "STORE", "ADD", "SUB", "OR", "HLT", "NOP"};
int line = 1;
vector<string> program;
vector<int> memory;
vector<int> waitMap(33, 0);
map<string, int> storeMap;
bool resolve = false;
ofstream opfile;

class Error {
private:
    static void perror(string msg) {
        // Print error on STDOUT.
        fprintf(stderr, "\033[1;31mError at Line %i: %s\033[0m\n", line, msg.c_str());
        return;
    }

public:
    static void instruction_not_found(string instr) {
        perror("Instruction \""+ instr + "\" not found in ISA.");
    }

    static void unexpected_args(string instr, int expected, int provided) {
        perror("Instruction \""+ instr + "\" expects " + to_string(expected) + 
                " arguments, " +  to_string(provided) + " provided.");
    }

    static void invalid_register(string R) {
        perror("Register \""+ R + "\" is not a valid register(R1-R32).");
    }
    
    static void invalid_offset(string val) {
        perror("\""+ val + "\" is not a valid Offset Value.");
    }

    static void dependent(string R, int line) {
        perror(R + " is dependent on the instruction at Line " + to_string(line) + ".");
    }
};
vector<string> parse_command(string command) {
    string temp = "";
    vector<string> res;

    for(auto c : command) {
        if (c == ' ' || c == ',') {
            if (temp.length()) {
                res.push_back(temp);
                temp = "";
            }
        }
        else {
            temp += c;
        }
    }

    if (temp.length()) res.push_back(temp);

    return res;
}

bool check_register(string s) {
    if (!s.size()) return false;
    char R = s[0];
    if (s.length() > 3 || R != 'R') {
        return false;
    }
    if (s.length() == 2 && (s[1] < '1' || s[1] > '9')) {
        return false;
    }
    if (s.length() == 3) {
        int num = (s[1] - '0')*10 + (s[2]-'0');

        if (num > 32 || num < 1) return false;
    }

    return true;
}

bool check_immediate(string s) {
    if (!s.size()) return false;
    char R = s[0];
    if (R != '#') {
        return false;       
    }
    for(unsigned int j = 1; j < s.length(); j++) {
        char c = s[j];
        if (c < '0' || c > '9') {
            return false;     
        }
    }

    return true;
}

string register_from_offset(string s) {
    int braceIdx;
    for(unsigned int i = 2; i < s.length() - 1; i++) {
        if (s[i] == '(') {
            braceIdx = i;
            break;
        }
    }

    string reg = "";
    for(unsigned int i = braceIdx+1; i < s.length() - 1; i++) reg += s[i];

    return reg;
}

bool check_offset(string s) {
    if (!s.size()) return false;
    if (s[s.length() - 1] != ')') return false;
    if (s[0] != '#') return false;
    bool brace = false;
    int braceIdx;
    for(unsigned int i = 2; i < s.length() - 1; i++) {
        if (s[i] == ')') return false;
        if (s[i] == '(') {
            if (!brace) {
                brace = true;
                braceIdx = i;
            }
            else return false;
        }
    }

    if (!brace) return false;

    string imd = "", reg = "";

    for(int i = 0; i < braceIdx; i++) imd += s[i];
    for(unsigned int i = braceIdx+1; i < s.length() - 1; i++) reg += s[i];

    return check_immediate(imd) && check_register(reg);
}

bool check_command(vector<string> args) {
    if (!args.size()) return false; // Empty Command

    string instr = args[0];

    if (find(instructions.begin(), instructions.end(), instr) == instructions.end()) {
        Error::instruction_not_found(instr);
        return false;
    }

    if (instr == instructions[0]) { // MOV
        if (args.size() != 3) {
            Error::unexpected_args(instr, 2, (args.size() - 1));
            return false;
        }
        if (!check_register(args[1])) {
            Error::invalid_register(args[1]);
            return false;
        }
        if (!check_register(args[2])) {
            Error::invalid_register(args[2]);
            return false;
        }
    }
    else if (instr == instructions[1]) { // LOAD
        if (args.size() != 3) {
            Error::unexpected_args(instr, 2, (args.size() - 1));
            return false;
        }
        if (!check_register(args[1])) {
            Error::invalid_register(args[1]);
            return false;
        }
        if (!check_offset(args[2])) {
            Error::invalid_offset(args[2]);
            return false;
        }
    }
    else if (instr == instructions[2]) { // STORE
        if (args.size() != 3) {
            Error::unexpected_args(instr, 2, (args.size() - 1));
            return false;
        }   
        if (!check_register(args[1])) {
            Error::invalid_register(args[1]);
            return false;
        }
        if (!check_offset(args[2])) {
            Error::invalid_offset(args[2]);
            return false;
        }
    }
    else if (instr == instructions[3]) { // ADD
        if (args.size() != 4) {
            Error::unexpected_args(instr, 3, (args.size() - 1));
            return false;
        }
        if (!check_register(args[1])) {
            Error::invalid_register(args[1]);
            return false;
        }
        if (!check_register(args[2])) {
            Error::invalid_register(args[2]);
            return false;
        }
        if (!check_register(args[3]) && !check_immediate(args[3])) {
            Error::invalid_register(args[3]);
            Error::invalid_offset(args[3]);
            return false;
        }
    }
    else if (instr == instructions[4]) { // SUB
        if (args.size() != 4) {
            Error::unexpected_args(instr, 3, (args.size() - 1));
            return false;
        }
        if (!check_register(args[1])) {
            Error::invalid_register(args[1]);
            return false;
        }
        if (!check_register(args[2])) {
            Error::invalid_register(args[2]);
            return false;
        }
        if (!check_register(args[3]) && !check_immediate(args[3])) {
            Error::invalid_register(args[3]);
            Error::invalid_offset(args[3]);
            return false;
        }
    }
    else if (instr == instructions[5]) { // OR
        if (args.size() != 4) {
            Error::unexpected_args(instr, 3, (args.size() - 1));
            return false;
        }
        if (!check_register(args[1])) {
            Error::invalid_register(args[1]);
            return false;
        }
        if (!check_register(args[2])) {
            Error::invalid_register(args[2]);
            return false;
        }
        if (!check_register(args[3]) && !check_immediate(args[3])) {
            Error::invalid_register(args[3]);
            Error::invalid_offset(args[3]);
            return false;
        }
    }
    else if (instr == instructions[6]) { // HLT
        if (args.size() != 1) {
            Error::unexpected_args(instr, 0, (args.size() - 1));
            return false;
        }
    }
    else if (instr == instructions[7]) { // NOP
        if (args.size() != 1) {
            Error::unexpected_args(instr, 0, (args.size() - 1));
            return false;
        }
    }
    return true;
}


int register_id(string s) {
    if (!check_register(s)) return -1;

    int num = 0;

    if (s.length() == 3) {
        num = (s[1] - '0')*10 + (s[2]-'0');
    }
    else if (s.length() == 2) {
        num = (s[1] - '0');
    }

    return num;
}

void resolve_dependency(int steps) {
    program.insert(program.end(), steps, "NOP");
    line += steps;
    for(unsigned int i = 1; i < 33; i++) {
        if (waitMap[i]) waitMap[i] = 0;
    }
    cout << "\033[1;32m" << "Quickly resolving dependecy by inserting " << steps << 
            " NOP instructions." << "\033[0m\n";

    return;
}

bool updateMap(vector<string> args) {
    for(unsigned int i = 1; i < 33; i++) {
        if (waitMap[i]) waitMap[i]--;
    }
    for(auto& it : storeMap) {
        it.second--;
        it.second = max(it.second, 0);
    }
    if (args[0] == instructions[6] || args[0] == instructions[7]) {
        return true;
    }
    else if(args[0] == instructions[0]) { // MOV
        int reg = register_id(args[2]);
        if (waitMap[reg]) {
            waitMap[register_id(args[1])] = 4;
            Error::dependent(args[2], (line-4+waitMap[reg]));

            if (resolve) {
                resolve_dependency(waitMap[reg]);
            }
            else {
                waitMap[register_id(args[1])] = 4;
                return false;
            }
        }
        waitMap[register_id(args[1])] = 4;
    }
    else if(args[0] == instructions[1]) { // LOAD
        int reg = register_id(register_from_offset(args[2]));
        if (waitMap[reg]) {
            // waitMap[register_id(args[1])] = 4;
            Error::dependent(args[2], (line-4+waitMap[reg]));
            if (resolve) {
                resolve_dependency(waitMap[reg]);
            }
            else {
                waitMap[register_id(args[1])] = 4;
                return false;
            }
        }
        if (storeMap[args[2]]) {
            Error::dependent(args[2], (line-3+storeMap[args[2]]));
            if (resolve) {
                resolve_dependency(storeMap[args[2]]);
            }
            else {
                waitMap[register_id(args[1])] = 4;
                return false;
            }
        }
        waitMap[register_id(args[1])] = 4;
        memory.push_back(line);
    }
    else if(args[0] == instructions[2]) { // STORE
        int reg = register_id(args[1]);
        if (waitMap[reg]) {
            Error::dependent(args[1], (line-4+waitMap[reg]));
            if (resolve) {
                resolve_dependency(waitMap[reg]);
            }
            else {
                waitMap[register_id(args[1])] = 4;
                return false;
            }
        }
        reg = register_id(register_from_offset(args[2]));
        if (waitMap[reg]) {
            Error::dependent(args[2], (line-4+waitMap[reg]));
            if (resolve) {
                resolve_dependency(waitMap[reg]);
            }
            else {
                waitMap[register_id(args[1])] = 4;
                return false;
            }
        }
        memory.push_back(line);
        storeMap[args[2]] = 3;
    }
    else if(args[0] == instructions[3]) { // ADD
        int reg = register_id(args[2]);
        if (waitMap[reg]) {
            Error::dependent(args[2], (line-4+waitMap[reg]));
            if (resolve) {
                resolve_dependency(waitMap[reg]);
            }
            else {
                waitMap[register_id(args[1])] = 4;
                return false;
            }
        }
        if (check_register(args[3])) {
            reg = register_id(args[3]);
            if (waitMap[reg]) {
                Error::dependent(args[3], (line-4+waitMap[reg]));
                if (resolve) {
                    resolve_dependency(waitMap[reg]);
                }
                else {
                    waitMap[register_id(args[1])] = 4;
                    return false;
                }
            }
        }
        waitMap[register_id(args[1])] = 4;
    }
    else if(args[0] == instructions[4]) { // SUB
        int reg = register_id(args[2]);
        if (waitMap[reg]) {
            Error::dependent(args[2], (line-4+waitMap[reg]));
            return false;
        }
        if (check_register(args[3])) {
            reg = register_id(args[3]);
            if (waitMap[reg]) {
                Error::dependent(args[3], (line-4+waitMap[reg]));
                if (resolve) {
                    resolve_dependency(waitMap[reg]);
                }
                else {
                    waitMap[register_id(args[1])] = 4;
                    return false;
                }
            }
        }
        waitMap[register_id(args[1])] = 4;
    }
    else if(args[0] == instructions[5]) { // OR
        int reg = register_id(args[2]);
        if (waitMap[reg]) {
            Error::dependent(args[2], (line-4+waitMap[reg]));
            if (resolve) {
                resolve_dependency(waitMap[reg]);
            }
            else {
                waitMap[register_id(args[1])] = 4;
                return false;
            }
        }
        if (check_register(args[3])) {
            reg = register_id(args[3]);
            if (waitMap[reg]) {
                Error::dependent(args[3], (line-4+waitMap[reg]));
                if (resolve) {
                    resolve_dependency(waitMap[reg]);
                }
                else {
                    waitMap[register_id(args[1])] = 4;
                    return false;
                }
            }
        }
        waitMap[register_id(args[1])] = 4;
    }

    return true;
}

int main(int argc, char** argv) {
    /* Check if program to be run as shell or Input has to be streamed from file. 
        If an input file is given as CLA, redirect STDIN to this file. */
    if (argc == 2) {
        if (freopen(argv[1], "r", stdin) == NULL) {
            fprintf(stderr, "\033[1;31mUnable to redirect STDIN from given input file.\033[0m\n");
            exit(1);
        }
    }
    else if (argc == 3) {
        if (strcmp(argv[1], "--resolve") || strcmp(argv[1], "-r")) {
            resolve = true;
        }
        else {
            fprintf(stderr, "\033[1;31mUnknown arguments provided, please check docs.\033[0m\n"); 
            exit(1);           
        }
        if (freopen(argv[2], "r", stdin) == NULL) {
            fprintf(stderr, "\033[1;31mUnable to redirect STDIN from given input file.\033[0m\n");
            exit(1);
        }
        // if () {
        //     fprintf(stderr, "\033[1;31mUnable to open file for writing resolved program.\033[0m\n");
        //     exit(1);
        // }
        opfile.open("output.txt");
        
    }
    else {
        fprintf(stderr, "\033[1;31mError: Expected program file as Command line argument.\033[0m\n");
        exit(1);
    }

    string command;
    while(getline(cin, command)) {
        if (!command.length()) continue; // Empty command

        vector<string> args = parse_command(command); // Parse command.

        // for(auto s : args) cout << s << endl;
        // if (!check_command(args)) cout << command << endl;
        assert(check_command(args)); // Check if command is valid.

        updateMap(args);

        program.push_back(command);
        line++;

        if (args[0] == "HLT") break;
    }

    if (resolve) {
        for(auto p : program) {
            opfile << p << endl;
        }
    }
    cout << "Total Amount of Memory Delay(clock cycles): " << memory.size() * MEMORY_CYCLE << endl;
    for(auto l : memory) {
        cout << "Memory Delay is present at Line " << l << "." << endl;
    }
    cout << "Total Clock Cycle Wasted due to Memory Delay: " << memory.size() * (MEMORY_CYCLE-1) * 3 << endl;
    cout << "Pipeliner exiting succesfully!" << endl;
    return 0;
}