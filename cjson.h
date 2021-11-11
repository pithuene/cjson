#ifndef CJSON_H
#define CJSON_H

#include <ctype.h>
#include <stdbool.h>
#include "./ds/ds/map.h"
#include "./ds/ds/vec.h"

enum JsonType {
  JsonNumber,
  JsonBool,
  JsonString,
  JsonObject,
  JsonArray,
  JsonNull,
  JsonUndefined,
};

typedef struct _JsonValue {
  enum JsonType type;
  union {
    double Number;
    bool Bool;
    char * String;
    map_t(struct _JsonValue *) Object;
    vec_t(struct _JsonValue *) Array;
  } value;
  vec_t(char *) keys; // TODO: Hack because I have not implemented looping over maps yet
} JsonValue;

bool json_parse(JsonValue ** result, char ** json);

void json_print(JsonValue * json, int depth);

#endif
