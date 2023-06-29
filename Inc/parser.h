#ifndef __PARSER_H__
#define __PARSER_H__

extern int  token;

enum {
  CONST = 0,
  VAR = 1,
  FUNC = 2,
  MAX_SYMBOL_TYPE = 3
};

/// AST - Base class for all expression nodes.
class AST {
public:
  virtual ~AST() = default;
};

/// ProgramAST
/// program ::= block '.'
class ProgramAST : public AST {
  std::unique_ptr<AST>  block;

public:
  ProgramAST(std::unique_ptr<AST> block) : block(std::move(block)) {}
};

/// BlockAST
/// block ::= declList statement
class BlockAST : public AST {
  std::unique_ptr<AST>  declList,statement;

public:
  BlockAST(std::unique_ptr<AST> declList,std::unique_ptr<AST> statement)
    : declList(std::move(declList)), statement(std::move(statement)) {}
};

/// DeclListAST
/// declList 
///    ::= <empty>
///    ::= declList decl
class DeclListAST : public AST {
  std::vector<std::unique_ptr<AST>>  declList;
public:
  DeclListAST(std::vector<std::unique_ptr<AST>> declList)
    : declList(std::move(declList)) {}
};

/// DeclAST
/// decl
///    ::= constDecl
///    ::= varDecl
///    ::= funcDecl
class DeclAST : public AST {
  std::unique_ptr<AST>  decl;

public:
  DeclAST(std::unique_ptr<AST> decl)
    : decl(std::move(decl)) {}
};

/// ConstDeclAST
/// constDecl ::= CONST numberList ';'
class ConstDeclAST : public AST {
  std::unique_ptr<AST> numberList;

public:
  ConstDeclAST(std::unique_ptr<AST> numberList)
    : numberList(std::move(numberList)) {}
};

/// NumberListAST
/// numberList
///    ::= IDENT EQ NUMBER
///    ::= numberList COMMA IDENT EQ NUMBER
class NumberListAST : public AST {
  std::string Name;
  float Val;
  std::unique_ptr<AST> numberList;

public:
  NumberListAST(std::string Name,float Val,std::unique_ptr<AST> numberList)
    : Name(Name),Val(Val),numberList(std::move(numberList)) {}
};

/// VarDeclAST
/// varDecl ::= VAR identList ';'
class VarDeclAST : public AST {
  std::unique_ptr<AST> identList;

public:
  VarDeclAST(std::unique_ptr<AST> identList)
    : identList(std::move(identList)) {}
};

/// IdentListAST
/// identList
///    ::= IDENT
///    ::= identList COMMA IDENT
class IdentListAST : public AST {
  std::string Name;
  std::unique_ptr<AST> identList;

public:
  IdentListAST(std::string Name,std::unique_ptr<AST> identList)
    : Name(Name),identList(std::move(identList)) {}
};

/// OptParListAST
/// optParList
///    ::= <empty>
///    ::= parList
class OptParListAST : public AST {
  std::unique_ptr<AST>  parList;

public:
  OptParListAST(std::unique_ptr<AST> parList)
    : parList(std::move(parList)) {}
};

/// ParListAST
/// parList
///    ::= IDENT
///    ::= parList COMMA IDENT
class ParListAST : public AST {
  std::vector<std::string>  parList;

public:
  ParListAST(std::vector<std::string> parList)
    : parList(std::move(parList)) {}
};

/// FuncDeclAST
/// funcDecl ::= FUNCTION IDENT '('  optParList ')' block ';'
class FuncDeclAST : public AST {
  std::string Name;
  std::unique_ptr<AST>  optParList,block;

public:
  FuncDeclAST(std::string Name,std::unique_ptr<AST> optParList,
    std::unique_ptr<AST> block) : Name(Name),
    optParList(std::move(optParList)), block(std::move(block)) {}
};

/// StatementAST
/// statement
///    ::= <empty>
///    ::= IDENT COLOEQ expression
///    ::= BEGINN statement stateList END
///    ::= IF condition THEN statement
///    ::= WHILE condition DO statement
///    ::= RETURN expression
///    ::= WRITE expression
///    ::= WRITELN
class StatementAST : public AST {
  int head_tok;
  std::string Name;
  std::unique_ptr<AST> expression;
  std::unique_ptr<AST> condition;
  std::unique_ptr<AST> statement;
  std::unique_ptr<AST> stateList;

public:
  StatementAST(int head_tok,std::string Name,std::unique_ptr<AST> expression,
    std::unique_ptr<AST> condition,std::unique_ptr<AST> statement,
    std::unique_ptr<AST> stateList) : head_tok(head_tok),Name(Name),
    expression(std::move(expression)),condition(std::move(condition)),
    statement(std::move(statement)),stateList(std::move(stateList)) {}
};

/// StateListAST
/// stateList
///    ::= <empty>
///    ::= stateList ';' statement
class StateListAST : public AST {
  std::unique_ptr<AST> stateList,statement;

public:
  StateListAST(std::unique_ptr<AST> stateList,
    std::unique_ptr<AST> statement) : stateList(std::move(stateList)),
    statement(std::move(statement)) {}
};

/// ConditionAST
/// condition
///    ::= ODD expression
///    ::= expression EQ expression
///    ::= expression NOTEQ expression
///    ::= expression LT expression
///    ::= expression GT expression
///    ::= expression LE expression
///    ::= expression GE expression
class ConditionAST : public AST {
  int op_tok;
  std::unique_ptr<AST> LHS,RHS;

public:
  ConditionAST(int op_tok,std::unique_ptr<AST> LHS,
    std::unique_ptr<AST> RHS) : op_tok(op_tok),
    LHS(std::move(LHS)),RHS(std::move(RHS)) {}
};

/// ExpressionAST
/// expression
///    ::= '-'  term termList
///    ::= term  termList
class ExpressionAST : public AST {
  int head_tok;
  std::unique_ptr<AST> term,termList;

public:
  ExpressionAST(int head_tok,std::unique_ptr<AST> term,
    std::unique_ptr<AST> termList) : head_tok(head_tok),
    term(std::move(term)),termList(std::move(termList)) {}
};

/// TermListAST
/// termList
///    ::= <empty>
///    ::= termList '+' term
///    ::= termList '-' term
class TermListAST : public AST {
  int op_tok;
  std::unique_ptr<AST> term,termList;

public:
  TermListAST(int op_tok,std::unique_ptr<AST> term,
    std::unique_ptr<AST> termList) : op_tok(op_tok),
    term(std::move(term)),termList(std::move(termList)) {}
};

/// TermAST
/// term ::= factor factList
class TermAST : public AST {
  std::unique_ptr<AST> factor,factList;

public:
  TermAST(std::unique_ptr<AST> factor,std::unique_ptr<AST> factList)
    : factor(std::move(factor)),factList(std::move(factList)) {}
};

/// FactListAST
/// factList
///    ::= <empty>
///    ::= factList '*' factor
///    ::= factList '/' factor
class FactListAST : public AST {
  int op_tok;
  std::unique_ptr<AST> factor,factList;

public:
  FactListAST(int op_tok,std::unique_ptr<AST> factor,
    std::unique_ptr<AST> factList) : op_tok(op_tok),
    factor(std::move(factor)),factList(std::move(factList)) {}
};

/// FactorAST
/// factor
///    ::= IDENT
///    ::= NUMBER
///    ::= IDENT '(' expList ')'
///    ::= '(' expression ')'
class FactorAST : public AST {
  std::string Name;
  float Val;
  std::unique_ptr<AST> expList,expression;

public:
  FactorAST(std::string Name,float Val,std::unique_ptr<AST> expList,
    std::unique_ptr<AST> expression) : Name(Name),Val(Val),
    expList(std::move(expList)),expression(std::move(expression)) {}
};

/// ExpListAST
/// expList
///    ::= expression
///    ::= expList ',' expression
class ExpListAST : public AST {
  std::unique_ptr<AST> expList,expression;

public:
  ExpListAST(std::unique_ptr<AST> expList,std::unique_ptr<AST> expression) :
    expList(std::move(expList)),expression(std::move(expression)) {}
};

extern std::unique_ptr<AST> parse(void);

#endif