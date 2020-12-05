#ifndef LEXER_H
#define LEXER_H
#pragma once

#include<iostream>
#include<string>
#include<regex>

using namespace std;

class TextLexer {

struct token {
    string type;
    string value;
};

public:

 
string obrisiKomentare(string line);

vector<string> tokeni(string line);


};


#endif