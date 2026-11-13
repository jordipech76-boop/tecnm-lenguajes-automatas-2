# Bitácora de Requerimientos y Auditoría IA — Sesión 12
**Materia:** Lenguajes y Autómatas II (SCD-1016)  
**Tema:** Atributos de Símbolos y Memoria  
**Docente Titular:** MCC. Iván Márquez Larios (`ijmarquezl`)  
**Estudiante:** Pech Ramírez Jordi Geovany  
**Matrícula:** 24531319  
**Track:** C (el harness evalúa C; la versión en Rust es opcional y no se califica con este harness)  

---

## 1. Fase 1: El Artesano (Dificultad Manual)
*Reto:* Estructura de registro de símbolos  
*Describe qué problema o restricción de memoria/sintaxis identificaste al codificar a mano:*
> - **Redondear hacia arriba:** el offset tiene que ser múltiplo del ancho. Si el cursor es 1 y el tipo es `int` (4), saco el resto (`1 % 4 = 1`) y le sumo lo que falta (3), así queda en 4.
> - **Qué regresar:** `asignar_offset` no regresa el offset, regresa dónde empieza la **siguiente** variable (`offset + ancho`). El offset se guarda en el registro.
> - **Comparar textos:** el tipo viene como texto, así que va con `strcmp`, no con `==`.
> - **`puntero`:** en x86-64 un puntero mide 8 bytes.

---

## 2. Fase 2: El Científico (Ingeniería de Requerimientos C-R-E-O)

### 2.1 Matriz de Especificación Formal
| Elemento C-R-E-O | Definición Técnica |
| :--- | :--- |
| **Contexto (C)** | C99 / GCC o Rust 2021 sin bibliotecas no estándar |
| **Rol (R)** | Ingeniero de software de sistemas y compiladores |
| **Especificación (E)** | Cálculo de desplazamientos y alineación |
| **Output (O)** | Solo código modular y análisis de complejidad |

### 2.2 Prompt Estructurado
```text
C: C11, gcc -std=c11 -Wall -Wextra -Werror, Linux. api_sesion12.h ya existe y no
   se puede modificar; ahí está struct Simbolo { const char *nombre;
   const char *tipo; int offset; }.
R: Ingeniero de compiladores experto en C y organización de memoria.
E: Implementa int ancho_de_tipo(const char *tipo) e
   int asignar_offset(Simbolo *simbolo, int cursor):
   1. Anchos: char 1, short 2, int 4, float 4, double 8, puntero 8; otro -> -1.
   2. asignar_offset redondea el cursor al múltiplo del ancho, guarda ese
      offset en el registro y devuelve offset + ancho.
   3. El cursor nunca retrocede.
   4. Registro NULL, cursor negativo o tipo desconocido -> -1.
O: Solo el .c, empezando con #include "api_sesion12.h", sin main() y sin printf.
```

---

## 3. Código Devuelto por el Modelo (Claude)
```c
#include "api_sesion12.h"
#include <string.h>

typedef struct {
    const char *nombre;
    int ancho;
} AnchoTipo;

static const AnchoTipo TIPOS[] = {
    {"char", 1}, {"short", 2}, {"int", 4},
    {"float", 4}, {"double", 8}, {"puntero", 8}
};

int ancho_de_tipo(const char *tipo) {
    if (tipo == NULL) {
        return -1;
    }
    for (size_t i = 0; i < sizeof(TIPOS) / sizeof(TIPOS[0]); i++) {
        if (strcmp(TIPOS[i].nombre, tipo) == 0) {
            return TIPOS[i].ancho;
        }
    }
    return -1;
}

int asignar_offset(Simbolo *simbolo, int cursor) {
    if (simbolo == NULL || cursor < 0) {
        return -1;
    }
    int ancho = ancho_de_tipo(simbolo->tipo);
    if (ancho < 0) {
        return -1;
    }
    /* Alinear: redondear cursor al siguiente multiplo de ancho */
    int alineado = (cursor + ancho - 1) / ancho * ancho;
    simbolo->offset = alineado;
    return alineado + ancho;
}
```

---

## 4. Fase 3: El Auditor (Resultados del Test Harness)
*Pruebas ejecutadas:* Alineación de tipos y tamaños en bytes

### Salida del harness
```text
=== AUDITORIA SESION 12 - Registro de simbolo y offsets alineados ===
Caso                               esperado:              obtenido:              resultado
[ 1] ancho_de_tipo("int")               esperado: 4            obtenido: 4            OK
[ 2] ancho_de_tipo("double")            esperado: 8            obtenido: 8            OK
[ 3] int en cursor 0                    esperado: 4            obtenido: 4            OK
[ 4] char e int: offset de b            esperado: 4            obtenido: 4            OK
[ 5] double desde cursor 4              esperado: 8            obtenido: 8            OK
[ 6] registro NULL                      esperado: -1           obtenido: -1           OK

RESULTADO: 6/6 pruebas superadas
TODO EN VERDE. Documenta tu auditoria en auditorias/auditoria_sesion12.md
```

* [ ] Caso base y límites superados sin *Crash*. (La versión de la IA da offsets negativos cerca de `INT_MAX`; ver reporte.)
* [x] No existen accesos fuera de límites ni *Memory Leaks*.
* [x] La complejidad teórica coincide con la implementada.

### Reporte Crítico
*¿Qué caso borde o problema omitió la IA y cómo lo solucionaste?*
> Artesano e IA sacaron 6/6. La IA redondea con `(cursor + ancho - 1) / ancho * ancho`, que es una fórmula correcta, pero **no revisa el desbordamiento**. Con un cursor muy grande la suma se pasa de `INT_MAX` y el offset sale negativo:
>
> ```text
> int    con cursor INT_MAX-2 -> regresa -2147483644, offset -2147483648
> double con cursor INT_MAX-5 -> regresa -2147483632, offset -2147483640
> ```
>
> Eso rompe la regla de que "el cursor nunca retrocede". Además, en C el desbordamiento de `int` es comportamiento indefinido. Mi artesano lo revisa antes de sumar y regresa -1:
>
> ```c
> if (cursor > INT_MAX - relleno - ancho) return -1;
> ```
>
> **Complejidad:** O(1); `ancho_de_tipo` compara contra una lista fija de 6 tipos.

---
**Firma digital:** jordipech76-boop