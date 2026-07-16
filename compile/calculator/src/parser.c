#include <parser.h>

#include <stdlib.h>
#include <math.h>

static bool is_digit(char chr) {
  return (chr >= '0' && chr <= '9');
}

Parser tokenize(const char* expr, int* error) {
  Parser parser = {0};
  Array tokens = {0};
  *error = 0;
  array_init(&tokens, sizeof(Token));

  for (size_t i = 0; ; ++ i) {
    const char chr = *(expr + i);

    Token token = {0};

    if (is_digit(chr) || chr == '.') {
      token.type = TOKEN_NUM;

      Array num = {0};
      array_init(&num, sizeof(char));
      array_append(&num, &chr);

      bool is_decimal = chr == '.';
      char next = *(expr + i + 1);
      for (; is_digit(next) || next == '.'; next = *(expr + i + 1)) {
        if (next == '.') {
          if (is_decimal) {
            *error = 2;
            break;
          } else {
            is_decimal = true;
          }
        }

        array_append(&num, &next);
        i ++;
      }

      if (*error == 0) {
        array_append(&num, &(char){'\0'});

        const char* num_str = (const char*)num.items;
        char* end_str;

        float final_num = strtof(num_str, &end_str);

        if (num_str == end_str) {
          *error = 2;
        }

        token.data.num = final_num;
      }

      array_free(&num);

      if (*error != 0) goto end;
      else goto append;
    }

    switch (chr) {
      case '\0':
        token.type = TOKEN_END;
        break;
      case ' ':
        continue;
      case '+':
        token.type = TOKEN_ADD;
        break;
      case '-':
        token.type = TOKEN_NEG;
        break;
      case '*':
        token.type = TOKEN_MUL;
        break;
      case '/':
        token.type = TOKEN_DIV;
        break;
      case '^':
        token.type = TOKEN_EXP;
        break;
      case '(':
        token.type = TOKEN_LP;
        break;
      case ')':
        token.type = TOKEN_RP;
        break;
      default:
        *error = 1;
        goto end;
    }

append:
    array_append(&tokens, &token);

    if (token.type == TOKEN_END) break;
  }

  parser.tokens = tokens;

end:
  if (*error != 0) array_free(&tokens);

  return parser;
}

void parser_free(Parser* parser) {
  array_free(&parser->tokens);
}

#define return_error(err) if (err != 0) return 0

static float expr(Parser* parser, int* error);
static float term(Parser* parser, int* error);
static float power(Parser* parser, int* error);
static float factor(Parser* parser, int* error);

static float expr(Parser* parser, int* error) {
  const Token* next_tok = (Token*)array_get(&parser->tokens, parser->current_pos);
  bool is_neg = false;

  if (next_tok->type == TOKEN_NEG) {
    parser->current_pos ++;
    is_neg = true;
  }

  float res = term(parser, error);
  return_error(*error);

  if (is_neg) {
    res = -res;
  }


  while (true) {
    const Token* tok = (Token*)array_get(&parser->tokens, parser->current_pos);

    float term_2;
    switch (tok->type) {
      case TOKEN_ADD:
        parser->current_pos ++;
        term_2 = term(parser, error);
        return_error(*error);
        res += term_2;
        break;
      case TOKEN_NEG:
        parser->current_pos ++;
        term_2 = term(parser, error);
        return_error(*error);
        res -= term_2;
        break;
      default:
        goto out;
    }
  }

out:
  return res;
}

static float term(Parser* parser, int* error) {
  float res = power(parser, error);
  return_error(*error);

  while (true) {
    const Token* tok = (Token*)array_get(&parser->tokens, parser->current_pos);

    float power_2;
    switch (tok->type) {
      case TOKEN_MUL:
        parser->current_pos ++;
        power_2 = power(parser, error);
        return_error(*error);
        res *= power_2;
        break;
      case TOKEN_DIV:
        parser->current_pos ++;
        power_2 = power(parser, error);
        return_error(*error);
        res /= power_2;
        break;
      default:
        goto out;
    }
  }

out:
  return res;
}

static float power(Parser* parser, int* error) {
  float res = factor(parser, error);
  return_error(*error);

  const Token* tok = (Token*)array_get(&parser->tokens, parser->current_pos);

  if (tok->type == TOKEN_EXP) {
    parser->current_pos ++;
    float power_2 = power(parser, error);
    return_error(*error);
    res = pow(res, power_2);
  }

  return res;
}

static float factor(Parser* parser, int* error) {
  const Token* tok = (Token*)array_get(&parser->tokens, parser->current_pos);

  switch (tok->type) {
    case TOKEN_NUM:
      parser->current_pos ++;
      return tok->data.num;
    case TOKEN_LP:
      parser->current_pos ++;
      float res = expr(parser, error);
      return_error(*error);
      const Token* next = (Token*)array_get(&parser->tokens, parser->current_pos);
      if (next->type != TOKEN_RP) {
        *error = 2;
        return 0;
      }
      parser->current_pos ++;
      return res;
    case TOKEN_END:
      *error = 3;
      return 0;
    default:
      *error = 1;
      return 0;
  }
}

float parse(Parser* parser, int* error) {
  float res = expr(parser, error);
  return_error(*error);

  const Token* tok = (Token*)array_get(&parser->tokens, parser->current_pos);

  if (tok->type != TOKEN_END) {
    *error = -1;
    return 0;
  }

  return res;
}

