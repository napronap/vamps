# 011 — Probar combate con la IA nueva

Continuación: [012 — Reparentar y mover el AnimBP](../012-animacion-vamps/editor.md). La migración todavía no reemplaza las asignaciones guardadas de esta guía.

**Pendiente para el usuario:** comprobar F6/R y daño/bloqueo en el viewport. Revisar Attack Trace si el ataque no produce daño. Antes de reiniciar el editor o empaquetar, cerrar Unreal y consolidar las clases reflejadas nuevas con una compilación completa. La sesión actual ya tiene el parche aplicado, los assets guardados y L_CharacterBasic configurado, con Passive inicial y PIE detenido.

[Reporte](reporte.md) · [Mesh, montage y socket existentes](../009-personajes-base/editor.md#4-socket-y-montage).

## 1. Compilación

**Hecho — Codex por MCP.** LiveCodingToolset.LiveCodingToolset.CompileLiveCoding devolvió Success y creó un parche real patch_3. VampsAIController, VampsCombatTestGameMode y VampsCombatTestPlayerController quedaron disponibles. No fue NoChanges.

**Pendiente — usuario:** al consolidar las clases nuevas, detener PIE, cerrar Unreal y compilar vampsEditor / Development Editor / Win64 para C:\dev\vamps\vamps.uproject desde Rider/Visual Studio. Reabrir después del éxito. La herramienta de Live Coding informó posibles problemas de empaquetado por cambios de tipos. No hay una operación MCP directa disponible para la compilación completa con el editor cerrado.

## 2. Configurar la IA reutilizable

**Hecho — Codex por MCP.** BlueprintTools.create/compile_blueprint, ObjectTools y AssetTools.save_assets/is_dirty.

En /Game/Vamps/Blueprints/BP_VampsAIController, Parent Class = VampsAIController. Class Defaults → AI:

- Attack Distance = 100 cm.
- Attack Interval = 1 s.
- Mixed Mode Duration = 4 s.
- Mode = Passive; el GameMode de prueba asigna su modo inicial al crear la instancia.

Compilar y guardar. La IA usa SetFocus nativo y SetMode; no requiere referencias al GameMode de prueba. Al integrarla en otro modo, asignar/poseer el personaje, dar un foco y elegir el modo después de la posesión.

## 3. Configurar controles de prueba

**Hecho — Codex por MCP.** DataAssetTools.create, AssetTools.duplicate, ObjectTools y BlueprintTools.

En /Game/Vamps/Input están IA_CycleAIMode e IA_ResetCombat, ambas Boolean y sin triggers adicionales. IMC_VampsCombatTest duplica IMC_Vamps y añade:

| Acción | Tecla | Evento nativo |
|---|---|---|
| IA_CycleAIMode | F6 | Started |
| IA_ResetCombat | R | Started |

Conserva WASD, Mouse2D, LMB y RMB con los modificadores de 009. No utiliza F1; BaseInput.ini asigna F1 a wireframe y no define un DebugExecBinding para F6.

En /Game/Vamps/Blueprints/BP_VampsCombatTestPlayerController, Parent Class = VampsCombatTestPlayerController. Class Defaults:

- Default Input Mapping Context = IMC_VampsCombatTest.
- Move/Look/Attack/Guard Action = las cuatro acciones existentes de /Game/Vamps/Input.
- Cycle AI Mode Action = IA_CycleAIMode.
- Reset Combat Action = IA_ResetCombat.

Compilar y guardar. BP_VampsPlayerController e IMC_Vamps siguen reservados al juego normal y no se modifican.

## 4. Configurar el GameMode y el mapa

**Hecho — Codex por MCP.** BlueprintTools, SceneTools, ObjectTools, AssetTools y ConfigSettingsToolset.

En /Game/Vamps/Blueprints/BP_VampsCombatTestGameMode, Parent Class = VampsCombatTestGameMode. Class Defaults:

| Propiedad | Valor |
|---|---|
| Default Pawn Class | BP_VampsCharacter_Player |
| Player Controller Class | BP_VampsCombatTestPlayerController |
| HUD Class | DuelHUD |
| Opponent Class | BP_VampsCharacter |
| Opponent Controller Class | BP_VampsAIController |
| Initial AI Mode | Passive |
| Player Spawn | Location (-220, 0, 100), Rotation (0, 0, 0), Scale (1, 1, 1) |
| Opponent Spawn | Location (220, 0, 100), Rotation (0, 180, 0), Scale (1, 1, 1) |

Compilar y guardar. HUD y controles de depuración solo deben asignarse en este GameMode de prueba.

En /Game/Vamps/Maps/L_CharacterBasic, World Settings → GameMode Override = BP_VampsCombatTestGameMode. Se retiró el VampsTarget colocado en la iteración 009; no colocar otro receptor para sustituirlo, porque el GameMode crea su rival. Se conserva PlayerStart y el suelo. Guardar el mapa.

BP_VampsGameMode normal usa BP_VampsCharacter_Player, BP_VampsPlayerController y HUD nativo. En Project Settings → Maps & Modes, Editor Startup Map y Game Default Map = L_CharacterBasic, Default GameMode = BP_VampsGameMode. El override del mapa selecciona el modo de prueba. Estos tres cambios se realizaron y consultaron mediante ConfigSettingsToolset; ya no apuntan al duelo viejo.

## 5. Comprobaciones realizadas

**Hecho — Codex por MCP (alcance parcial).** EditorAppToolset inició/detuvo PIE, SceneTools consultó actores, AbilitySystemInspectorToolset consultó atributos/habilidades/tags y LogsToolset revisó LogPlayLevel. Los modos iniciales se seleccionaron temporalmente desde los defaults del Blueprint y se restauró Passive al terminar.

- Passive: jugador, rival y controller/IA nuevos; HUD activo, vida 100/100 y habilidades inactivas.
- Guard: rival con GA_Guard activa y tag Duel.State.Guard.
- Attack: rival se acercó a unos 69 cm entre centros y activó GA_Attack/tag Attack. No se observó pérdida de vida del jugador.
- Sidestep: rival cambió posición y orientación, sin activar ataque/guardia.
- Mixed: se observó desplazamiento con el modo Mixed seleccionado; el ciclo completo queda para observar manualmente.
- GameMode normal: un jugador, controller normal, IMC_Vamps y HUD nativo; sin rival, IA ni componentes de depuración de combate.

No hubo errores ni warnings de LogPlayLevel en esas sesiones. El guardado final confirmó los assets sin cambios pendientes, override de prueba restaurado, Initial AI Mode = Passive e IsPIERunning = false.

## 6. Interacción pendiente

**Pendiente — usuario.**

1. Pulsar Play local en L_CharacterBasic con foco en el viewport. Deben aparecer el jugador y un rival. El HUD debe mostrar AI: Passive y las ayudas F6/R.
2. Pulsar F6 una vez por cambio: Passive → Attack → Guard → Sidestep → Mixed → Passive. El cambio cancela las acciones anteriores y espera 0,75 s antes de empezar las decisiones del nuevo modo. La vista no debe cambiar a wireframe por F6.
3. En Attack, mantener RMB para cubrirse y soltar para recibir un contacto. En Guard, acercarse y golpear al rival: debe conservar vida y cancelar el ataque del jugador. Confirmar vida/estados con el HUD.
4. Si los ataques se activan pero no quitan vida, abrir AM_BasicAttack y revisar el Notify State Attack Trace en la pista del montage: Branching Point, Trigger Chance = 1 y ventana durante el contacto del puño, antes del blend-out. Revisar también el socket FistHit y que el golpe use esa mano. Los tiempos/offsets deben ajustarse en preview; el MCP disponible no permite editar/verificar ese notify. Ver el paso 4 de 009 para los detalles.
5. En Sidestep, comprobar que el rival se desplaza lateralmente mientras mira al jugador. En Mixed, observar Attack/Guard/Sidestep en orden, cuatro segundos por comportamiento, sin dejar guardia o ataque activos al cambiar.
6. Pulsar R durante movimiento, ataque y guardia: ambos personajes deben volver a sus posiciones, recuperar vida máxima y cancelar habilidades/trazas. La IA conserva el modo seleccionado y reinicia su espera inicial; Mixed comienza nuevamente por Attack. Volver a pulsar RMB/LMB para solicitar nuevas acciones después del reinicio.

No se automatizan estos inputs ni se agregan suites, fixtures o arneses. El modo de prueba es la escena de depuración solicitada.
