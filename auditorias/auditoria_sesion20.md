# Bitácora de Requerimientos y Auditoría IA — Sesión 20
**Materia:** Lenguajes y Autómatas II (SCD-1016)  
**Tema:** Hito 2: Analizador Semántico y Scopes  
**Docente Titular:** MCC. Iván Márquez Larios (`ijmarquezl`)  
**Estudiante:** Pech Ramírez Jordi Geovany  
**Matrícula:** 24531319  
**Track:** C (el harness evalúa C; la versión en Rust es opcional y no se califica con este harness)  

---

## 1. Fase 1: El Artesano (Dificultad Manual)
*Reto:* Integración de Tabla de Símbolos y AST  
*Describe qué problema o restricción de memoria/sintaxis identificaste al codificar a mano:*
> - **El nivel:** `profundidad_de` empieza en 0 en el ámbito actual y suma 1 cada vez que sube por `padre`. Regresa en el primer ámbito donde encuentra el nombre, así el más interno gana (*shadowing*).
> - **Dos errores distintos:** -2 si me pasan `NULL` y -1 si el nombre no existe. Al principio regresaba -1 en los dos casos.
> - **Contar sin repetir:** lo más difícil. Un nombre se cuenta solo si no aparece en un ámbito más interno **ni antes en el mismo ámbito**. Hice una función `tiene(a, nombre, hasta)` para revisar solo los primeros símbolos.
> - **`AMBITO_MAX`:** el arreglo tiene 8 lugares, así que nunca leo más allá aunque `total` diga otra cosa.

---

## 2. Fase 2: El Científico (Ingeniería de Requerimientos C-R-E-O)

### 2.1 Matriz de Especificación Formal
| Elemento C-R-E-O | Definición Técnica |
| :--- | :--- |
| **Contexto (C)** | C99 / GCC o Rust 2021 sin bibliotecas no estándar |
| **Rol (R)** | Ingeniero de software de sistemas y compiladores |
| **Especificación (E)** | Contrato de Validación Semántica Integral |
| **Output (O)** | Solo código modular y análisis de complejidad |

### 2.2 Prompt Estructurado
```text
C: C11, gcc -std=c11 -Wall -Wextra -Werror, Linux. api_sesion20.h ya existe y no
   se puede modificar: struct Ambito { const char *nombre;
   const char *simbolos[AMBITO_MAX]; int total; struct Ambito *padre; }
   con AMBITO_MAX = 8.
R: Ingeniero de compiladores experto en C y resolución de nombres.
E: Implementa int profundidad_de(const Ambito *actual, const char *nombre) e
   int contar_visibles(const Ambito *actual):
   1. profundidad_de devuelve el nivel donde está el nombre (0 = ámbito actual),
      -1 si no existe y -2 si algún argumento es NULL.
   2. El ámbito más interno siempre gana (shadowing).
   3. contar_visibles cuenta cada nombre distinto una sola vez en toda la cadena.
O: Solo el .c, empezando con #include "api_sesion20.h", sin main() y sin printf.
```

---

## 3. Código Devuelto por el Modelo (Claude)
```c
#include "api_sesion20.h"
#include <stddef.h>
#include <string.h>

static int contiene(const Ambito *ambito, const char *nombre) {
    for (int i = 0; i < ambito->total; i++) {
        if (strcmp(ambito->simbolos[i], nombre) == 0) {
            return 1;
        }
    }
    return 0;
}

int profundidad_de(const Ambito *actual, const char *nombre) {
    if (actual == NULL || nombre == NULL) {
        return -2;
    }
    int nivel = 0;
    for (const Ambito *a = actual; a != NULL; a = a->padre) {
        if (contiene(a, nombre)) {
            return nivel;
        }
        nivel++;
    }
    return -1;
}

int contar_visibles(const Ambito *actual) {
    if (actual == NULL) {
        return 0;
    }
    int cuenta = 0;
    for (const Ambito *a = actual; a != NULL; a = a->padre) {
        for (int i = 0; i < a->total; i++) {
            /* Se cuenta solo si ningun ambito mas interno ya lo tiene */
            int visto = 0;
            for (const Ambito *b = actual; b != a; b = b->padre) {
                if (contiene(b, a->simbolos[i])) {
                    visto = 1;
                    break;
                }
            }
            if (!visto) {
                cuenta++;
            }
        }
    }
    return cuenta;
}
```

---

## 4. Fase 3: El Auditor (Resultados del Test Harness)
*Pruebas ejecutadas:* Casos complejos de shadowing y ámbito

### Salida del harness
```text
=== AUDITORIA SESION 20 - Resolucion de nombres y visibilidad ===
Caso                               esperado:              obtenido:              resultado
[ 1] nombre en el ambito actual         esperado: 0            obtenido: 0            OK
[ 2] nombre dos niveles arriba          esperado: 2            obtenido: 2            OK
[ 3] shadowing gana el interno          esperado: 0            obtenido: 0            OK
[ 4] nombre no visible                  esperado: -1           obtenido: -1           OK
[ 5] visibles sin repetir               esperado: 3            obtenido: 3            OK
[ 6] ambito NULL                        esperado: -2           obtenido: -2           OK

RESULTADO: 6/6 pruebas superadas
TODO EN VERDE. Documenta tu auditoria en auditorias/auditoria_sesion20.md
```

* [ ] Caso base y límites superados sin *Crash*. (La versión de la IA se cae con un símbolo `NULL`; ver reporte.)
* [x] No existen accesos fuera de límites ni *Memory Leaks*.
* [x] La complejidad teórica coincide con la implementada.

### Reporte Crítico
*¿Qué caso borde o problema omitió la IA y cómo lo solucionaste?*
> Artesano e IA sacaron 6/6. La IA resuelve bien el *shadowing* entre ámbitos, pero **solo revisa repetidos en ámbitos más internos, no dentro del mismo**:
>
> | Caso | IA | Mi artesano |
> | :--- | :--- | :--- |
> | `"x"` declarado 2 veces en el mismo ámbito | 2 visibles | 1 visible |
> | un símbolo `NULL` en la tabla | se cae (*Segmentation fault*) | 2 visibles, sin error |
>
> 1. **Repetido en el mismo nivel:** el contrato dice "cada nombre una sola vez". Normalmente la redeclaración la frena antes el `E02`, pero si llega una tabla así, la IA cuenta de más. Lo solucioné revisando también los símbolos anteriores del mismo ámbito (`tiene(a, s, i)`).
> 2. **Símbolo `NULL`:** la IA hace `strcmp(NULL, ...)` y truena; en mi versión se salta.
>
> **Complejidad:** `profundidad_de` es O(p·k), con p ámbitos y k ≤ 8 símbolos cada uno. `contar_visibles` es O((p·k)²) en el peor caso, porque por cada símbolo revisa los ámbitos de adentro; con k ≤ 8 no pesa.

---
**Firma digital:** jordipech76-boop