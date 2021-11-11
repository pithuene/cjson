#include <stdio.h>
#include "cjson.h"

int main() {
  char * json = "[123, [2345,  \n 1241.512, [123]]]";
  JsonValue * result;
  bool success = json_parse(&result, &json);
  printf("Success: ");
  printf(success ? "true\n" : "false\n");

  json_print(result, 0);
  return 0;
}
