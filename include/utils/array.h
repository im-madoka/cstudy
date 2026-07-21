#ifndef CSTUDY_UTILS_ARRAY_H
#define CSTUDY_UTILS_ARRAY_H

#include <stdlib.h>

#if !defined(__STDC_VERSION__) || __STDC_VERSION__ < 202311L
  #include <stdbool.h>

  #define nullptr NULL
#endif

// Dynamic array sructure.
typedef struct {
  void* items;
  size_t size;
  size_t capacity;
  size_t element_size;
} Array;

// Initialize the Array.
void array_init(Array * array, const size_t element_size);
// Free and reset the Array.
void array_free(Array * array);
// Clear the Array but not free the Array. You can reuse it without need of re-init.
void array_clear(Array * array);

// Insert an item at pos.
const void* array_insert(Array * array, const size_t pos, const void* item);
// Insert an item to the tail of the array.
const void* array_append(Array * array, const void* item);
// Insert an item to the head of the item.
const void* array_prepend(Array * array, const void* item);

// Delete an item.
bool array_del(Array * array, const size_t pos);
// Delete an item an move it out.
bool array_get_and_del(Array * array, const size_t pos, void * out);
// Delete the last item and move it out.
bool array_pop(Array * array, void * out);

// Set the item on the pos.
void* array_set(Array * array, const size_t pos, const void* item);
// Get the item on the pos.
void* array_get(const Array * array, const size_t pos);

// Reverse the array.
bool array_reverse(Array * array);
// Copy the array. The new array must be a valid initialized Array.
bool array_copy(Array * dst, const Array * array);

#endif //CSTUDY_UTILS_ARRAY_H

#ifdef CSTUDY_UTILS_ARRAY_IMPL

#include <string.h>

void array_init(Array * array, const size_t element_size) {
  array->items = nullptr;
  array->size = 0;
  array->capacity = 0;
  array->element_size = element_size;
}

void array_free(Array * array) {
  free(array->items);
  
  array->items = nullptr;
  array->size = 0;
  array->capacity = 0;
  array->element_size = 0;
}

void array_clear(Array * array) {
  array->size = 0;
}

const void* array_insert(Array * array, const size_t pos, const void* item) {
  if (pos > array->size) return nullptr;

  if (array->size >= array->capacity) {
    size_t capacity = array->capacity;
    if (0 == capacity) capacity = 128;
    else capacity += capacity / 2;

    void* items = realloc(array->items, capacity * array->element_size);
    if (nullptr == items) return nullptr;
    array->items = items;
    array->capacity = capacity;
  }

  char* base = (char*)array->items;

  memmove(base + (pos + 1) * array->element_size, base + pos * array->element_size,
          (array->size - pos) * array->element_size);
  memcpy(base+pos * array->element_size, item, array->element_size);

  array->size ++;

  return item;
}

const void* array_append(Array * array, const void* item) {
  return array_insert(array, array->size, item);
}

const void* array_prepend(Array * array, const void* item) {
  return array_insert(array, 0, item);
}

bool array_del(Array * array, const size_t pos) {
  if (pos >= array->size) return false;

  char* base = (char*)array->items;

  memmove(base + pos * array->element_size, base + (pos + 1) * array->element_size,
          (array->size - pos - 1) * array->element_size);

  array->size --;

  return true;
}

bool array_get_and_del(Array * array, const size_t pos, void * out) {
  if (pos >= array->size) return false;

  char* base = (char*)array->items;

  memmove(out, base + pos * array->element_size, array->element_size);
  memmove(base + pos * array->element_size, base + (pos + 1) * array->element_size,
          (array->size - pos - 1) * array->element_size);

  array->size --;

  return true;
}

bool array_pop(Array * array, void * out) {
  if (array->size == 0) return false;
  
  return array_get_and_del(array, array->size - 1, out);
}


void* array_set(Array * array, const size_t pos, const void* item) {
  if (pos >= array->size) return nullptr;

  char* base = (char*)array->items;

  memcpy(base + pos * array->element_size, item, array->element_size);

  return base + pos * array->element_size;
}

void* array_get(const Array * array, const size_t pos) {
  if (pos >= array->size) return nullptr;

  char* base = (char*)array->items;

  return base + pos * array->element_size;
}

bool array_reverse(Array * array) {
  if (array->size < 2) return true;
  
  void* buffer = malloc(array->element_size);
  if (buffer == nullptr) return false;

  char* base = (char*)array->items;
  
  for (size_t i = 0; i < array->size / 2; ++ i) {
    memcpy(buffer, base + i * array->element_size, array->element_size);
    memcpy(base + i * array->element_size, base + (array->size - i - 1) * array->element_size, array->element_size);
    memcpy(base + (array->size - i - 1) * array->element_size, buffer, array->element_size);
  }

  free(buffer);

  return true;
}

bool array_copy(Array * dst, const Array * array) {
  if (dst == NULL || array == NULL) return false;
  if (dst == array) return true;
  
  if (dst->capacity < array->capacity) {
    size_t capacity = array->capacity;

    void* items = realloc(dst->items, capacity * array->element_size);
    if (nullptr == items) return false;
    dst->items = items;
    dst->capacity = capacity;
  }

  dst->element_size = array->element_size;
  dst->size = array->size;

  memcpy(dst->items, array->items, array->size * array->element_size);

  return true;
}

#endif //CSTUDY_UTILS_ARRAY_IMPL
