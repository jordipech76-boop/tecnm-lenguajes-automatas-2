/* semantica.c - tabla de simbolos con ambitos y diagnosticos (--check). */
#include "semantica.h"
#include "parser.h"   /* MAX_ERRORES */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* =========================================================
 * PARTE 2: ANÁLISIS SEMÁNTICO (--check)
 * ========================================================= */
typedef struct Symbol {
    char *name;
    bool initialized;
    struct Symbol *next;
} Symbol;

typedef struct Scope {
    Symbol *head;
    struct Scope *parent;
} Scope;

typedef struct {
    const char *filename;
    Scope *current_scope;
    int error_count;
} SemaCtx;

static void report_sema_error(SemaCtx *ctx, int line, int col, const char *code, const char *msg, const char *ident) {
    if (ctx->error_count >= MAX_ERRORES) return;
    if (ident) {
        printf("%s:%d:%d: error %s: %s '%s'\n", ctx->filename, line, col, code, msg, ident);
    } else {
        printf("%s:%d:%d: error %s: %s\n", ctx->filename, line, col, code, msg);
    }
    ctx->error_count++;
}

static void scope_push(SemaCtx *ctx) {
    Scope *s = (Scope *)calloc(1, sizeof(Scope));
    if (!s) exit(2);
    s->parent = ctx->current_scope;
    ctx->current_scope = s;
}

static void scope_pop(SemaCtx *ctx) {
    Scope *s = ctx->current_scope;
    if (!s) return;
    Symbol *sym = s->head;
    while (sym) {
        Symbol *next = sym->next;
        free(sym->name);
        free(sym);
        sym = next;
    }
    ctx->current_scope = s->parent;
    free(s);
}

static Symbol *scope_lookup_current(Scope *scope, const char *name) {
    for (Symbol *s = scope ? scope->head : NULL; s != NULL; s = s->next) {
        if (strcmp(s->name, name) == 0) return s;
    }
    return NULL;
}

static Symbol *scope_lookup_all(Scope *scope, const char *name) {
    for (Scope *sc = scope; sc != NULL; sc = sc->parent) {
        Symbol *found = scope_lookup_current(sc, name);
        if (found) return found;
    }
    return NULL;
}

/* Evalúa si una subexpresión es una constante con valor 0 (para E04) */
static bool is_const_zero(const ASTNode *expr, int *out_val) {
    if (!expr) return false;
    if (expr->type == AST_NUM) {
        if (out_val) *out_val = expr->num_val;
        return true;
    }
    if (expr->type == AST_NEG) {
        int v = 0;
        if (is_const_zero(expr->left, &v)) {
            if (out_val) *out_val = -v;
            return true;
        }
        return false;
    }
    if (expr->type == AST_BINARIA) {
        int l = 0, r = 0;
        if (is_const_zero(expr->left, &l) && is_const_zero(expr->right, &r)) {
            if (strcmp(expr->text, "+") == 0) { if (out_val) *out_val = l + r; return true; }
            if (strcmp(expr->text, "-") == 0) { if (out_val) *out_val = l - r; return true; }
            if (strcmp(expr->text, "*") == 0) { if (out_val) *out_val = l * r; return true; }
            if (strcmp(expr->text, "/") == 0 && r != 0) { if (out_val) *out_val = l / r; return true; }
        }
    }
    return false;
}

static void sema_check_expr(SemaCtx *ctx, const ASTNode *expr) {
    if (!expr) return;

    switch (expr->type) {
        case AST_NUM:
            break;

        case AST_VAR: {
            Symbol *sym = scope_lookup_all(ctx->current_scope, expr->text);
            if (!sym) {
                report_sema_error(ctx, expr->line, expr->col, "E01", "variable no declarada", expr->text);
            } else if (!sym->initialized) {
                report_sema_error(ctx, expr->line, expr->col, "E01", "variable leída antes de tener valor", expr->text);
            }
            break;
        }

        case AST_NEG:
            sema_check_expr(ctx, expr->left);
            break;

        case AST_BINARIA:
            sema_check_expr(ctx, expr->left);
            sema_check_expr(ctx, expr->right);
            if (strcmp(expr->text, "/") == 0) {
                int rval = 1;
                if (is_const_zero(expr->right, &rval) && rval == 0) {
                    report_sema_error(ctx, expr->line, expr->col, "E04", "división entre cero constante", NULL);
                }
            }
            break;

        default:
            break;
    }
}

static void sema_check_stmt(SemaCtx *ctx, const ASTNode *stmt) {
    if (!stmt) return;

    switch (stmt->type) {
        case AST_DECLARAR: {
            /* 1. Primero validamos la expresión derecha (para detectar `int x = x + 1;`) */
            if (stmt->left) {
                sema_check_expr(ctx, stmt->left);
            }
            /* 2. Verificamos si ya existe en el MISMO ámbito (E02) */
            Symbol *existing = scope_lookup_current(ctx->current_scope, stmt->text);
            if (existing) {
                report_sema_error(ctx, stmt->line, stmt->col, "E02", "redeclaración en el mismo ámbito de", stmt->text);
                if (stmt->left) {
                    existing->initialized = true;
                }
            } else {
                Symbol *sym = (Symbol *)calloc(1, sizeof(Symbol));
                if (!sym) exit(2);
                sym->name = lj_strdup(stmt->text);
                sym->initialized = (stmt->left != NULL);
                sym->next = ctx->current_scope->head;
                ctx->current_scope->head = sym;
            }
            break;
        }

        case AST_ASIGNAR: {
            sema_check_expr(ctx, stmt->left);
            Symbol *sym = scope_lookup_all(ctx->current_scope, stmt->text);
            if (!sym) {
                report_sema_error(ctx, stmt->line, stmt->col, "E01", "variable no declarada", stmt->text);
            } else {
                sym->initialized = true;
            }
            break;
        }

        case AST_IMPRIMIR:
            sema_check_expr(ctx, stmt->left);
            break;

        case AST_BLOQUE:
            scope_push(ctx);
            for (const ASTNode *s = stmt->extra; s != NULL; s = s->next) {
                sema_check_stmt(ctx, s);
            }
            scope_pop(ctx);
            break;

        case AST_SI:
            sema_check_expr(ctx, stmt->left);
            sema_check_stmt(ctx, stmt->right);
            if (stmt->extra) {
                sema_check_stmt(ctx, stmt->extra);
            }
            break;

        case AST_MIENTRAS:
            sema_check_expr(ctx, stmt->left);
            sema_check_stmt(ctx, stmt->right);
            break;

        default:
            break;
    }
}

int sema_check_program(const char *filename, const ASTNode *root, int initial_errors) {
    if (!root) return initial_errors;

    SemaCtx ctx = {
        .filename = filename ? filename : "<entrada>",
        .current_scope = NULL,
        .error_count = initial_errors
    };

    scope_push(&ctx); /* Ámbito global */
    for (const ASTNode *s = root->extra; s != NULL; s = s->next) {
        sema_check_stmt(&ctx, s);
    }
    scope_pop(&ctx);

    return ctx.error_count;
}