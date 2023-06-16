#include    <cctype>
#include    <cstdio>
#include    <cstdlib>
#include    <map>
#include    <memory>
#include    <string>
#include    <utility>
#include    <vector>
#include    <iostream>

#include    "compile.h"
#include    "lexer.h"
#include    "error.h"
#include    "table.h"
#include    "parser.h"

int compile(void)
{
    token = getNextTok();
  
    auto P = parse();

    int num_of_errors = getNumOfErrors();

    if (num_of_errors!=0) {
        std::cout<<num_of_errors<<" errors!!\n";
    }

    return  num_of_errors<MIN_ERROR;
}