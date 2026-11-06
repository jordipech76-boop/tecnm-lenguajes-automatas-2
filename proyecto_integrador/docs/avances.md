# Avances del proyecto

**Equipo:** José Manuel, Oscar Mauricio, María Jesús, Jordi Geovany y Héctor Iván  
**Repositorio:** https://github.com/jordipech76-boop/tecnm-lenguajes-automatas-2  
**Tag actual:** ninguno todavía (el primero será `v0.1-hito1`)

Completa una sección antes de cada clase de avance. Tres bullets y el commit.

---

## Avance 0 · Sesión 05 · Hito 0

1. **Qué compila hoy** (comando exacto): `cd proyecto_integrador && make` genera `./lenguajito` sin warnings.
2. **Qué falla y por qué**: nada del compilador todavía; solo estaba la estructura que da el curso.
3. **Qué sigue**: leer `docs/gramatica.md`, repartir roles y empezar el lexer.
4. **Commit**: (pegar aquí el hash del commit donde se subió `proyecto_integrador/`)

---

## Avance 1 · Sesión 08

1. **Qué compila hoy** (comando exacto): `make` y `./lenguajito tests/propias/valido1.lj --tokens` imprime un token por línea con `línea:columna TIPO lexema` y `EOF` al final. Ignora espacios y comentarios `//`.
2. **Qué falla y por qué**: el lexer ya marca `S01` con línea y columna (`./lenguajito tests/propias/invalido1.lj --tokens`), pero `--ast`, `--check` y los demás modos todavía dicen "sin implementar" porque falta el parser.
3. **Qué sigue**: parser descendente recursivo de expresiones, declaraciones y asignaciones, y `--ast`, para tener `make test-hito1` en verde en la sesión 10.
4. **Commit**: (pegar aquí el hash del commit del lexer)

---

## Avance 2 · Sesión 15

1. **Qué compila hoy** (comando exacto):
2. **Qué falla y por qué**:
3. **Qué sigue**:
4. **Commit**:

---

## Avance 3 · Sesión 25

1. **Qué compila hoy** (comando exacto):
2. **Qué falla y por qué**:
3. **Qué sigue**:
4. **Commit**:

---

## Avance 4 · Sesión 35

1. **Qué compila hoy** (comando exacto):
2. **Qué falla y por qué**:
3. **Qué sigue**:
4. **Commit**:
