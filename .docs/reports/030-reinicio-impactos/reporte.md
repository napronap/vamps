# 030 — Reinicio opcional de impactos por arma

La etapa 2 fue cerrada posteriormente por confirmación del usuario; véase [031 — Cierre del paso 2 y reglas del paso 3](../031-cierre-paso-2-y-reglas-paso-3/reporte.md). Las comprobaciones de esta iteración se conservan sin atribuirles observaciones posteriores.

## Resultado — 2026-10-18

Se implementó AN_ResetWeaponHits con WeaponIndices editables por aparición del notify. Los índices corresponden al orden de Weapons en el DA del atacante y comienzan en 0. El valor inicial del AN es [0]; [0, 1] selecciona explícitamente ambas armas. Un array vacío es configuración inválida y no significa todas las armas.

Se conserva un único ANS_AttackTrace por sección y un registro de víctimas independiente por arma. Sin ningún AN de reinicio, cada arma solo puede impactar una vez a cada víctima durante el paso, como antes. Los reinicios no avanzan ComboStep, no borran el contacto habilitante y no modifican la continuación al cerrar Combo Window, NA ni las distancias y tiempos del DA. Todas las armas continúan trazando durante el ANS; el nuevo AN solo rearma las seleccionadas.

La decisión reemplaza las alternativas abiertas de [029](../029-revision-paso-2/reporte.md). No cierra la etapa 2 ni incorpora Guard, datos nuevos por subimpacto o varias ventanas de trazas.

## Archivos y conexiones

| Archivo / clase | Cambio y relación |
|---|---|
| Public/Animation/AN_ResetWeaponHits.h y Private/Animation/AN_ResetWeaponHits.cpp — UAN_ResetWeaponHits | Nuevo UAnimNotify nativo, branching point de montage, desactivado en preview. WeaponIndices es EditAnywhere. Entrega payload e índices al personaje; no almacena estado de combate compartido en el notify. |
| Public/Characters/VampsCharacter.h y Private/Characters/VampsCharacter.cpp — AVampsCharacter | ResetCombatWeaponHits comprueba autoridad, reproducción y sección por IsCurrentCombatNotify. Requiere Contacts y una selección no vacía. Valida todos los índices y actores antes de tocar ningún registro; después reinicia únicamente los seleccionados. El array de actores se crea en el mismo orden que las entradas del DA. |
| Public/Combat/VampsWeapon.h y Private/Combat/VampsWeapon.cpp — AVampsWeapon | ResetImpactHits limpia HitActors y la historia del barrido sin desactivar tracing. ResetSectionHits sigue desactivando tracing y reutiliza esa limpieza. La primera muestra después del AN usa la pose actual; no vuelve a barrer el recorrido anterior al reinicio. |
| Private/Combat/Abilities/GA_Attack.cpp — UGA_Attack | Valida cada AN opcional antes de iniciar el ataque: tiempo estrictamente dentro de AttackTrace, selección no vacía e índices existentes en Weapons del DA. Conserva la exigencia de exactamente un AttackTrace por sección. |
| Reporte/guía 030, índice y enlace desde 029 | Registran implementación, configuración opcional y verificaciones; las especificaciones y roadmap conservan su estado. |

Las rutas de código son relativas a Source/vamps/. No se cambió la clase del DA ni sus structs; no hace falta migrar propiedades. No se asignaron notifies ni se guardaron assets. Los BP_VampsGreatSword y personajes usan la lógica nativa actualizada; Manny y AnimBP siguen compartidos.

La lectura MCP previa confirmó que /Game/Vamps/Data/DA_VampsCombatSettings referencia AM_Executioner_Combo y una BP_VampsGreatSword en Weapons[0], socket HandGrip_R y transform identidad. Sus adicionales estaban en 0,5 s por paso; alineación/empuje 130/100, 130/100 y 150/250, NA 0,5 s y HitReactionMontage=None. Esta lectura no autoriza sustituir futuras configuraciones por esos valores.

## Validación y diagnóstico

Los errores previos a la activación incluyen montage, sección, notify y datos inválidos. En ejecución, una selección inválida cancela todo el reinicio con diagnóstico de función, personaje, notify, índice y DA; no deja un reinicio parcial. Los notifies de otra reproducción/sección o sin autoridad se ignoran como eventos ajenos al combate vigente, igual que los ANS existentes. No se agregaron asserts fatales ni fallbacks.

El AN no fuerza un impacto: permite que un próximo barrido válido vuelva a contactar. Si el arma sigue tocando al enemigo, ese nuevo contacto puede llegar inmediatamente. No habilita ventanas individuales de daño por arma ni cambia el perfil por paso de daño, empuje o restricción ofensiva.

## Verificaciones y pendientes

- Revisados el contrato de UAnimNotify/BranchingPointNotify en UE 5.8, los nombres propios contra las jerarquías base y el orden DA→actores equipados.
- Revisados los caminos sin AN, selección de una o varias armas, índice fuera de rango, lista vacía y rechazo completo antes de mutar. Son comprobaciones de código, no ejecuciones de combate.
- Aplicado clang-format a los siete archivos C++ afectados y revisados los resultados; git diff --check no reportó errores de whitespace.
- LiveCodingToolset.LiveCodingToolset.CompileLiveCoding devolvió Result: Success y enlazó/aplicó UnrealEditor-vamps.patch_0. Fue una compilación real, no NoChanges.
- ObjectTools.list_properties/get_properties confirmaron WeaponIndices como array de enteros, default [0] y bShouldFireInEditor=false. get_class devolvió /Script/vamps.AN_ResetWeaponHits, sin clase transitoria.
- La relectura completa por MCP de Weapons, AttackMontage, ComboSteps, NARecoveryDuration y HitReactionMontage coincidió con la lectura previa: se conservaron también los adicionales de 0,5 s. No se guardó ningún asset ni se necesitó reiniciar.
- Comprobados los enlaces de los entregables nuevos. No se inició PIE ni se comprobó un reinicio real por notify en esta iteración; la disponibilidad de la clase y los defaults sí quedaron verificados en el editor.
- Pendiente observar un reinicio real con los notifies y las animaciones elegidas. No se insertó un AN en un frame supuesto. Pasos en [editor.md](editor.md). No se agregaron ni ejecutaron tests automatizados.
