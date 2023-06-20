#include    <cctype>
#include    <cstdio>
#include    <cstdlib>
#include    <map>
#include    <memory>
#include    <string>
#include    <utility>
#include    <vector>
#include    <iostream>

#include    "error.h"
#include    "parser.h"

static int  errorNo;

int getNumOfErrors(void)
{
    return  errorNo;
}

int incNumOfErrors(void)
{
    return  ++errorNo;
}

std::unique_ptr<AST>    printError(const char* str)
{
    std::cout<<str<<'\n';
    incNumOfErrors();
    
    return  nullptr;
}