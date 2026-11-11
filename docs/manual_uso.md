# Manual de Uso

## 1. Compilar de Simple a SML
py src/compilador/compilador.py programas/simple/ejemplo.simple programas/sml/ejemplo.sml

## 2. Ejecutar Simulador en C
gcc src/simpletron/simpletron.c -o simpletron
./simpletron programas/sml/ejemplo.sml
