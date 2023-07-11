#ifndef __PARSER_H__
#define __PARSER_H__

#include  "lexer.h"

extern int  token,indent;

enum {
  CONST = 0,
  VAR = 1,
  FUNC = 2,
  MAX_SYMBOL_TYPE = 3
};

#define insert_indent(i)  (std::string((i),' '))

/// AST - Base class for all expression nodes.
class AST {
public:
  virtual ~AST() = default;
  virtual void print() = 0;
};

/// ProgramAST
/// program ::= block '.'
class ProgramAST : public AST {
  std::unique_ptr<AST>  block;

public:
  ProgramAST(std::unique_ptr<AST> block) : block(std::move(block)) {}
  void print() {
    indent = 0;
    if (block) block->print();
    std::cout<<".\n"; 
  }
};

/// BlockAST
/// block ::= declList statement
class BlockAST : public AST {
  std::unique_ptr<AST>  declList,statement;

public:
  BlockAST(std::unique_ptr<AST> declList,std::unique_ptr<AST> statement)
    : declList(std::move(declList)), statement(std::move(statement)) {}
  void print() { 
    if (declList) declList->print();
    if (statement) statement->print();
  }
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
  void print() { for (size_t i=0;i<declList.size();i++) declList[i]->print(); }
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
  void print() { if (decl) decl->print(); }
};

/// ConstDeclAST
/// constDecl ::= CONST numberList ';'
class ConstDeclAST : public AST {
  std::unique_ptr<AST> numberList;

public:
  ConstDeclAST(std::unique_ptr<AST> numberList)
    : numberList(std::move(numberList)) {}
  void print() {
    std::cout<<insert_indent(indent)<<"const ";
    numberList->print();
    std::cout<<";\n";
  }
};

/// NumberListAST
/// numberList
///    ::= IDENT EQ NUMBER
///    ::= numberList COMMA IDENT EQ NUMBER
class NumberListAST : public AST {
  std::vector<std::pair<std::string,int>> numberList;

public:
  NumberListAST(std::vector<std::pair<std::string,int>> numberList)
    : numberList(std::move(numberList)) {}
  void print() {
    for (size_t i=0;i<numberList.size();i++) {
      std::cout<<numberList[i].first
            <<'='<<numberList[i].second;
      std::cout<<((i==numberList.size()-1)?"":",");
    }
  }
};

/// VarDeclAST
/// varDecl ::= VAR identList ';'
class VarDeclAST : public AST {
  std::unique_ptr<AST> identList;

public:
  VarDeclAST(std::unique_ptr<AST> identList)
    : identList(std::move(identList)) {}
  void print() { 
    std::cout<<insert_indent(indent)<<"var ";
    identList->print();
    std::cout<<";\n"; 
  }
};

/// IdentListAST
/// identList
///    ::= IDENT
///    ::= identList COMMA IDENT
class IdentListAST : public AST {
  std::vector<std::string>  identList;

public:
  IdentListAST(std::vector<std::string> identList)
    : identList(std::move(identList)) {}
  void print() {
    for (size_t i=0;i<identList.size();i++) {
      std::cout<<identList[i]<<((i==identList.size()-1)?"":",");
    }
  }
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
  void print() { if (parList) parList->print(); }
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
  void print() {
    for (size_t i=0;i<parList.size();i++) {
      std::cout<<parList[i]<<((i==parList.size()-1)?"":",");
    }
  }
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
  void print() {
    std::cout<<"function "<<Name<<" (";
    if (optParList) optParList->print();
    std::cout<<")\n";
    indent++;
    if (block) block->print();
    std::cout<<";\n";
    indent--;
  }
};

/// StatementAST
/// statement
///    ::= <empty>
///    ::= IDENT COLOEQ expression
///    ::= BEGIN statement stateList END
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
    std::unique_ptr<AST> condition,std::unique_ptr<AST> statement,std::unique_ptr<AST> stateList) : 
    head_tok(head_tok),Name(Name),expression(std::move(expression)),condition(std::move(condition)),
    statement(std::move(statement)),stateList(std::move(stateList)) {}
  void print() {
    int temp_indent = 0;

    switch (head_tok) {
      case tok_id :
        std::cout<<insert_indent(indent)<<Name<<":=";
        expression->print();
        break;
      case tok_begin :
        std::cout<<insert_indent(indent)<<"begin\n";indent++;
        if (statement) statement->print();
        if (stateList) stateList->print();
        indent--;std::cout<<insert_indent(indent)<<"end";
        break;
      case tok_if :
        std::cout<<insert_indent(indent)<<"if ";
        condition->print();
        std::cout<<" then ";
        std::swap(temp_indent,indent);
        statement->print();
        std::swap(temp_indent,indent);
        break;
      case tok_while :
        std::cout<<insert_indent(indent)<<"while ";
        condition->print();
        std::cout<<" do\n";
        statement->print();
        break;
      case tok_ret :
        std::cout<<insert_indent(indent)<<"return ";
        expression->print();
        break;
      case tok_write :
        std::cout<<insert_indent(indent)<<"write ";
        expression->print();
        break;
      case tok_writeln :
        std::cout<<insert_indent(indent)<<"writeln";
        break;
      default:
        // do nothing
        break;
    }
  }
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
  void print() {
    std::cout<<";\n";
    if (statement) statement->print();
    if (stateList) {
      stateList->print();
    }
  }
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
  void print() {
    if (op_tok == tok_odd) {
      std::cout<<"odd ";
      LHS->print();
    }
    else {
      LHS->print();
      switch (op_tok) {
        case tok_equal : std::cout<<" = ";break;
        case tok_notequal : std::cout<<" != ";break;
        case tok_less : std::cout<<" < ";break;
        case tok_greater : std::cout<<" > ";break;
        case tok_lessequal : std::cout<<" <= ";break;
        case tok_greaterequal : std::cout<<" >= ";break;
        default : /*do nothing*/;break;
      }
      RHS->print();
    }
  }
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
  void print() {
    if (head_tok) std::cout<<"-";
    if (term) term->print();
    if (termList) termList->print();
  }
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
  void print() {
    if (termList) {
      termList->print();
    }
    std::cout<<((op_tok==tok_plus)?'+':'-');
    term->print();

  }
};

/// TermAST
/// term ::= factor factList
class TermAST : public AST {
  std::unique_ptr<AST> factor,factList;

public:
  TermAST(std::unique_ptr<AST> factor,std::unique_ptr<AST> factList)
    : factor(std::move(factor)),factList(std::move(factList)) {}
  void print() {
    if (factor) factor->print();
    if (factList) factList->print();
  }
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
  
  void print() {
    if (factList) {
      factList->print();
    }
    std::cout<<((op_tok==tok_div)?'/':'*');
    factor->print();
  }
};

enum {
  IDENT = -1,
  NUMBER = -2,
  FUNC_CALL = -3,
  EXPRESSION = -4
};

/// FactorAST
/// factor
///    ::= IDENT
///    ::= NUMBER
///    ::= IDENT '(' expList ')'
///    ::= '(' expression ')'
class FactorAST : public AST {
  std::string Name;
  int Val;
  std::unique_ptr<AST> expList,expression;
  int factType;

public:
  FactorAST(std::string Name,int Val,std::unique_ptr<AST> expList,
    std::unique_ptr<AST> expression,int factType) : Name(Name),Val(Val),
    expList(std::move(expList)),expression(std::move(expression)),factType(factType) {}
  void print() {
    switch (factType) {
      case IDENT : std::cout<<Name;break;
      case NUMBER : std::cout<<Val;break;
      case FUNC_CALL :
        std::cout<<Name<<"(";
        if (expList) expList->print();
        std::cout<<")";
        break;
      case EXPRESSION :
        std::cout<<"(";
        expression->print();
        std::cout<<")";
        break;
      default : /*do nothing*/;break;
    }
  }
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
  void print() {
    if (expression) expression->print();
    if (expList) {
      std::cout<<",";
      expList->print();
    }
  }
};

extern std::unique_ptr<AST> parse(void);

#endif