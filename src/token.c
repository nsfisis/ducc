#include "token.h"
#include "common.h"
#include "json.h"

const char* token_kind_stringify(TokenKind k) {
    if (k == TokenKind_eof)
        return "<eof>";
    else if (k == TokenKind_hash)
        return "#";
    else if (k == TokenKind_hashhash)
        return "##";
    else if (k == TokenKind_whitespace)
        return "<whitespace>";
    else if (k == TokenKind_newline)
        return "<new-line>";
    else if (k == TokenKind_other)
        return "<other>";
    else if (k == TokenKind_character_constant)
        return "<character-constant>";
    else if (k == TokenKind_header_name)
        return "<header-name>";
    else if (k == TokenKind_pp_directive_define)
        return "#define";
    else if (k == TokenKind_pp_directive_elif)
        return "#elif";
    else if (k == TokenKind_pp_directive_elifdef)
        return "#elifdef";
    else if (k == TokenKind_pp_directive_elifndef)
        return "#elifndef";
    else if (k == TokenKind_pp_directive_else)
        return "#else";
    else if (k == TokenKind_pp_directive_embed)
        return "#embed";
    else if (k == TokenKind_pp_directive_endif)
        return "#endif";
    else if (k == TokenKind_pp_directive_error)
        return "#error";
    else if (k == TokenKind_pp_directive_if)
        return "#if";
    else if (k == TokenKind_pp_directive_ifdef)
        return "#ifdef";
    else if (k == TokenKind_pp_directive_ifndef)
        return "#ifndef";
    else if (k == TokenKind_pp_directive_include)
        return "#include";
    else if (k == TokenKind_pp_directive_line)
        return "#line";
    else if (k == TokenKind_pp_directive_non_directive)
        return "#<non-directive>";
    else if (k == TokenKind_pp_directive_nop)
        return "#";
    else if (k == TokenKind_pp_directive_pragma)
        return "#pragma";
    else if (k == TokenKind_pp_directive_undef)
        return "#undef";
    else if (k == TokenKind_pp_directive_warning)
        return "#warning";
    else if (k == TokenKind_pp_operator_defined)
        return "defined";
    else if (k == TokenKind_pp_operator___has_c_attribute)
        return "__has_c_attribute";
    else if (k == TokenKind_pp_operator___has_embed)
        return "__has_embed";
    else if (k == TokenKind_pp_operator___has_include)
        return "__has_include";
    else if (k == TokenKind_keyword_alignas)
        return "alignas";
    else if (k == TokenKind_keyword_alignof)
        return "alignof";
    else if (k == TokenKind_keyword_auto)
        return "auto";
    else if (k == TokenKind_keyword_bool)
        return "bool";
    else if (k == TokenKind_keyword_break)
        return "break";
    else if (k == TokenKind_keyword_case)
        return "case";
    else if (k == TokenKind_keyword_char)
        return "char";
    else if (k == TokenKind_keyword_const)
        return "const";
    else if (k == TokenKind_keyword_constexpr)
        return "constexpr";
    else if (k == TokenKind_keyword_continue)
        return "continue";
    else if (k == TokenKind_keyword_default)
        return "default";
    else if (k == TokenKind_keyword_do)
        return "do";
    else if (k == TokenKind_keyword_double)
        return "double";
    else if (k == TokenKind_keyword_else)
        return "else";
    else if (k == TokenKind_keyword_enum)
        return "enum";
    else if (k == TokenKind_keyword_extern)
        return "extern";
    else if (k == TokenKind_keyword_false)
        return "false";
    else if (k == TokenKind_keyword_float)
        return "float";
    else if (k == TokenKind_keyword_for)
        return "for";
    else if (k == TokenKind_keyword_goto)
        return "goto";
    else if (k == TokenKind_keyword_if)
        return "if";
    else if (k == TokenKind_keyword_inline)
        return "inline";
    else if (k == TokenKind_keyword_int)
        return "int";
    else if (k == TokenKind_keyword_long)
        return "long";
    else if (k == TokenKind_keyword_nullptr)
        return "nullptr";
    else if (k == TokenKind_keyword_register)
        return "register";
    else if (k == TokenKind_keyword_restrict)
        return "restrict";
    else if (k == TokenKind_keyword_return)
        return "return";
    else if (k == TokenKind_keyword_short)
        return "short";
    else if (k == TokenKind_keyword_signed)
        return "signed";
    else if (k == TokenKind_keyword_sizeof)
        return "sizeof";
    else if (k == TokenKind_keyword_static)
        return "static";
    else if (k == TokenKind_keyword_static_assert)
        return "static_assert";
    else if (k == TokenKind_keyword_struct)
        return "struct";
    else if (k == TokenKind_keyword_switch)
        return "switch";
    else if (k == TokenKind_keyword_thread_local)
        return "thread_local";
    else if (k == TokenKind_keyword_true)
        return "true";
    else if (k == TokenKind_keyword_typedef)
        return "typedef";
    else if (k == TokenKind_keyword_typeof)
        return "typeof";
    else if (k == TokenKind_keyword_typeof_unqual)
        return "typeof_unqual";
    else if (k == TokenKind_keyword_union)
        return "union";
    else if (k == TokenKind_keyword_unsigned)
        return "unsigned";
    else if (k == TokenKind_keyword_void)
        return "void";
    else if (k == TokenKind_keyword_volatile)
        return "volatile";
    else if (k == TokenKind_keyword_while)
        return "while";
    else if (k == TokenKind_keyword__Atomic)
        return "_Atomic";
    else if (k == TokenKind_keyword__BitInt)
        return "_BitInt";
    else if (k == TokenKind_keyword__Complex)
        return "_Complex";
    else if (k == TokenKind_keyword__Decimal128)
        return "_Decimal128";
    else if (k == TokenKind_keyword__Decimal32)
        return "_Decimal32";
    else if (k == TokenKind_keyword__Decimal64)
        return "_Decimal64";
    else if (k == TokenKind_keyword__Generic)
        return "_Generic";
    else if (k == TokenKind_keyword__Imaginary)
        return "_Imaginary";
    else if (k == TokenKind_keyword__Noreturn)
        return "_Noreturn";
    else if (k == TokenKind_and)
        return "&";
    else if (k == TokenKind_andand)
        return "&&";
    else if (k == TokenKind_arrow)
        return "->";
    else if (k == TokenKind_assign)
        return "=";
    else if (k == TokenKind_assign_add)
        return "+=";
    else if (k == TokenKind_assign_and)
        return "&=";
    else if (k == TokenKind_assign_div)
        return "/=";
    else if (k == TokenKind_assign_lshift)
        return "<<=";
    else if (k == TokenKind_assign_mod)
        return "%=";
    else if (k == TokenKind_assign_mul)
        return "*=";
    else if (k == TokenKind_assign_or)
        return "|=";
    else if (k == TokenKind_assign_rshift)
        return ">>=";
    else if (k == TokenKind_assign_sub)
        return "-=";
    else if (k == TokenKind_assign_xor)
        return "^=";
    else if (k == TokenKind_brace_l)
        return "{";
    else if (k == TokenKind_brace_r)
        return "}";
    else if (k == TokenKind_bracket_l)
        return "[";
    else if (k == TokenKind_bracket_r)
        return "]";
    else if (k == TokenKind_colon)
        return ":";
    else if (k == TokenKind_comma)
        return ",";
    else if (k == TokenKind_dot)
        return ".";
    else if (k == TokenKind_ellipsis)
        return "...";
    else if (k == TokenKind_eq)
        return "==";
    else if (k == TokenKind_ge)
        return ">=";
    else if (k == TokenKind_gt)
        return ">";
    else if (k == TokenKind_ident)
        return "<identifier>";
    else if (k == TokenKind_le)
        return "le";
    else if (k == TokenKind_literal_int)
        return "<integer>";
    else if (k == TokenKind_literal_str)
        return "<string>";
    else if (k == TokenKind_lshift)
        return "<<";
    else if (k == TokenKind_lt)
        return "lt";
    else if (k == TokenKind_minus)
        return "-";
    else if (k == TokenKind_minusminus)
        return "--";
    else if (k == TokenKind_ne)
        return "!=";
    else if (k == TokenKind_not)
        return "!";
    else if (k == TokenKind_or)
        return "|";
    else if (k == TokenKind_oror)
        return "||";
    else if (k == TokenKind_paren_l)
        return "(";
    else if (k == TokenKind_paren_r)
        return ")";
    else if (k == TokenKind_percent)
        return "%";
    else if (k == TokenKind_plus)
        return "+";
    else if (k == TokenKind_plusplus)
        return "++";
    else if (k == TokenKind_question)
        return "?";
    else if (k == TokenKind_rshift)
        return ">>";
    else if (k == TokenKind_semicolon)
        return ";";
    else if (k == TokenKind_slash)
        return "/";
    else if (k == TokenKind_star)
        return "*";
    else if (k == TokenKind_tilde)
        return "~";
    else if (k == TokenKind_xor)
        return "^";
    else
        unreachable();
}

BOOL is_pp_directive(TokenKind k) {
    return k == TokenKind_pp_directive_define || k == TokenKind_pp_directive_elif ||
           k == TokenKind_pp_directive_elifdef || k == TokenKind_pp_directive_elifndef ||
           k == TokenKind_pp_directive_else || k == TokenKind_pp_directive_embed || k == TokenKind_pp_directive_endif ||
           k == TokenKind_pp_directive_error || k == TokenKind_pp_directive_if || k == TokenKind_pp_directive_ifdef ||
           k == TokenKind_pp_directive_ifndef || k == TokenKind_pp_directive_include ||
           k == TokenKind_pp_directive_line || k == TokenKind_pp_directive_non_directive ||
           k == TokenKind_pp_directive_nop || k == TokenKind_pp_directive_pragma || k == TokenKind_pp_directive_undef ||
           k == TokenKind_pp_directive_warning;
}

const char* token_stringify(Token* tok) {
    TokenKind k = tok->kind;
    if (k == TokenKind_pp_directive_non_directive) {
        char* buf = calloc(strlen(tok->value.string) + 1 + 1, sizeof(char));
        sprintf(buf, "#%s", tok->value.string);
        return buf;
    } else if (k == TokenKind_literal_int) {
        const char* kind_str = token_kind_stringify(k);
        char* buf = calloc(10 + strlen(kind_str) + 3 + 1, sizeof(char));
        sprintf(buf, "%d (%s)", tok->value.integer, kind_str);
        return buf;
    } else if (k == TokenKind_other || k == TokenKind_character_constant || k == TokenKind_ident ||
               k == TokenKind_literal_str) {
        const char* kind_str = token_kind_stringify(k);
        char* buf = calloc(strlen(tok->value.string) + strlen(kind_str) + 3 + 1, sizeof(char));
        sprintf(buf, "%s (%s)", tok->value.string, kind_str);
        return buf;
    } else {
        return token_kind_stringify(k);
    }
}

void token_build_json(JsonBuilder* builder, Token* tok) {
    jsonbuilder_object_start(builder);
    jsonbuilder_object_member_start(builder, "kind");
    jsonbuilder_string(builder, token_kind_stringify(tok->kind));
    jsonbuilder_object_member_end(builder);
    jsonbuilder_object_member_start(builder, "value");
    if (tok->kind == TokenKind_pp_directive_non_directive) {
        char* buf = calloc(strlen(tok->value.string) + 1 + 1, sizeof(char));
        sprintf(buf, "#%s", tok->value.string);
        jsonbuilder_string(builder, buf);
    } else if (tok->kind == TokenKind_literal_int) {
        jsonbuilder_integer(builder, tok->value.integer);
    } else if (tok->kind == TokenKind_other || tok->kind == TokenKind_character_constant ||
               tok->kind == TokenKind_ident || tok->kind == TokenKind_literal_str) {
        jsonbuilder_string(builder, tok->value.string);
    } else {
        jsonbuilder_null(builder);
    }
    jsonbuilder_object_member_end(builder);
    jsonbuilder_object_member_start(builder, "loc");
    sourcelocation_build_json(builder, &tok->loc);
    jsonbuilder_object_member_end(builder);
    jsonbuilder_object_end(builder);
}

void tokens_init(TokenArray* tokens, size_t capacity) {
    tokens->len = 0;
    tokens->capacity = capacity;
    tokens->data = calloc(tokens->capacity, sizeof(Token));
}

void tokens_reserve(TokenArray* tokens, size_t size) {
    if (size <= tokens->capacity)
        return;
    while (tokens->capacity < size) {
        tokens->capacity *= 2;
    }
    tokens->data = realloc(tokens->data, tokens->capacity * sizeof(Token));
    memset(tokens->data + tokens->len, 0, (tokens->capacity - tokens->len) * sizeof(Token));
}

Token* tokens_push_new(TokenArray* tokens) {
    tokens_reserve(tokens, tokens->len + 1);
    return &tokens->data[tokens->len++];
}

Token* tokens_pop(TokenArray* tokens) {
    if (tokens->len == 0) {
        return NULL;
    } else {
        return &tokens->data[--tokens->len];
    }
}

void tokens_build_json(JsonBuilder* builder, TokenArray* tokens) {
    jsonbuilder_object_start(builder);
    jsonbuilder_object_member_start(builder, "len");
    jsonbuilder_integer(builder, tokens->len);
    jsonbuilder_object_member_end(builder);
    jsonbuilder_object_member_start(builder, "data");
    jsonbuilder_array_start(builder);
    for (int i = 0; i < tokens->len; ++i) {
        jsonbuilder_array_element_start(builder);
        token_build_json(builder, &tokens->data[i]);
        jsonbuilder_array_element_end(builder);
    }
    jsonbuilder_array_end(builder);
    jsonbuilder_object_member_end(builder);
    jsonbuilder_object_end(builder);
}
