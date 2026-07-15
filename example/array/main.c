#include <stdio.h>

#define CSTUDY_UTILS_ARRAY_IMPL
#include <utils/array.h>

int main(void) {
  Array array;

  array_init(&array, sizeof(int));

  array_append(&array, &(int){1});
  array_append(&array, &(int){2});
  array_append(&array, &(int){3});
  array_append(&array, &(int){4});
  array_append(&array, &(int){5});
  array_append(&array, &(int){6});

  for (size_t i = 0; i < array.size; ++ i) {
    printf("%d ", *(int*)array_get(&array, i));
  }

  printf("\n");
  array_reverse(&array);

  for (size_t i = 0; i < array.size; ++ i) {
    printf("%d ", *(int*)array_get(&array, i));
  }

  array_free(&array);
  
  return 0;
}
