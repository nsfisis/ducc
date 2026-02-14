#ifndef DUCC_AST_H
#define DUCC_AST_H

#include "ducc.h"

typedef enum {
    StorageClass_unspecified,
    StorageClass_auto,
    StorageClass_constexpr,
    StorageClass_extern,
    StorageClass_register,
    StorageClass_static,
    StorageClass_thread_local,
    StorageClass_typedef,
} StorageClass;

const char* storageclass_stringify(StorageClass s);

typedef enum {
    TypeKind_unknown,

    TypeKind_void,
    TypeKind_char,
    TypeKind_schar,
    TypeKind_uchar,
    TypeKind_short,
    TypeKind_ushort,
    TypeKind_int,
    TypeKind_uint,
    TypeKind_long,
    TypeKind_ulong,
    TypeKind_llong,
    TypeKind_ullong,
    TypeKind_float,
    TypeKind_double,
    TypeKind_ldouble,
    TypeKind_bool,
    TypeKind_struct,
    TypeKind_union,
    TypeKind_enum,
    TypeKind_ptr,
    TypeKind_array,
    TypeKind_func,
} TypeKind;

const char* type_kind_stringify(TypeKind k);

struct AstNode;
typedef struct AstNode AstNode;

typedef struct {
    AstNode* defs;
    size_t index;
} TypeRef;

typedef struct Type {
    TypeKind kind;
    StorageClass storage_class;
    // Check `base` instead of `kind` to test if the type is an array or a pointer.
    struct Type* base;
    int array_size;
    TypeRef ref;
    struct Type* result;
    AstNode* params;
} Type;

Type* type_new(TypeKind kind);
Type* type_dup(Type* src);
Type* type_new_ptr(Type* base);
Type* type_new_array(Type* elem, int size);
Type* type_new_static_string(int len);
Type* type_array_to_ptr(Type* ty);
Type* type_new_func(Type* result, AstNode* params);
bool type_is_unsized(Type* ty);

int type_sizeof_struct(Type* ty);
int type_sizeof_union(Type* ty);
int type_alignof_struct(Type* ty);
int type_alignof_union(Type* ty);
int type_offsetof(Type* ty, const char* name);
Type* type_member_typeof(Type* ty, const char* name);

int type_sizeof(Type* ty);
int type_alignof(Type* ty);

int to_aligned(int n, int a);

typedef enum {
    AstNodeKind_unknown,
    AstNodeKind_nop,

    AstNodeKind_array_initializer,
    AstNodeKind_assign_expr,
    AstNodeKind_binary_expr,
    AstNodeKind_break_stmt,
    AstNodeKind_case_label,
    AstNodeKind_cast_expr,
    AstNodeKind_cond_expr,
    AstNodeKind_continue_stmt,
    AstNodeKind_default_label,
    AstNodeKind_deref_expr,
    AstNodeKind_do_while_stmt,
    AstNodeKind_enum_def,
    AstNodeKind_enum_member,
    AstNodeKind_expr_stmt,
    AstNodeKind_for_stmt,
    AstNodeKind_func,
    AstNodeKind_func_call,
    AstNodeKind_func_decl,
    AstNodeKind_func_def,
    AstNodeKind_goto_stmt,
    AstNodeKind_gvar,
    AstNodeKind_gvar_decl,
    AstNodeKind_if_stmt,
    AstNodeKind_int_expr,
    AstNodeKind_double_expr,
    AstNodeKind_label_stmt,
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
    AstNodeKind_switch_stmt,
    AstNodeKind_type,
    AstNodeKind_typedef_decl,
    AstNodeKind_unary_expr,
    AstNodeKind_union_decl,
    AstNodeKind_union_def,

    // Intermediate ASTs: they are used only in parsing, not for parse result.
    AstNodeKind_declarator,
} AstNodeKind;

const char* astnode_kind_stringify(AstNodeKind k);

// Expression nodes
typedef struct {
    int value;
} IntExprNode;

typedef struct {
    double value;
} DoubleExprNode;

typedef struct {
    int idx;
} StrExprNode;

typedef struct {
    int op;
    AstNode* operand;
} UnaryExprNode;

typedef struct {
    int op;
    AstNode* lhs;
    AstNode* rhs;
} BinaryExprNode;

typedef struct {
    int op;
    AstNode* lhs;
    AstNode* rhs;
} LogicalExprNode;

typedef struct {
    int op;
    AstNode* lhs;
    AstNode* rhs;
} AssignExprNode;

typedef struct {
    AstNode* operand;
} CastExprNode;

typedef struct {
    AstNode* cond;
    AstNode* then;
    AstNode* else_;
} CondExprNode;

typedef struct {
    AstNode* operand;
} DerefExprNode;

typedef struct {
    AstNode* operand;
} RefExprNode;

typedef struct {
    AstNode* func;
    AstNode* args;
} FuncCallNode;

// Statement nodes
typedef struct {
    AstNode* cond;
    AstNode* then;
    AstNode* else_;
} IfStmtNode;

typedef struct {
    AstNode* init;
    AstNode* cond;
    AstNode* update;
    AstNode* body;
} ForStmtNode;

typedef struct {
    AstNode* cond;
    AstNode* body;
} DoWhileStmtNode;

typedef struct {
    AstNode* expr;
    AstNode* body;
} SwitchStmtNode;

typedef struct {
    int value;
    AstNode* body;
} CaseLabelNode;

typedef struct {
    AstNode* body;
} DefaultLabelNode;

typedef struct {
    const char* name;
    AstNode* body;
} LabelStmtNode;

typedef struct {
    AstNode* expr;
} ReturnStmtNode;

typedef struct {
    const char* label;
} GotoStmtNode;

typedef struct {
    AstNode* expr;
} ExprStmtNode;

// Declaration nodes
typedef struct {
    const char* name;
    AstNode* params;
    AstNode* body;
    int stack_size;
} FuncDefNode;

typedef struct {
    const char* name;
    int stack_offset;
} LvarNode;

typedef struct {
    AstNode* expr;
} LvarDeclNode;

typedef struct {
    const char* name;
    int stack_offset;
} ParamNode;

typedef struct {
    const char* name;
    AstNode* expr;
} GvarDeclNode;

typedef struct {
    const char* name;
    AstNode* members;
} StructDefNode;

typedef struct {
    const char* name;
    AstNode* members;
} UnionDefNode;

typedef struct {
    const char* name;
    AstNode* members;
} EnumDefNode;

typedef struct {
    const char* name;
    int value;
} EnumMemberNode;

typedef struct {
    const char* name;
    AstNode* init;
} DeclaratorNode;

typedef struct {
    const char* name;
} FuncNode;

typedef struct {
    const char* name;
} GvarNode;

typedef struct {
    const char* name;
} StructMemberNode;

typedef struct {
    const char* name;
} TypedefDeclNode;

typedef struct {
    AstNode* list;
} ArrayInitializerNode;

typedef struct {
    AstNode* items;
    int len;
    int cap;
} ListNode;

struct AstNode {
    AstNodeKind kind;
    Type* ty;
    union {
        IntExprNode* int_expr;
        DoubleExprNode* double_expr;
        StrExprNode* str_expr;
        UnaryExprNode* unary_expr;
        BinaryExprNode* binary_expr;
        LogicalExprNode* logical_expr;
        AssignExprNode* assign_expr;
        CastExprNode* cast_expr;
        CondExprNode* cond_expr;
        DerefExprNode* deref_expr;
        RefExprNode* ref_expr;
        FuncCallNode* func_call;
        IfStmtNode* if_stmt;
        ForStmtNode* for_stmt;
        DoWhileStmtNode* do_while_stmt;
        SwitchStmtNode* switch_stmt;
        CaseLabelNode* case_label;
        DefaultLabelNode* default_label;
        LabelStmtNode* label_stmt;
        ReturnStmtNode* return_stmt;
        GotoStmtNode* goto_stmt;
        ExprStmtNode* expr_stmt;
        FuncDefNode* func_def;
        LvarNode* lvar;
        LvarDeclNode* lvar_decl;
        ParamNode* param;
        GvarDeclNode* gvar_decl;
        StructDefNode* struct_def;
        UnionDefNode* union_def;
        EnumDefNode* enum_def;
        EnumMemberNode* enum_member;
        DeclaratorNode* declarator;
        FuncNode* func;
        GvarNode* gvar;
        StructMemberNode* struct_member;
        TypedefDeclNode* typedef_decl;
        ArrayInitializerNode* array_initializer;
        ListNode* list;
    } as;
};

typedef struct {
    AstNode* funcs;
    AstNode* vars;
    const char** str_literals;
} Program;

AstNode* ast_new(AstNodeKind kind);
AstNode* ast_new_list(int capacity);
void ast_append(AstNode* list, AstNode* item);
AstNode* ast_new_int(int v);
AstNode* ast_new_double(double v);
AstNode* ast_new_unary_expr(int op, AstNode* operand);
AstNode* ast_new_binary_expr(int op, AstNode* lhs, AstNode* rhs);

AstNode* ast_new_assign_expr(int op, AstNode* lhs, AstNode* rhs);
AstNode* ast_new_assign_add_expr(AstNode* lhs, AstNode* rhs);
AstNode* ast_new_assign_sub_expr(AstNode* lhs, AstNode* rhs);
AstNode* ast_new_ref_expr(AstNode* operand);
AstNode* ast_new_deref_expr(AstNode* operand);
AstNode* ast_new_member_access_expr(AstNode* obj, const char* name);
AstNode* ast_new_cast_expr(AstNode* operand, Type* result_ty);
AstNode* ast_new_logical_expr(int op, AstNode* lhs, AstNode* rhs);
AstNode* ast_new_cond_expr(AstNode* cond, AstNode* then, AstNode* else_);
AstNode* ast_new_str_expr(int idx, Type* ty);
AstNode* ast_new_func_call(AstNode* func, AstNode* args);
AstNode* ast_new_func(const char* name, Type* ty);
AstNode* ast_new_gvar(const char* name, Type* ty);
AstNode* ast_new_lvar(const char* name, int stack_offset, Type* ty);

AstNode* ast_new_nop(void);
AstNode* ast_new_break_stmt(void);
AstNode* ast_new_continue_stmt(void);
AstNode* ast_new_return_stmt(AstNode* expr);
AstNode* ast_new_expr_stmt(AstNode* expr);
AstNode* ast_new_if_stmt(AstNode* cond, AstNode* then, AstNode* else_);
AstNode* ast_new_for_stmt(AstNode* init, AstNode* cond, AstNode* update, AstNode* body);
AstNode* ast_new_do_while_stmt(AstNode* cond, AstNode* body);
AstNode* ast_new_switch_stmt(AstNode* expr);
AstNode* ast_new_case_label(int value, AstNode* body);
AstNode* ast_new_default_label(AstNode* body);
AstNode* ast_new_goto_stmt(const char* label);
AstNode* ast_new_label_stmt(const char* name, AstNode* body);

AstNode* ast_new_declarator(const char* name, Type* ty);
AstNode* ast_new_func_def(const char* name, Type* ty, AstNode* params, AstNode* body, int stack_size);
AstNode* ast_new_enum_member(const char* name, int value);
AstNode* ast_new_typedef_decl(const char* name, Type* ty);
AstNode* ast_new_struct_def(const char* name);
AstNode* ast_new_union_def(const char* name);
AstNode* ast_new_enum_def(const char* name);
AstNode* ast_new_array_initializer(AstNode* list);

#endif
