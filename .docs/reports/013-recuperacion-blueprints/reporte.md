# 013 — Recuperación del setup Vamps

Continuación: [014 — Copias nativas con nomenclatura Vamps](../014-copias-nomenclatura-vamps/reporte.md) sustituye el HUD, efecto y tags activos que todavía tenían nombres Duel. Las animaciones ahora usan AS_BasicAttack1/2/3.

## Resultado

Se restaura la configuración de los personajes y GameModes y se reconstruye la animación mediante MCP directo, exclusivamente sobre `/Game/Vamps`. No se consultaron ni modificaron assets de `/Game/Duel`, ni se ejecutaron movimientos o limpieza global de redirectors.

El usuario regeneró BP_VampsCharacter_Player y trasladó los recursos de animación antes de esta iteración. Codex conservó esas rutas y nombres. No hubo cambios de C++ ni de configuración del proyecto en esta recuperación.

## Assets y conexiones

| Asset en /Game/Vamps | Trabajo y configuración final |
|---|---|
| Blueprints/BP_VampsCharacter_Player | Padre VampsCharacter_Player confirmado. Restaurados SKM_Manny_Simple, ABP_VampsCharacter_C y AM_BasicAttack; AttackDamage = 12; auto posesión de jugador/IA deshabilitada. Cámara nativa con brazo de 400 cm y ControlRotation, confirmada en defaults y PIE. |
| Blueprints/BP_VampsCharacter | Padre VampsCharacter confirmado. Restaurados montage, AnimClass, transform del mesh y actualización de huesos; corregidos auto posesión de IA y colisión del mesh a NoCollision. |
| Animations/BS_VampsCharacter | Reconstruidos sampleData y ejes en el asset existente: Speed 0–420; MM_Idle en 0, MF_Unarmed_Walk_Fwd en 180 y MF_Unarmed_Jog_Fwd en 420; Rate = 1, Loop habilitado. Skeleton SK_Mannequin confirmado. No se copió lógica de Third Person. |
| Animations/ABP_VampsCharacter | Padre VampsAnimInstance confirmado. Creados nuevos nodos Speed, BlendSpace Player y DefaultSlot; conectados a OutputPose y eliminados los nodos anteriores. EventGraph vacío: la actualización vive en C++. El graph mantiene locomoción y montage, sin añadir pose de guardia. |
| Blueprints/BP_VampsGameMode | Restaurado Default Pawn Class = BP_VampsCharacter_Player. Controller BP_VampsPlayerController y HUD nativo confirmados. |
| Blueprints/BP_VampsCombatTestGameMode | Restaurado Default Pawn Class = BP_VampsCharacter_Player. Controller de prueba, rival BP_VampsCharacter, IA BP_VampsAIController, HUD nativo DuelHUD y posiciones de 011 confirmados. Initial AI Mode = Passive al terminar. |
| Blueprints/BP_VampsPlayerController y BP_VampsCombatTestPlayerController | Padres, cuatro acciones y contextos IMC_Vamps/IMC_VampsCombatTest verificados. El de prueba conserva IA_CycleAIMode e IA_ResetCombat. Compilados y guardados. |
| Blueprints/BP_VampsAIController | Padre y defaults Passive, AttackDistance 100, AttackInterval 1, MixedModeDuration 4 confirmados. Compilado y guardado. |
| Maps/L_CharacterBasic | Override BP_VampsCombatTestGameMode confirmado y restaurado después de inspeccionar temporalmente el modo normal; mapa guardado. |

Ambos meshes usan Location (0,0,-88), Rotation (0,-90,0), AlwaysTickPoseAndRefreshBones y sin optimización de frecuencia. Cápsula 34 × 88 cm, velocidad 420 y orientación al movimiento se verificaron; solo el jugador tiene cámara.

AM_BasicAttack conserva un segmento de AS_DuelAttack1 de un segundo y DefaultSlot. Ambos están ahora en `/Game/Vamps/Animations`; el montage se verificó, sin modificarlo. Las animaciones visuales de Characters/Mannequins siguen siendo recursos compartidos.

## Verificaciones

- AnimBP y siete Blueprints Vamps compilados con warnings_as_errors = true, sin errores ni warnings de compilación.
- PIE Passive: jugador regenerado y rival, GAS con Attack/Guard a nivel 1 y vida 100/100, cámara del jugador, controller de prueba, IA y HUD activo. Ambos meshes referencian el AnimBP nuevo y ambos personajes el montage trasladado.
- PIE Attack temporal: rival se acercó y activó GA_Attack y su tag. No se observó daño al jugador. No se considera confirmado el contacto ni el notify de trazas.
- PIE con GameMode normal temporal: un jugador, controller normal y HUD nativo; sin IA ni controller de prueba.
- Sin errores ni warnings de LogPlayLevel. PIE detenido, Passive y override de combate restaurados. Todos los assets de Vamps consultados con is_dirty = false.
- Dependencias registradas de todos los assets Vamps consultadas: ninguna apunta a `/Game/Duel`. Esto no implica borrar ni reparar assets históricos.
- Se conservaron los inputs WASD/Mouse2D/LMB/RMB, F6/R y sus modificadores. No se automatizaron inputs ni se ejecutaron tests.

La presentación visual, F6/R y el contacto siguen siendo comprobaciones manuales. El MCP no expone lectura/edición de los notifies del montage ni consulta directa de la instancia de animación en ejecución. Consultar la [guía](editor.md).

Esta configuración reemplaza los pendientes de migración de [012](../012-animacion-vamps/reporte.md) y las antiguas rutas de animación de [009](../009-personajes-base/reporte.md). El funcionamiento de IA/GameMode continúa según [011](../011-ia-combate-prueba/reporte.md).

## Revisión posterior: qué sigue usando el nombre Duel

Consulta de solo lectura, sin borrados ni cambios de código/assets. Se volvieron a consultar las dependencias de los 22 assets Vamps: ninguna apunta a `/Game/Duel`. Config y vamps.uproject tampoco referencian Duel. Esto cubre el setup Vamps; no certifica otros mapas o assets fuera de ese árbol.

VampsDuelCharacter, DuelPlayerController, DuelAIController, DuelGameMode y DuelAnimInstance constituyen el grupo nativo anterior y no son requeridos por el código nuevo. Su eliminación debe hacerse junto con los assets históricos que los usan.

No borrar por coincidencia de nombre: DuelHUD sigue siendo el HUD del GameMode de prueba; GE_DuelDamage sigue aplicando daño en VampsCharacter; DuelTags y las rutas Duel.State.Attack/Guard y Duel.Data.Damage son usadas por las habilidades y el efecto. AM_BasicAttack referencia `/Game/Vamps/Animations/AS_DuelAttack1`. Estas piezas activas deben conservarse o renombrarse y reasignarse antes de retirarlas.
