# Roadmap de prototipos

## 1. Duelo contra la IA

Un jugador se enfrenta a un rival controlado por la computadora en una arena de prueba. Ambos usan el [alcance de la primera versión del combate](combat/functional.md#primera-versión).

Esta etapa reúne cadenas de hasta tres golpes, pulsaciones con buffer, bloqueo continuado, parry, modo NA y salidas mediante movimiento entre golpes, junto con lock-on y cámara. La recuperación visual no inmoviliza al personaje. La lógica de combate acepta acciones tanto del jugador como de la IA. Dash, stances y RPS quedan fuera de esta demo.

**Criterio de salida:** se pueden ejecutar y reiniciar los casos funcionales; las reglas permiten explicar cada golpe, defensa y oportunidad de respuesta. Implementar según los [accionables del núcleo local](combat/technical.md#accionables-núcleo-local).

## 2. Duelo online

Dos jugadores disputan el mismo duelo desde clientes separados, manteniendo el alcance de la etapa anterior. Se ejecutan los [accionables de predicción y autoridad](combat/technical.md#accionables-predicción-y-autoridad) y la [validación online](combat/technical.md#accionables-validación-online).

**Criterio de salida:** ambos clientes coinciden en los resultados de los intercambios. Las pruebas con latencia y pérdida de paquetes cumplen los límites de respuesta acordados.

Al completar esta etapa, elegir el orden de incorporación de las [expansiones del combate](combat/functional.md#expansiones-del-combate) y la [diferenciación de personajes](characters/functional.md). Los perfiles y sus skills se desarrollan después de esta demo local y online; no amplían sus criterios de salida. Validar cada incorporación con el mismo circuito de pruebas.

## 3. Equipos contra la IA

Un jugador participa con aliados controlados por la computadora frente a un equipo rival controlado por la IA. Se incorporan las reglas de equipo, la selección entre varios objetivos y las interacciones entre aliados y enemigos.

**Criterio de salida:** una partida por equipos se puede completar; la selección de objetivos y la resolución de acciones concurrentes respetan las reglas del juego.

## 4. Equipos online

Se conectan los participantes de una partida por equipos y se valida el combate con múltiples jugadores actuando simultáneamente.

**Criterio de salida:** la partida completa funciona con la cantidad objetivo de participantes y cumple los límites acordados de rendimiento y respuesta en red.

## Preguntas

1. ¿Qué secuencias reproducibles de ataque y defensa debe ejecutar el rival del primer duelo?
2. ¿Qué expansión incorporaremos primero después de validar el núcleo online y antes de pasar a equipos?
