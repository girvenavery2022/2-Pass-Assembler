//
//  main.cpp
//  Program 5
//
//  Created by Avery Girven on 3/26/21.
//

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <iterator>
#include <list>
#include <sstream>
#include <map>
#include "instructions.cpp"
using namespace std;

class assembler{
private:
    string line;
    string label;
    string instruction;
    string operand;
    string comment;
    int LC;
    int lineNum;
    map<int,string> symtab;
    map<int,string>::iterator it;
public:
    instructions f[17];
    assembler(string input){
        line = input;
        
    }
    void store_symbol_table(string symbol,int LC); // store symbol
    void check_symbol_table(string symbol,int LC);  // check symbol
    void create_table();
    string search_table(string line);
    int hex_to_dec(string hex);  //convert hexadecimal to decimal
    void partition();           // break up line into columns
    void first_pass();          // perfrom first pass
    void second_pass();         // perform second pass
    void extract_label();
    void extract_operand();
    void extract_instruction();
    void extract_comment();
    void trim_whitespaces();
    void print(ofstream &output); // print table
    
};
int assembler::hex_to_dec(string op){
    int size = static_cast<int>(op.size());
    int decimal = 0;
    int base = 1;
        for(int i = size; i >= 0;i--){ // iterate
            if(op[i] >= '0' && op[i] <= '9'){
                decimal += (op[i] - 48) * base; // subtract 48
                base = base*16; // multiple it by base
            }
            else if(op[i] >= 'A' && op[i] <= 'F'){
                decimal += (op[i] - 55) * base; // subtract 55
                base = base*16;     // multiple it by base
            }
        }
    return decimal;
}
void assembler::create_table(){
    f[0] = {"MOVE#D","303C",4};
    f[1] = {"MOVE#>","33FC",8};
    f[2] = {"MOVE<D","3039",6};
    f[3] = {"MOVED>","33C0",6};
    f[4] = {"ADDI#D","303C",4};
    f[5] = {"ADDI#>","33FC",8};
    f[6] = {"ADD<D","3039",6};
    f[7] = {"ADDD>","33C0",6};
    f[8] = {"SUBI#D","303C",4};
    f[9] = {"SUBI#>","33FC",8};
    f[10] = {"SUB<D","3039",6};
    f[11] = {"SUBD>","33C0",6};
    f[12] = {"TRAP#1","4E40",2};
    f[13] = {"TRAP#2","4E41",2};
    f[14] = {"TRAP#3","4E42",2};
    f[15].direct = "ORG";
    f[16].direct = "END";
}
void assembler::trim_whitespaces(){
    label.erase(remove(label.begin(),label.end(),' '),label.end());
    instruction.erase(remove(instruction.begin(),instruction.end(),' '),instruction.end());
    operand.erase(remove(operand.begin(),operand.end(),' '),operand.end());
    comment.erase(remove(comment.begin(),comment.end(),' '),comment.end());
}
void assembler::partition(){ // parse line starting with comment
    extract_comment();
    extract_operand();
    extract_instruction();
    extract_label();
    trim_whitespaces();
}
void assembler::extract_comment(){
    for(int i = 0; i <line.size();i++){ // iterate through line
        if(line[i] == ';'){ // line has a comment
            comment = line.substr(i+1,line.size()); // pull comment out
            line.erase(i,line.size());      // erase comment from line
        }
        else                // no comment
            comment = " ";
    }
}
void assembler::extract_operand(){
    for(int i = 0; i < line.size();i++){ // iterate through line
        if(line[i]== '$'||line[i]=='#'){ // line has an address or number
            operand = line.substr(i+1,line.size()); // pull them out
            line.erase(i,line.size());  // erase them from line
        }
        else if(line[i] == 'D' && line[i+1] == 'O'){ // uses DO register
            operand = line.substr(i,line.size());    // pull it from line
            line.erase(i,line.size());          // erase it from line
        }
    }
}
void assembler::extract_instruction(){
    for(int i = 0; i <line.size();i++){     // iterate through line
        if(line[i]=='M'&&line[i+1]=='O'&&line[i+2] =='V'&&line[i+3] == 'E'){ // line uses MOVE
            instruction = line.substr(i,line.size()); // pull it
            line.erase(i,line.size());      // erase it from line
        }
        else if(line[i]=='A'&&line[i+1]=='D'&&line[i+2] =='D'){ // line has ADD
            instruction = line.substr(i,line.size()); // pull it
            line.erase(i,line.size());  // erase it from line
        }
        else if(line[i]=='A'&&line[i+1]=='D'&&line[i+2]=='D'&&line[i+3] =='I'){ // line uses ADDI
            instruction = line.substr(i,line.size()); // pull it
            line.erase(i,line.size());  // erase it from line;
        }
        else if(line[i]=='S'&&line[i+1]=='U'&&line[i+2] =='B'){
            instruction = line.substr(i,line.size()); // pull it
            line.erase(i,line.size());  // erase it from line;
        }
        else if(line[i]=='S'&&line[i+1]=='U'&&line[i+2] =='B' && line[i+3] =='I'){ // line uses SUBI
            instruction = line.substr(i,line.size()); // pull it
            line.erase(i,line.size());  // erase it from line;
        }
        else if(line[i]=='O'&&line[i+1]=='R'&&line[i+2]=='G'){ // line has an ORG directive
            instruction = line.substr(i,line.size()); // pull it
            line.erase(i,line.size());  // erase it from line;
        }
        else if(line[i]=='E'&&line[i+1]=='N'&&line[i+2]=='D'){ // line has an END directive
            instruction = line.substr(i,line.size()); // pull it
            line.erase(i,line.size());  // erase it from line;
        }
        else if(line[i]=='T'&&line[i+1]=='R'&&line[i+2] =='A' && line[i+3] =='P'){ // line has TRAP
            instruction = line.substr(i,line.size()); // pull it
            line.erase(i,line.size());  // erase it from line;
        }
    }
}
void assembler::extract_label(){ // remaining line is a label
    label = line;       // assign line to label
    line.erase(0,line.size());  // erase it from line;
}

string assembler::search_table(string line){
    string code = " ";
    for(int i = 0;i < 17;i++){
        if(f[i].instr == line){
            code = f[i].opcode;
            break;
        }
        else if(f[i].direct == line){
            code = f[i].direct;
            break;
        }
    }
    return code;
}
void assembler::first_pass(){
    partition();
    if(label != ""){
        symtab.insert(pair<int,string>(LC,label));
        LC += 1;
    }
    else{
        if(instruction == "ORG"){
            LC = hex_to_dec(operand); // set LC
        }
        else{
            if(instruction == "END"){
                second_pass();
            }
            else{
                LC += 1;
            }
        }
    }
    
}
void assembler::second_pass(){
    cout << "not there yet" << endl;
}
void assembler::print(ofstream &output){
    output << left << setw(15) <<  "Address" <<      "     ";
    output << left << setw(15) << "Machine Code" <<  "     ";
    output << left << setw(15) << "Operands" <<      "     ";
    output << left << setw(15) << "Intstructions" << "     ";
    //cout << label << endl;
    //cout << instruction << endl;
    //cout << operand << endl;
    //cout << comment << endl;
    //cout << endl;
    for (it = symtab.begin(); it != symtab.end(); ++it)
    cout << (*it).second << " => " << (*it).first << endl;
}
int main() {
    string line = " ";
    ifstream input;
    ofstream output;
    input.open("assembly.txt");
    output.open("OutPut.txt");
    if(!input){
        cout << "Invalid" << endl;
    }
    else{
        int i = 0;
        while(getline(input,line)){;
        //getline(input,line);
        assembler a(line);
        a.create_table();
        a.first_pass();
        a.print(output);
            i++;
        }
    }
    return 0;
}
