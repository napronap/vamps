# Contraste con la documentación y la base de Vamps

Fuente principal: [análisis de las cuatro grabaciones y el diálogo R01–R07](analisis.md). Las reglas elegidas para Vamps están en [alcance/functional.md](alcance/functional.md) y su [base técnica](alcance/technical.md), separados de las especificaciones instaladas. No hubo cambios de código. Ranged y SUPPORT quedan fuera; crítico queda incluido.

## Qué representa cada fuente

Los documentos actuales de [combate funcional](../../combat/functional.md), [combate técnico](../../combat/technical.md), [juego funcional](../../game/functional.md), [juego técnico](../../game/technical.md) y [roadmap](../../roadmap.md) describen un prototipo deliberadamente reducido. Una diferencia con el video no convierte esa reducción en un bug: sí impide usarla como descripción fiel de FGO.

Los documentos de [identidad](../../characters/functional.md) y su [técnico](../../characters/technical.md) contienen criterios para etapas posteriores, no reglas demostradas de esta referencia. Los reportes [002](../002-mvp-duelo/reporte.md) y [003](../003-combate-animado/reporte.md) registran modelos retirados; no son la especificación vigente. Los reportes [009](../009-personajes-base/reporte.md), [011](../011-ia-combate-prueba/reporte.md), [014](../014-copias-nomenclatura-vamps/reporte.md) y [015](../015-retirada-duel/reporte.md) explican la base Vamps posterior.

## Diferencias de comportamiento

| Tema | Descripción existente | Resultado del contraste y corrección para describir la referencia |
|---|---|---|
| Entrada ofensiva/defensiva | LMB ataca y RMB mantiene guardia | RMB compartido solicita una acción; defensa puede resolverse durante preparación o dentro de una acción ofensiva ya iniciada. No limitarla al período anterior a encender tracing |
| Sostener guardia | Activa desde presionar hasta soltar | Vamps usa pulsaciones individuales; combo acepta solo dentro de ventana y descarta el click temprano. No hay hold que repita combo ni buffer que conserve la solicitud anticipada |
| Bloqueo sin daño | Receptor en guardia no pierde HP | Barrera/GUARD coinciden con pérdida de HP. Vamps adopta residual según stats que puede matar; fórmula en P02 |
| Cancelación del agresor | Bloquear cancela ataque, montage y trazas del agresor | El rival puede continuar sus impactos; no copiar cancelación total incondicional de su ofensiva |
| Cobertura | Cualquier dirección | Vamps conserva 360° por decisión explícita. La referencia lateral no demuestra por sí sola cobertura trasera; guardia efectiva puede imponer al agresor como nuevo target |
| Consumo de guardia | No se consume al bloquear | Original conserva cartas al defender; Vamps añade recurso de guardia y break/stun, sin modelar cartas. Fórmulas de recurso pertenecen al diseño propio que emula ese comportamiento |
| Daño sin reacción | Solo resta vida | Reacción, desplazamiento y Stunned se separan de draw. La particularidad original de mano/consumo queda documentada y excluida de Vamps conforme a R01 |
| Un ataque/un contacto | Un solo ataque, una sola afectación a cada receptor | Acción/carta, impacto y cadena son unidades distintas; hay impactos múltiples y Extra Attack. No deduplicar por toda una cadena ni adjudicar un único contacto a todo repertorio |
| Continuación | Pulsar durante un ataque no agrega otro | Ventana por paso y pulsación individual; fallo corta, acierto a otro enemigo o golpe bloqueado habilitan sin retarget ofensivo. El defensor puede retomar ofensiva. Cierre voluntario al final de paso por evasión/no continuar |
| Recuperación | No hay NA ni restricciones adicionales | NA es draw. Bloquea ataque/guardia y permite locomoción; terminar guardia no lo dispara. Un golpe durante NA no demuestra que haya causado ese draw |
| Recuperación tras defensa | Guardia mantenida sin fases de resolución propias | Puede demorarse la salida ofensiva tras bloquear manteniendo RMB habilitado para solicitar ataque/defensa. Separar entrada, ejecución y presentación; no convertir esa demora aparente en NA |
| Acercamiento del ataque | El ataque no mueve automáticamente al personaje | Avance ofensivo adaptable y limitado por personaje/acción; puede quedar corto y el golpe se ejecuta igualmente. Separar desplazamiento permitido de alcance de contacto. Root motion escalado es hipótesis, no obligación técnica |
| Movimiento durante combate | Movimiento siempre disponible sin restricciones | Locomoción durante NA, reacción/stun aparte y cierre por evasión al final del paso. Shift sin dirección será evasión hacia atrás |
| Lock-on y cámara | Tab activa/libera; mouse controla cámara libre | Lock-on continuo, F recorre orden HUD y guardia fuerza retarget; corrección/zoom/planos de referencia y fade de oclusores visibles. No se presupone dos umbrales internos ni quitar colisión al desvanecer |
| Duelo y muerte | Dos participantes; HP cero no agrega muerte o resultado | Vamps elige equipos, puntuación, muerte/respawn del mismo personaje y portador con recompensa mayor; excluye reservas |
| Recursos y skills | Núcleo solo Health/MaxHealth; ideas futuras de dos skills | Vamps conserva cadena/NA sin cartas, crítico y recurso de guardia; excluye SUPPORT y Extra. Ultimate propia posterior sin cinemática global. El NP canalizado original se documenta por explicación, no por video |
| Personajes | Misma base en la demo; perfiles futuros | Comienzo ofensivo, desplazamiento y contactos varían por repertorio/acción Q/A/B. No se adoptan arquetipos de balance ni ranged para el primer alcance |
| Red | Prototipo local con autoridad | El video no revela predicción, compensación de latencia ni resolución de contactos simultáneos. No se corrige arquitectura de red desde imágenes |

La vida 100, daño 12, radio del puño 12 cm y distancia de ataque 100 cm son parámetros del prototipo. No son valores de FGO deducibles de la grabación ni deben calibrarse contra píxeles sin una escala de mundo.

## Desajustes entre los documentos y el código actual

La lectura de fuentes confirma además diferencias documentales ajenas al video. Se registran aquí sin editar los documentos originales:

- Los técnicos todavía nombran `AVampsDuelCharacter`, `ADuelPlayerController`, `ADuelGameMode`, `ADuelHUD`, `UDuelAnimInstance`, `UGE_DuelDamage` y assets `/Game/Duel`. El grupo activo utiliza Vamps; 014/015 explican el cambio y la retirada histórica.
- El juego funcional menciona F1 para IA y Tab para lock-on. El controller de prueba actual incorpora `CycleAIModeAction` y `ResetCombatAction`; 011 y el HUD vigente identifican **F6/R**. El controller normal inspeccionado enlaza Move, Look, Attack y Guard; no tiene un binding nativo de lock-on.
- El técnico de combate describe Tick para orientación hacia lock-on. La base `AVampsCharacter` actual desactiva Tick y orienta locomoción mediante CharacterMovement. `AVampsCharacter_Player` añade el brazo y la cámara, sin lógica de seguimiento de objetivo.
- La resolución vigente sí coincide con la simplificación funcional: `ReceiveHit` cancela la habilidad del agresor y retorna si el receptor está en guardia; si no, aplica `UGE_VampsDamage`. No incorpora reacción ni muerte.

Estas diferencias documentales deben corregirse al redactar el documento final para no diseñar sobre tipos retirados o capacidades que ya no están en la base. La grabación no valida los assets del proyecto y no se consultaron defaults actuales del editor.

## Relaciones actuales que afectan el rediseño

| Pieza comprobada en Source | Responsabilidad actual | Consecuencia del análisis, si se adopta esa regla |
|---|---|---|
| `AVampsPlayerController` | Entradas separadas Attack/Guard; Move relativo al yaw; Look libre | Una entrada compartida necesita una interpretación contextual. Cambiar el mapping no basta para decidir cuándo atacar o defender |
| `AVampsCharacter` | Aloja GAS y resuelve `ReceiveHit` | La recepción tendría que distinguir daño defendido, daño abierto y consecuencias de reacción; no limitarse a cancelar al atacante |
| `UGA_Attack` | Un montage a velocidad normal; limpia trazas al terminar | La acción necesita distinguir preparación de comienzo ofensivo, consumo, avance permitido y resultado de contacto. La cadena puede terminar antes de su tercera acción |
| `UGA_Guard` | Tag activo hasta liberación; exclusión con Attack | La exclusión total durante el montage impide defensa válida dentro de la ofensiva. Separar solicitud, fases y resultado para reproducir esa interacción |
| `UFistHitboxComponent` | Sweep del puño, deduplicación por ataque y comprobación de pared | Un ataque de varios impactos o de área necesita otra definición de eventos válidos. No se generaliza el único puño a todo repertorio |
| `UANS_AttackTrace` | Enciende/apaga trazas de la reproducción propietaria | Distinguir contacto de animación sigue siendo útil. La grabación no demuestra que FGO use la misma solución ni decide el arbitraje de fases/contactos |
| `UVampsAttributeSet` / `UGE_VampsDamage` | Vida y efecto instantáneo de daño | Reutilizar responsabilidades de atributos/daño; agregar residual, guardia y crítico con fórmulas definidas. Ultimate posterior no justifica atributos/bindings anticipados |
| `AVampsCharacter_Player` | Cámara con SpringArm/ControlRotation | Lock-on continuo, selección, tolerancia lateral y corrección de encuadre necesitan comportamiento propio; el brazo existente no los resuelve |
| `AVampsAIController` | Solicita las mismas acciones del personaje en modos de observación | Una IA futura debe usar las reglas compartidas; las decisiones internas de los rivales grabados no son observables |
| `AVampsCombatTestGameMode` / controller de prueba | Escena local, rival y controles de depuración | No equivale a equipos, reservas, Grial ni victoria. Esas capas dependen de la decisión de alcance |
| `UVampsAnimInstance` / HUD de prueba | Presentación de locomoción, guardia, vida y estado básico | La referencia exige señales distintas para acción, impactos, disponibilidad y stun si se eligen esas mecánicas |

Los Blueprints propios `/Game/Vamps/Blueprints/BP_VampsCharacter`, `BP_VampsCharacter_Player`, `BP_VampsPlayerController` y `BP_VampsCombatTestGameMode` conectan estas clases según los reportes 013–015. La animación propia y `AM_BasicAttack` pertenecen al circuito descrito allí. No se verificaron ni modificaron esas asignaciones en esta iteración y no se reutilizó lógica del ejemplo Third Person.

## Qué queda mejor definido y qué sigue abierto

El análisis permite reemplazar ambigüedades por separaciones concretas:

- **Input físico / solicitud / acción aceptada:** RMB iluminado no significa que el personaje haya empezado un ataque o una guardia.
- **Acción ofensiva / impacto / cadena:** una acción puede afectar varias veces; varias acciones pueden integrar una cadena con ataque extra.
- **Acción / contacto ofensivo / defensa:** empezar montage y activar tracing propio no excluyen toda posibilidad defensiva durante la acción.
- **Defensa / consumo / cancelación rival:** defender conserva cartas y puede reducir daño sin terminar toda la presión entrante.
- **Draw / reacción / stun / movilidad:** NA pertenece al draw; reacción y stun tienen consecuencias distintas. Redraw conserva locomoción y perder HP no lo dispara universalmente.
- **Objetivo / amenaza / cámara / orientación:** acertar a otro enemigo no fuerza selección; guardia efectiva sí puede hacerlo. La cámara conserva composición de referencia y oclusores preparados se desvanecen sin perder colisión.
- **Derrota individual / reserva / resultado de equipo:** la caída de un personaje no cierra la partida.

No se recupera automáticamente el modelo de 002: buffer, reloj común, NA pausado, cobertura consumible, parry y prioridades por identificador no quedan demostrados. Tampoco se decide entre timers, notifies, secciones de montage, root motion o nuevas clases por las imágenes. Rango/adaptación son reglas de comportamiento; el mecanismo se elige después. El cierre por fallo permite una ofensiva fuera de alcance: no introducir rechazo previo por distancia como copia de esa escena.

El [alcance consolidado](alcance/functional.md) incorpora R01–R07 y la continuación tras contacto defendido; la [base técnica](alcance/technical.md) conecta esas decisiones con responsabilidades actuales. [P02–P04](preguntas.md#reglas-que-requieren-respuesta) contienen solo precisiones restantes. La revisión de videos permanece cerrada, sin cambiar roadmap, código o setup Unreal.
