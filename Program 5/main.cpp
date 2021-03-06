//
//  main.cpp
//  Program 5
//  CIS 310 
//  Created by Avery Girven on 3/26/21.
//

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <iterator>
#include <sstream>
#include <map>
#include "non_MRI_table.cpp"
#include "MRI_table.cpp"
#include "instructions.cpp"
using namespace std;

class assembler{
private:
    string line;
    string label;
    string instruction;
    string operand;
    string operand_type;
    string comment;
    string op1;
    string op2;
    string binary_op;
    string code;
    string instru;
    bool error;
    int LC;
    map<string,string> symtab;
    map<string,string>::iterator it;
public:
    instructions f[17];
    
    void create_table();          //create table
    string search_table();      // search through table
    int hex_to_dec(string hex);  //convert hexadecimal to decimal
    string dec_to_hex(int dec);  // convert address to hex
    void partition();           // break up line into columns
    void sep_operands();        // break apart operands
    void first_pass(ifstream &myfile,ofstream &output); // first pass
    void second_pass(ifstream &input,ofstream &output);   // second pass
    void extract_label();   // extract label
    void extract_operand(); // extract operand
    void extract_instruction(); // extract instruction
    void extract_comment(); // extract comment
    void trim_whitespaces(); // trim white spaces
    string add_zeros(string hex); // pretty up the addresses for output
    bool search_psuedo(); // table look up for psuedo instructions
    bool search_MRI(); // table look up for MRI
    bool search_non_MRI();  // table look up for non-MRI
    void Operation();       //execute operation
    void print(ofstream &output, ofstream &symtab); // print table
    
};

// takes a hexadecimal string in and returns the decimal equivelant
int assembler::hex_to_dec(string op){
    int size = static_cast<int>(op.size());
    int decimal = 0;
    int base = 1;
        for(int i = size; i >= 0;i--){
            if(op[i] == '$'){
                op.erase(i);
            }
            if(op[i] >= '0' && op[i] <= '9'){
                decimal += (op[i] - 48) * base;
                base = base*16;
            }
            else if(op[i] >= 'A' && op[i] <= 'F'){
                decimal += (op[i] - 55) * base;
                base = base*16;
            }
        }
    return decimal;
}

// takes in a decimal and returns the hexadecimal string equivelant
string assembler::dec_to_hex(int dec){
    string hex;
    int i = 0;
    while (dec != 0) {
        int temp = 0;
        temp = dec % 16;
        if (temp < 10) {
            hex.insert(i,1,temp + 48);
            i++;
        }
        else {
            hex.insert(i,1,temp + 55);
            i++;
        }
        dec = dec / 16;
    }
    reverse(hex.begin(),hex.end());
    return hex;
}

void assembler::create_table(){
    f[0] = {"MOVE#DO","303C",4};
    f[1] = {"MOVE#$","33FC",8};
    f[2] = {"MOVE$DO","3039",6};
    f[3] = {"MOVEDO$","33C0",6};
    f[4] = {"ADDI#DO","0640",4};
    f[5] = {"ADDI#$","0679",8};
    f[6] = {"ADD$DO","DO79",6};
    f[7] = {"ADDDO$","D179",6};
    f[8] = {"SUBI#DO","0440",4};
    f[9] = {"SUBI#$","0479",8};
    f[10] = {"SUB$DO","9079",6};
    f[11] = {"SUBDO$","9179",6};
    f[12] = {"TRAP#","4E40",2};
    f[13] = {"TRAP#1","4E41",2};
    f[14] = {"TRAP#2","4E42",2};
    f[15].direct = "ORG";
    f[16].direct = "END";
}

// trim whitespaces from string
void assembler::trim_whitespaces(){
    label.erase(remove(label.begin(),label.end(),' '),label.end());
    instruction.erase(remove(instruction.begin(),instruction.end(),' '),instruction.end());
    operand.erase(remove(operand.begin(),operand.end(),' '),operand.end());
    comment.erase(remove(comment.begin(),comment.end(),' '),comment.end());
}

// parse line starting with comment
void assembler::partition(){
    extract_comment(); // pull comments first
    extract_operand(); // pull opernads second
    extract_instruction(); // pull instructions third
    extract_label(); // pull labels lastly
    trim_whitespaces(); // trim whites spaces
    binary_op = instruction; // make binary operation
    binary_op += operand_type;
}

void assembler::extract_comment(){
    for(int i = 0; i <line.size();i++){
        if(line[i] == ';'){ // line has a comment
            comment = line.substr(i+1,line.size()); // pull comment out
            line.erase(i,line.size());      // erase comment from line
        }
        else
            comment = " ";
    }
}

void assembler::extract_operand(){
    op2 = "";
    operand_type = "";
    for(int i = 0; i < line.size();i++){
        if(line[i]== '$'||line[i]=='#'){
            operand = line.substr(i,line.size()); // pull it from line
            line.erase(i,line.size());          // erase from line
        }
        else if(line[i] == 'D' && line[i+1] == 'O'){
            operand = line.substr(i,line.size());   // pull it from line
            line.erase(i,line.size());          // erase it from line
        }
    }
    for(int i = 0; i < operand.size();i++){
        if(operand[i]== '$'||operand[i]=='#'){
            operand_type.push_back(operand[i]);
        }
        else if(operand[i] == 'D' & operand[i+1] == 'O'){
            operand_type.push_back(operand[i]);
            operand_type.push_back(operand[i+1]);
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
    instru = " ";
    instru.append(instruction);
    instru.append(operand);
}

// remaining line is a label
void assembler::extract_label(){
    label = line;       // assign line to label
    line.erase(0,line.size());  // erase it from line;
}

// manipulate operand into op1 and op2 respectively
void assembler::Operation(){
    for(int i = 0; i < operand.size();i++){
        if(operand[i] == ','){
            op2 = operand.substr(i+1,operand.size());
            operand.erase(i,operand.size());
        }
    }
    for(int i = 0;i < operand.size();i++){
        if(operand[i]=='$'){
            operand = operand.substr(i+1,operand.size());
        }
        else if(operand[i] == '#'){
            operand = operand.substr(i+1,operand.size());
            operand = dec_to_hex(stoi(operand));
        }
    }
    for(int i = 0;i < op2.size();i++){
        if(op2[i] == '$'){
            op2 = op2.substr(i+1,op2.size());
        }
        else if(op2[i] == '#'){
            op2 = op2.substr(i+1,op2.size());
            op2 = dec_to_hex(stoi(op2));
        }
    }
    op1 = operand;
    operand.append(op2);
}

//searches the table to find the binary_ops machine code
string assembler::search_table(){
    string code = " ";
    for(int i = 0;i < 17;i++){
        if(f[i].instr == binary_op){
            code = f[i].opcode;
            break;
        }
        else if(f[i].direct == binary_op){
            code = f[i].direct;
            break;
        }
    }
    return code;
}
// open and search psuedo 
bool assembler::search_psuedo(){
    bool present = false;
    string ln;
    ifstream pt;
    pt.open("psuedotab.txt");
    if(!pt){
        cout << "invalid file" << endl;
    }
    else{
        while(getline(pt,ln)){
            if(ln == instruction){
                present = true;
                break;
            }
            else
                present = false;
        }
    }
    pt.close();
    return present;
}
// open and search MRI table
bool assembler::search_MRI(){
    bool present = false;
    string ln;
    ifstream MRI;
    MRI.open("MRI.txt");
    if(!MRI){
        cout << "invalid file" << endl;
    }
    else{
        while(getline(MRI,ln)){
            if(ln == binary_op){
                present = true;
                break;
            }
            else
                present = false;
        }
    }
    MRI.close();
    return present;
}
// search open, and search non-MRI table
bool assembler::search_non_MRI(){
    bool present = false;
    string ln;
    ifstream non_MRI;
    non_MRI.open("non-MRI.txt");
    if(!non_MRI){
        cout << "invalid file" << endl;
    }
    else{
        while(getline(non_MRI,ln)){
            if(ln == binary_op){
                present = true;
                break;
            }
            else
                present = false;
        }
    }
    non_MRI.close();
    return present;
}

// performs the first pass on the program
void assembler::first_pass(ifstream &input,ofstream &output){
    int i = 0;
    LC = 0;
    while(getline(input,line)){
        operand_type = "";
        partition();
        if(label != ""){
            string hex = dec_to_hex(LC);
            hex = add_zeros(hex);
            symtab.insert(pair<string,string>(hex,label));
            LC += 1;
        }
        else{
            if(instruction == "ORG"){
                LC = hex_to_dec(operand); // set LC
            }
            else{
                if(instruction == "END"){
                    second_pass(input,output);
                }
                else{
                    LC += 1;
                }
            }
        }
        i++;
    }
    input.close();
}

// performs the second pass on the function
void assembler::second_pass(ifstream &input,ofstream &output){
    string hex;
    string mo;
    ifstream input2;
    input2.open("assembly.txt");
    LC = 0;
    while(getline(input2,line)){
        partition();
        Operation();
        if(search_psuedo()){
            if(instruction == "ORG"){
                LC = hex_to_dec(operand);
            }
            else{
                if(instruction == "END"){
                    break;
                }
                else{
                    LC = LC + 1;
                }
            }
        }
        else if(search_MRI()){
            mo = search_table();
            hex = dec_to_hex(LC);
            if(op1 == "DO"){
                operand = op2;
            }
            else if(op2 == "DO"){
                operand = op1;
            }
            output << setw(16) << setfill(' ') << add_zeros(hex);
            output << setw(20) << mo;
            output << setw(18) << operand;
            output << setw(17) << instru << endl;
 
            LC = LC + 1;
        }
        else if(search_non_MRI()){
            if(binary_op == "TRAP#"){
                instruction.append(operand_type);
                instruction.append(operand);
                binary_op = instruction;
                mo = search_table();
            }
            else{
                mo = search_table();
            }
            if(binary_op == "TRAP#"){
                operand = "-";
            }
            else if(binary_op == "TRAP#1"){
                operand = "-";
            }
            else if(binary_op == "TRAP#2"){
                operand = "-";
            }
            hex = dec_to_hex(LC);
            output << setw(16) << setfill(' ') << add_zeros(hex);
            output << setw(20)  << mo;
            output << setw(18) << operand;
            output << setw(17) << instru << endl;
            LC = LC + 1;
        }
        else{                // error in code
            output << "Error in code" << endl;
            break;
        }
    }
}

// recovers address size after converting back to hexadecimal form
string assembler::add_zeros(string hex){
    int num_zeros = 0;
    int hex_size = static_cast<int>(hex.size());
    num_zeros = 8 - hex_size; // calculate the number of zeros required
    hex.insert(0,num_zeros,'0');    // insert them
    return hex;
}

// formats the output for the symbol table and output.txt
void print_format(ofstream &output,ofstream &Symtab){
    output << left << setw(16) << "Address";   // output table format
    output << left << setw(20) << "Machine Code";
    output << left << setw(19) << "Operands";
    output << left << setw(15) << "Instructions" << endl;
    output << setfill('-') << setw(81) << "-" << endl;
    Symtab << " Label" << " | " << "LC" << endl;  // symtab format
    Symtab << "------------------" << endl;
}

// prints the map containing the pair for the table
void assembler::print(ofstream &output,ofstream &Symtab){
    for (it = symtab.begin(); it != symtab.end(); ++it){
        Symtab << "|" << (*it).second << " | " << (*it).first << "|" << endl;
    }
    Symtab << "------------------" << endl;
    
}

int main() {
    string line = " ";
    ifstream input;
    ofstream output;
    ofstream symbtab;
    input.open("assembly.txt");
    output.open("OutPut.txt");
    symbtab.open("SymTab.txt");
    if(!input){
        cout << "Invalid" << endl;
    }
    else{
        assembler a;
        a.create_table();
        print_format(output,symbtab);
        a.first_pass(input,output);
        a.print(output,symbtab);
    }
    return 0;
}
