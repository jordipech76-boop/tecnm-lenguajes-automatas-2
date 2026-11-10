# Bitácora de Requerimientos y Auditoría IA — Sesión 09
**Materia:** Lenguajes y Autómatas II (SCD-1016)  
**Tema:** Reporte Formal de Errores Semánticos  
**Docente Titular:** MCC. Iván Márquez Larios (`ijmarquezl`)  
**Estudiante:** Pech Ramírez Jordi Geovany  
**Matrícula:** 24531319  
**Track:** C (el harness evalúa C; la versión en Rust es opcional y no se califica con este harness)  

---

## 1. Fase 1: El Artesano (Dificultad Manual)
*Reto:* Diagnóstico con línea y columna  
*Describe qué problema o restricción de memoria/sintaxis identificaste al codificar a mano:*
> - **`sprintf` vs `snprintf`:** con `sprintf` no hay límite y si el mensaje es largo se sale del buffer. `snprintf` recibe el tamaño, nunca escribe más de `cap-1` caracteres y siempre pone el `'\0'`.
> - **El tipo de `cap`:** `snprintf` pide un `size_t` y `cap` es `int`, así que va con cast `(size_t)cap`, y antes reviso que `cap > 0`.
> - **`NULL` en un `%s`:** si `archivo` o `mensaje` vienen en `NULL`, el `%s` es comportamiento indefinido, así que los cambio por `""`.
> - **Formato exacto:** el harness compara el texto completo, así que no puede haber un espacio o un punto de más.

---

## 2. Fase 2: El Científico (Ingeniería de Requerimientos C-R-E-O)

### 2.1 Matriz de Especificación Formal
| Elemento C-R-E-O | Definición Técnica |
| :--- | :--- |
| **Contexto (C)** | C99 / GCC o Rust 2021 sin bibliotecas no estándar |
| **Rol (R)** | Ingeniero de software de sistemas y compiladores |
| **Especificación (E)** | Estructura estándar de errores GCC |
| **Output (O)** | Solo código modular y análisis de complejidad |

### 2.2 Prompt Estructurado
```text
C: C11, gcc -std=c11 -Wall -Wextra -Werror, Linux. api_sesion09.h ya existe y no
   se puede modificar.
R: Ingeniero de compiladores experto en C y diagnósticos de error.
E: Implementa exactamente
     void reportar(const char *archivo, int linea, int columna,
                   const char *mensaje, char *salida, int cap);
   1. Escribe en salida el formato exacto "archivo:linea:columna: error: mensaje",
      sin espacios extra ni punto final.
   2. Nunca escribas más de cap-1 caracteres; el buffer es del que llama.
   3. El resultado siempre termina en '\0'.
O: Solo el .c, empezando con #include "api_sesion09.h", sin main() y sin printf.
```

---

## 3. Código Devuelto por el Modelo (Gemini en Colab)
```c
#include "api_sesion09.h"
#include <stdio.h>
#include <stddef.h>

void reportar(const char *archivo, int linea, int columna, const char *mensaje, char *salida, int cap) {
    if (salida == NULL || cap <= 0) {
        return;
    }

    const char *safe_archivo = archivo ? archivo : "";
    const char *safe_mensaje = mensaje ? mensaje : "";

    snprintf(salida, (size_t)cap, "%s:%d:%d: error: %s", safe_archivo, linea, columna, safe_mensaje);
}
```

---

## 4. Fase 3: El Auditor (Resultados del Test Harness)
*Pruebas ejecutadas:* Múltiples errores sin abortar parser

### Salida del harness
```text
=== AUDITORIA SESION 09 - Diagnostico con linea y columna ===
Caso                               esperado:              obtenido:              resultado
[ 1] main.c:14:5
     esperado: main.c:14:5: error: tipo incompatible
     obtenido: main.c:14:5: error: tipo incompatible
     -> OK
[ 2] a.c:1:1
     esperado: a.c:1:1: error: x
     obtenido: a.c:1:1: error: x
     -> OK
[ 3] linea 0
     esperado: x.c:0:0: error: y
     obtenido: x.c:0:0: error: y
     -> OK
[ 4] cap=10 trunca                      esperado: main.c:14    obtenido: main.c:14    OK

RESULTADO: 4/4 pruebas superadas
TODO EN VERDE. Documenta tu auditoria en auditorias/auditoria_sesion09.md
```

* [x] Caso base y límites superados sin *Crash*.
* [x] No existen accesos fuera de límites ni *Memory Leaks*.
* [x] La complejidad teórica coincide con la implementada.

### Reporte Crítico
*¿Qué caso borde o problema omitió la IA y cómo lo solucionaste?*
> Artesano e IA sacaron 4/4 y el código de Gemini es casi igual al mío: los dos usan `snprintf` y cambian los `NULL` por `""`. Lo que **omitimos los dos** es que `snprintf` corta por **bytes**, no por letras. En UTF-8 una letra con acento (`ó`) ocupa 2 bytes, y si el corte cae en medio queda un carácter roto:
>
> ```text
> completo: a.lj:2:5: error: redeclaración
> cap=30 -> "a.lj:2:5: error: redeclaraci�"   (último byte C3 suelto)
> ```
>
> El harness no lo prueba porque sus mensajes no llevan acentos. Para corregirlo, después de cortar habría que retroceder mientras el último byte sea parte de una letra a medias (bytes `10xxxxxx` o un inicio `11xxxxxx` sin terminar).
>
> Otro detalle: acepta línea y columna negativas (`a.lj:-1:-3: error: x`), y en el compilador siempre empiezan en 1.
>
> **Complejidad:** O(n) en tiempo, donde n es el largo del mensaje (`snprintf` lo recorre una vez), y O(1) de memoria extra.

---
**Firma digital:** jordipech76-boop
