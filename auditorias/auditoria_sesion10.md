# Bitácora de Requerimientos y Auditoría IA — Sesión 10
**Materia:** Lenguajes y Autómatas II (SCD-1016)  
**Tema:** Hito 1: Mini-Evaluador de Expresiones  
**Docente Titular:** MCC. Iván Márquez Larios (`ijmarquezl`)  
**Estudiante:** Pech Ramírez Jordi Geovany  
**Matrícula:** 24531319  
**Track:** C (el harness evalúa C; la versión en Rust es opcional y no se califica con este harness)  

---

## 1. Fase 1: El Artesano (Dificultad Manual)
*Reto:* Integración AST y Tipado Estático  
*Describe qué problema o restricción de memoria/sintaxis identificaste al codificar a mano:*
> - **La precedencia:** al principio no entendía por qué hay tres funciones. `expr` maneja `+ -`, `termino` maneja `* /` y `primaria` maneja números y paréntesis. Como `expr` llama a `termino`, la multiplicación se resuelve primero.
> - **Asociatividad:** con recursión `20-5-3` daba 18. Con un ciclo que va acumulando en `v` da 12, que es lo correcto.
> - **Los espacios:** hay que llamar `saltar()` antes de cada número u operador, si no `"  7 "` falla.
> - **Hacerlo corto:** mi primera versión quedaba muy larga; la compacté usando `for` en lugar de `while` y juntando condiciones, cuidando que siguiera dando 6/6.

---

## 2. Fase 2: El Científico (Ingeniería de Requerimientos C-R-E-O)

### 2.1 Matriz de Especificación Formal
| Elemento C-R-E-O | Definición Técnica |
| :--- | :--- |
| **Contexto (C)** | C99 / GCC o Rust 2021 sin bibliotecas no estándar |
| **Rol (R)** | Ingeniero de software de sistemas y compiladores |
| **Especificación (E)** | Contrato formal de Frontend Semántico |
| **Output (O)** | Solo código modular y análisis de complejidad |

### 2.2 Prompt Estructurado
```text
C: C11, gcc -std=c11 -Wall -Wextra -Werror, Linux. api_sesion10.h ya existe y no
   se puede modificar. Ya existen el struct Parser { const char *s; int error; },
   la función saltar() que salta espacios y evaluar_expresion(), que no se tocan.
R: Ingeniero de compiladores experto en C y parsers descendentes recursivos.
E: Completa long primaria(Parser *p), long termino(Parser *p) y long expr(Parser *p):
   1. primaria: si hay '(' evalúa expr y exige ')'; si hay dígito lee el entero;
      si no, p->error = 2.
   2. termino: primaria seguida de cero o más '*' o '/' primaria.
   3. expr: termino seguido de cero o más '+' o '-' termino.
   4. Asociatividad a la izquierda (20-5-3 = 12) y espacios permitidos.
   5. División entre cero: p->error = 1 sin ejecutar la división.
   6. Sin variables ni menos unario.
O: Solo el archivo .c completo, empezando con #include "api_sesion10.h",
   sin main() y sin printf.
```

---

## 3. Código Devuelto por el Modelo (Gemini en Colab)
```c
long primaria(Parser *p) {
    saltar(p);
    if (p->error != 0) return 0;
    if (*p->s == '(') {
        p->s++;
        long v = expr(p);
        saltar(p);
        if (*p->s != ')') { if (p->error == 0) p->error = 2; return 0; }
        p->s++;
        return v;
    }
    if (isdigit((unsigned char)*p->s)) {
        long v = 0;
        while (isdigit((unsigned char)*p->s)) { v = v * 10 + (*p->s - '0'); p->s++; }
        return v;
    }
    p->error = 2;
    return 0;
}

long termino(Parser *p) {
    long v = primaria(p);
    saltar(p);
    while (p->error == 0 && (*p->s == '*' || *p->s == '/')) {
        char op = *p->s;
        p->s++;
        long d = primaria(p);
        if (p->error != 0) return 0;
        if (op == '*') {
            v = v * d;
        } else {
            if (d == 0) { p->error = 1; return 0; }
            v = v / d;
        }
        saltar(p);
    }
    return v;
}

long expr(Parser *p) {
    long v = termino(p);
    saltar(p);
    while (p->error == 0 && (*p->s == '+' || *p->s == '-')) {
        char op = *p->s;
        p->s++;
        long d = termino(p);
        if (p->error != 0) return 0;
        v = (op == '+') ? v + d : v - d;
        saltar(p);
    }
    return v;
}
```
*(También devolvió el `struct Parser`, `saltar()` y `evaluar_expresion()` sin cambios respecto a la plantilla.)*

---

## 4. Fase 3: El Auditor (Resultados del Test Harness)
*Pruebas ejecutadas:* Batería de 10 expresiones mixtas complejas

### Salida del harness
```text
=== AUDITORIA SESION 10 - Mini-evaluador de expresiones (Hito 1) ===
Caso                               esperado:              obtenido:              resultado
[ 1] "2+3*4"                            esperado: 14           obtenido: 14           OK
[ 2] "(2+3)*4"                          esperado: 20           obtenido: 20           OK
[ 3] "10/0" -> error 1                  esperado: 1            obtenido: 1            OK
[ 4] "2+" -> error 2                    esperado: 2            obtenido: 2            OK
[ 5] "  7 "                             esperado: 7            obtenido: 7            OK
[ 6] "5-10"                             esperado: -5           obtenido: -5           OK

RESULTADO: 6/6 pruebas superadas
TODO EN VERDE. Documenta tu auditoria en auditorias/auditoria_sesion10.md
```

* [ ] Caso base y límites superados sin *Crash*. (Se cae con paréntesis anidados muy profundos; ver reporte.)
* [x] No existen accesos fuera de límites ni *Memory Leaks*.
* [x] La complejidad teórica coincide con la implementada.

### Reporte Crítico
*¿Qué caso borde o problema omitió la IA y cómo lo solucionaste?*
> Artesano e IA sacaron 6/6, y la lógica de Gemini es la misma que la mía (la mía es más compacta). Probé casos que el harness no tiene y **los dos** fallan igual:
>
> | Entrada | Resultado | Problema |
> | :--- | :--- | :--- |
> | `99999999999999999999` | `7766279631452241919`, error 0 | el número no cabe en `long` y da otro sin avisar |
> | `9999999999*9999999999` | `7766279611452241921`, error 0 | la multiplicación se desborda |
> | 2 millones de `(` | `Segmentation fault` | cada `(` es una llamada recursiva y se llena la pila |
>
> **Cómo lo solucionaría:**
> - Desbordamiento: antes de `v * 10 + dígito` (y de cada `*`, `+`, `-`) revisar contra `LONG_MAX` de `<limits.h>` y marcar error.
> - Pila: llevar un contador de profundidad en el `Parser` y marcar error 2 si pasa de un límite (por ejemplo, 1000 paréntesis).
>
> No los apliqué porque cambian el struct de la plantilla y el contrato no define un código para desbordamiento.
>
> **Complejidad:** O(n) en tiempo (cada carácter se lee una vez) y O(p) de pila, donde p es la profundidad de los paréntesis.

---
**Firma digital:** jordipech76-boop
