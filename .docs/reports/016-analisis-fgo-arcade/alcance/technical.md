# Base técnica del alcance de Vamps

Este documento conecta [el alcance funcional](functional.md) con las responsabilidades verificadas de la base actual. Describe trabajo futuro; esta iteración solo modifica investigación/documentación. Los documentos existentes y el setup de [015](../../015-retirada-duel/reporte.md) conservan su contenido.

## Base comprobada

| Pieza actual | Responsabilidad comprobada | Cambio que exige el alcance |
|---|---|---|
| `AVampsPlayerController` | Move relativo al yaw, Look libre, Attack Started y Guard press/release separados | Una entrada Started compartida; selección de target, evasión/sprint y ventana de continuación |
| `AVampsCharacter` | GAS, solicitudes y recepción de contacto | Contexto de cadena/fase, resolución defendida/abierta, reacción y disponibilidad sin cartas |
| `UGA_Attack` / `UGA_Guard` | Montage único y guardia mantenida; exclusión total por tags | La acción propia no debe bloquear toda posibilidad defensiva desde el inicio hasta el fin del montage |
| `UFistHitboxComponent` / `UANS_AttackTrace` | Sweep por socket, deduplicación por ataque y ventanas de trazado | Eventos de contacto por acción/impacto, cualquier enemigo válido y resultado que habilita continuidad |
| `UVampsAttributeSet` / `UGE_VampsDamage` | Health/MaxHealth y aplicación instantánea de daño | Stats necesarios para residual, crítico y guardia, con fórmulas elegidas antes de agregarlos |
| `AVampsCharacter_Player` | SpringArm de 400 y cámara ligada a ControlRotation | Seguimiento continuo de target, encuadre, corrección, presentación por acción y oclusores |
| `AVampsCombatTestGameMode` | Crea rival, reinicia vida/posición y permite modos IA de observación | Separar esa escena de observación de partida por equipos, muerte, respawn y puntuación |

La [comparación](../comparacion.md#relaciones-actuales-que-afectan-el-rediseño) conserva las diferencias documentales y conexiones conocidas `/Game/Vamps`. No se verificaron defaults de assets en Unreal ni se presupone que la lógica nueva ya exista. No se reutiliza lógica Third Person.

## Entrada, acción y disponibilidad

La pulsación compartida produce una solicitud contextual única; mantener RMB no genera repetición. Se comprueba por separado si puede iniciar acción, si está dentro de ventana de continuación y si puede responder defensivamente. Un click anterior a la ventana no se conserva para un combo posterior. Si se acepta una continuación dentro de la ventana, solo se necesita registrar la continuación del paso actual; no una cola general de inputs futuros.

El estado necesario distingue paso de cadena, fase de acción, contacto habilitante, continuación aceptada, tiempos de recuperación, NA, reacción y stun. Disponibilidad de entrada, comienzo efectivo de acción y presentación se evalúan por separado. La recuperación tras bloquear no debe imponer un candado global al botón compartido ni activar NA: se siguen evaluando solicitudes ofensivas/defensivas aunque la salida ofensiva tarde más. No se impone un timer adicional fijo por bloqueo ni se agrega una cola de clicks fuera de ventana.

Estos conceptos no se reemplazan por un único tag Attack que excluya toda guardia. La posibilidad defensiva cubre la acción completa conforme al funcional, con validación de solicitud y contacto; no se deriva solamente de si están encendidas las trazas ofensivas ni del aspecto de la pose.

Se usará tiempo de la acción y sus ventanas para disponibilidad, y contactos espaciales para decidir qué receptor fue alcanzado. Esa separación no pretende reconstruir el sistema interno de FGO. La combinación concreta de notifies, tiempo/montage y resolución autoritativa se elegirá junto con el arbitraje de simultaneidad. No se incorpora reloj común, prioridad por ID ni desempate de stats antes de resolver esa elección.

## Contacto y resolución

El detector entrega agresor, receptor, acción y evento de impacto. Mantener la identidad del agresor permite retarget al bloquear, incluso cuando la amenaza no coincide con el seleccionado. El objetivo sirve para dirigir movimiento/cámara; no filtra todos los contactos válidos.

La deduplicación debe cubrir un evento de impacto, sin impedir impactos posteriores permitidos por otra ventana de la misma acción o paso. El resultado informa contacto habilitante, daño defendido/abierto y consecuencias de reacción. Resolver guardia no cancela por defecto la habilidad completa del agresor, como hace actualmente `ReceiveHit`.

La continuación requiere pulsación válida y contacto del paso. Un golpe defendido informa contacto habilitante para el atacante, igual que un acierto abierto; no necesita producir daño abierto para permitir otro paso. El bloqueo del defensor conserva su contexto y las solicitudes habilitadas, sin cerrar su cadena ni programar NA por sí mismo. La recuperación puede retrasar el contacto ofensivo; la presentación puede iniciar ataque y terminar en bloqueo según la llegada del golpe. Se distinguen los resultados de ambos participantes. Fallar y cerrar voluntariamente terminan la cadena y programan recuperación/NA. No hay representación de cartas preparadas, usadas, mano, draw, bonus Q/A/B ni hooks para agregarlos luego.

## Movimiento y animación

Cada una de las tres acciones necesita tiempos/ventanas, contactos permitidos, máximo de avance y recuperación. El desplazamiento solicitado se limita por la acción y la geometría; debe poder quedar corto frente a un target lejano. No se rechaza toda solicitud por distancia inicial ni se garantiza alcanzar al seleccionado.

Root motion escalado, motion warping y desplazamiento dirigido son opciones a evaluar con las animaciones elegidas, no decisiones recuperadas del video. Se elegirá una solución concreta antes de implementar esa parte, sin mantener varias alternativas como fallbacks. La frontera de fin de paso debe coordinar cierre por evasión y continuidad; no se incorpora cancelación libre en toda fase.

## Target y cámara

El controller conserva selección y orden de rivales mostrado por HUD. El evento de guardia efectiva impone el agresor como target; acertar ofensivamente a otro rival no produce el mismo evento. Cámara y HUD reciben el mismo target, evitando que cada uno elija por cercanía de forma independiente.

La cámara calcula la relación visual jugador/target y aplica tolerancia/corrección, distancia visual y presentación de ataque/defensa. La elección entre FOV y distancia se hará al calibrar esa presentación; no se presupone una fórmula de dos umbrales ni valores de la grabación.

La selección de oclusores para desvanecer afecta su material/presentación local; no altera automáticamente colisión de personaje o límites del mapa. Se restaura la presentación cuando dejan de ocultar la vista. El tratamiento de cámara frente a sólidos se define con el mapa; un boundary invisible no requiere material de fade.

## Partida y stats

La responsabilidad de partida registra equipos, bajas, puntos, portador y condiciones de fin. La derrota individual termina acciones/participación del personaje antes del respawn del mismo tipo. El portador modifica el valor de una baja; su selección, transferencia y efectos adicionales dependen de las decisiones funcionales restantes.

Health continúa como atributo de daño. Stats de reducción, crítico y guardia se agregan cuando exista su regla, sin copiar números del HUD ni crear atributos sin uso. Guardia necesita consumo, recuperación y transición de guard break/stun; crítico necesita probabilidad, bonificación y relación con bloqueo.

SUPPORT y reservas quedan fuera. La ultimate posterior no justifica hoy clases, bindings, atributos o assets de carga; se diseñará cuando corresponda esa etapa.

## Orden de implementación y comprobación

1. Cerrar contactos habilitantes, solicitud defensiva y desempate; implementar el intercambio compartido y tres pasos con sus ventanas.
2. Completar recuperación, reacción, guardia/guard break y crítico con reglas/valores definidos.
3. Incorporar avance, evasión/sprint, target forzado y cámara/oclusión.
4. Conectar partida, puntuación, muerte, respawn y portador con el formato elegido.

Cada parte se comprobará con revisión de código, compilación por MCP cuando corresponda y observación manual en el editor. Las operaciones de assets se harán por herramientas MCP directas soportadas y se documentará lo manual que falte. No se agregan ni ejecutan tests automatizados.

Las [precisiones funcionales](functional.md#precisiones-para-cerrar-el-documento-final) y la selección de mecanismo de acción/contacto son las decisiones que aún faltan; esta base no entrega implementación ni assets configurados.
