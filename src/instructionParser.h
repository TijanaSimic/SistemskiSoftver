#pragma once
#ifndef INSTRUCTIONPARSER_H
#define INSTRUCTIONPARSER_H
#include<string>
 
#include<vector>
#include<regex>
using namespace std;

struct Operand {
    string tipAdresiranja;
    int registar;
    string literal;
    string simbol;
    char lowHigh='/';
    

    Operand() {};
   
    
};

  
Operand* tipAdresiranja(string instrukcija,string operand);
 void pronadjiLiteral(string operand, Operand *op);
void pronadjiRegistar(string operand, Operand *op);

 





#endif