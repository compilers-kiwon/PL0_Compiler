#ifndef __ERROR_H__
#define __ERROR_H__

#include    "parser.h"

extern int  getNumOfErrors(void);
extern int  incNumOfErrors(void);

extern std::unique_ptr<AST> printError(const char*);

#endif