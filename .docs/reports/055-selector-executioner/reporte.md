# 055 — Selector local de Executioner y START

La presentación del selector pasa a [058 — Widgets UMG](../058-ui-widgets/reporte.md), manteniendo las acciones y el flujo aprobados aquí.

## Resultado

**Paso 6.2 completado — usuario, 2026-10-27.** El usuario confirmó «listo lo compile y probe, funciona todo» y pidió registrar el cierre. Selección de Executioner, START sin otra confirmación y PLAY AGAIN de vuelta al selector conservando la elección quedan aprobados. Diseño en [054](../054-revision-seleccion-personaje/reporte.md) y operaciones reproducibles en la [guía](editor.md).

La compilación y comprobación funcional final se atribuyen al usuario. No se afirma una nueva compilación por MCP ni se generan capturas individuales de cada caso a partir de esa confirmación general. No quedan pendientes obligatorios para cerrar 6.2; la siguiente etapa es 7, multijugador y autoridad de red.

## Archivos y responsabilidades

Rutas paralelas bajo `Source/vamps/Public/` y `Source/vamps/Private/`:

| Archivos | Cambio y relación |
|---|---|
| `GameModes/VampsMatchGameMode.h/.cpp` | Añade Selecting a EVampsMatchPhase y bExecutionerSelected, visible en la instancia. StartPlay valida e instancia los cuatro personajes existentes y abre selección con ellos detenidos. OpenCharacterSelection pausa combate/movimiento, cancela plazos, recupera la vista del pawn y habilita mouse. SelectExecutioner registra la única opción. StartLocalRound sustituye a RestartLocalRound y solo prepara la ronda desde Selecting con elección realizada. |
| `UI/VampsCombatTestHUD.h/.cpp` | Dibuja selector opaco con EXECUTIONER y START; añade hitboxes de selección/inicio. START aparece inactivo y sin hitbox hasta seleccionar. NotifyHitBoxClick entrega cada acción al GameMode; PLAY AGAIN llama a OpenCharacterSelection. Selección y error de configuración se presentan antes de las vistas de combate/debug. |

`EnableMatchMenuInput` comparte la configuración de cursor/clicks entre selector y resultado. Exige controller válido, informa el fallo y cambia a Unavailable sin anunciar éxito. El inicio también valida ese controller antes de restaurar participantes. Las acciones de menú fuera de su fase y START sin selección se rechazan como condiciones normales del flujo.

## Ciclo y conexiones

- Al abrir el mapa se validan y crean los mismos actores de 6.1. Quedan sin movimiento, combate ni animación durante Selecting; no empieza el contador de preparación. El selector cubre la escena y no hay preview 3D.
- Seleccionar Executioner activa START. Un click en START reinicia estadísticas, rachas, puntos, recursos y posiciones mediante la restauración existente, y abre Preparing durante 5 s. Los siguientes clicks no pueden iniciar otra ronda mientras ya está preparando/activa.
- Todos usan las referencias actuales de Executioner. Con una única opción los tres bots reciben ese mismo personaje; no se requiere otro sorteo ni contenido adicional. Se permiten repetidos y respawn conserva el actor/configuración.
- Finished conserva resultado/estadísticas hasta pulsar PLAY AGAIN. Esa acción vuelve a Selecting, limpia los plazos y termina cualquier cámara libre pendiente, sin reiniciar todavía el contador. La elección local se conserva y START queda habilitado para otra ronda.
- START restaura montages, recursos y cámara mediante RestoreParticipant. bExecutionerSelected pertenece a la instancia del GameMode y empieza en false en una sesión nueva; no se guarda en disco.
- El HUD de observación mantiene R/F6/F7 y las vistas de combate/debug con F9. El selector se presenta independientemente de esa alternancia.

El GameMode sigue siendo la autoridad: HUD no decide fases, daño ni puntuación. No se modifica el resolver, los permisos compartidos ni la cámara; Selecting queda fuera de AllowsCombat y AreEnemies, que ya exigen Active. La IA conserva su comprobación de permiso de partida antes de decidir.

## Assets y configuración

No se crean ni modifican assets, mapas, mappings, Data Assets ni clases/structs de sus propiedades. No hay captura/restauración nueva de DA porque esta iteración no modifica esos tipos ni configura sus assets.

`/Game/Vamps/Maps/L_LocalMatch` conserva `/Game/Vamps/Blueprints/BP_VampsMatchGameMode`, padre nativo VampsMatchGameMode. Sus personajes BP_VampsCharacter_Player/BP_VampsCharacter, controller BP_VampsMatchPlayerController, HUD nativo VampsCombatTestHUD y DA_VampsCombatSettings continúan proporcionando la configuración de Executioner. Las conexiones existentes constan en 051; no se releyeron mediante MCP en esta sesión. El segundo personaje y su DA siguen fuera de esta entrega.

## Verificaciones y límites

- Revisados selección inicial, START deshabilitado, guardas de fase frente a clicks repetidos, ausencia de reloj/combate durante selección, revancha con preselección y vuelta desde cámara libre. RestoreParticipant conserva su limpieza de montages y recursos antes de otra preparación.
- Contrastadas las APIs de hitboxes, color, cursor y modo de entrada con los headers/fuentes de Unreal 5.8 instalado. Los hitboxes usan el mismo modo GameAndUI ya utilizado por el resultado; cada acción vuelve a validar fase en el GameMode.
- Revisados los identificadores nuevos contra GameModeBase/HUD y su jerarquía hasta UObjectBase. No se mantienen llamadas a RestartLocalRound en los fuentes propios.
- clang-format 22.1.8 aplicado a los cuatro archivos propios modificados; diff sin errores de espacios. Solo cambian fuentes, reportes y sus enlaces.
- Durante la implementación no había herramientas MCP directas de Unreal disponibles: Codex no compiló, abrió/reinició el editor, ejecutó PIE ni guardó Blueprints. El usuario confirmó después la compilación y el funcionamiento; no se atribuye ese resultado a un Success/NoChanges nuevo por MCP ni al build anterior de 051.
- No se agregan ni ejecutan tests automatizados ni se automatiza la interfaz. La revisión de código y el cierre funcional del usuario conservan atribuciones distintas.

La [guía](editor.md) conserva compilación y observación como operaciones realizadas por el usuario y pasos reproducibles. El cierre actualiza roadmap, índice y documentación funcional/técnica de partida, además de la conexión del selector en jugador. No se modifica código, configuración ni assets al registrar el cierre.

## Continuación

Análisis posterior en [056 — Servidor dedicado y alojamiento](../056-analisis-servidor-dedicado/reporte.md). El usuario eligió dedicado; esa decisión sustituye la elección anfitrión/dedicado pendiente de este cierre. Las demás decisiones y la viabilidad actual de Cloudflare se detallan allí.

Las etapas 0–6 están completadas. Sigue 7: integrar solicitudes de ataque/defensa/movilidad de clientes, estado de selección/partida/participantes y presentación compartida bajo autoridad del servidor. La cámara sigue siendo local. Antes de implementar esa entrega se debe precisar servidor anfitrión o dedicado, cantidad/distribución de humanos y bots y coordinación del inicio/selección entre participantes; no se trasladan automáticamente las decisiones de aceptación local de 6.2 al juego online.

Después siguen calibración y cierre de la base en 8, y extensiones propias en 9. El segundo personaje y su DA pueden prepararse por separado cuando el usuario lo decida; no son un requisito pendiente de 6.2.
