# 078 — Comentarios de CombatState

Se documentaron los cinco valores de `EVampsActionPhase` y los 19 campos de `FVampsCombatState` en `VampsCombatState.h`. Los comentarios explican qué guarda el personaje en cada campo, cuándo puede continuar el combo y qué permisos lee el resolver. Distinguen la fase del ataque de la reacción al recibir un contacto y la defensa armada de la posibilidad de usarla.

Se verificó que las declaraciones, los valores iniciales y la exposición a Blueprint quedaron intactos. Pasaron `clang-format` y `git diff --check`. [Guía del editor](editor.md).
