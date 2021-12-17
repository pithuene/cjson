#include <stdio.h>

#include "cjson.h"

// Reads a quoted string from in.
static char * read_quoted_string(char ** in) {
  if (**in != '"') {
    return NULL;
  }

  (*in)++; // Skip the "

  char * beginning = *in; // Copy the pointer to read without modifying
  char * end = beginning;

  // Find the end of the string
  bool escape = false; // next char escaped
  while (*end != 0 && !(*end == '"' && !escape)) {
    if (*end == '\\' && !escape) {
      escape = true;
    } else {
      escape = false;
    }
    end++;
  }

  if (*end != '"') {
    printf("Found unhandled character '%c' in string\n", *end);
    return NULL;
  }

  size_t length = ((size_t) end) - ((size_t) beginning);

  char * string = malloc(length + 1);
  strncpy(string, beginning, length);
  
  // Move pointer after the value
  end++; // Skip the "
  *in = end;

  return string;
}

static bool parse_string(JsonValue ** result, char ** json) {
  char * string = read_quoted_string(json);
  if (string == NULL) {
    return false;
  }
  
  *result = malloc(sizeof(JsonValue));
  (*result)->type = JsonString;
  (*result)->value.String = string;

  return true;
}

static bool parse_null(JsonValue ** result, char ** json) {
  if (!strncmp(*json, "null", 4)) {
    *json += 4;

    *result = malloc(sizeof(JsonValue));
    (*result)->type = JsonNull;

    return true;
  }
  return false;
}

static bool parse_undefined(JsonValue ** result, char ** json) {
  if (!strncmp(*json, "undefined", 9)) {
    *json += 9;

    *result = malloc(sizeof(JsonValue));
    (*result)->type = JsonUndefined;

    return true;
  }
  return false;
}

static bool parse_bool(JsonValue ** result, char ** json) {
  if (!strncmp(*json, "true", 4)) {
    *json += 4;
    *result = malloc(sizeof(JsonValue));
    (*result)->type = JsonBool;
    (*result)->value.Bool = true;
    return true;
  }
  if (!strncmp(*json, "false", 5)) {
    *json += 5;
    *result = malloc(sizeof(JsonValue));
    (*result)->type = JsonBool;
    (*result)->value.Bool = false;
    return true;
  }
  return false;
}

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
      // TODO: Handle multiple commas [1,,,,,5] currently parses fine as [1, 5]
      // TODO: Same happens in the object
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

static bool parse_object(JsonValue ** result, char ** json) {
  char * ptr = *json; // Copy the pointer to read without modifying
  if (*ptr != '{') { // Not an object
    return false;
  }
  ptr++; // Skip over the {

  map_t(JsonValue *) values = map(JsonValue *, 2);
  vec_t(char *) keys = vec(char *, 2);

  JsonValue * map_val;
  char * map_key;
  bool success = true;
  while (true) {
    map_key = read_quoted_string(&ptr);
    if (map_key == NULL) {
      success = false;
    } else {
      if (*ptr != ':') {
        printf("Found unhandled character '%c' in object where : was expected\n", *ptr);
        success = false;
      } else {
        ptr++;
        success = json_parse(&map_val, &ptr);
      }
    }

    if (success) {
      mapput(values, map_key, map_val);
      vecpush(keys, map_key);
    } else {
      if (isspace(*ptr) || *ptr == ',') {
        ptr++;
      } else if (*ptr == '}') { // End of array
        break;
      } else {
        printf("Found unhandled character '%c' in object\n", *ptr);
        // TODO: Free all values
        return false;
      }
    }
  }

  if (*ptr == '}') {
    ptr++;
    *json = ptr;

    *result = malloc(sizeof(JsonValue));
    (*result)->type = JsonObject;
    (*result)->value.Object = values;
    return true;
  } else {
    printf("Found unhandled character '%c' in object\n", *ptr);
    return false;
  }
}

bool json_parse(JsonValue ** result, char ** json) {
  while (**json != 0) {
    if (isspace(**json)) {
      (*json)++;
      continue;
    }
    if (parse_object(result, json)) break;
    if (parse_array(result, json)) break;
    if (parse_null(result, json)) break;
    if (parse_undefined(result, json)) break;
    if (parse_bool(result, json)) break;
    if (parse_number(result, json)) break;
    if (parse_string(result, json)) break;
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
  int i;
  switch (json->type) {
    case JsonNumber:
      print_depth(depth);
      printf("%f,\n", json->value.Number);
      break;
    case JsonString:
      print_depth(depth);
      printf("\"%s\",\n", json->value.String);
      break;
    case JsonNull:
      print_depth(depth);
      printf("null,\n");
      break;
    case JsonUndefined:
      print_depth(depth);
      printf("undefined,\n");
      break;
    case JsonBool:
      print_depth(depth);
      printf(json->value.Bool ? "true,\n" : "false,\n");
      break;
    case JsonArray:
      print_depth(depth);
      printf("[\n");
      for (i = 0; i < veclen(json->value.Array); i++) {
        json_print(json->value.Array[i], depth+1);
      }
      print_depth(depth);
      printf("],\n");
      break;
    case JsonObject:
      print_depth(depth);
      printf("{\n");
      mapforeach(char * key, JsonValue * value, json->value.Object) {
        print_depth(depth + 1);
        printf("\"%s\":\n", key);
        json_print(value, depth+2);
      }
      print_depth(depth);
      printf("},\n");
      break;
  }
}
