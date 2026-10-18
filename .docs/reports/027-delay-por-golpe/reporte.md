# 027 — Tiempo adicional de restricción por golpe

## Resultado — 2026-10-17

Se agregó AdditionalOffensiveRestrictionDuration en cada entrada de ComboSteps del DA del atacante, junto con TargetAlignmentDistance y KnockbackDistance. Es un tiempo adicional en segundos, finito y no negativo; su valor inicial es 0 s para conservar el comportamiento que estaba probando el usuario.

La restricción total es la duración completa de la sección del atacante dividida por su velocidad efectiva, más el tiempo adicional de ese golpe. Se conserva su aplicación al bloquear: impide nuevos inicios y continuaciones ofensivas, sin crear una restricción nueva para impactos abiertos ni modificar NA. El tiempo adicional se suma después del ajuste de velocidad.

El usuario completó la compilación y reapertura. Codex verificó por MCP el campo nuevo, conservó/reaplicó la configuración y confirmó que el DA guardado coincide exactamente con la captura previa más los adicionales en cero. El arranque y cierre en PIE no generaron errores de combate. Continúa [026 — Diagnósticos del paso 2](../026-diagnosticos-paso-2/reporte.md). No cierra la etapa 2.

## Configuración preservada antes del cambio

La búsqueda por MCP encontró un asset de VampsCombatSettings: /Game/Vamps/Data/DA_VampsCombatSettings. Se leyeron todas sus propiedades expuestas, se registraron en [data-assets-antes.json](data-assets-antes.json) y se guardó el asset por MCP antes de modificar la estructura. El asset no tenía cambios sin guardar.

| Paso | TargetAlignmentDistance | KnockbackDistance | Adicional final |
|---|---:|---:|---:|
| 1 | 130 cm | 100 cm | 0 s |
| 2 | 130 cm | 100 cm | 0 s |
| 3 | 150 cm | 250 cm | 0 s |

Se preservan también AttackMontage = /Game/Vamps/Animations/Executioner/AM_Executioner_Combo, NARecoveryDuration = 0,5 s, HitReactionMontage vacío y una entrada de Weapons con BP_VampsGreatSword en HandGrip_R y transform identidad. Estos son los valores leídos del editor, no los defaults del código ni los valores de iteraciones anteriores.

Tras el arranque limpio, los valores anteriores seguían intactos y el campo nuevo ya tenía cero. Se reaplicó ComboSteps con las tres entradas capturadas y los adicionales en cero; save_assets devolvió true. La relectura de todas las propiedades coincidió exactamente con lo esperado e is_dirty devolvió false. La captura final está en [data-assets-despues.json](data-assets-despues.json).

## Archivos y conexiones

- Source/vamps/Public/Combat/Data/VampsCombatSettings.h: agrega el float reflejado al struct FVampsCombatStepSettings, con unidad segundos, mínimo de editor cero y tooltip. El orden y la cantidad de entradas del array no cambian.
- Source/vamps/Private/Combat/Abilities/GA_Attack.cpp: rechaza un adicional negativo o no finito antes de reproducir el ataque, con sección y valor en el diagnóstico.
- Source/vamps/Public/Combat/VampsCombatResolver.h y Private/Combat/VampsCombatResolver.cpp: el contacto captura OffensiveRestrictionDuration como sección ajustada por velocidad + adicional del paso. Valida el adicional y el total antes de encolar; al bloquear, entrega ese total al receptor. Las distancias y el orden de resolución se mantienen.
- Source/vamps/Public/Characters/VampsCharacter.h y Private/Characters/VampsCharacter.cpp: renombra el parámetro de ApplyOffensiveRestriction a RestrictionDuration para representar el total recibido. Conserva inicio desde el contacto y vencimiento más lejano entre restricciones sucesivas.
- AGENTS.md: incorpora el procedimiento obligatorio de inventario, captura durable, preservación, restauración y verificación de assets existentes al cambiar su clase de DA o structs contenidos. Incluye referencias, arrays, transforms, guardado selectivo y recargas estructurales.

Manny, AnimBP, montages y Blueprints de armas conservan sus conexiones. Solo se guardó por MCP el DA afectado con su configuración original y el campo nuevo. No se modificaron especificaciones funcionales/técnicas ni roadmap.

## Verificaciones y pendientes

- AssetTools.find_assets, ObjectTools.list_properties/get_properties y AssetTools.save_assets confirmaron inventario, captura y guardado previo del DA.
- Revisión de la suma y sus llamadores; clang-format aplicado a los archivos C++ modificados y git diff --check sin errores de whitespace.
- El primer intento de compilación completa del usuario fue rechazado por UHT porque el formateador dividió el tooltip de UPROPERTY en dos literales. Se acortó a un literal y se revisó el resultado formateado. El usuario confirmó después compilación correcta y reapertura; el log de UBT registró Result: Succeeded.
- Se eligió arranque limpio para el cambio de tamaño del struct; no se aplicó Live Coding estructural ni se guardaron assets bajo tipos transitorios. La compilación completa/reapertura se deja al usuario conforme a las reglas MCP del repositorio.
- Después de reabrir, ObjectTools.list_properties confirmó el float nuevo y tipos nativos sin referencias transitorias. get_class devolvió /Script/vamps.VampsCombatSettings. El inventario siguió mostrando un único DA; search_subclasses devolvió únicamente la clase base.
- ObjectTools.get_properties confirmó igualdad de las cinco propiedades completas respecto de la captura anterior, añadiendo exclusivamente el nuevo campo a cada golpe. set_properties y save_assets devolvieron true; la relectura y comprobación de is_dirty=false confirmaron el estado final. Después de PIE el DA siguió sin cambios pendientes.
- StartPIE/StopPIE en L_CharacterBasic: dos personajes en Idle, ComboStep=0 y CanStart=true, un arma por personaje y ambos referidos al mismo DA preservado. LogVampsCombat no registró errores en el arranque ni en el cierre. PIE quedó detenido.
- Pendiente únicamente calibrar el adicional y observar el efecto durante bloqueos reales con las animaciones elegidas. No se midió un bloqueo con adicional distinto de cero; el cálculo se verificó por revisión y compilación. Pasos en la [guía](editor.md). No se agregaron ni ejecutaron tests automatizados.
