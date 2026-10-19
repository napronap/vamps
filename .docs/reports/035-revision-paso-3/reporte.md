# 035 — Revisión del estado del paso 3

## Resultado — 2026-10-19

La implementación de resultados de daño y defensa está presente. La configuración actual avanzó respecto de [033](../033-resultados-combate/reporte.md) y [034](../034-montage-guardia/reporte.md): Guard y Hit tienen montages independientes asignados y guardados, y ContactReactionDuration vale 1 s en los tres pasos. El roadmap mantiene correctamente la etapa **pendiente de cierre manual**; esta revisión no acredita un cierre ni asigna un porcentaje de avance.

No hubo cambios de código, configuración ni assets por Codex. Los cambios de assets encontrados eran previos a esta revisión. Se agregan este reporte, su [guía](editor.md), la [captura MCP](estado-mcp.json) y el enlace desde el índice. Los reportes anteriores conservan su evidencia histórica.

## Estado comprobado

- Revisión del código de UVampsCombatResolver, AVampsCharacter, UVampsCombatSettings, UVampsAttributeSet, UVampsCombatMovementComponent, UVampsAnimInstance y HUD: están implementados daño abierto/residual, crítico por contacto, Guard/Hit/Stun, memoria defensiva por atacante/paso/generación, carga/recuperación/ruptura, cancelación de ofensiva sin crear NA y resolución conjunta de contactos capturados. Esta lectura no sustituye la observación en juego.
- MCP responde sobre /Game/Vamps/Maps/L_CharacterBasic; PIE está detenido. No se inició otra sesión ni se simularon acciones.
- BP_VampsCharacter y BP_VampsCharacter_Player heredan respectivamente de /Script/vamps.VampsCharacter y /Script/vamps.VampsCharacter_Player. Ambos referencian /Game/Vamps/Data/DA_VampsCombatSettings y tienen AttackDamage=12. El mesh del jugador es SKM_Manny_Simple y usa /Game/Vamps/Animations/Executioner/ABP_Executioner.
- El DA conserva una BP_VampsGreatSword en HandGrip_R con transform identidad, AM_Executioner_Combo y tres pasos con alineación/empuje 130/100, 130/100 y 150/250 cm. NA=0,5 s; reacción=1 s por paso; crítico=24 al 10 %; reducción plana=10; capacidad/carga=100/25; recuperación=20/s después de 1 s; stun=2 s; solicitud defensiva=0,5 s.
- HitReactionMontage apunta a /Game/Vamps/Animations/Executioner/AM_Executioner_Hit y GuardReactionMontage a /Game/Vamps/Animations/Executioner/AM_Executioner_Block. Son distintos entre sí y del ataque. Ambos tienen DefaultSlot, RateScale=1, Auto Blend Out activo y TimeStretchCurve sin marcadores. Block contiene Defence_Hit_IP de GreatSwordAnimV1; Hit contiene Hit_RM del mismo pack. Longitudes: 1,7667 y 1,5 s. El código ajusta la reproducción a la duración lógica de reacción y suprime root motion durante ella.
- DA y ambos montages tienen is_dirty=false: están guardados en disco. Las rutas de skeleton de reacción y ataque difieren; las listas compatibleSkeletons consultadas están vacías. Esto por sí solo no demuestra un fallo: queda observar la reproducción y sus transiciones en el mesh efectivo.

## Evidencia de intercambios ya realizados

El log disponible de esta sesión, leído por MCP, contiene 154 contactos entre el jugador y el rival. Son acciones anteriores a esta auditoría, no ejecutadas por Codex.

| Resultado | Daño | Contactos |
|---|---:|---:|
| Abierto normal | 12 | 68 |
| Abierto crítico | 24 | 7 |
| Bloqueado normal | 2 | 69 |
| Bloqueado crítico | 14 | 10 |

Los valores coinciden con los DA y el daño de ambos personajes. La mayor carga registrada es 69,17/100 y no hay GuardBreak en esos contactos. El log permite acreditar estos resultados de daño; no incluye suficiente estado para certificar duración de reacciones, muerte por residual, memoria A→B→A, permisos de movimiento o reproducción visual.

La consulta de errores de combate, Blueprint, PlayLevel y fatales devolvió cero entradas. Una consulta adicional encontró warnings de PoseAsset desactualizados respecto de sus animaciones fuente dentro de GreatSwordAnimV1 y DualSwordAnimationV4. No se modificaron esos recursos ni se atribuye a esos warnings un fallo de combate.

## Compilación y estado de Git

La compilación completa correcta de 034 consta en su build.json y build.stdout.log. En esta revisión no se recompiló ni reinició el editor: no se cambió código y la compilación histórica no se presenta como una ejecución en esta PC.

Al iniciar la revisión, Git mostraba:

- Modificado: Content/Vamps/Data/DA_VampsCombatSettings.uasset.
- Sin seguimiento: Content/Vamps/Animations/Executioner/AM_Executioner_Block.uasset.
- Sin seguimiento: Content/Vamps/Animations/Executioner/AM_Executioner_Hit.uasset.

Los tres archivos existen localmente y sus valores se leyeron por MCP, pero estos cambios no forman parte de un commit. Para conservarlos al cambiar de PC deben incluirse juntos en Git. No se agregaron al índice, confirmaron ni enviaron cambios.

## Qué falta acreditar para cerrar 3

1. Ruptura al alcanzar capacidad, residual en el contacto que rompe, stun de 2 s sin defensa y carga cero al salir.
2. Residual que lleva HP a cero, guardia 360° y memoria A→B→A con varios agresores.
3. Renovación por subimpacto del mismo paso, admisión nueva entre pasos, cancelación de ofensiva propia sin crear NA, restricciones durante reacción y limpieza por R.
4. Presentación real de Guard/Hit y distinción de stun/NA, con salida por el plazo lógico actual de 1 s para Guard/Hit.

Estos puntos pueden haberse observado en el trabajo anterior, pero no hay confirmación de cierre ni evidencia suficiente en los reportes/log consultados. No se presentan como funcionalidades ausentes. La [guía](editor.md) indica cómo completar o confirmar la revisión.
