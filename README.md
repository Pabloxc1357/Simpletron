# Simulador Simpletron y Compilador Simple

## Estructura del Repositorio
- src/simpletron/: Codigo fuente del simulador C (simpletron.c)
- src/compilador/: Compilador Simple a SML en Python (compilador.py)
- src/expresiones/: Modulo de evaluacion de expresiones (evaluador.py)
- programas/simple/: Codigo fuente en lenguaje Simple (.simple)
- programas/sml/: Archivos traducidos a SML (.sml)
- docs/: Documentacion (diseno.md, requerimientos.md)
- pruebas/: Casos de prueba (entradas y salidas)
- evidencias/: Registros de ejecucion y ramas Git

## Instrucciones de Ejecucion
1. Traducir de Simple a SML:
   py src/compilador/compilador.py programas/simple/ejemplo.simple programas/sml/ejemplo.sml
2. Ejecutar Simulador en C:
   .\simpletron.exe programas/sml/ejemplo.sml
