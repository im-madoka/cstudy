#pragma once

//! expr = (Token::Neg)? term ((Token::Add | Token::Neg) term)*
//! term = power ((Token::Mul | Token::Div) power)*
//! power = factor (Token::Exp power)?
//! factor = Token::Number | Token::LP expr Token::RP

#include <utils/array.h>

typedef enum {
  #include "token.def"
} TokenType;

typedef struct {
  union {
    float num;
  } data;
  TokenType type;
} Token;

typedef struct {
  Array tokens;
  size_t current_pos;
} Parser;

Parser tokenize(const char* expr, int* error);
void parser_free(Parser* parser);

float parse(Parser* parser, int* error);
