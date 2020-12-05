#include<iostream>
#include<string>
#include<fstream>
#include<vector>
#include"instructionParser.h"
#include"symbolData.h"
#include"lexer.h"
#include"asembler.h"
#define REV(n) ((n<<24) | (((n>>16)<<24)>>16) |(((n<<16)>>24)<<16)|(n>>24))
#include<bits/stdc++.h>
#include<sstream>
#include<stdio.h>
using namespace std;

int main(int argc, char* argv[]) {


ifstream inputFile;
 
string line;

TextLexer textLexer=TextLexer();
vector<Symbol> tabelaSimbola;
Asembler asembler=Asembler(tabelaSimbola,textLexer, argv[2]);
 

inputFile.open(argv[1]);
 
if(inputFile.good()) {
while(getline(inputFile, line)) 
{
asembler.obradaLinijeKoda(line);
}
}
  

asembler.ispis();
 
  
}