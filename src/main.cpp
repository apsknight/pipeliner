/**
 * Computer Organisation and Architecture (CS3L002) Assignment
 * @author: Aman Pratp Singh <aps10@iitbbs.ac.in>
 * Main Entrypoint module.
 */

#include <bits/stdc++.h>
using namespace std;

const vector<string> instructions{"MOV", "LOAD", "STORE", "ADD", "SUB", "OR", "HLT", "NOP"};
int line = 1;
vector<string> program;
vector<int> waitMap(33, 0);
bool resolve = false;
void perror(string msg) {
    // Print error on STDOUT.
    fprintf(stderr, "\033[1;31mError at Line %i: %s\033[0m\n", line, msg.c_str());
    return;
}

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
        // perror("Register \""+ s + "\" is not a valid register(R1-R32).");
        return false;
    }
    if (s.length() == 2 && (s[1] < '1' || s[1] > '9')) {
        // perror("Register \""+ s + "\" is not a valid register(R1-R32).");
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
        // perror("\""+ s + "\" is not a valid Immediate Value.");
        return false;       
    }
    for(int j = 1; j < s.length(); j++) {
        char c = s[j];
        if (c < '0' || c > '9') {
            // perror("\""+ s + "\" is not a valid Immediate Value.");
            return false;     
        }
    }

    return true;
}

string register_from_offset(string s) {
    int braceIdx;
    for(int i = 2; i < s.length() - 1; i++) {
        if (s[i] == '(') {
            braceIdx = i;
            break;
        }
    }

    string reg = "";
    for(int i = braceIdx+1; i < s.length() - 1; i++) reg += s[i];

    return reg;
}

bool check_offset(string s) {
    if (!s.size()) return false;
    if (s[s.length() - 1] != ')') return false;
    if (s[0] != '#') return false;
    bool brace = false;
    int braceIdx;
    for(int i = 2; i < s.length() - 1; i++) {
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
    for(int i = braceIdx+1; i < s.length() - 1; i++) reg += s[i];

    return check_immediate(imd) && check_register(reg);
}

bool check_command(vector<string> args) {
    if (!args.size()) return false; // Empty Command

    string instr = args[0];

    if (find(instructions.begin(), instructions.end(), instr) == instructions.end()) {
        perror("Instruction \""+ instr + "\" not found in ISA.");
        return false;
    }

    if (instr == instructions[0]) { // MOV
        if (args.size() != 3) {
            perror("Instruction \""+ instr + "\" expects 2 arguments, " +  (char)(args.size() - 1 + '0') + " provided.");
            return false;
        }
        if (!check_register(args[1])) {
            perror("Register \""+ args[1] + "\" is not a valid register(R1-R32).");
            return false;
        }
        if (!check_register(args[2])) {
            perror("Register \""+ args[2] + "\" is not a valid register(R1-R32).");
            return false;
        }
    }
    else if (instr == instructions[1]) { // LOAD
        if (args.size() != 3) {
            perror("Instruction \""+ instr + "\" expects 2 arguments, " +  (char)(args.size() - 1 + '0') + " provided.");
            return false;
        }
        if (!check_register(args[1])) {
            perror("Register \""+ args[1] + "\" is not a valid register(R1-R32).");
            return false;
        }
        if (!check_offset(args[2])) {
            perror("\""+ args[2] + "\" is not a valid Offset Value.");
            return false;
        }
    }
    else if (instr == instructions[2]) { // STORE
        if (args.size() != 3) {
            perror("Instruction \""+ instr + "\" expects 2 arguments, " +  (char)(args.size() - 1 + '0') + " provided.");
            return false;
        }   
        if (!check_register(args[1])) {
            perror("Register \""+ args[1] + "\" is not a valid register(R1-R32).");
            return false;
        }
        if (!check_offset(args[2])) {
            perror("\""+ args[2] + "\" is not a valid Offset Value.");
            return false;
        }
    }
    else if (instr == instructions[3]) { // ADD
        if (args.size() != 4) {
            perror("Instruction \""+ instr + "\" expects 3 arguments, " +  (char)(args.size() - 1 + '0') + " provided.");
            return false;
        }
        if (!check_register(args[1])) {
            perror("Register \""+ args[1] + "\" is not a valid register(R1-R32).");
            return false;
        }
        if (!check_register(args[2])) {
            perror("Register \""+ args[2] + "\" is not a valid register(R1-R32).");
            return false;
        }
        if (!check_register(args[3]) && !check_immediate(args[3])) {
            perror("\""+ args[2] + "\" is not a valid register or Immediate Value.");
            return false;
        }
    }
    else if (instr == instructions[4]) { // SUB
        if (args.size() != 4) {
            perror("Instruction \""+ instr + "\" expects 3 arguments, " +  (char)(args.size() - 1 + '0') + " provided.");
            return false;
        }
        if (!check_register(args[1])) {
            perror("Register \""+ args[1] + "\" is not a valid register(R1-R32).");
            return false;
        }
        if (!check_register(args[2])) {
            perror("Register \""+ args[2] + "\" is not a valid register(R1-R32).");
            return false;
        }
        if (!check_register(args[3]) && !check_immediate(args[3])) {
            perror("\""+ args[2] + "\" is not a valid register or Immediate Value.");
            return false;
        }
    }
    else if (instr == instructions[5]) { // OR
        if (args.size() != 4) {
            perror("Instruction \""+ instr + "\" expects 3 arguments, " +  (char)(args.size() - 1 + '0') + " provided.");
            return false;
        }
        if (!check_register(args[1])) {
            perror("Register \""+ args[1] + "\" is not a valid register(R1-R32).");
            return false;
        }
        if (!check_register(args[2])) {
            perror("Register \""+ args[2] + "\" is not a valid register(R1-R32).");
            return false;
        }
        if (!check_register(args[3]) && !check_immediate(args[3])) {
            perror("\""+ args[2] + "\" is not a valid register or Immediate Value.");
            return false;
        }
    }
    else if (instr == instructions[6]) { // HLT
        if (args.size() != 1) {
            perror("Instruction \""+ instr + "\" expects 0 arguments, " +  (char)(args.size() - 1 + '0') + " provided.");
            return false;
        }
    }
    else if (instr == instructions[7]) { // NOP
        if (args.size() != 1) {
            perror("Instruction \""+ instr + "\" expects 0 arguments, " +  (char)(args.size() - 1 + '0') + " provided.");
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

bool updateMap(vector<string> args) {
    for(int i = 1; i < 33; i++) {
        if (waitMap[i]) waitMap[i]--;
    }
    if (args[0] == instructions[6] || args[0] == instructions[7]) {
        return true;
    }
    else if(args[0] == instructions[0]) { // MOV
        int reg = register_id(args[2]);
        if (waitMap[reg]) {
            perror("Register " + args[2] + " is dependent on the instruction at Line " + (char)(line-4+waitMap[reg]+'0') + ".");
            if (resolve) {
                program.insert(program.end(), waitMap[reg], "NOP");
                line += waitMap[reg];
            }
            return false;
        }
        waitMap[register_id(args[1])] = 4;
    }
    else if(args[0] == instructions[1]) { // LOAD
        int reg = register_id(register_from_offset(args[2]));
        if (waitMap[reg]) {
            perror("Register " + args[2] + " is dependent on the instruction at Line " + (char)(line-4+waitMap[reg]+'0') + ".");
            if (resolve) {
                program.insert(program.end(), waitMap[reg], "NOP");
                line += waitMap[reg];
            }
            return false;
        }
        waitMap[register_id(args[1])] = 4;
    }
    else if(args[0] == instructions[2]) { // STORE
        int reg = register_id(args[1]);
        if (waitMap[reg]) {
            perror("Register " + args[1] + " is dependent on the instruction at Line " + (char)(line-4+waitMap[reg]+'0') + ".");
            if (resolve) {
                program.insert(program.end(), waitMap[reg], "NOP");
                line += waitMap[reg];
            }
            return false;
        }
        reg = register_id(register_from_offset(args[2]));
        if (waitMap[reg]) {
            perror("Register " + args[2] + " is dependent on the instruction at Line " + (char)(line-4+waitMap[reg]+'0') + ".");
            if (resolve) {
                program.insert(program.end(), waitMap[reg], "NOP");
                line += waitMap[reg];
            }
            return false;
        }
    }
    else if(args[0] == instructions[3]) { // ADD
        int reg = register_id(args[2]);
        if (waitMap[reg]) {
            perror("Register " + args[2] + " is dependent on the instruction at Line " + (char)(line-4+waitMap[reg]+'0') + ".");
            if (resolve) {
                program.insert(program.end(), waitMap[reg], "NOP");
                line += waitMap[reg];
            }
            return false;
        }
        if (check_register(args[3])) {
            reg = register_id(args[3]);
            if (waitMap[reg]) {
                perror("Register " + args[3] + " is dependent on the instruction at Line " + (char)(line-4+waitMap[reg]+'0') + ".");
                if (resolve) {
                    program.insert(program.end(), waitMap[reg], "NOP");
                    line += waitMap[reg];
                }
                return false;
            }
        }
        waitMap[register_id(args[1])] = 4;
    }
    else if(args[0] == instructions[4]) { // SUB
        int reg = register_id(args[2]);
        if (waitMap[reg]) {
            perror("Register " + args[2] + " is dependent on the instruction at Line " + (char)(line-4+waitMap[reg]+'0') + ".");
            if (resolve) {
                program.insert(program.end(), waitMap[reg], "NOP");
                line += waitMap[reg];
            }
            return false;
        }
        if (check_register(args[3])) {
            reg = register_id(args[3]);
            if (waitMap[reg]) {
                perror("Register " + args[3] + " is dependent on the instruction at Line " + (char)(line-4+waitMap[reg]+'0') + ".");
                if (resolve) {
                    program.insert(program.end(), waitMap[reg], "NOP");
                    line += waitMap[reg];
                }
                return false;
            }
        }
        waitMap[register_id(args[1])] = 4;
    }
    else if(args[0] == instructions[5]) { // OR
        int reg = register_id(args[2]);
        if (waitMap[reg]) {
            perror("Register " + args[2] + " is dependent on the instruction at Line " + (char)(line-4+waitMap[reg]+'0') + ".");
            if (resolve) {
                program.insert(program.end(), waitMap[reg], "NOP");
                line += waitMap[reg];
            }
            return false;
        }
        if (check_register(args[3])) {
            reg = register_id(args[3]);
            if (waitMap[reg]) {
                perror("Register " + args[3] + " is dependent on the instruction at Line " + (char)(line-4+waitMap[reg]+'0') + ".");
                if (resolve) {
                    program.insert(program.end(), waitMap[reg], "NOP");
                    line += waitMap[reg];
                }
                return false;
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
            fprintf(stderr, "Unable to redirect STDIN from given input file.\n");
            exit(1);
        }
    }

    string command;
    while(getline(cin, command)) {
        if (!command.length()) continue; // Empty command

        vector<string> args = parse_command(command); // Parse command.

        // for(auto s : args) cout << s << endl;

        assert(check_command(args)); // Check if command is valid.

        updateMap(args);
        program.push_back(command);
        line++;

        if (args[0] == "HLT") break;
    }

    return 0;
}