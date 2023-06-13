#include    <cctype>
#include    <cstdio>
#include    <cstdlib>
#include    <map>
#include    <memory>
#include    <string>
#include    <utility>
#include    <vector>
#include    <iostream>

static int  errorNo;

int getNumOfErrors(void)
{
    return  errorNo;
}

int incNumOfErrors(void)
{
    return  ++errorNo;
}