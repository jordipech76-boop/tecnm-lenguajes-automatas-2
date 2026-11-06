# Bitácora de Requerimientos y Auditoría IA — Sesión 06
**Materia:** Lenguajes y Autómatas II (SCD-1016)  
**Tema:** Sistema de Tipos Primitivos  
**Docente Titular:** MCC. Iván Márquez Larios (`ijmarquezl`)  
**Estudiante:** Pech Ramírez Jordi Geovany  
**Matrícula:** 24531319  
**Track:** C (el harness evalúa C; la versión en Rust es opcional y no se califica con este harness)  

---

## 1. Fase 1: El Artesano (Dificultad Manual)
*Reto:* Chequeo de compatibilidad de tipos  
*Describe qué problema o restricción de memoria/sintaxis identificaste al codificar a mano:*
> - **La regla de la jerarquía:** al principio no tenía claro hacia qué lado se permite. Se puede asignar a un tipo **igual o más ancho** (int → float sí, float → int no), y como el `enum` ya trae los valores en orden (CHAR=1, INT=2, FLOAT=3), basta con `destino >= origen`.
> - **TIPO_ERROR:** vale 0, así que con la comparación sola `compatible(INT, ERROR)` daría 1. Por eso hice la función `es_valido()` para descartar primero cualquier tipo que no sea CHAR, INT o FLOAT.
> - **`nombre_tipo`:** con un `switch` que regresa el texto de cada tipo, y `"desconocido"` si llega un valor fuera del enum.

---

## 2. Fase 2: El Científico (Ingeniería de Requerimientos C-R-E-O)

### 2.1 Matriz de Especificación Formal
| Elemento C-R-E-O | Definición Técnica |
| :--- | :--- |
| **Contexto (C)** | C99 / GCC o Rust 2021 sin bibliotecas no estándar |
| **Rol (R)** | Ingeniero de software de sistemas y compiladores |
| **Especificación (E)** | Matriz de compatibilidad estática |
| **Output (O)** | Solo código modular y análisis de complejidad |

### 2.2 Prompt Estructurado
```text
C: C11, gcc -std=c11 -Wall -Wextra -Werror, Linux. api_sesion06.h ya existe y no
   se puede modificar:
   typedef enum { TIPO_ERROR = 0, TIPO_CHAR = 1, TIPO_INT = 2, TIPO_FLOAT = 3 } Tipo;
R: Ingeniero de compiladores experto en C y verificación semántica de tipos.
E: Implementa int compatible(Tipo destino, Tipo origen) y
   const char *nombre_tipo(Tipo t):
   1. Jerarquía CHAR < INT < FLOAT.
   2. compatible = 1 si se asigna sin perder datos (destino igual o más ancho), 0 si no.
   3. Asignar a un tipo más angosto se rechaza (requiere cast explícito).
   4. TIPO_ERROR nunca es compatible con nada.
   5. nombre_tipo devuelve "char", "int", "float" o "error".
O: Solo el .c, empezando con #include "api_sesion06.h", sin main() y sin printf.
```

---

## 3. Código Devuelto por el Modelo
```c
#include "api_sesion06.h"

int compatible(Tipo destino, Tipo origen) {
    /* TIPO_ERROR nunca es compatible con nada */
    if (destino == TIPO_ERROR || origen == TIPO_ERROR) {
        return 0;
    }
    /* Jerarquia CHAR < INT < FLOAT: solo se permite ensanchar */
    return destino >= origen;
}

const char *nombre_tipo(Tipo t) {
    switch (t) {
        case TIPO_CHAR:  return "char";
        case TIPO_INT:   return "int";
        case TIPO_FLOAT: return "float";
        case TIPO_ERROR: return "error";
        default:         return "desconocido";
    }
}
```

---

## 4. Fase 3: El Auditor (Resultados del Test Harness)
*Pruebas ejecutadas:* Rechazo de asignaciones incompatibles

### Salida del harness
```text
=== AUDITORIA SESION 06 - Tabla de compatibilidad de tipos ===
Caso                               esperado:              obtenido:              resultado
[ 1] compatible(INT,INT)                esperado: 1            obtenido: 1            OK
[ 2] compatible(FLOAT,INT)              esperado: 1            obtenido: 1            OK
[ 3] compatible(INT,FLOAT)              esperado: 0            obtenido: 0            OK
[ 4] compatible(INT,ERROR)              esperado: 0            obtenido: 0            OK
[ 5] nombre_tipo(INT)                   esperado: int          obtenido: int          OK
[ 6] nombre_tipo(ERROR)                 esperado: error        obtenido: error        OK

RESULTADO: 6/6 pruebas superadas
TODO EN VERDE. Documenta tu auditoria en auditorias/auditoria_sesion06.md
```

* [x] Caso base y límites superados sin *Crash*.
* [x] No existen accesos fuera de límites ni *Memory Leaks*.
* [x] La complejidad teórica coincide con la implementada.

### Reporte Crítico
*¿Qué caso borde o problema omitió la IA y cómo lo solucionaste?*
> Artesano e IA sacaron 6/6. La IA solo descarta `TIPO_ERROR`, pero **no revisa que el tipo sea uno válido**. En C un `enum` es un entero, así que puede llegar un valor fuera de la lista (una variable sin inicializar o un tipo nuevo que se agregue después). El harness no lo prueba porque solo usa los 4 tipos del enum. Lo comprobé con un `main` de prueba:
>
> | Llamada | IA | Mi artesano |
> | :--- | :--- | :--- |
> | `compatible((Tipo)9, TIPO_INT)` | 1 | 0 |
> | `compatible((Tipo)-1, TIPO_CHAR)` | 1 | 0 |
>
> La IA acepta como compatible un tipo que no existe. Lo solucioné en mi versión revisando primero que los dos tipos sean válidos:
>
> ```c
> static int es_valido(Tipo t) {
>     return t == TIPO_CHAR || t == TIPO_INT || t == TIPO_FLOAT;
> }
> ```
>
> **Complejidad:** O(1) en tiempo y memoria; solo son comparaciones, sin ciclos ni `malloc`.

---
**Firma digital:** jordipech76-boop
