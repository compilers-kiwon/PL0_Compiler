#ifndef __LEXER_H__
#define __LEXER_H__

extern int  getNextTok(void);
extern std::string getTokStr(void);
extern int getTokNumVal(void);

enum Token {
    tok_begin = -1,
    tok_end = -2,
    tok_if = -3,
    tok_then = -4,
    tok_while = -5,
    tok_do = -6,
    tok_ret = -7,
    tok_func = -8,
    tok_var = -9,
    tok_const = -10,
    tok_odd = -11,
    tok_write = -12,
    tok_writeln = -13,

    tok_plus = -14,
    tok_minus = -15,
    tok_mult = -16,
    tok_div = -17,
    tok_lparen = -18,
    tok_rparen = -19,
    tok_equal = -20,
    tok_less = -21,
    tok_greater = -22,
    tok_notequal = -23,
    tok_lessequal = -24,
    tok_greaterequal = -25,
    tok_comma = -26,
    tok_period = -27,
    tok_semicolon = -28,
    tok_assign = -29,

    tok_id = -30,
    tok_num = -31,

    tok_eof = -32,
    tok_none = -33
};

#endif