#include    <cctype>
#include    <cstdio>
#include    <cstdlib>
#include    <map>
#include    <memory>
#include    <string>
#include    <utility>
#include    <vector>
#include    <iostream>

#include    "lexer.h"
#include    "parser.h"

int token;

int block(int pIndex)
{
    do {
        std::cout<<token<<","<<getTokStr()<<'\n';
        token = getNextTok();
    } while (token!=tok_eof && token!=tok_none);

    return  1;
}