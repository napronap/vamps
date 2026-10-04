# Juego y partida

Juego 3D de acción online con personajes diferenciados por sus habilidades. El [sistema de combate](../combat/functional.md) toma como referencia FGO Arcade.

## Formato

Las partidas enfrentan equipos de combatientes. El desarrollo avanza desde duelos contra la IA hasta el formato completo según el [roadmap](../roadmap.md).

## Duelo local de prueba

Un jugador y un rival comparten las reglas y parámetros del [primer combate](../combat/functional.md#primera-versión). Llegar a cero de vida muestra victoria o derrota. `R` reinicia posiciones, vida, habilidades, inputs pendientes y la secuencia de IA.

Controles iniciales: WASD para moverse, click izquierdo para el shared button, Tab para activar o liberar lock-on, mouse para cámara libre y F1 para cambiar el modo del rival.

La IA ofrece cinco modos reproducibles: `Passive` espera; `Combo` se acerca y encadena ataques; `Reactive` intenta parry o continuación defensiva según las ventanas visibles; `Sidestep` se mueve lateralmente al detectar un ataque; `Mixed` alterna los tres comportamientos activos cada cuatro segundos. El reinicio conserva el modo elegido y reinicia su secuencia, con 0,75 s antes de comenzar.

## Preguntas

1. ¿Cuántos combatientes forman cada equipo y cuántos equipos se enfrentan?
2. ¿Qué objetivo persigue cada jugador o equipo y qué condición determina la victoria o derrota?
3. ¿Cómo transcurre una partida desde la entrada hasta el resultado: selección de personaje, preparación, combate, rondas y revancha?
4. ¿Qué ocurre cuando un personaje es derrotado: queda eliminado, reaparece o se cambia por otro?
5. ¿Cómo es el espacio de juego: arena cerrada, zonas conectadas o recorrido con objetivos?
6. ¿Qué duración buscamos para una partida y cómo termina si se agota el tiempo?
