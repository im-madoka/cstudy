#include <stdio.h>

#include <parser.h>

#define CSTUDY_UTILS_ARRAY_IMPL
#include <utils/array.h>

int main(void) {
  int error;
  Parser parser = tokenize("(1 + 1 * 25.5 + 2^2) * 2", &error);

  if (error != 0) return error;

  float res = parse(&parser, &error);

  if (error != 0) return error;

  printf("%f\n", res);

  parser_free(&parser);
  
  return 0;
}
