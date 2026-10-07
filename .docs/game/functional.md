# Juego y partida

El prototipo actual permite observar [un ataque y una guardia](../combat/functional.md). El formato de equipos y los perfiles de personajes pertenecen a etapas posteriores.

## Duelo local de prueba

El escenario contiene al jugador y un rival con IA básica para observar distintas situaciones. Los dos usan las mismas reglas de ataque y guardia.

| Control | Acción |
|---|---|
| WASD | Movimiento |
| Mouse | Cámara libre cuando no hay lock-on |
| LMB | Un ataque |
| RMB mantenido | Guardia |
| Tab | Activar o liberar lock-on |
| R | Restaurar posiciones y vida; cancelar acciones |
| F1 | Cambiar el modo del rival |

El HUD conserva vida y estado de ambos personajes: IDLE, ATTACK o GUARD. También muestra el modo del rival. No hay mensajes de victoria/derrota.

## Modos del rival

F1 recorre estos modos y vuelve al primero:

| Modo | Comportamiento |
|---|---|
| Passive | Quieto, sin atacar ni cubrirse; modo inicial |
| Attack | Se acerca y repite el único ataque, esperando a terminarlo y al intervalo entre intentos |
| Guard | Quieto, mantiene guardia; permite comprobar la cancelación del atacante |
| Sidestep | Se desplaza lateralmente alrededor del jugador, sin atacar ni cubrirse |
| Mixed | Alterna Attack, Guard y Sidestep, cuatro segundos cada uno |

Cambiar de modo cancela la acción y el movimiento anteriores. Después de cambiar o reiniciar, el rival espera 0,75 segundos antes de actuar. R conserva el modo seleccionado y reinicia su secuencia. La IA no se detiene por vida cero: todavía no hay muerte ni fin de partida.

El acercamiento es en línea recta, sin buscar rutas alrededor de obstáculos. La distancia inicial para intentar atacar es 100 cm entre personajes; la hitbox del puño debe alcanzar de verdad al receptor para producir un contacto. Los intentos se separan al menos un segundo.

La [guía del golpe de puño](../reports/006-golpe-de-puno/editor.md) reúne la configuración vigente y enlaza las comprobaciones de IA.
