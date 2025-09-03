#include "json.h"
#include "common.h"

struct JsonBuilder {
    StrBuilder buf;
    // not supported by ducc for now
    // bool has_output_element[256];
    bool* has_output_element;
    int depth;
};

JsonBuilder* jsonbuilder_new() {
    JsonBuilder* b = calloc(1, sizeof(JsonBuilder));
    strbuilder_init(&b->buf);
    b->has_output_element = calloc(256, sizeof(bool));
    return b;
}

const char* jsonbuilder_get_output(JsonBuilder* b) {
    return b->buf.buf;
}

static void emit_comma_if_needed(JsonBuilder* b) {
    if (b->has_output_element[b->depth - 1]) {
        strbuilder_append_char(&b->buf, ',');
    }
}

void jsonbuilder_object_start(JsonBuilder* b) {
    strbuilder_append_char(&b->buf, '{');
    b->has_output_element[b->depth++] = false;
}

void jsonbuilder_object_end(JsonBuilder* b) {
    b->depth--;
    strbuilder_append_char(&b->buf, '}');
}

void jsonbuilder_object_member_start(JsonBuilder* b, const char* key) {
    emit_comma_if_needed(b);
    jsonbuilder_string(b, key);
    strbuilder_append_char(&b->buf, ':');
}

void jsonbuilder_object_member_end(JsonBuilder* b) {
    b->has_output_element[b->depth - 1] = true;
}

void jsonbuilder_array_start(JsonBuilder* b) {
    strbuilder_append_char(&b->buf, '[');
    b->has_output_element[b->depth++] = false;
}

void jsonbuilder_array_end(JsonBuilder* b) {
    b->depth--;
    strbuilder_append_char(&b->buf, ']');
}

void jsonbuilder_array_element_start(JsonBuilder* b) {
    emit_comma_if_needed(b);
}

void jsonbuilder_array_element_end(JsonBuilder* b) {
    b->has_output_element[b->depth - 1] = true;
}

void jsonbuilder_null(JsonBuilder* b) {
    strbuilder_append_string(&b->buf, "null");
}

void jsonbuilder_boolean(JsonBuilder* b, bool value) {
    strbuilder_append_string(&b->buf, value ? "true" : "false");
}

void jsonbuilder_integer(JsonBuilder* b, int value) {
    char buf[32];
    sprintf(buf, "%d", value);
    strbuilder_append_string(&b->buf, buf);
}

void jsonbuilder_string(JsonBuilder* b, const char* value) {
    if (!value) {
        jsonbuilder_null(b);
        return;
    }

    strbuilder_append_char(&b->buf, '"');

    for (const char* p = value; *p; p++) {
        if (*p == '"') {
            strbuilder_append_string(&b->buf, "\\\"");
        } else if (*p == '\\') {
            strbuilder_append_string(&b->buf, "\\\\");
        } else if (*p == '\b') {
            strbuilder_append_string(&b->buf, "\\b");
        } else if (*p == '\f') {
            strbuilder_append_string(&b->buf, "\\f");
        } else if (*p == '\n') {
            strbuilder_append_string(&b->buf, "\\n");
        } else if (*p == '\r') {
            strbuilder_append_string(&b->buf, "\\r");
        } else if (*p == '\t') {
            strbuilder_append_string(&b->buf, "\\t");
        } else {
            strbuilder_append_char(&b->buf, *p);
        }
    }

    strbuilder_append_char(&b->buf, '"');
}
