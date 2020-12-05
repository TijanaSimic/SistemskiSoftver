#include "instructionParser.h"
#include <iostream>

void pronadjiRegistar(string operand, Operand *op)
{
     if (operand[0] == '*')
        operand = operand.substr(1, operand.size());
    if (regex_match(operand, regex("^\\*{0,1}%r[0-7]$")))
    {

        operand.erase(0, operand.find_last_of('r') + 1);
    
        op->registar = stoi(operand);
     }
    else if (regex_match(operand, regex("\\*{0,1}\\S{0,}\\(*%r[0-7][lh]*\\)*$")))
    {
        if (operand.at(operand.find_last_of('r') + 2) == 'l')
        {
            op->lowHigh = 'l';
        }
        else if (operand.at(operand.find_last_of('r') + 2) == 'h')
            op->lowHigh = 'h';
        else
        {
            op->lowHigh = '/';
        }

        operand.erase(0, operand.find_last_of('r') + 1);
    
        op->registar = stoi(operand);
     }
    else if (regex_match(operand, regex(".*\\*{0,1}\\({0,1}%pc{1}[hl]{0,1}\\){0,1}")))
    {
        op->registar = 7;
    }
} 
void pronadjiLiteral(string operand, Operand *op)
{

    if (regex_match(operand, regex("\\*{0,1}\\${0,1}[+-]*0x[0-9abcdefABCDEF]+\\(%r[0-7]{1}[lh]{0,1}\\)")))

    {
        if (operand.find_first_of('(') != -1)
            operand.erase(operand.find_first_of('('));
        if (operand.find_first_of('*') != -1)
            operand.erase(operand.find_first_of('*'), 1);
        if (operand.find_first_of('$') != -1)
            operand.erase(operand.find_first_of('$'), 1);
        op->literal = operand;
    }
    else if (regex_match(operand, regex("\\*{0,1}\\${0,1}[+-]*\\d+\\(%r[0-7][lh]*\\)")))

    {
        if (operand.find_first_of('(') != -1)
            operand.erase(operand.find_first_of('('));
        if (operand.find_first_of('*') != -1)
            operand.erase(operand.find_first_of('*'), 1);
        if (operand.find_first_of('$') != -1)
            operand.erase(operand.find_first_of('$'), 1);

         op->literal = operand;
    }
    else if (regex_match(operand, regex("\\*{0,1}\\${0,1}[+-]*0x[0-9abcdefABCDEF]+")))

    {
        if (operand.find_first_of('*') != -1)
            operand.erase(operand.find_first_of('*'), 1);
        if (operand.find_first_of('$') != -1)
            operand.erase(operand.find_first_of('$'), 1);
        op->literal = operand;
    }
    else if (regex_match(operand, regex("\\*{0,1}\\${0,1}[+-]*\\d{1,}")))

    {    
        if (operand.find_first_of('*') != -1)
            operand.erase(operand.find_first_of('*'), 1);
        if (operand.find_first_of('$') != -1)
            operand.erase(operand.find_first_of('$'), 1);
   
        op->literal = operand;
    }
}
Operand *tipAdresiranja(string instrukcija, string operand)
{
    Operand *op = new Operand();
    try
    {
        if (regex_match(operand, regex("^\\*{0,1}%r[0-7]{1}[hl]{0,1}$")))
        {
            if (operand.at(0) != '*')
            {

                if (regex_match(instrukcija, regex("^(jmp)|(jne)|(jeq)|(jgt)[w]{0,1}$")))
                    throw "Pogresna adresiranja";
            }
            else
            {
                if (regex_match(instrukcija,regex("^(jmp)|(jne)|(jeq)|(jgt)[w]{0,1}$"))==false)
                throw "Pogresna adresiranja.";
                operand.erase(0, 1);
            }

            op->tipAdresiranja = "regDir";
            pronadjiRegistar(operand, op);
  
            return op;
        }
        else if (regex_match(operand, regex("^\\*{0,1}\\(%r[0-7]{1}[hl]{0,1}\\)$")))
        {
            if (operand.at(0) != '*')
            {

                if (regex_match(instrukcija, regex("^(jmp)|(jne)|(jeq)|(jgt)[w]{0,1}$")))
                    throw "Pogresna adresiranja";
            }
            else
            {
                if (regex_match(instrukcija,regex("^(jmp)|(jne)|(jeq)|(jgt)[w]{0,1}$"))==false)
                throw "Pogresna adresiranja.";
                operand.erase(0, 1);
            }
            op->tipAdresiranja = "regInd";
            pronadjiRegistar(operand, op);
            if(op->lowHigh!='/') throw "L i H moze stajati samo uz registarsko direktno adresiranje.";
 
            return op;
        }
        else if (regex_match(operand, regex("^\\*{0,1}[+-]*(\\d{1,}|0x[0-9abcdefABCDEF]{1,})\\(%r[0-7]{1}[hl]{0,1}\\)$")))
        {
            if (operand.at(0) != '*')
            {

                if (regex_match(instrukcija, regex("^(jmp)|(jne)|(jeq)|(jgt)[w]{0,1}$")))
                    throw "Pogresna adresiranja";
            }
            else
            {
                if (regex_match(instrukcija,regex("^(jmp)|(jne)|(jeq)|(jgt)[w]{0,1}$"))==false)
                throw "Pogresna adresiranja.";
                operand.erase(0, 1);
            }
            op->tipAdresiranja = "regIndPomL";
            pronadjiRegistar(operand, op);

            if(op->lowHigh!='/') throw "L i H moze stajati samo uz registarsko direktno adresiranje.";
            pronadjiLiteral(operand, op);
          
      
            return op;
        }  
        else if (regex_match(operand, regex("^\\*{0,1}\\S+\\(%r[0-7]{1}[hl]{0,1}\\)$")))
        {
            if (operand.at(0) != '*')
            {

                if (regex_match(instrukcija, regex("^(jmp)|(jne)|(jeq)|(jgt)[w]{0,1}$")))
                    throw "Pogresna adresiranja";
            }
            else
            {
                if (regex_match(instrukcija,regex("^(jmp)|(jne)|(jeq)|(jgt)[w]{0,1}$"))==false)
                throw "Pogresna adresiranja.";
                operand.erase(0, 1);
            }
            op->tipAdresiranja = "regIndPomS";
            pronadjiRegistar(operand, op);

            if(op->lowHigh!='/') throw "L i H moze stajati samo uz registarsko direktno adresiranje.";
            if (operand.at(0) == '*')
                operand.erase(0, 1);
            if (op->registar == 7)
                op->tipAdresiranja = "regIndPom";
            operand.erase(operand.find_last_of('('));
            op->simbol = operand;
         
            return op;
        } 
        else if (regex_match(operand, regex("^\\*{0,1}[+-]*(\\d{1,}|0x[0-9a-fA-F]{1,})\\({1}%pc[lh]*\\){1}$")))
        {
            throw "Adresiranja nisu odgovarajuca.";

        } //literal + pc

        else if (regex_match(operand, regex("^\\*{0,1}\\S+\\({1}%pc\\){1}$")))
        {
            if (operand.at(0) != '*')
            {

                if (regex_match(instrukcija, regex("^(jmp)|(jne)|(jeq)|(jgt)[w]{0,1}$")))
                    throw "Pogresna adresiranja";
            }
            else
            {
                if (regex_match(instrukcija,regex("^(jmp)|(jne)|(jeq)|(jgt)[w]{0,1}$"))==false)
                throw "Pogresna adresiranja.";
                operand.erase(0, 1);
            }
            op->tipAdresiranja = "regIndPom";
            pronadjiRegistar(operand, op);
            
            operand.erase(operand.find_last_of('('));
            op->simbol = operand;
 
            return op;
        } //pc simbol
        else if (regex_match(operand, regex("^\\*{0,1}\\${0,1}[+-]*(\\d{1,}|0x[0-9a-fA-F]{1,})$")))
        {
            if (instrukcija == "jmp" || instrukcija == "jne" || instrukcija == "jeq" || instrukcija == "jgt")
            {
                if (operand.at(0) == '*')
                    op->tipAdresiranja = "memL";
                else if (operand.at(0) == '$')
                    throw "Pogresno adresiranje.";
                else
                    op->tipAdresiranja = "immedL";
            }
            else
            {
                if (operand.at(0) == '$')
                    op->tipAdresiranja = "immedL";
                else if (operand.at(0) == '*')
                    throw "Pogresno adresiranje.";
                else
                    op->tipAdresiranja = "memL";
            }

            pronadjiLiteral(operand, op);
  
            return op;
        }

        else if (regex_match(operand, regex("^\\${0,1}\\*{0,1}\\S{1,}\\S*$")))
        {
            if (instrukcija == "jmp" || instrukcija == "JMP" || instrukcija == "jne" || instrukcija == "JNE" || instrukcija == "JEG" || instrukcija == "jeq" || instrukcija == "jgt" || instrukcija == "JGT")
            {
                if (operand.at(0) == '*')
                    op->tipAdresiranja = "mem";
                else if (operand.at(0) == '$')
                    throw "Pogresno adresiranje.";
                else
                    op->tipAdresiranja = "immed";
            }
            else
            {
                if (operand.at(0) == '$')
                    op->tipAdresiranja = "immed";
                else if (operand.at(0) == '*')
                    throw "Pogresno adresiranje.";
                else
                    op->tipAdresiranja = "mem";
            }

            if (operand.at(0) == '*' || operand.at(0) == '$')
                operand.erase(0, 1);
            op->simbol = operand;
   
            return op;
        }
        else
            throw "Pogresno adresiranje.";
    }

    catch (const char *e)
    {
        cout << e;
        exit(1);
    }

    return nullptr;
}
