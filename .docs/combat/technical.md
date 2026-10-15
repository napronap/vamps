# Implementación del combate de referencia

Reglas: [functional.md](functional.md). Punto de partida comprobado en fuentes: [prototipo](../prototype/technical.md). Este documento define los cambios de arquitectura necesarios para implementar la base.

## Responsabilidades

| Pieza existente | Cambio necesario |
|---|---|
| `AVampsCharacter` | Conservar GAS y recepción de contactos; distinguir cadena/fase, disponibilidad, resultado defendido/abierto y reacción |
| `UGA_Attack` / `UGA_Guard` | Reemplazar montage único y guardia mantenida mutuamente excluyentes por acciones y defensa contextual conforme al funcional |
| `UFistHitboxComponent` / `UANS_AttackTrace` | Conservar detección espacial y ventanas; identificar acción/evento de impacto y comunicar contacto habilitante |
| `UVampsAttributeSet` / `UGE_VampsDamage` | Conservar atributos y aplicación de daño; incorporar guardia, residual y crítico cuando se elijan sus reglas |
| `UVampsAnimInstance` | Presentar locomoción y resultados de combate sin convertirse en autoridad de disponibilidad |

La [entrada compartida y selección](../player/technical.md) entrega solicitudes al núcleo; la IA utiliza las mismas reglas. Ningún controlador aplica daño ni decide por su cuenta si hubo bloqueo.

## Entrada, acción y disponibilidad

Se comprueba por separado inicio de acción, ventana de continuación y posibilidad defensiva. El estado necesario distingue paso, fase, contacto habilitante, continuación aceptada, recuperación, NA, reacción y stun. La continuación aceptada corresponde al paso actual; no requiere una cola general de inputs futuros.

Disponibilidad de entrada, comienzo efectivo de acción y presentación son responsabilidades distintas. La recuperación tras bloqueo no impone un candado global al botón ni un timer adicional universal. Los tags actuales que excluyen toda guardia durante Attack deben cambiar: la defensa válida cubre toda la acción, no solo el tramo previo a encender las trazas.

El tiempo de la acción determina sus ventanas; los contactos espaciales determinan qué receptor fue alcanzado. Se elegirá la combinación concreta de notifies, tiempo/montage y resolución autoritativa junto con el arbitraje de simultaneidad. No se adopta reloj común, prioridad por ID ni desempate por stats sin esa decisión.

## Contacto y resolución

El detector entrega agresor, receptor, acción y evento de impacto. La selección dirige movimiento/cámara; no filtra todos los contactos válidos. Al incorporar equipos, la elegibilidad distingue enemigos de aliados.

La deduplicación cubre un evento de impacto, permitiendo impactos posteriores definidos para la misma acción. El resultado comunica contacto habilitante, daño defendido/abierto y consecuencias de reacción. La guardia efectiva informa además el agresor para retarget; un acierto ofensivo a otro rival no emite ese cambio de selección.

`ReceiveHit` debe dejar de cancelar por defecto la habilidad completa del agresor al bloquear. El contacto defendido habilita continuación para el atacante. El defensor conserva contexto de cadena y solicitudes disponibles; no programa NA por bloquear. Fallo, cierre voluntario y fin del tercer paso conducen al cierre y recuperación descritos en el funcional.

No se representan mano, cartas listas/usadas, draw, Q/A/B, Extra Attack ni hooks para incorporarlos después.

## Acciones, desplazamiento y animación

Cada paso necesita tiempos/ventanas, contactos permitidos, máximo de avance y recuperación. El avance respeta acción y geometría y puede quedar corto. No se introduce rechazo general por distancia inicial para evitar el caso de fallo observado.

Se elegirá un mecanismo de movimiento con las animaciones disponibles. Root motion escalado, motion warping y desplazamiento dirigido son opciones de diseño, no mecanismos demostrados por los videos ni fallbacks para mantener simultáneamente. Fin de paso, continuación y cierre por evasión deben usar la misma frontera temporal.

Los stats se agregan cuando tengan una fórmula y un uso. Health conserva su responsabilidad; guardia necesita consumo/regeneración y transición a break/stun; crítico necesita probabilidad, bonificación e interacción con daño defendido. La pose y el HUD presentan el resultado resuelto, sin deducirlo visualmente ni duplicar autoridad.

## Preguntas de implementación

1. ¿Cómo se representan y sincronizan fase, contactos y fronteras de paso para resolver la [vigencia defensiva y simultaneidad](functional.md#preguntas)?
2. ¿Qué mecanismo de avance se adapta a los clips elegidos y limita el desplazamiento por acción y colisión?

El [roadmap](../roadmap.md) ordena la implementación y sus comprobaciones. Las extensiones propias no justifican infraestructura anticipada.
