enum TypeKind {
    TypeKind_unknown,

    TypeKind_char,
    TypeKind_int,
    TypeKind_long,
    TypeKind_void,
    TypeKind_ptr,
    TypeKind_array,
    TypeKind_enum,
    TypeKind_struct,
};
typedef enum TypeKind TypeKind;

struct AstNode;

struct Type {
    TypeKind kind;
    struct Type* to;
    int array_size;
    struct AstNode* def;
};
typedef struct Type Type;

Type* type_new(TypeKind kind) {
    Type* ty = calloc(1, sizeof(Type));
    ty->kind = kind;
    return ty;
}

Type* type_new_ptr(Type* to) {
    Type* ty = calloc(1, sizeof(Type));
    ty->kind = TypeKind_ptr;
    ty->to = to;
    return ty;
}

Type* type_new_array(Type* elem, int size) {
    Type* ty = calloc(1, sizeof(Type));
    ty->kind = TypeKind_array;
    ty->to = elem;
    ty->array_size = size;
    return ty;
}

Type* type_array_to_ptr(Type* ty) {
    return type_new_ptr(ty->to);
}

int type_is_unsized(Type* ty) {
    return ty->kind != TypeKind_void;
}

int type_sizeof_struct(Type* ty);
int type_alignof_struct(Type* ty);
int type_offsetof(Type* ty, const String* name);
Type* type_member_typeof(Type* ty, const String* name);

int type_sizeof(Type* ty) {
    if (!type_is_unsized(ty)) {
        fatal_error("type_sizeof: type size cannot be determined");
    }

    if (ty->kind == TypeKind_ptr) {
        return 8;
    } else if (ty->kind == TypeKind_char) {
        return 1;
    } else if (ty->kind == TypeKind_int) {
        return 4;
    } else if (ty->kind == TypeKind_long) {
        return 8;
    } else if (ty->kind == TypeKind_enum) {
        return 4;
    } else if (ty->kind == TypeKind_array) {
        return type_sizeof(ty->to) * ty->array_size;
    } else {
        return type_sizeof_struct(ty);
    }
}

int type_alignof(Type* ty) {
    if (!type_is_unsized(ty)) {
        fatal_error("type_alignof: type size cannot be determined");
    }

    if (ty->kind == TypeKind_ptr) {
        return 8;
    } else if (ty->kind == TypeKind_char) {
        return 1;
    } else if (ty->kind == TypeKind_int) {
        return 4;
    } else if (ty->kind == TypeKind_long) {
        return 8;
    } else if (ty->kind == TypeKind_enum) {
        return 4;
    } else if (ty->kind == TypeKind_array) {
        return type_alignof(ty->to);
    } else {
        return type_alignof_struct(ty);
    }
}

int to_aligned(int n, int a) {
    return (n + a - 1) / a * a;
}

enum AstNodeKind {
    AstNodeKind_unknown,
    AstNodeKind_nop,

    AstNodeKind_assign_expr,
    AstNodeKind_binary_expr,
    AstNodeKind_break_stmt,
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
};
typedef enum AstNodeKind AstNodeKind;

#define node_items __n1
#define node_len __i
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
#define node_int_value __i
#define node_idx __i
#define node_op __i
#define node_stack_offset __i

struct AstNode {
    AstNodeKind kind;
    String name;
    Type* ty;
    struct AstNode* __n1;
    struct AstNode* __n2;
    struct AstNode* __n3;
    struct AstNode* __n4;
    int __i;
};
typedef struct AstNode AstNode;

struct Program {
    AstNode* funcs;
    char** str_literals;
};
typedef struct Program Program;

AstNode* ast_new(AstNodeKind kind) {
    AstNode* ast = calloc(1, sizeof(AstNode));
    ast->kind = kind;
    return ast;
}

AstNode* ast_new_list(int capacity) {
    AstNode* list = ast_new(AstNodeKind_list);
    list->node_items = calloc(capacity, sizeof(AstNode));
    list->node_len = 0;
    return list;
}

void ast_append(AstNode* list, AstNode* item) {
    if (list->kind != AstNodeKind_list) {
        fatal_error("ast_append: ast is not a list");
    }
    if (!item) {
        return;
    }
    memcpy(list->node_items + list->node_len, item, sizeof(AstNode));
    ++list->node_len;
}

AstNode* ast_new_int(int v) {
    AstNode* e = ast_new(AstNodeKind_int_expr);
    e->node_int_value = v;
    e->ty = type_new(TypeKind_int);
    return e;
}

AstNode* ast_new_unary_expr(int op, AstNode* operand) {
    AstNode* e = ast_new(AstNodeKind_unary_expr);
    e->node_op = op;
    e->node_operand = operand;
    e->ty = type_new(TypeKind_int);
    return e;
}

AstNode* ast_new_binary_expr(int op, AstNode* lhs, AstNode* rhs) {
    AstNode* e = ast_new(AstNodeKind_binary_expr);
    e->node_op = op;
    e->node_lhs = lhs;
    e->node_rhs = rhs;
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
    e->node_op = op;
    e->node_lhs = lhs;
    e->node_rhs = rhs;
    e->ty = lhs->ty;
    return e;
}

AstNode* ast_new_assign_add_expr(AstNode* lhs, AstNode* rhs) {
    if (lhs->ty->kind == TypeKind_ptr || lhs->ty->kind == TypeKind_array) {
        rhs = ast_new_binary_expr(TokenKind_star, rhs, ast_new_int(type_sizeof(lhs->ty->to)));
    } else if (rhs->ty->kind == TypeKind_ptr || rhs->ty->kind == TypeKind_array) {
        lhs = ast_new_binary_expr(TokenKind_star, lhs, ast_new_int(type_sizeof(rhs->ty->to)));
    }
    return ast_new_assign_expr(TokenKind_assign_add, lhs, rhs);
}

AstNode* ast_new_assign_sub_expr(AstNode* lhs, AstNode* rhs) {
    if (lhs->ty->kind == TypeKind_ptr || lhs->ty->kind == TypeKind_array) {
        rhs = ast_new_binary_expr(TokenKind_star, rhs, ast_new_int(type_sizeof(lhs->ty->to)));
    }
    return ast_new_assign_expr(TokenKind_assign_sub, lhs, rhs);
}

AstNode* ast_new_ref_expr(AstNode* operand) {
    AstNode* e = ast_new(AstNodeKind_ref_expr);
    e->node_operand = operand;
    e->ty = type_new_ptr(operand->ty);
    return e;
}

AstNode* ast_new_deref_expr(AstNode* operand) {
    AstNode* e = ast_new(AstNodeKind_deref_expr);
    e->node_operand = operand;
    e->ty = operand->ty->to;
    return e;
}

AstNode* ast_new_member_access_expr(AstNode* obj, const String* name) {
    AstNode* e = ast_new(AstNodeKind_deref_expr);
    e->node_operand = ast_new_binary_expr(TokenKind_plus, obj, ast_new_int(type_offsetof(obj->ty->to, name)));
    e->ty = type_member_typeof(obj->ty->to, name);
    e->node_operand->ty = type_new_ptr(e->ty);
    return e;
}

int type_sizeof_struct(Type* ty) {
    int next_offset = 0;
    int struct_align = 0;
    int padding;

    int i;
    for (i = 0; i < ty->def->node_members->node_len; ++i) {
        AstNode* member = ty->def->node_members->node_items + i;
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

int type_alignof_struct(Type* ty) {
    int struct_align = 0;

    int i;
    for (i = 0; i < ty->def->node_members->node_len; ++i) {
        AstNode* member = ty->def->node_members->node_items + i;
        int align = type_alignof(member->ty);

        if (struct_align < align) {
            struct_align = align;
        }
    }
    return struct_align;
}

int type_offsetof(Type* ty, const String* name) {
    if (ty->kind != TypeKind_struct) {
        fatal_error("type_offsetof: type is not a struct");
    }

    int next_offset = 0;

    int i;
    for (i = 0; i < ty->def->node_members->node_len; ++i) {
        AstNode* member = ty->def->node_members->node_items + i;
        int size = type_sizeof(member->ty);
        int align = type_alignof(member->ty);

        next_offset = to_aligned(next_offset, align);
        if (string_equals(&member->name, name)) {
            return next_offset;
        }
        next_offset += size;
    }

    fatal_error("type_offsetof: member not found");
}

Type* type_member_typeof(Type* ty, const String* name) {
    if (ty->kind != TypeKind_struct) {
        fatal_error("type_offsetof: type is not a struct");
    }

    int i;
    for (i = 0; i < ty->def->node_members->node_len; ++i) {
        AstNode* member = ty->def->node_members->node_items + i;
        if (string_equals(&member->name, name)) {
            return member->ty;
        }
    }

    fatal_error("type_offsetof: member not found");
}
