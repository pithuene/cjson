#include <stdio.h>
#include "cjson.h"

int main() {
  char * json = "{ \"nested\": {\"test\": null, \"Hello World\": [123.52  ] \n}}";
  //char * json = "[ \"Hello World\", true, null, 123, [2345, undefined, \n 1241.512, [123, false]]]";
  JsonValue * result;
  bool success = json_parse(&result, &json);
  printf("Success: ");
  printf(success ? "true\n" : "false\n");

  json_print(result, 0);
  return 0;
}
