#include "ast.h"
#include "common.h"
#include "preprocess.h"

const char* storageclass_stringify(StorageClass s) {
    if (s == StorageClass_unspecified)
        return "";
    if (s == StorageClass_auto)
        return "auto";
    if (s == StorageClass_constexpr)
        return "constexpr";
    if (s == StorageClass_extern)
        return "extern";
    if (s == StorageClass_register)
        return "register";
    if (s == StorageClass_static)
        return "static";
    if (s == StorageClass_thread_local)
        return "thread_local";
    if (s == StorageClass_typedef)
        return "typedef";
    else
        unreachable();
}

const char* type_kind_stringify(TypeKind k) {
    if (k == TypeKind_unknown)
        return "<unknown>";
    else if (k == TypeKind_void)
        return "void";
    else if (k == TypeKind_char)
        return "char";
    else if (k == TypeKind_schar)
        return "signed char";
    else if (k == TypeKind_uchar)
        return "unsigned char";
    else if (k == TypeKind_short)
        return "short";
    else if (k == TypeKind_ushort)
        return "unsigned short";
    else if (k == TypeKind_int)
        return "int";
    else if (k == TypeKind_uint)
        return "unsigned int";
    else if (k == TypeKind_long)
        return "long";
    else if (k == TypeKind_ulong)
        return "unsigned long";
    else if (k == TypeKind_llong)
        return "long long";
    else if (k == TypeKind_ullong)
        return "unsigned long long";
    else if (k == TypeKind_float)
        return "float";
    else if (k == TypeKind_double)
        return "double";
    else if (k == TypeKind_ldouble)
        return "long double";
    else if (k == TypeKind_bool)
        return "bool";
    else if (k == TypeKind_struct)
        return "struct";
    else if (k == TypeKind_union)
        return "union";
    else if (k == TypeKind_enum)
        return "enum";
    else if (k == TypeKind_ptr)
        return "<pointer>";
    else if (k == TypeKind_array)
        return "<array>";
    else if (k == TypeKind_func)
        return "<function>";
    else
        unreachable();
}

static AstNode* members_of(Type* ty) {
    AstNode* def = &ty->ref.defs->as.list->items[ty->ref.index];
    if (def->kind == AstNodeKind_struct_def)
        return def->as.struct_def->members;
    else
        return def->as.union_def->members;
}

Type* type_new(TypeKind kind) {
    Type* ty = calloc(1, sizeof(Type));
    ty->kind = kind;
    return ty;
}

Type* type_dup(Type* src) {
    Type* ty = malloc(sizeof(Type));
    memcpy(ty, src, sizeof(Type));
    return ty;
}

Type* type_new_ptr(Type* base) {
    Type* ty = type_new(TypeKind_ptr);
    ty->base = base;
    return ty;
}

Type* type_new_array(Type* base, int size) {
    Type* ty = type_new(TypeKind_array);
    ty->base = base;
    ty->array_size = size;
    return ty;
}

Type* type_new_static_string(int len) {
    return type_new_array(type_new(TypeKind_char), len + 1);
}

Type* type_array_to_ptr(Type* ty) {
    return type_new_ptr(ty->base);
}

Type* type_new_func(Type* result, AstNode* params) {
    Type* ty = type_new(TypeKind_func);
    ty->result = result;
    ty->params = params;
    return ty;
}

bool type_is_unsized(Type* ty) {
    return ty->kind == TypeKind_void;
}

int type_sizeof(Type* ty) {
    if (type_is_unsized(ty)) {
        fatal_error("type_sizeof: type size cannot be determined");
    }

    if (ty->kind == TypeKind_char || ty->kind == TypeKind_schar || ty->kind == TypeKind_uchar ||
        ty->kind == TypeKind_bool)
        return 1;
    else if (ty->kind == TypeKind_short || ty->kind == TypeKind_ushort)
        return 2;
    else if (ty->kind == TypeKind_int || ty->kind == TypeKind_uint || ty->kind == TypeKind_float)
        return 4;
    else if (ty->kind == TypeKind_long || ty->kind == TypeKind_ulong || ty->kind == TypeKind_llong ||
             ty->kind == TypeKind_ullong || ty->kind == TypeKind_double)
        return 8;
    else if (ty->kind == TypeKind_struct)
        return type_sizeof_struct(ty);
    else if (ty->kind == TypeKind_union)
        return type_sizeof_union(ty);
    else if (ty->kind == TypeKind_enum)
        return 4;
    else if (ty->kind == TypeKind_ptr)
        return 8;
    else if (ty->kind == TypeKind_array)
        return type_sizeof(ty->base) * ty->array_size;
    else if (ty->kind == TypeKind_func)
        return 8;
    else
        unreachable();
}

int type_alignof(Type* ty) {
    if (type_is_unsized(ty)) {
        fatal_error("type_alignof: type size cannot be determined");
    }

    if (ty->kind == TypeKind_char || ty->kind == TypeKind_schar || ty->kind == TypeKind_uchar ||
        ty->kind == TypeKind_bool)
        return 1;
    else if (ty->kind == TypeKind_short || ty->kind == TypeKind_ushort)
        return 2;
    else if (ty->kind == TypeKind_int || ty->kind == TypeKind_uint || ty->kind == TypeKind_float)
        return 4;
    else if (ty->kind == TypeKind_long || ty->kind == TypeKind_ulong || ty->kind == TypeKind_llong ||
             ty->kind == TypeKind_ullong || ty->kind == TypeKind_double)
        return 8;
    else if (ty->kind == TypeKind_struct)
        return type_alignof_struct(ty);
    else if (ty->kind == TypeKind_union)
        return type_alignof_union(ty);
    else if (ty->kind == TypeKind_enum)
        return 4;
    else if (ty->kind == TypeKind_ptr)
        return 8;
    else if (ty->kind == TypeKind_array)
        return type_alignof(ty->base);
    else if (ty->kind == TypeKind_func)
        return 8;
    else
        unreachable();
}

int to_aligned(int n, int a) {
    return (n + a - 1) / a * a;
}

const char* astnode_kind_stringify(AstNodeKind k) {
    switch (k) {
    case AstNodeKind_unknown:
        return "unknown";
    case AstNodeKind_nop:
        return "nop";
    case AstNodeKind_array_initializer:
        return "array_initializer";
    case AstNodeKind_assign_expr:
        return "assign_expr";
    case AstNodeKind_binary_expr:
        return "binary_expr";
    case AstNodeKind_break_stmt:
        return "break_stmt";
    case AstNodeKind_case_label:
        return "case_label";
    case AstNodeKind_cast_expr:
        return "cast_expr";
    case AstNodeKind_cond_expr:
        return "cond_expr";
    case AstNodeKind_continue_stmt:
        return "continue_stmt";
    case AstNodeKind_default_label:
        return "default_label";
    case AstNodeKind_deref_expr:
        return "deref_expr";
    case AstNodeKind_do_while_stmt:
        return "do_while_stmt";
    case AstNodeKind_enum_def:
        return "enum_def";
    case AstNodeKind_enum_member:
        return "enum_member";
    case AstNodeKind_expr_stmt:
        return "expr_stmt";
    case AstNodeKind_for_stmt:
        return "for_stmt";
    case AstNodeKind_func:
        return "func";
    case AstNodeKind_func_call:
        return "func_call";
    case AstNodeKind_func_decl:
        return "func_decl";
    case AstNodeKind_func_def:
        return "func_def";
    case AstNodeKind_goto_stmt:
        return "goto_stmt";
    case AstNodeKind_gvar:
        return "gvar";
    case AstNodeKind_gvar_decl:
        return "gvar_decl";
    case AstNodeKind_if_stmt:
        return "if_stmt";
    case AstNodeKind_int_expr:
        return "int_expr";
    case AstNodeKind_double_expr:
        return "double_expr";
    case AstNodeKind_label_stmt:
        return "label_stmt";
    case AstNodeKind_list:
        return "list";
    case AstNodeKind_logical_expr:
        return "logical_expr";
    case AstNodeKind_lvar:
        return "lvar";
    case AstNodeKind_lvar_decl:
        return "lvar_decl";
    case AstNodeKind_param:
        return "param";
    case AstNodeKind_ref_expr:
        return "ref_expr";
    case AstNodeKind_return_stmt:
        return "return_stmt";
    case AstNodeKind_str_expr:
        return "str_expr";
    case AstNodeKind_struct_decl:
        return "struct_decl";
    case AstNodeKind_struct_def:
        return "struct_def";
    case AstNodeKind_struct_member:
        return "struct_member";
    case AstNodeKind_switch_stmt:
        return "switch_stmt";
    case AstNodeKind_type:
        return "type";
    case AstNodeKind_typedef_decl:
        return "typedef_decl";
    case AstNodeKind_unary_expr:
        return "unary_expr";
    case AstNodeKind_union_decl:
        return "union_decl";
    case AstNodeKind_union_def:
        return "union_def";
    case AstNodeKind_declarator:
        return "declarator";
    default:
        unreachable();
    }
}

AstNode* ast_new(AstNodeKind kind) {
    AstNode* ast = calloc(1, sizeof(AstNode));
    ast->kind = kind;
    return ast;
}

AstNode* ast_new_list(int capacity) {
    if (capacity == 0)
        unreachable();
    AstNode* list = ast_new(AstNodeKind_list);
    list->as.list = calloc(1, sizeof(ListNode));
    list->as.list->cap = capacity;
    list->as.list->len = 0;
    list->as.list->items = calloc(list->as.list->cap, sizeof(AstNode));
    return list;
}

void ast_append(AstNode* list, AstNode* item) {
    if (list->kind != AstNodeKind_list) {
        fatal_error("ast_append: ast is not a list");
    }
    if (!item) {
        return;
    }
    if (list->as.list->cap <= list->as.list->len) {
        list->as.list->cap *= 2;
        list->as.list->items = realloc(list->as.list->items, sizeof(AstNode) * list->as.list->cap);
        memset(list->as.list->items + list->as.list->len, 0,
               sizeof(AstNode) * (list->as.list->cap - list->as.list->len));
    }
    memcpy(list->as.list->items + list->as.list->len, item, sizeof(AstNode));
    ++list->as.list->len;
}

AstNode* ast_new_int(int v) {
    AstNode* e = ast_new(AstNodeKind_int_expr);
    e->as.int_expr = calloc(1, sizeof(IntExprNode));
    e->as.int_expr->value = v;
    e->ty = type_new(TypeKind_int);
    return e;
}

AstNode* ast_new_double(double v) {
    AstNode* e = ast_new(AstNodeKind_double_expr);
    e->as.double_expr = calloc(1, sizeof(DoubleExprNode));
    e->as.double_expr->value = v;
    e->ty = type_new(TypeKind_double);
    return e;
}

AstNode* ast_new_unary_expr(int op, AstNode* operand) {
    AstNode* e = ast_new(AstNodeKind_unary_expr);
    e->as.unary_expr = calloc(1, sizeof(UnaryExprNode));
    e->as.unary_expr->op = op;
    e->as.unary_expr->operand = operand;
    e->ty = type_new(TypeKind_int);
    return e;
}

AstNode* ast_new_binary_expr(int op, AstNode* lhs, AstNode* rhs) {
    AstNode* e = ast_new(AstNodeKind_binary_expr);
    e->as.binary_expr = calloc(1, sizeof(BinaryExprNode));
    e->as.binary_expr->op = op;
    e->as.binary_expr->lhs = lhs;
    e->as.binary_expr->rhs = rhs;
    if (op == TokenKind_plus) {
        if (lhs->ty->kind == TypeKind_ptr) {
            e->ty = lhs->ty;
        } else if (lhs->ty->kind == TypeKind_array) {
            e->ty = type_array_to_ptr(lhs->ty);
        } else if (rhs->ty->kind == TypeKind_ptr) {
            e->ty = rhs->ty;
        } else if (rhs->ty->kind == TypeKind_array) {
            e->ty = type_array_to_ptr(rhs->ty);
        } else {
            e->ty = type_new(TypeKind_int);
        }
    } else if (op == TokenKind_minus) {
        if (lhs->ty->kind == TypeKind_ptr) {
            e->ty = lhs->ty;
        } else if (lhs->ty->kind == TypeKind_array) {
            e->ty = type_array_to_ptr(lhs->ty);
        } else {
            e->ty = type_new(TypeKind_int);
        }
    } else {
        e->ty = type_new(TypeKind_int);
    }
    return e;
}

AstNode* ast_new_assign_expr(int op, AstNode* lhs, AstNode* rhs) {
    AstNode* e = ast_new(AstNodeKind_assign_expr);
    e->as.assign_expr = calloc(1, sizeof(AssignExprNode));
    e->as.assign_expr->op = op;
    e->as.assign_expr->lhs = lhs;
    e->as.assign_expr->rhs = rhs;
    e->ty = lhs->ty;
    return e;
}

AstNode* ast_new_assign_add_expr(AstNode* lhs, AstNode* rhs) {
    if (lhs->ty->base) {
        rhs = ast_new_binary_expr(TokenKind_star, rhs, ast_new_int(type_sizeof(lhs->ty->base)));
    } else if (rhs->ty->base) {
        lhs = ast_new_binary_expr(TokenKind_star, lhs, ast_new_int(type_sizeof(rhs->ty->base)));
    }
    return ast_new_assign_expr(TokenKind_assign_add, lhs, rhs);
}

AstNode* ast_new_assign_sub_expr(AstNode* lhs, AstNode* rhs) {
    if (lhs->ty->base) {
        rhs = ast_new_binary_expr(TokenKind_star, rhs, ast_new_int(type_sizeof(lhs->ty->base)));
    }
    return ast_new_assign_expr(TokenKind_assign_sub, lhs, rhs);
}

AstNode* ast_new_ref_expr(AstNode* operand) {
    AstNode* e = ast_new(AstNodeKind_ref_expr);
    e->as.ref_expr = calloc(1, sizeof(RefExprNode));
    e->as.ref_expr->operand = operand;
    e->ty = type_new_ptr(operand->ty);
    return e;
}

AstNode* ast_new_deref_expr(AstNode* operand) {
    AstNode* e = ast_new(AstNodeKind_deref_expr);
    e->as.deref_expr = calloc(1, sizeof(DerefExprNode));
    e->as.deref_expr->operand = operand;
    e->ty = operand->ty->base;
    return e;
}

AstNode* ast_new_member_access_expr(AstNode* obj, const char* name) {
    AstNode* e = ast_new(AstNodeKind_deref_expr);
    e->as.deref_expr = calloc(1, sizeof(DerefExprNode));
    e->as.deref_expr->operand =
        ast_new_binary_expr(TokenKind_plus, obj, ast_new_int(type_offsetof(obj->ty->base, name)));
    e->ty = type_member_typeof(obj->ty->base, name);
    e->as.deref_expr->operand->ty = type_new_ptr(e->ty);
    return e;
}

AstNode* ast_new_cast_expr(AstNode* operand, Type* result_ty) {
    AstNode* e = ast_new(AstNodeKind_cast_expr);
    e->as.cast_expr = calloc(1, sizeof(CastExprNode));
    e->as.cast_expr->operand = operand;
    e->ty = result_ty;
    return e;
}

AstNode* ast_new_logical_expr(int op, AstNode* lhs, AstNode* rhs) {
    AstNode* e = ast_new(AstNodeKind_logical_expr);
    e->as.logical_expr = calloc(1, sizeof(LogicalExprNode));
    e->as.logical_expr->op = op;
    e->as.logical_expr->lhs = lhs;
    e->as.logical_expr->rhs = rhs;
    e->ty = type_new(TypeKind_int);
    return e;
}

AstNode* ast_new_cond_expr(AstNode* cond, AstNode* then, AstNode* else_) {
    AstNode* e = ast_new(AstNodeKind_cond_expr);
    e->as.cond_expr = calloc(1, sizeof(CondExprNode));
    e->as.cond_expr->cond = cond;
    e->as.cond_expr->then = then;
    e->as.cond_expr->else_ = else_;
    e->ty = then->ty;
    return e;
}

AstNode* ast_new_str_expr(int idx, Type* ty) {
    AstNode* e = ast_new(AstNodeKind_str_expr);
    e->as.str_expr = calloc(1, sizeof(StrExprNode));
    e->as.str_expr->idx = idx;
    e->ty = ty;
    return e;
}

AstNode* ast_new_func_call(AstNode* func, AstNode* args) {
    AstNode* e = ast_new(AstNodeKind_func_call);
    e->as.func_call = calloc(1, sizeof(FuncCallNode));
    e->as.func_call->func = func;
    e->as.func_call->args = args;
    return e;
}

AstNode* ast_new_func(const char* name, Type* ty) {
    AstNode* e = ast_new(AstNodeKind_func);
    e->as.func = calloc(1, sizeof(FuncNode));
    e->as.func->name = name;
    e->ty = ty;
    return e;
}

AstNode* ast_new_gvar(const char* name, Type* ty) {
    AstNode* e = ast_new(AstNodeKind_gvar);
    e->as.gvar = calloc(1, sizeof(GvarNode));
    e->as.gvar->name = name;
    e->ty = ty;
    return e;
}

AstNode* ast_new_lvar(const char* name, int stack_offset, Type* ty) {
    AstNode* e = ast_new(AstNodeKind_lvar);
    e->as.lvar = calloc(1, sizeof(LvarNode));
    e->as.lvar->name = name;
    e->as.lvar->stack_offset = stack_offset;
    e->ty = ty;
    return e;
}

AstNode* ast_new_nop(void) {
    return ast_new(AstNodeKind_nop);
}

AstNode* ast_new_break_stmt(void) {
    return ast_new(AstNodeKind_break_stmt);
}

AstNode* ast_new_continue_stmt(void) {
    return ast_new(AstNodeKind_continue_stmt);
}

AstNode* ast_new_return_stmt(AstNode* expr) {
    AstNode* e = ast_new(AstNodeKind_return_stmt);
    e->as.return_stmt = calloc(1, sizeof(ReturnStmtNode));
    e->as.return_stmt->expr = expr;
    return e;
}

AstNode* ast_new_expr_stmt(AstNode* expr) {
    AstNode* e = ast_new(AstNodeKind_expr_stmt);
    e->as.expr_stmt = calloc(1, sizeof(ExprStmtNode));
    e->as.expr_stmt->expr = expr;
    return e;
}

AstNode* ast_new_if_stmt(AstNode* cond, AstNode* then, AstNode* else_) {
    AstNode* e = ast_new(AstNodeKind_if_stmt);
    e->as.if_stmt = calloc(1, sizeof(IfStmtNode));
    e->as.if_stmt->cond = cond;
    e->as.if_stmt->then = then;
    e->as.if_stmt->else_ = else_;
    return e;
}

AstNode* ast_new_for_stmt(AstNode* init, AstNode* cond, AstNode* update, AstNode* body) {
    AstNode* e = ast_new(AstNodeKind_for_stmt);
    e->as.for_stmt = calloc(1, sizeof(ForStmtNode));
    e->as.for_stmt->init = init;
    e->as.for_stmt->cond = cond;
    e->as.for_stmt->update = update;
    e->as.for_stmt->body = body;
    return e;
}

AstNode* ast_new_do_while_stmt(AstNode* cond, AstNode* body) {
    AstNode* e = ast_new(AstNodeKind_do_while_stmt);
    e->as.do_while_stmt = calloc(1, sizeof(DoWhileStmtNode));
    e->as.do_while_stmt->cond = cond;
    e->as.do_while_stmt->body = body;
    return e;
}

AstNode* ast_new_switch_stmt(AstNode* expr) {
    AstNode* e = ast_new(AstNodeKind_switch_stmt);
    e->as.switch_stmt = calloc(1, sizeof(SwitchStmtNode));
    e->as.switch_stmt->expr = expr;
    return e;
}

AstNode* ast_new_case_label(int value, AstNode* body) {
    AstNode* e = ast_new(AstNodeKind_case_label);
    e->as.case_label = calloc(1, sizeof(CaseLabelNode));
    e->as.case_label->value = value;
    e->as.case_label->body = body;
    return e;
}

AstNode* ast_new_default_label(AstNode* body) {
    AstNode* e = ast_new(AstNodeKind_default_label);
    e->as.default_label = calloc(1, sizeof(DefaultLabelNode));
    e->as.default_label->body = body;
    return e;
}

AstNode* ast_new_goto_stmt(const char* label) {
    AstNode* e = ast_new(AstNodeKind_goto_stmt);
    e->as.goto_stmt = calloc(1, sizeof(GotoStmtNode));
    e->as.goto_stmt->label = label;
    return e;
}

AstNode* ast_new_label_stmt(const char* name, AstNode* body) {
    AstNode* e = ast_new(AstNodeKind_label_stmt);
    e->as.label_stmt = calloc(1, sizeof(LabelStmtNode));
    e->as.label_stmt->name = name;
    e->as.label_stmt->body = body;
    return e;
}

AstNode* ast_new_declarator(const char* name, Type* ty) {
    AstNode* e = ast_new(AstNodeKind_declarator);
    e->as.declarator = calloc(1, sizeof(DeclaratorNode));
    e->as.declarator->name = name;
    e->ty = ty;
    return e;
}

AstNode* ast_new_func_def(const char* name, Type* ty, AstNode* params, AstNode* body, int stack_size) {
    AstNode* e = ast_new(AstNodeKind_func_def);
    e->as.func_def = calloc(1, sizeof(FuncDefNode));
    e->as.func_def->name = name;
    e->as.func_def->params = params;
    e->as.func_def->body = body;
    e->as.func_def->stack_size = stack_size;
    e->ty = ty;
    return e;
}

AstNode* ast_new_enum_member(const char* name, int value) {
    AstNode* e = ast_new(AstNodeKind_enum_member);
    e->as.enum_member = calloc(1, sizeof(EnumMemberNode));
    e->as.enum_member->name = name;
    e->as.enum_member->value = value;
    return e;
}

AstNode* ast_new_typedef_decl(const char* name, Type* ty) {
    AstNode* e = ast_new(AstNodeKind_typedef_decl);
    e->as.typedef_decl = calloc(1, sizeof(TypedefDeclNode));
    e->as.typedef_decl->name = name;
    e->ty = ty;
    return e;
}

AstNode* ast_new_struct_def(const char* name) {
    AstNode* e = ast_new(AstNodeKind_struct_def);
    e->as.struct_def = calloc(1, sizeof(StructDefNode));
    e->as.struct_def->name = name;
    return e;
}

AstNode* ast_new_union_def(const char* name) {
    AstNode* e = ast_new(AstNodeKind_union_def);
    e->as.union_def = calloc(1, sizeof(UnionDefNode));
    e->as.union_def->name = name;
    return e;
}

AstNode* ast_new_enum_def(const char* name) {
    AstNode* e = ast_new(AstNodeKind_enum_def);
    e->as.enum_def = calloc(1, sizeof(EnumDefNode));
    e->as.enum_def->name = name;
    return e;
}

AstNode* ast_new_array_initializer(AstNode* list) {
    AstNode* e = ast_new(AstNodeKind_array_initializer);
    e->as.array_initializer = calloc(1, sizeof(ArrayInitializerNode));
    e->as.array_initializer->list = list;
    return e;
}

int type_sizeof_struct(Type* ty) {
    int next_offset = 0;
    int struct_align = 0;

    for (int i = 0; i < members_of(ty)->as.list->len; ++i) {
        AstNode* member = &members_of(ty)->as.list->items[i];
        int size = type_sizeof(member->ty);
        int align = type_alignof(member->ty);

        next_offset = to_aligned(next_offset, align);
        next_offset += size;
        if (struct_align < align) {
            struct_align = align;
        }
    }
    return to_aligned(next_offset, struct_align);
}

int type_sizeof_union(Type* ty) {
    int union_size = 0;
    int union_align = 0;

    for (int i = 0; i < members_of(ty)->as.list->len; ++i) {
        AstNode* member = &members_of(ty)->as.list->items[i];
        int size = type_sizeof(member->ty);
        int align = type_alignof(member->ty);

        size = to_aligned(size, align);
        if (union_size < size) {
            union_size = size;
        }
        if (union_align < align) {
            union_align = align;
        }
    }
    return to_aligned(union_size, union_align);
}

int type_alignof_struct(Type* ty) {
    int struct_align = 0;

    for (int i = 0; i < members_of(ty)->as.list->len; ++i) {
        AstNode* member = &members_of(ty)->as.list->items[i];
        int align = type_alignof(member->ty);

        if (struct_align < align) {
            struct_align = align;
        }
    }
    return struct_align;
}

int type_alignof_union(Type* ty) {
    int union_align = 0;

    for (int i = 0; i < members_of(ty)->as.list->len; ++i) {
        AstNode* member = &members_of(ty)->as.list->items[i];
        int align = type_alignof(member->ty);

        if (union_align < align) {
            union_align = align;
        }
    }
    return union_align;
}

int type_offsetof(Type* ty, const char* name) {
    if (ty->kind == TypeKind_union) {
        return 0;
    }
    if (ty->kind != TypeKind_struct) {
        fatal_error("type_offsetof: type is neither a struct nor a union");
    }

    int next_offset = 0;

    for (int i = 0; i < members_of(ty)->as.list->len; ++i) {
        AstNode* member = &members_of(ty)->as.list->items[i];
        int size = type_sizeof(member->ty);
        int align = type_alignof(member->ty);

        next_offset = to_aligned(next_offset, align);
        if (strcmp(member->as.struct_member->name, name) == 0) {
            return next_offset;
        }
        next_offset += size;
    }

    fatal_error("type_offsetof: member not found");
}

Type* type_member_typeof(Type* ty, const char* name) {
    if (ty->kind != TypeKind_struct && ty->kind != TypeKind_union) {
        fatal_error("type_member_typeof: type is neither a struct nor a union");
    }

    for (int i = 0; i < members_of(ty)->as.list->len; ++i) {
        AstNode* member = &members_of(ty)->as.list->items[i];
        if (strcmp(member->as.struct_member->name, name) == 0) {
            return member->ty;
        }
    }

    fatal_error("type_offsetof: member not found");
}
