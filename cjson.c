#include <stdio.h>

#include "cjson.h"

static bool parse_number(JsonValue ** result, char ** json) {
  char * beginning = *json; // Copy the pointer to read without modifying
  char * end = beginning;

  // Find the end of the number
  while (isdigit(*end) || *end == '.') {
    // TODO: Handle numbers starting with .
    end++;
  }

  if (beginning == end) { // No digits found
    return false;
  }

  double value = strtod(beginning, NULL);
  
  *result = malloc(sizeof(JsonValue));
  (*result)->type = JsonNumber;
  (*result)->value.Number = value;

  // Move pointer after the value
  *json = end;

  return true;
}

static bool parse_array(JsonValue ** result, char ** json) {
  char * ptr = *json; // Copy the pointer to read without modifying
  if (*ptr != '[') { // Not an array
    return false;
  }
  ptr++; // Skip over the [

  vec_t(JsonValue *) values = vec(JsonValue *, 2);

  JsonValue * array_val;
  bool success = true;
  while (true) {
    success = json_parse(&array_val, &ptr);
    if (success) {
      vecpush(values, array_val);
    } else {
      if (isspace(*ptr) || *ptr == ',') {
        ptr++;
      } else if (*ptr == ']') { // End of array
        break;
      } else {
        printf("Found unhandled character '%c' in array\n", *ptr);
        // TODO: Free all values
        return false;
      }
    }
  }

  if (*ptr == ']') {
    ptr++;
    *json = ptr;

    *result = malloc(sizeof(JsonValue));
    (*result)->type = JsonArray;
    (*result)->value.Array = values;
    return true;
  } else {
    printf("Found unhandled character '%c' in array\n", *ptr);
    return false;
  }
}

bool json_parse(JsonValue ** result, char ** json) {
  while (**json != 0) {
    if (isspace(**json)) {
      (*json)++;
      continue;
    }
    if (parse_array(result, json)) break;
    if (parse_number(result, json)) break;
    return false;
  }
  return true;
}

static void print_depth(int depth) {
  int i;
  for (i = 0; i < depth; i++) {
    printf("  ");
  }
}

void json_print(JsonValue * json, int depth) {
  switch (json->type) {
    case JsonNumber:
      print_depth(depth);
      printf("%f,\n", json->value.Number);
      break;
    case JsonArray:
      print_depth(depth);
      printf("[\n");
      int i;
      for (i = 0; i < veclen(json->value.Array); i++) {
        json_print(json->value.Array[i], depth+1);
      }
      print_depth(depth);
      printf("],\n");
      break;
  }
}
