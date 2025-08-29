#ifndef DUCC_JSON_H
#define DUCC_JSON_H

#include "std.h"

struct JsonBuilder;
typedef struct JsonBuilder JsonBuilder;

JsonBuilder* jsonbuilder_new();
const char* jsonbuilder_get_output(JsonBuilder* b);

void jsonbuilder_object_start(JsonBuilder* b);
void jsonbuilder_object_end(JsonBuilder* b);
void jsonbuilder_object_member_start(JsonBuilder* b, const char* key);
void jsonbuilder_object_member_end(JsonBuilder* b);

void jsonbuilder_array_start(JsonBuilder* b);
void jsonbuilder_array_end(JsonBuilder* b);
void jsonbuilder_array_element_start(JsonBuilder* b);
void jsonbuilder_array_element_end(JsonBuilder* b);

void jsonbuilder_null(JsonBuilder* b);
void jsonbuilder_boolean(JsonBuilder* b, BOOL value);
void jsonbuilder_integer(JsonBuilder* b, int value);
void jsonbuilder_string(JsonBuilder* b, const char* value);

#define JSON_DUMP(x, f) \
    do { \
        JsonBuilder* _builder = jsonbuilder_new(); \
        f(_builder, (x)); \
        fprintf(stderr, "%s", jsonbuilder_get_output(_builder)); \
    } while (0)

#endif
