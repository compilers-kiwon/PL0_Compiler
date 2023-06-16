#include    <cctype>
#include    <cstdio>
#include    <cstdlib>
#include    <map>
#include    <memory>
#include    <string>
#include    <utility>
#include    <vector>
#include    <iostream>

#include    "main.h"
#include    "lexer.h"

static int  cur_tok;
static int  numVal;
static std::string  str;

static int  getTok(void);

int getNextTok(void) { return (cur_tok=getTok()); }
std::string getTokStr(void) { return str; }
int getTokNumVal(void) { return numVal; }

static std::map<std::string,int>  keywordTable = {
    {"begin",tok_begin},{"end",tok_end},{"if",tok_if},{"then",tok_then},
    {"while",tok_while},{"do",tok_do},{"return",tok_ret},
    {"function",tok_func},{"var",tok_var},{"const",tok_const},
    {"odd",tok_odd},{"write",tok_write},{"writeln",tok_writeln},
};

static std::map<std::string,int> opSymTable = {
    {"+",tok_plus},{"-",tok_minus},{"*",tok_mult},{"/",tok_div},
    {"(",tok_lparen},{")",tok_rparen},{"=",tok_equal},{"<",tok_less},
    {">",tok_greater},{"<>",tok_notequal},{"<=",tok_lessequal},
    {">=",tok_greaterequal},{",",tok_comma},{".",tok_period},
    {";",tok_semicolon},{":=",tok_assign}
};

static int  getNextChar(void){ return fgetc(src); }

/// gettok - Return the next token from source file.
static int  getTok(void)
{
    static int  lastChar = ' ';

    // Skip any whitespace.
    while (isspace(lastChar))
        lastChar = getNextChar();

    if (isalpha(lastChar)) {    // identifier: [a-zA-Z][a-zA-Z0-9]*
        str = lastChar;
        while (isalnum((lastChar=getNextChar())))
            str += lastChar;
        
        if ( keywordTable.find(str) != keywordTable.end() )
        {
            return  keywordTable[str];
        }

        return  tok_id;
    }

    if (isdigit(lastChar)) {    // Number: [0-9]+
        str.clear();
        do {
            str += lastChar;
            lastChar = getNextChar();
        } while (isdigit(lastChar));

        numVal = atoi(str.c_str());
        return  tok_num;
    }

    if (lastChar == '#') {
        // Comment until end of line.
        do {
            lastChar = getNextChar();
        } while (lastChar!=EOF && lastChar!='\n' && lastChar!='\r');

        if (lastChar != EOF) {
            return  getTok();
        }
    }

    // Check for end of file.  Don't eat the EOF.
    if (lastChar == EOF)
        return  tok_eof;
    
    str = lastChar;
    lastChar = getNextChar();

    if ((str=="<" && (lastChar=='>' || lastChar=='='))
            || ((str==">" || str==":") && lastChar=='=') ) {
        str += lastChar;
        lastChar = getNextChar();
    }

    if (opSymTable.find(str) != opSymTable.end() ) {
        return  opSymTable[str];
    }

    return  tok_none;
}