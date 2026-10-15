# Combate

## Primera versión

Hay un solo ataque y una guardia.

- LMB reproduce un ataque. Mantenerlo o pulsar durante el ataque no agrega otro.
- Mantener RMB activa guardia; soltarlo la termina. Ataque y guardia no se activan juntos.
- Si el puño alcanza a alguien que está en guardia, se cancela el ataque del agresor y el defensor no recibe daño.
- Si el receptor no está en guardia, pierde vida. No se agrega ninguna otra reacción.

La guardia cubre cualquier dirección y no se consume al bloquear. Un ataque alcanza una sola vez a cada personaje, aunque el puño lo toque durante varios frames. Solo cuenta el tramo del golpe marcado en la animación. Hay una única hitbox sobre el puño que realiza ese golpe.

Movimiento, cámara y lock-on permanecen disponibles. El HUD muestra vida y estado: libre, ataque o guardia. Reiniciar restaura vida y posiciones y cancela las acciones.

Vida inicial: 100. Daño inicial: 12. Llegar a cero no agrega una lógica de muerte, victoria ni derrota.

No hay combos, ventanas de combo, fases, resultados de combate, buffer, parry, NA, empujes, reacción al daño ni cobertura angular. El ataque no mueve automáticamente al personaje. El rival puede acercarse y solicitar estas mismas acciones mediante los [modos de IA básica](../game/functional.md#modos-del-rival).
