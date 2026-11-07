# Editor — Partida local 2v2

El flujo vigente de muerte y cámara durante el respawn está en la [guía de 067](../067-muerte-camara-y-danio/editor.md).

El mapa está configurado y guardado. El usuario confirmó el funcionamiento el 2026-10-27 y cerró las comprobaciones manuales en [053](../053-cierre-local-y-documentacion/reporte.md). No queda configuración ni observación obligatoria para cerrar 6.1. Esta guía conserva las operaciones de implementación y sus pasos reproducibles.

## 1. MCP y compilación

**Hecho — usuario.** MCP instalado y activación al abrir mediante editor.bat, según confirmación del usuario.

**Hecho — Codex por MCP.** Descubrimiento de herramientas/schemas; compilación inicial y final mediante LiveCodingToolset.LiveCodingToolset.CompileLiveCoding, ambas Success con parches reales. Además, cierre/build.bat/editor.bat externos autorizados expresamente por el usuario: build completo Succeeded. Sin automatización de UI.

Para reconstruir desde disco en otra sesión: cerrar editor, ejecutar build.bat y abrir con editor.bat. El cambio final de pose se compiló/aplicó por Live Coding; no hubo otro build completo después.

## 2. Preservar DA y configurar muerte

**Hecho — Codex por MCP.** ObjectTools.search_subclasses y AssetTools.find_assets buscaron clase/derivadas en todas las raíces. ObjectTools.list_properties/get_properties capturaron todos los valores de DA_VampsCombatSettings; AssetTools.save_assets guardó el original antes de modificar su clase. [Respaldo](data-assets-antes.json).

En Content Browser → `/Game/Vamps/Animations/Executioner`, crear AM_Executioner_Death y AM_Executioner_GuardDeath, con las secuencias `/Game/GreatSwordAnimV1/Animation/RM/A_GreatSwordCombatAnimationV1_Death_RM` y `A_GreatSwordCombatAnimationV1_Defence_Death_RM`, respectivamente. Un segmento completo DefaultSlot, RateScale 1, Enable Auto Blend Out desactivado. Duraciones 0.866666675 y 1.266666651 s. Codex usó AssetTools.duplicate sobre un montage propio y ObjectTools.set_properties; guardó y releyó tras carga limpia. SequenceLength es derivado, no se escribió como editable.

Abrir `/Game/Vamps/Data/DA_VampsCombatSettings`: DeathMontage y GuardDeathMontage apuntan a esos montages. Conservar todo lo anterior, incluidos Weapons y ComboSteps completos. Comparación final sin diferencias; guardado limitado a assets afectados. [Valores finales](data-assets-despues.json).

## 3. Blueprints

**Hecho — Codex por MCP.** BlueprintTools.create, ObjectTools.set_properties/get_properties, BlueprintTools.compile_blueprint y AssetTools.save_assets.

Crear `/Game/Vamps/Blueprints/BP_VampsMatchPlayerController`, padre VampsMatchPlayerController. Class Defaults: DefaultInputMappingContext IMC_Vamps; MoveAction IA_Move; CombatAction IA_Combat; CycleTargetAction IA_CycleTarget; MobilityAction IA_Mobility; PlayerCameraManagerClass BP_VampsPlayerCameraManager. Input en `/Game/Vamps/Input`, cámara en `/Game/Vamps/Blueprints`.

Crear `/Game/Vamps/Blueprints/BP_VampsMatchGameMode`, padre VampsMatchGameMode. DefaultPawnClass BP_VampsCharacter_Player; BotCharacterClass BP_VampsCharacter; PlayerControllerClass BP_VampsMatchPlayerController; HUDClass VampsCombatTestHUD. ScoreTarget 1000, BaseKillScore 100, StreakScoreBonus 25; PreparationDuration 5, RoundDuration 300, RespawnDelay 5, SpawnProtectionDuration 2. Compilar y guardar ambos.

StartingMaxHealth del Blueprint de personaje configura la vida, valor inicial 100. El respawn usa MaxHealth vigente.

## 4. Mapa y configuración

**Hecho — Codex por MCP.** AssetTools.duplicate/save_assets, SceneTools.load_level/add_to_scene_from_class/trace_world y ObjectTools.set_properties/get_properties.

Duplicar `/Game/Vamps/Maps/L_CharacterBasic` como L_LocalMatch; guardar antes de abrir la copia. World Settings → GameMode Override = BP_VampsMatchGameMode. Conservar el original.

Agregar exactamente dos VampsTeamSpawnPoint: TeamSpawn_A, TeamIndex 0, Location (-200,0,100), Rotation (0,0,0); TeamSpawn_B, TeamIndex 1, Location (600,0,100), Rotation (0,180,0). Escala (1,1,1). Cada origen dispone dos puestos a ±100 cm a su derecha. Suelo comprobado a Z=0. Guardar mapa. [Lectura final](mapa-mcp.json).

Maps & Modes: Editor Startup Map y Game Default Map L_LocalMatch; Default GameMode BP_VampsMatchGameMode. Codex dejó esos valores en DefaultEngine.ini. El override de L_CharacterBasic conserva observación.

## 5. Observación

**Hecho — Codex por MCP, verificación parcial.** StartPIE, consultas de actores/propiedades y CaptureEditorImage verificaron cuatro participantes, tres bots, equipos, inicio, bajas, recompensas, respawns y HUD visible. Una ronda terminó con B ganador 1100–975; movimiento/combate detenidos y respawns cancelados. Luego StopPIE y parche final por Live Coding. Sin automatizar teclas o mouse.

## Comprobación manual final

**Hecho — usuario, 2026-10-27.** Confirmación general de funcionamiento registrada en 053, después de revisar los pendientes en 052. Para reproducirla, abrir L_LocalMatch y pulsar Play:

1. Ver preparación de 5 s y reloj de 5 min. F9 alterna combate/debug conservando el overlay.
2. Al morir, observar variante normal/defensiva y cuerpo sin bloqueo. Durante 5 s mover cámara con WASD/mouse. Al reaparecer debe volver la cámara propia y retirarse la pose de muerte; esta corrección final se compiló al cerrar la iteración.
3. Confirmar vida máxima, guardia cero, energía máxima y protección hasta 2 s. Ataque aceptado la termina antes; solicitud rechazada no.
4. Al finalizar, revisar estadísticas y pulsar PLAY AGAIN: puntos/K/D/rachas a cero, posiciones y recursos restaurados, otra preparación.

Cierres simultáneo y por tiempo conservan revisión de código y aprobación general del usuario, sin nuevas trazas individuales de esos casos. No se requieren tests automatizados.
