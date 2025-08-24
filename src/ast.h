#ifndef DUCC_AST_H
#define DUCC_AST_H

#include "std.h"

enum TypeKind {
    TypeKind_unknown,

    TypeKind_char,
    TypeKind_short,
    TypeKind_int,
    TypeKind_long,
    TypeKind_void,
    TypeKind_ptr,
    TypeKind_array,
    TypeKind_enum,
    TypeKind_struct,
    TypeKind_union,
};
typedef enum TypeKind TypeKind;

const char* type_kind_stringify(TypeKind k);

struct AstNode;

struct Type {
    TypeKind kind;
    // Check `base` instead of `kind` to test if the type is an array or a pointer.
    struct Type* base;
    int array_size;
    struct AstNode* def;
};
typedef struct Type Type;

void type_dump(Type* ty);
Type* type_new(TypeKind kind);
Type* type_new_ptr(Type* base);
Type* type_new_array(Type* elem, int size);
Type* type_new_static_string(int len);
Type* type_array_to_ptr(Type* ty);
BOOL type_is_unsized(Type* ty);

int type_sizeof_struct(Type* ty);
int type_sizeof_union(Type* ty);
int type_alignof_struct(Type* ty);
int type_alignof_union(Type* ty);
int type_offsetof(Type* ty, const char* name);
Type* type_member_typeof(Type* ty, const char* name);

int type_sizeof(Type* ty);
int type_alignof(Type* ty);

int to_aligned(int n, int a);

enum AstNodeKind {
    AstNodeKind_unknown,
    AstNodeKind_nop,

    AstNodeKind_assign_expr,
    AstNodeKind_binary_expr,
    AstNodeKind_break_stmt,
    AstNodeKind_cond_expr,
    AstNodeKind_continue_stmt,
    AstNodeKind_deref_expr,
    AstNodeKind_do_while_stmt,
    AstNodeKind_enum_def,
    AstNodeKind_enum_member,
    AstNodeKind_expr_stmt,
    AstNodeKind_for_stmt,
    AstNodeKind_func_call,
    AstNodeKind_func_decl,
    AstNodeKind_func_def,
    AstNodeKind_gvar,
    AstNodeKind_gvar_decl,
    AstNodeKind_if_stmt,
    AstNodeKind_int_expr,
    AstNodeKind_list,
    AstNodeKind_logical_expr,
    AstNodeKind_lvar,
    AstNodeKind_lvar_decl,
    AstNodeKind_param,
    AstNodeKind_ref_expr,
    AstNodeKind_return_stmt,
    AstNodeKind_str_expr,
    AstNodeKind_struct_decl,
    AstNodeKind_struct_def,
    AstNodeKind_struct_member,
    AstNodeKind_type,
    AstNodeKind_typedef_decl,
    AstNodeKind_unary_expr,
    AstNodeKind_union_decl,
    AstNodeKind_union_def,
    AstNodeKind_union_member,

    // Intermediate ASTs: they are used only in parsing, not for parse result.
    AstNodeKind_declarator,
};
typedef enum AstNodeKind AstNodeKind;

#define node_items __n1
#define node_len __i1
#define node_cap __i2
#define node_expr __n1
#define node_lhs __n1
#define node_rhs __n2
#define node_operand __n1
#define node_cond __n1
#define node_init __n2
#define node_update __n3
#define node_then __n2
#define node_else __n3
#define node_body __n4
#define node_members __n1
#define node_params __n1
#define node_args __n1
#define node_int_value __i1
#define node_idx __i1
#define node_op __i1
#define node_stack_offset __i1
#define node_stack_size __i1

struct AstNode {
    AstNodeKind kind;
    const char* name;
    Type* ty;
    struct AstNode* __n1;
    struct AstNode* __n2;
    struct AstNode* __n3;
    struct AstNode* __n4;
    int __i1;
    int __i2;
};
typedef struct AstNode AstNode;

struct Program {
    AstNode* funcs;
    AstNode* vars;
    const char** str_literals;
};
typedef struct Program Program;

AstNode* ast_new(AstNodeKind kind);
AstNode* ast_new_list(int capacity);
void ast_append(AstNode* list, AstNode* item);
AstNode* ast_new_int(int v);
AstNode* ast_new_unary_expr(int op, AstNode* operand);
AstNode* ast_new_binary_expr(int op, AstNode* lhs, AstNode* rhs);

AstNode* ast_new_assign_expr(int op, AstNode* lhs, AstNode* rhs);
AstNode* ast_new_assign_add_expr(AstNode* lhs, AstNode* rhs);
AstNode* ast_new_assign_sub_expr(AstNode* lhs, AstNode* rhs);
AstNode* ast_new_ref_expr(AstNode* operand);
AstNode* ast_new_deref_expr(AstNode* operand);
AstNode* ast_new_member_access_expr(AstNode* obj, const char* name);

#endif
