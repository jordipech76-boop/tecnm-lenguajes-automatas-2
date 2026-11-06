# Compilador de Lenguajito · Proyecto Integrador

**Curso:** Lenguajes y Autómatas II (SCD-1016)  
**Equipo:** José Manuel, Oscar Mauricio, María Jesús, Jordi Geovany y Héctor Iván  
**Tag actual:** `v0.1-hito1`

Compilador escrito en C11 (`gcc -std=c11 -Wall -Wextra -Werror`, solo `libc`) con un parser descendente recursivo LL(1) hecho a mano. Las reglas están en [`../PROYECTO.md`](../PROYECTO.md) y el lenguaje en [`docs/gramatica.md`](docs/gramatica.md).

## Cómo se usa

```bash
cd proyecto_integrador
make                 # construye ./lenguajito
make test-hito1      # pruebas del Hito 1 (4/4)

./lenguajito tests/propias/valido1.lj --tokens   # un token por línea
./lenguajito tests/propias/valido2.lj --ast      # árbol en una línea
./lenguajito tests/propias/invalido2.lj --check  # diagnósticos, sale con 1
```

## Qué funciona (Hito 1)

- `--tokens`: `línea:columna TIPO lexema`, ignora espacios y comentarios `//`, termina en `EOF` y marca `S01` en caracteres inválidos.
- `--ast`: declaraciones, asignaciones, `if/else`, `while`, `print`, bloques y expresiones con precedencia; el `-` unario es `(neg ...)` y el `+` unario no genera nodo.
- `--check`: `S01` de sintaxis con recuperación (varios errores por pasada, máximo 20), `E01`, `E02` y `E04`, en `stdout` con el formato `archivo:línea:columna: error EXX: mensaje`.

## Qué no funciona todavía

- `--run` (Hito 2), `--tac` (Hito 3) y `--asm` (Hito 4): responden "todavía sin implementar" y salen con 1.
- Un error de sintaxis descarta la sentencia completa, así que puede aparecer un `E01` de más después (por ejemplo, `int x = 5 # 2;` y luego usar `x`).

## Archivos

| Archivo | Qué hace | Responsable |
| :--- | :--- | :--- |
| `src/main.c`, `src/compilador.h` | interfaz de línea de comandos (vienen del curso) | — |
| `src/compilador.c` | conecta las etapas según el modo | Jordi |
| `src/lexer.h`, `src/lexer.c` | tokens con línea y columna | María |
| `src/parser.h`, `src/parser.c` | parser LL(1): expresiones (Oscar) y sentencias (Jordi) | Oscar y Jordi |
| `src/ast.h`, `src/ast.c` | nodos del AST, impresión `--ast` y liberación de memoria | Jordi |
| `src/semantica.h`, `src/semantica.c` | tabla de símbolos con ámbitos y `--check` | José Manuel |
| `tests/propias/` | pruebas del equipo | Héctor |
| `docs/avances.md`, `docs/entrega_hito1.md` | avances y bitácora de IA | María y Héctor |
