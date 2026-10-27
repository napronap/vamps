# Base técnica del juego

Se usa Unreal Engine 5.8 y GAS. Reglas: [juego y partida](functional.md). Dependencias: [combate](../combat/technical.md), [jugador](../player/technical.md) y [base existente](../prototype/technical.md).

## Código y recursos

Las clases y Blueprints propios heredan de clases nativas de Unreal. No se copia ni hereda lógica del ejemplo Third Person ni de sus variantes; solo se reutilizan sus recursos visuales. Los archivos propios se organizan por responsabilidad en `Source/vamps/Public/` y `Source/vamps/Private/`.

`AVampsCharacter` aloja GAS y combate; `AVampsCharacter_Player` añade componentes de cámara; `AVampsPlayerController` interpreta entrada y `AVampsAIController` solicita las mismas acciones. `AVampsMatchGameMode` administra la partida local. `AVampsCombatTestGameMode` y su controller conservan el escenario de observación con R/F6/F7. Ambos modos usan `AVampsCombatTestHUD`; F9 pertenece al controller común.

## Estado de partida y participantes

`AVampsMatchGameMode`, derivado de AGameModeBase, conserva `FVampsMatchParticipant`: personaje, equipo, kills, muertes, racha y plazos de respawn/protección. `EVampsMatchPhase` distingue Unavailable, Selecting, Preparing, Active y Finished. `StartPlay` valida configuración, encuentra un `AVampsTeamSpawnPoint` por equipo, crea cuatro participantes y abre el selector: jugador y bot en A contra dos bots en B, todos Executioner. Las estadísticas sobreviven al respawn y se rehabilita el mismo actor. `AllowsCombat` limita acciones a participantes vivos durante la fase activa.

La puntuación se deriva de la [recompensa por racha](functional.md#puntuación), sin estado de portador ni estadísticas duplicadas para calcularla. Los parámetros de partida conservan responsabilidad separada de las propiedades del personaje.

## Contactos, baja y puntuación

`AVampsCharacter::CanEngageEnemy` y `AVampsMatchGameMode::AreEnemies` comparten elegibilidad entre lock-on, HUD e IA. `CanHitCombatTarget` añade permiso de combate y protección del receptor; armas y resolver lo consultan antes de registrar el contacto o habilitar continuación.

`UVampsCombatResolver` captura atacante, receptor y generaciones. `AVampsCharacter::ReceiveCombatHit` aplica el GameplayEffect y, si ese contacto causa la muerte, entrega atacante y `bBlocked` a `RegisterCombatDeath`. El callback de Health limpia combate; no decide autor ni variante de muerte. La comprobación de vida rechaza contactos posteriores contra el mismo eliminado.

`RegisterCombatDeath` guarda la recompensa con la racha de la víctima anterior a la consolidación. Al terminar contactos, continuación, dash y locomoción, el resolver llama a `CompleteCombatFrame`: contabiliza kills/muertes/puntos, reinicia rachas de eliminados y presenta muertes antes de evaluar resultado. Los intercambios letales del lote conservan sus recompensas y una víctima que también mata termina con racha cero.

`FinishLocalRound` aplica las reglas de resultado. El K/D usa totales de equipo, productos cruzados y tratamiento explícito de cero muertes; no usa el redondeo mostrado por el HUD.

## Respawn y recursos del personaje

Cada `AVampsTeamSpawnPoint` identifica su equipo; el modo coloca a sus integrantes a ±100 cm de su eje derecho. `RespawnAt` y `ProtectionEndsAt` conservan los plazos del [funcional](functional.md#respawn-y-protección). `BeginCombatAction` retira la protección al aceptar el inicio del ataque. Baja y resultado cancelan los plazos que ya no correspondan.

`StartingMaxHealth` configura el máximo inicial por personaje. `RestoreParticipant` detiene montages, llama a `ResetCombatState`, restaura Health desde MaxHealth, reposiciona y rehabilita colisión/movimiento. El reset limpia guardia y restaura energía desde la capacidad de movilidad del DA. La vida no es un parámetro global del GameMode.

Modificar una clase o estructura de DA requiere la captura/restauración previa por MCP exigida en AGENTS.md. La preservación realizada para los campos de muerte se conserva en [051](../reports/051-partida-local/reporte.md#preservación-del-da-y-mcp); futuros cambios requieren una captura nueva.

`AVampsCombatTestGameMode::ResetCombat` conserva su función de restauración para observación; la partida usa su propio ciclo de bajas y respawn, sin duplicar la resolución de combate.

## Montages de muerte y cámara de espera

`UVampsCombatSettings` declara `DeathMontage` y `GuardDeathMontage`. `HasValidDeathMontages` valida las referencias antes de admitir participantes, con diagnóstico no fatal y sin sustitución de variantes. `/Game/Vamps/Data/DA_VampsCombatSettings` referencia `/Game/Vamps/Animations/Executioner/AM_Executioner_Death` y `AM_Executioner_GuardDeath`.

La variante se conserva desde `FVampsPendingContact::bBlocked` del contacto letal. Después del lote, `PresentCombatDeath` desactiva la cápsula y reproduce el montage correspondiente. Auto Blend Out desactivado conserva la pose durante la espera; el respawn la retira deteniendo montages. No se deduce desde el estado de guardia ya limpiado.

`AVampsMatchPlayerController` crea un ACameraActor independiente con el punto de vista actual. Durante la espera, WASD lo desplaza a 800 cm/s y mouse controla yaw/pitch. El manager delega al comportamiento base para ese ViewTarget. `FinishRespawnCamera` devuelve la vista al pawn y destruye la cámara; el resultado detiene su control y la revancha restaura la vista del personaje.

Las lecturas, guardado y comparación del DA de 051 acreditan la incorporación de estas referencias sin diferencias en sus campos anteriores.

## HUD, resultado y nueva partida

`AVampsCombatTestHUD::DrawMatchHUD` agrega marcador, equipo, plazos, protección y resultado sobre las vistas combate/debug. Los contadores se derivan de plazos del modo. `NotifyHitBoxClick` entrega selección a `SelectExecutioner`, START a `StartLocalRound` y PLAY AGAIN a `OpenCharacterSelection`; el HUD no calcula ganadores ni modifica estadísticas directamente.

El resultado bloquea combate/movimiento, pausa meshes y cancela respawns. Habilita cursor y entrada para el botón. PLAY AGAIN vuelve a Selecting sin iniciar preparación. `StartLocalRound` exige Selecting y personaje elegido; limpia puntuación, estadísticas, rachas y contactos pendientes, restaura participantes y entrada de juego, e inicia los 5 s de preparación. La acción local basta para esta entrega.

`/Game/Vamps/Maps/L_LocalMatch` es el mapa de arranque/juego con override `BP_VampsMatchGameMode`. Este Blueprint hereda de VampsMatchGameMode y conecta `BP_VampsCharacter_Player`, `BP_VampsCharacter`, `BP_VampsMatchPlayerController` y HUD nativo VampsCombatTestHUD. El controller hereda de VampsMatchPlayerController y referencia IMC_Vamps, IA_Move, IA_Combat, IA_CycleTarget, IA_Mobility y BP_VampsPlayerCameraManager. Las rutas y parámetros completos están en [051](../reports/051-partida-local/editor.md).

## Selección de personaje (6.2)

`OpenCharacterSelection` implementa el [flujo de selección e inicio](functional.md#selección-de-personaje-e-inicio) antes de Preparing. Detiene personajes, cancela plazos, finaliza cualquier cámara libre y habilita mouse mediante `EnableMatchMenuInput`. Selecting no habilita combate, movimiento ni decisiones de IA. No avanza un contador hasta START.

La única opción es Executioner, conectada a los Blueprints de personaje y DA existentes. `bExecutionerSelected` registra esa elección en el GameMode; `SelectExecutioner` solo actúa durante Selecting. Los bots reutilizan su clase configurada: con una opción todos son Executioner y no hace falta otro sorteo. Las referencias se validan al crear los participantes, sin sustituciones ni otro DA de combate.

`DrawCharacterSelection` dibuja una vista opaca con EXECUTIONER y START. START no tiene hitbox hasta elegir el personaje. Selección y resultado usan GameAndUI con cursor/clicks habilitados; preparar la ronda devuelve GameOnly. Las guardas de fase rechazan acciones repetidas fuera del estado válido. PLAY AGAIN conserva la elección y START restaura participantes con las reglas existentes. Una sesión nueva empieza sin selección; no hay guardado entre sesiones ni otro mapa. Implementación y comprobación en [055](../reports/055-selector-executioner/reporte.md).

## Autoridad y red

La base existente ejecuta habilidades `ServerOnly` y solicitudes con autoridad. Replicar atributos no equivale a tener combate online completo. La reproducción de la referencia requiere diseñar entrega de solicitudes, estado autoritativo y presentación para varios participantes cuando llegue la integración de red.

Los videos no determinan predicción, compensación de latencia ni arbitraje. La elección de esos mecanismos no recupera automáticamente modelos retirados de reportes anteriores. No se agrega infraestructura de red o compatibilidad con cartas por anticipación.

## Dependencias de implementación

El [roadmap](../roadmap.md) prioriza núcleo, movilidad/presentación y partida antes de [extensiones Vamps](../extensions/technical.md). Equipos, recompensa por racha y respawn dependen de daño/muerte resueltos y selección coherente para cámara/HUD. 6.2 incorpora el selector con Executioner; el segundo personaje se preparará después, sin adelantar skills ni perfiles de 9. Las comprobaciones se hacen mediante revisión de código, compilación por MCP directo cuando corresponda y observación manual del editor.

El detalle de sincronización entre clientes se aborda en 7, después del ciclo local y la selección de personaje de 6.2.
