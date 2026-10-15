# Prototipo local existente

La escena de observación conserva un ataque y una guardia separados. Es el punto de partida para implementar la [base FGOA adaptada](../game/functional.md), cuyas reglas se describen en [combate](../combat/functional.md) y [jugador](../player/functional.md). Implementación existente: [technical.md](technical.md).

## Controles y escena

| Control del setup documentado en 013–015 | Acción |
|---|---|
| WASD | Movimiento |
| Mouse | Cámara libre |
| LMB | Un ataque |
| RMB mantenido | Guardia; soltar termina la acción |
| F6 | Cambiar el modo del rival |
| R | Restaurar vida/posiciones y cancelar acciones |

El controller normal no implementa lock-on ni un binding Tab. F6/R pertenecen al controller de prueba. La asignación de teclas reside en assets; esta iteración revisa fuentes y reportes, sin inspeccionar nuevamente sus defaults en Unreal.

## Ataque y guardia actuales

Ataque y guardia se excluyen. Mantener o volver a pulsar ataque durante el montage no agrega otro. Un contacto con receptor en guardia cancela el ataque del agresor y no causa daño. Sin guardia resta vida y no agrega reacción. Guardia cubre cualquier dirección y no se consume al bloquear.

Solo cuenta el tramo marcado por notify en la animación. Una hitbox sobre el puño alcanza una vez a cada personaje por ataque; el golpe no produce avance automático. Movimiento y cámara libre siguen disponibles. El HUD muestra vida y estados IDLE, ATTACK o GUARD, además del modo de IA.

Los valores iniciales nativos son vida 100 y daño 12. Llegar a cero no produce muerte ni resultado; la IA sigue actuando. Reiniciar restaura vida y posiciones y cancela acciones. No hay combos, NA, parry, crítico, recurso de guardia, reacción ni partida por equipos implementados.

## Modos del rival

F6 recorre Passive, Attack, Guard, Sidestep y Mixed, y vuelve al primero. Passive es el inicial del setup documentado.

| Modo | Comportamiento |
|---|---|
| Passive | Quieto, sin atacar ni cubrirse |
| Attack | Se acerca en línea recta y repite el ataque, esperando a que termine y al intervalo entre intentos |
| Guard | Quieto, mantiene guardia |
| Sidestep | Se mueve lateralmente alrededor del jugador, sin atacar ni cubrirse |
| Mixed | Alterna Attack, Guard y Sidestep cada cuatro segundos |

Cambiar modo cancela acción y movimiento anteriores. El primer comportamiento tras posesión, cambio o reinicio espera 0,75 segundos. R conserva el modo y reinicia la secuencia. No se buscan rutas alrededor de obstáculos.

Attack intenta golpear a 100 cm entre personajes y separa intentos al menos un segundo; la hitbox debe alcanzar físicamente al receptor. Son parámetros del prototipo, no medidas deducidas de FGO.
