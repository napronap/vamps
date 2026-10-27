# 051 — Implementación de la partida local 2v2

Cierre posterior: [053 — Comprobación del usuario y documentación vigente](../053-cierre-local-y-documentacion/reporte.md). El usuario confirmó el funcionamiento el 2026-10-27; los pendientes de observación de esta implementación quedan cerrados con esa atribución.

Revisión posterior del repositorio: [052 — Estado tras la partida local](../052-estado-repo-tras-partida/reporte.md), con [continuación en el editor](../052-estado-repo-tras-partida/editor.md). Conserva la evidencia de esta implementación y distingue sus pendientes de los desfases de documentación.

## Resultado

Implementado 6.1: jugador y bot en A contra dos bots en B, todos Executioner; preparación, puntuación por racha, muerte, respawn, protección, cámara libre, resultado y botón de revancha. Selección de personaje queda en 6.2. Continúa [050](../050-reglas-partida-local/reporte.md); operaciones reproducibles en la [guía](editor.md).

El mapa predeterminado pasa a `/Game/Vamps/Maps/L_LocalMatch`. L_CharacterBasic conserva el modo de observación. No se agrega portador ni aceptación entre clientes.

## Clases y archivos

Rutas de headers bajo `Source/vamps/Public/` y fuentes bajo `Source/vamps/Private/`, con árboles paralelos:

| Archivos | Cambio y relación |
|---|---|
| Nuevos `GameModes/VampsMatchGameMode.h/.cpp` | Autoridad local: participantes, equipos, plazos, K/D, rachas, puntos, resultado y reinicio. Recibe bajas del personaje y consolida el lote después del resolver. Rehabilita los mismos actores al reaparecer. |
| Nuevo `GameModes/VampsTeamSpawnPoint.h` | PlayerStart propio con TeamIndex 0/1. Exige un origen por equipo; dispone puestos a ±100 cm de su eje derecho. |
| Nuevos `Player/VampsMatchPlayerController.h/.cpp` | Cámara independiente durante la espera, WASD/mouse a 800 cm/s; restaura la vista del pawn al reaparecer. |
| `Characters/VampsCharacter.h/.cpp` | StartingMaxHealth por personaje; elegibilidad compartida, notificación del contacto letal, muerte normal/defensiva y detención al terminar. Ataque aceptado retira protección. |
| `Combat/Data/VampsCombatSettings.h/.cpp` | DeathMontage y GuardDeathMontage requeridos por partida, con validación no fatal y sin sustituciones. |
| `Characters/VampsLockOnComponent.cpp`, `Combat/VampsWeapon.cpp` | Selección y trazas excluyen aliados/eliminados; protección rechaza contacto antes de registrar víctima o continuación. |
| `Combat/VampsCombatResolver.cpp` | Completa el frame de partida después de contactos, continuación, dash y locomoción. Conserva intercambios simultáneos. |
| `AI/VampsAIController.cpp` | Busca enemigos por lock-on; respeta preparación, muerte y resultado sin resetear cadáveres desde Decide. |
| `Player/VampsPlayerController.h/.cpp`, `Player/VampsCombatTestPlayerController.h/.cpp` | F9 pasa al controller común; R/F6/F7 permanecen en observación. |
| `UI/VampsCombatTestHUD.h/.cpp` | Marcador, reloj, contadores, protección, ganador/empate, K/D individual/equipo y botón nativo PLAY AGAIN sobre ambas vistas. |

Objetivo 1000; recompensa `100 + 25 × racha de la víctima`. Se captura la racha anterior al lote, se acreditan kills/puntos y se reinician rachas de eliminados antes de evaluar victoria. El K/D usa totales de equipo, productos cruzados y caso de cero muertes; igualdad completa produce empate.

Preparación 5 s, partida 300 s, respawn 5 s, protección hasta 2 s. El respawn restaura Health desde MaxHealth, guardia cero y energía máxima. La variante de muerte usa bBlocked del contacto letal, incluida ruptura letal. Cuerpo visible sin bloqueo y pose final retenida durante espera. Restaurar al participante detiene explícitamente montages para retirar esa pose.

El resultado bloquea combate/movimiento, pausa meshes y cancela respawns. La revancha limpia estadísticas, acciones, recursos y posiciones antes de otra preparación. El vencimiento de tiempo decide puntos y luego K/D.

## Assets y configuración

- `/Game/Vamps/Blueprints/BP_VampsMatchPlayerController`, padre nativo VampsMatchPlayerController: IMC_Vamps, IA_Move, IA_Combat, IA_CycleTarget, IA_Mobility y BP_VampsPlayerCameraManager, leídos del controller propio existente.
- `/Game/Vamps/Blueprints/BP_VampsMatchGameMode`, padre nativo VampsMatchGameMode: DefaultPawnClass BP_VampsCharacter_Player, BotCharacterClass BP_VampsCharacter, controller nuevo y HUD nativo VampsCombatTestHUD; parámetros 1000/100/25 puntos y 5/300/5/2 s.
- `/Game/Vamps/Animations/Executioner/AM_Executioner_Death` y `AM_Executioner_GuardDeath`: montages propios con un segmento DefaultSlot de Death_RM y Defence_Death_RM de GreatSwordAnimV1. Duraciones 0.866666675 y 1.266666651 s, RateScale 1, Auto Blend Out desactivado. Carga limpia recalculó/verificó sus duraciones derivadas.
- `/Game/Vamps/Data/DA_VampsCombatSettings`: incorpora esas dos referencias y conserva todos los campos anteriores.
- `/Game/Vamps/Maps/L_LocalMatch`: copia del mapa propio, WorldSettings con BP_VampsMatchGameMode. TeamSpawn_A (-200,0,100), yaw 0, equipo 0; TeamSpawn_B (600,0,100), yaw 180, equipo 1. Suelo comprobado por trazas en cuatro puestos.
- `Config/DefaultEngine.ini`: mapa de juego/arranque y modo global apuntan al ciclo nuevo. El override de L_CharacterBasic conserva observación.
- Reportes 051, enlace desde 050 e índice actualizados. Las especificaciones y roadmap previos se conservan como planificación de 050; aquí se registra la implementación y sus límites.

## Preservación del DA y MCP

Se buscaron clase y derivadas en todas las raíces: solo apareció DA_VampsCombatSettings. Se capturaron todas sus propiedades y se guardó antes de modificar la clase. [Respaldo completo](data-assets-antes.json), [lectura tras Live Coding](data-assets-live-coding.json) y [comparación final](data-assets-despues.json): `differences: []`. Arrays, structs, transforms y referencias anteriores conservados; campos nuevos asignados y guardado limitado a assets afectados después del arranque limpio, sin referencias transitorias.

MCP respondió en la URL configurada del proyecto. Se descubrieron herramientas y schemas antes de invocarlas; ProgrammaticToolset solo orquestó herramientas registradas. Sin automatización de UI ni ejecución alternativa de operaciones de editor.

## Verificaciones

- Live Coding inicial: Success y parche real aplicado; [salida](live-coding-1.json).
- Build completo con editor cerrado: Succeeded; [salida](compilacion-limpia.txt). Cierre/build/reapertura externos autorizados expresamente por el usuario, apertura con editor.bat. El arranque dentro del sandbox no completó caché; se cerró esa instancia y se abrió normalmente fuera del sandbox.
- Blueprints nuevos compilados por MCP con warnings_as_errors, propiedades releídas y assets guardados; [conexiones finales](data-assets-despues.json), [mapa](mapa-mcp.json).
- PIE: cuatro personajes, tres AIControllers, equipos 0/0/1/1, recursos iniciales y transición a Active; [observación inicial](pie-observacion-1.json).
- Bajas y respawns sucesivos, K/D persistente y recompensas 100/125/150; [observación intermedia](pie-observacion-2.json), [log](partida.log). CaptureEditorImage mostró HUD, marcador y reloj durante PIE.
- Cierre natural por objetivo: B ganó 1100–975. Finished con cuatro personajes sin movimiento ni solicitudes, meshes pausados, respawns cancelados y cursor/clicks habilitados; [estado final](pie-resultado.json).
- Corrección final de retirada de pose de muerte al restaurar: Live Coding Success con parche real; [salida](live-coding-2.json). No se repitió una ronda completa después, atendiendo el pedido de terminar rápido.
- Formateador aplicado y revisión de espacios. Sin tests automatizados ni inyección de inputs.

## Comprobación final y límites

El usuario confirmó el funcionamiento tras la revisión 052 y cerró las comprobaciones de F9, cámara libre, muerte/respawn, protección y PLAY AGAIN; [registro 053](../053-cierre-local-y-documentacion/reporte.md). La [guía](editor.md#comprobación-manual-final) conserva los pasos reproducibles. Cierre simultáneo y por tiempo conservan su revisión de código y la aprobación general, sin nuevas trazas individuales de esos escenarios.

El usuario indicó que parecía funcionar bien y pidió terminar rápido. PIE quedó detenido, con L_LocalMatch abierto y parche final aplicado. 6.2 y red mantienen su alcance posterior.
