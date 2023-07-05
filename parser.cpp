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
#include    "error.h"

int token;

static std::unique_ptr<AST> parseProgram(void);
static std::unique_ptr<AST> ParseBlock(void);
static std::unique_ptr<AST> ParseDeclList(std::vector<std::string>&);
static std::unique_ptr<AST> ParseStatement(void);
static std::unique_ptr<AST> ParseDecl(std::vector<std::string>&);
static std::unique_ptr<AST> ParseConstDecl(std::vector<std::string>&);
static std::unique_ptr<AST> ParseNumberList(std::vector<std::string>&);
static std::unique_ptr<AST> ParseVarDecl(std::vector<std::string>&);
static std::unique_ptr<AST> ParseIdentList(std::vector<std::string>&);
static std::unique_ptr<AST> ParseFuncDecl(std::vector<std::string>&);
static std::unique_ptr<AST> ParseOptParList(std::vector<std::string>&);
static std::unique_ptr<AST> ParseParList(std::vector<std::string>&);
static std::unique_ptr<AST> ParseExpression(void);
static std::unique_ptr<AST> ParseTerm(void);
static std::unique_ptr<AST> ParseFactor(void);
static std::unique_ptr<AST> ParseExpList(void);
static std::unique_ptr<AST> ParseFactList(void);
static std::unique_ptr<AST> ParseTermList(void);
static std::unique_ptr<AST> ParseStateList(void);
static std::unique_ptr<AST> ParseCondition(void);

static std::map<std::string,std::vector<int>>   symTab;

#define add_new_symbol(name,type,buffer) \
    (symTab[(name)].push_back(1<<(type)),(buffer).push_back((name)))

static bool is_available_symbol(std::string& name,int type)
{
    if (symTab.find(name)==symTab.end() ||
            symTab[name].empty() || symTab[name].back()!=(1<<type))
        return  false;

    return  true;
}

static int  removeSymbols(const std::vector<std::string>& sym)
{
    for (size_t i=0;i<sym.size();i++) {
        symTab[sym[i]].pop_back();
    }

    return  0;
}

std::unique_ptr<AST> parse(void)
{
    return  std::move(parseProgram());
}

/// program ::= block '.'
static std::unique_ptr<AST> parseProgram(void)
{
    auto B = ParseBlock();

    if (!B) return printError("Cannot find block!!");
    if (token != tok_period) return printError("Expected '.' at end of program");

    token = getNextTok();
    if (token != tok_eof) return printError("Program should have been finished.");

    return  std::make_unique<ProgramAST>(std::move(B));
}

/// block ::= declList statement
static std::unique_ptr<AST> ParseBlock(void)
{
    std::vector<std::string>    localSymbols;

    auto D = ParseDeclList(localSymbols);
    auto S = ParseStatement();

    removeSymbols(localSymbols);
    return  std::make_unique<BlockAST>(std::move(D),std::move(S));
}

/// declList 
///    ::= <empty>
///    ::= decl decList
static std::unique_ptr<AST> ParseDeclList(std::vector<std::string>& sym)
{
    std::vector<std::unique_ptr<AST>> declList;

    for(auto D=ParseDecl(sym);D!=nullptr;D=ParseDecl(sym)) {
        declList.push_back(std::move(D));
    }

    return std::make_unique<DeclListAST>(std::move(declList));
}

/// decl
///    ::= constDecl
///    ::= varDecl
///    ::= funcDecl
static std::unique_ptr<AST> ParseDecl(std::vector<std::string>& sym)
{
    auto D = ParseConstDecl(sym);

    if (!D) D = ParseVarDecl(sym);
    if (!D) D = ParseFuncDecl(sym);
    if (!D) return nullptr;
    
    return std::make_unique<DeclAST>(std::move(D));
}

/// funcDecl ::= FUNCTION IDENT '('  optParList ')' block ';'
static std::unique_ptr<AST> ParseFuncDecl(std::vector<std::string>& sym)
{
    std::string name;

    if(token != tok_func) return nullptr;
    token = getNextTok();

    if (token != tok_id) return printError("Expected ID");
    
    name = getTokStr();
    token = getNextTok();

    add_new_symbol(name,FUNC,sym);
    if (token != tok_lparen) return printError("Expected (");

    token = getNextTok();

    auto O = ParseOptParList(sym);
    if (token != tok_rparen) return printError("Expected )");

    token = getNextTok();
    auto B = ParseBlock();

    if (!B) return nullptr;
    if (token != tok_semicolon) return printError("Expected ;");
    
    token = getNextTok();

    return std::make_unique<FuncDeclAST>(name,std::move(O),std::move(B));
}

/// optParList
///    ::= <empty>
///    ::= parList
static std::unique_ptr<AST> ParseOptParList(std::vector<std::string>& sym)
{
    auto P = ParseParList(sym);
    if (!P) return nullptr;

    return std::make_unique<OptParListAST>(std::move(P));
}

/// parList
///    ::= IDENT
///    ::= parList COMMA IDENT
static std::unique_ptr<AST> ParseParList(std::vector<std::string>& sym)
{
    std::string name;
    std::vector<std::string> parList;

    do {
        if (token != tok_id) return nullptr;
        name = getTokStr();

        add_new_symbol(name,VAR,sym);
        parList.push_back(name);

        if ((token=getNextTok()) != tok_comma) break;
        token = getNextTok();
    } while (true);

    return (parList.empty())?nullptr:std::make_unique<ParListAST>(parList);
}

/// varDecl ::= VAR identList ';'
static std::unique_ptr<AST> ParseVarDecl(std::vector<std::string>& sym)
{
    if (token != tok_var) return nullptr;
    token = getNextTok();

    auto IL = ParseIdentList(sym);
    if (!IL) return nullptr;

    if (token != tok_semicolon) return nullptr;
    token = getNextTok();

    return std::make_unique<VarDeclAST>(std::move(IL));
}

/// identList
///    ::= IDENT
///    ::= identList COMMA IDENT
static std::unique_ptr<AST> ParseIdentList(std::vector<std::string>& sym)
{
    std::string name;
    std::vector<std::string> identList;
    
    do {
        if (token != tok_id) return nullptr;
        name = getTokStr();

        add_new_symbol(name,VAR,sym);
        identList.push_back(name);

        if ((token=getNextTok()) != tok_comma) break;
        token = getNextTok();
    } while (true);
    
    return (identList.empty())?nullptr:std::make_unique<IdentListAST>(identList);
}

/// constDecl ::= CONST numberList ';'
static std::unique_ptr<AST> ParseConstDecl(std::vector<std::string>& sym)
{
    if (token != tok_const) return nullptr;
    token = getNextTok();

    auto NL = ParseNumberList(sym);
    if (!NL) return nullptr;

    if (token != tok_semicolon) return nullptr;
    token = getNextTok();

    return std::make_unique<ConstDeclAST>(std::move(NL));
}

/// numberList
///    ::= IDENT EQ NUMBER
///    ::= numberList COMMA IDENT EQ NUMBER
static std::unique_ptr<AST> ParseNumberList(std::vector<std::string>& sym)
{
    std::string name;
    int val;
    std::vector<std::pair<std::string,int>> numberList;

    do {
        if (token != tok_id) return nullptr;

        name = getTokStr();
        token = getNextTok();

        add_new_symbol(name,CONST,sym);
        if (token != tok_equal) return nullptr;

        token = getNextTok();
        if (token != tok_num) return nullptr;

        val = getTokNumVal();
        numberList.push_back(std::make_pair(name,val));

        if ((token = getNextTok()) != tok_comma) break;
        token = getNextTok();
    } while (true);

    return (numberList.empty())?nullptr:std::make_unique<NumberListAST>(numberList);
}

/// statement
///    ::= <empty>
///    ::= IDENT COLOEQ expression
///    ::= BEGINN statement stateList END
///    ::= IF condition THEN statement
///    ::= WHILE condition DO statement
///    ::= RETURN expression
///    ::= WRITE expression
///    ::= WRITELN
static std::unique_ptr<AST> ParseStatement(void)
{
    std::string name;

    switch (token) {
        case  tok_id: {
            name = getTokStr();
            if (!is_available_symbol(name,VAR)) {std::cout<<name<<'\n';return printError("L-value should be a variable");}

            token = getNextTok();

            if (token != tok_assign) return printError("Expected :=");
            token = getNextTok();

            auto E = ParseExpression();
            if (!E) return nullptr;

            return std::make_unique<StatementAST>(tok_id,name,std::move(E),nullptr,nullptr,nullptr);
        }
        case  tok_begin: {
            token = getNextTok();

            auto S = ParseStatement();
            auto SL = ParseStateList();
            
            if (token != tok_end) return nullptr;
            token = getNextTok();
            
            return std::make_unique<StatementAST>(tok_begin,"",nullptr,nullptr,std::move(S),std::move(SL));
        }
        case  tok_if: {
            token = getNextTok();
            auto C = ParseCondition();

            if (!C) return nullptr;
            if (token != tok_then) return nullptr;
            
            token = getNextTok();
            auto S = ParseStatement();

            return std::make_unique<StatementAST>(tok_if,"",nullptr,std::move(C),std::move(S),nullptr);
        }
        case  tok_while: {
            token = getNextTok();
            auto C = ParseCondition();

            if (!C) return nullptr;
            if (token != tok_do) return nullptr;
            
            token = getNextTok();
            auto S = ParseStatement();
            
            return std::make_unique<StatementAST>(tok_while,"",nullptr,std::move(C),std::move(S),nullptr);
        }
        case  tok_ret: {
            token = getNextTok();
            auto E = ParseExpression();
            
            if (!E) return nullptr;
            return std::make_unique<StatementAST>(tok_ret,"",std::move(E),nullptr,nullptr,nullptr);
        }
        case  tok_write: {
            token = getNextTok();
            auto E = ParseExpression();

            if (!E) return nullptr;
            return std::make_unique<StatementAST>(tok_write,"",std::move(E),nullptr,nullptr,nullptr);
        }
        case  tok_writeln: {
            token = getNextTok();
            return std::make_unique<StatementAST>(tok_writeln,"",nullptr,nullptr,nullptr,nullptr);
        }
        default:
            break;
    }

    return nullptr;
}

/// expression
///     ::= '-'  term termList
///     ::= term  termList
static std::unique_ptr<AST> ParseExpression(void)
{
    if (token == tok_minus) {
        token = getNextTok();

        auto T = ParseTerm();
        if (!T) return nullptr;

        auto TL = ParseTermList();
        return std::make_unique<ExpressionAST>('-',std::move(T),std::move(TL));
    }

    auto T = ParseTerm();
    if (!T) return nullptr;

    auto TL = ParseTermList();
    return std::make_unique<ExpressionAST>(0,std::move(T),std::move(TL));
}

/// term ::= factor factList
static std::unique_ptr<AST> ParseTerm(void)
{
    auto F = ParseFactor();
    if (!F) return nullptr;

    return std::make_unique<TermAST>(std::move(F),ParseFactList());
}

/// factor
///    ::= IDENT
///    ::= NUMBER
///    ::= IDENT '(' expList ')'
///    ::= '(' expression ')'
static std::unique_ptr<AST> ParseFactor(void)
{
    if (token == tok_id) {
        std::string name = getTokStr();

        token = getNextTok();
        if (token != tok_lparen) {
            if (!is_available_symbol(name,VAR) &&
                    !is_available_symbol(name,CONST)){std::cout<<name<<'\n';
                return  printError("There is no such a variable or constant");}
            return std::make_unique<FactorAST>(name,0.0,nullptr,nullptr);
        }

        if (!is_available_symbol(name,FUNC))
            return  printError("There is no such a callee");

        token = getNextTok();
   
        auto EL = ParseExpList();
        if (!EL || token!=tok_rparen) return nullptr;
    
        token = getNextTok();
        return std::make_unique<FactorAST>(name,0.0f,std::move(EL),nullptr);
    }

    if (token == tok_num) {
        token = getNextTok();
        return std::make_unique<FactorAST>("",getTokNumVal(),nullptr,nullptr);
    }

    if (token == tok_lparen) {
        token = getNextTok();
    
        auto E = ParseExpression();
        if(!E || token!=tok_rparen) return nullptr;

        token = getNextTok();
        return std::make_unique<FactorAST>("",0.0,nullptr,std::move(E));
    }

    return nullptr;
}

/// expList
///    ::= expression
///    ::= expList ',' expression
static std::unique_ptr<AST> ParseExpList(void)
{
    auto E = ParseExpression();
    if (!E) return nullptr;

    if (token != tok_comma)
        return std::make_unique<ExpListAST>(nullptr,std::move(E));

    token = getNextTok();
    return std::make_unique<ExpListAST>(ParseExpList(),std::move(E));
}

/// factList
///    ::= <empty>
///    ::= factList '*' factor
///    ::= factList '/' factor
static std::unique_ptr<AST> ParseFactList(void)
{
    int op_tok = token;

    if (op_tok!=tok_mult && op_tok!=tok_div) return nullptr;
    token = getNextTok();

    auto F = ParseFactor();
    if (!F) return nullptr;

    return std::make_unique<FactListAST>(op_tok,std::move(F),ParseFactList());
}

/// termList
///    ::= <empty>
///    ::= termList '+' term
///    ::= termList '-' term
static std::unique_ptr<AST> ParseTermList(void)
{
    int op_tok = token;

    if (op_tok!=tok_plus && op_tok!=tok_minus) return nullptr;
    token = getNextTok();

    auto T = ParseTerm();
    if (!T) return nullptr;

    return std::make_unique<TermListAST>(op_tok,std::move(T),ParseTermList());
}

/// stateList
///    ::= <empty>
///    ::= stateList ';' statement
static std::unique_ptr<AST> ParseStateList(void)
{
    if (token != tok_semicolon) return nullptr;
    token = getNextTok();

    auto S = ParseStatement();
    return std::make_unique<StateListAST>(ParseStateList(),std::move(S));
}

/// condition
///    ::= ODD expression
///    ::= expression EQ expression
///    ::= expression NOTEQ expression
///    ::= expression LT expression
///    ::= expression GT expression
///    ::= expression LE expression
///    ::= expression GE expression
static std::unique_ptr<AST> ParseCondition(void)
{
    if (token == tok_odd) {
        token = getNextTok();
        
        auto LHS = ParseExpression();
        if (!LHS) return nullptr;

        return std::make_unique<ConditionAST>(tok_odd,std::move(LHS),nullptr);
    }

    auto LHS = ParseExpression();
    if (!LHS) return nullptr;

    int op_tok = token;

    if (op_tok!=tok_equal && op_tok!=tok_notequal 
            && op_tok!=tok_less && op_tok!=tok_greater 
            && op_tok!=tok_lessequal && op_tok!=tok_greaterequal)
        return printError("Expected conditional operator");

    token = getNextTok();

    auto RHS = ParseExpression();
    if (!RHS) return nullptr;

    return std::make_unique<ConditionAST>(op_tok,std::move(LHS),std::move(RHS));
}