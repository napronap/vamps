# 054 — Decisiones previas a selección de personaje (6.2)

Implementación posterior: [055 — Selector local de Executioner y START](../055-selector-executioner/reporte.md), con [compilación y observación](../055-selector-executioner/editor.md). Este reporte conserva las decisiones; el estado de implementación y sus límites están en 055.

## Resultado — 2026-10-27

6.2 agrega selección de personaje y preparación de participantes sobre el ciclo local cerrado en [053](../053-cierre-local-y-documentacion/reporte.md). El usuario acordó el selector sencillo con elección propia, bots aleatorios, repetidos permitidos y revancha que vuelve al selector conservando la preselección. Precisó que basta seleccionar personaje y pulsar START, sin confirmación adicional, y que el único personaje disponible es Executioner. El segundo personaje y su DA se prepararán después.

No hubo cambios de código, configuración ni assets. Este reporte y su [guía](editor.md) consolidan el análisis y las decisiones. Se actualizan el par funcional/técnico de partida, roadmap e índice. El alcance está definido y pendiente de implementación; no quedan decisiones de producto bloqueantes para este selector con una opción.

## Alcance ya definido

- Entrega local 2v2: jugador y bot en A contra dos bots en B. No hay cambio de equipos, jugadores adicionales ni lobby online dentro del alcance actual.
- El personaje elegido se conserva al morir; no hay reservas ni sustitución durante el respawn.
- Se conservan reglas compartidas de combate, muerte, protección, puntuación y resultado de 6.1. La preparación inicial dura 5 s y requiere tener participantes listos antes de iniciar.
- Ranged y SUPPORT permanecen fuera del primer alcance. Perfiles propios, dos skills, parry y ultimate pertenecen a 9; no son requisitos del selector.

## Estado existente y consecuencias

| Pieza revisada | Estado | Consecuencia para 6.2 |
|---|---|---|
| `GameModes/VampsMatchGameMode.h/.cpp` en los árboles Public/Private | `StartPlay` exige un pawn local ya creado y una BotCharacterClass común, crea tres bots y llama a RestartLocalRound. | Hay que introducir selección antes de activar esa preparación y resolver qué configuración usa cada participante. No alcanza con dibujar un menú. |
| `FVampsMatchParticipant` | Conserva Fighter, equipo, estadísticas y plazos; no identifica una opción de catálogo. | La selección debe quedar asociada al participante y conservarse durante su vida y respawn. Su representación es una decisión de implementación. |
| `AVampsCharacter::BeginPlay` | Inicializa GAS, vida, armas y capacidades desde propiedades del personaje y CombatSettings. | La configuración elegida debe estar disponible antes de esa inicialización; cambiar un nombre o mesh después no aplica por sí solo un personaje completo. |
| Configuración de personaje | StartingMaxHealth y AttackDamage están en el personaje; armas, combo, reacciones, muertes y movilidad en UVampsCombatSettings. | Se conserva la configuración de Executioner; no se necesitan variantes ni datos del segundo personaje. |
| `RestartLocalRound` / `RestoreParticipant` | Rehabilitan los mismos actores y reinician estadísticas/recursos; no cambian configuración de personaje. | PLAY AGAIN deberá volver al selector y START preparar la siguiente ronda, conservando las reglas actuales de restauración. |
| `AVampsCombatTestHUD` / `AVampsMatchPlayerController` | HUD de combate/resultado, click de revancha y cámara libre; no hay vista de selección. | Hace falta selección con mouse y START, con HUD de partida y combate inactivos mientras se elige. |

Los headers propios están en `Source/vamps/Public/` y los fuentes en `Source/vamps/Private/`, con rutas paralelas. No se encontró una clase de catálogo, selector o persistencia de selección en esos árboles.

## Contenido disponible

En `/Game/Vamps` hay una configuración completa documentada de Executioner, con `BP_VampsCharacter_Player`, `BP_VampsCharacter`, `DA_VampsCombatSettings`, armas, AnimBP y montages propios. Los dos Blueprints representan roles de control; no acreditan dos personajes distintos del catálogo. Solo se encontró un archivo de DA de combate bajo Content/Vamps/Data.

Existen carpetas de recursos GreatSwordAnimV1 y DualSwordAnimationV4 en Content. El usuario confirmó que el otro personaje solo tiene animaciones y todavía no tiene DA ni configuración integrada; lo preparará después. 6.2 entrega el flujo con una sola opción y no incluye la creación de ese contenido.

El funcional de [identidades](../../extensions/characters/functional.md#alcance) conserva el único melee de esta entrega. [Partida](../../game/functional.md#selección-de-personaje-e-inicio) concreta el selector con Executioner sin adelantar las identidades/skills de 9.

## Flujo acordado

Reglas principales en [selección de personaje e inicio](../../game/functional.md#selección-de-personaje-e-inicio): selector simple con Executioner, elección propia y botón START como única acción de inicio. START prepara la ronda y abre los 5 s existentes; no hay otro botón de confirmación ni diálogo. PLAY AGAIN vuelve al selector conservando la preselección. Se asignan bots desde el catálogo al iniciar y se permiten repetidos; actualmente todos serán Executioner.

La interfaz usa nombres/botones y mouse, sin preview 3D, retratos obligatorios, tiempo límite ni guardado entre sesiones. Se conservan cámara e IA compartidas. El catálogo futuro no es una condición para implementar o cerrar el flujo actual.

## Trabajo técnico pendiente

1. Representar el catálogo y sus conexiones de personaje sin duplicar lógica. Validar que cada entrada tenga sus referencias completas antes de iniciar partida; una entrada inválida debe informar su error y bloquear el inicio afectado, sin reemplazarla silenciosamente por Executioner.
2. Separar selección, preparación y ronda activa. Crear/configurar y poseer los personajes con sus datos elegidos antes de habilitar combate, conservando los equipos y formación existentes.
3. Conectar selección/START y devolución de entrada al juego. Aplicar la política de bots y revancha, y comprobar que respawn conserva identidad/configuración.
4. Reutilizar referencias completas de Executioner. No crear otro DA, repertorio ni Blueprint de personaje para rellenar el catálogo. Los assets propios del selector se limitarán a los necesarios para su implementación.

No es necesario pedir al usuario que elija UMG frente a Canvas, nombres de clases o formato interno de datos. Son decisiones técnicas que deben ajustarse al catálogo y flujo acordados. La persistencia entre mapas solo sería necesaria si se elige separar el selector en otro mapa; no se añade por anticipación.

## Verificaciones y límites

Se revisaron roadmap, pares de partida e identidades, cierre 053, fuentes de GameMode/personaje/controller/HUD, configuración de combate e inventario de archivos de Content/Vamps. No hay herramientas MCP directas de Unreal disponibles en esta sesión: no se releyeron propiedades actuales, compilaron Blueprints ni se inició PIE. Los archivos existentes y las capturas anteriores no acreditan por sí solos la compatibilidad de un personaje nuevo.

No se ejecutaron tests automatizados ni se automatizó la interfaz. La [guía](editor.md) separa las decisiones de diseño de la futura configuración del editor. Si la implementación modifica clases o estructuras de DA, primero deberá descubrir/capturar/guardar por MCP todos los assets afectados según AGENTS.md; esta revisión no los modifica.

## Continuación

El siguiente trabajo es implementar el flujo definido. No queda pendiente elegir catálogo, política de bots/repetidos, inicio o revancha para esta entrega. La incorporación del segundo personaje se abordará cuando el usuario prepare sus datos; no bloquea 6.2 ni adelanta balance final, selección online o skills.
