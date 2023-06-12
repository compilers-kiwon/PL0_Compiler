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
#include    "compile.h"
#include    "codegen.h"

FILE*   src;

static int  open_source_file(const std::string&);
static int  close_source_file(void);

int main(void)
{
    std::string  file_name;

    std::cout<<"Enter source file name :\n>>";
    std::cin>>file_name;

    if( open_source_file(file_name) && compile() )
    {
        execute();
        close_source_file();
    }

    return  0;
}

static int  open_source_file(const std::string& file_name)
{
    src = fopen(file_name.c_str(),"r");
    return  (src!=NULL);
}

static int  close_source_file(void)
{
    fclose(src);
    return  0;
}