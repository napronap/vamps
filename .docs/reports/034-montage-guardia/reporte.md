# 034 — Montages independientes de bloqueo y daño

## Alcance — 2026-10-18

Corrección del paso 3: Guard necesita su propio montage, además de HitReactionMontage. La implementación 033 solo exponía el montaje de daño y señalaba Guard al AnimBP. Se agrega GuardReactionMontage sin reutilizar el de Hit ni elegir un asset por suposición.

## Cambios y conexiones

- `Source/vamps/Public/Combat/Data/VampsCombatSettings.h`: HitReactionMontage y el nuevo GuardReactionMontage aparecen en Reaction del DA, como referencias independientes.
- `Source/vamps/Private/Combat/Data/VampsCombatSettings.cpp`: HasValidReactionSettings rechaza que las dos referencias asignadas apunten al mismo asset, con diagnóstico no fatal que identifica DA y montage.
- `Source/vamps/Private/Characters/VampsCharacter.cpp`: ApplyCombatReaction elige GuardReactionMontage en Guard e HitReactionMontage en Hit. Reutiliza la lógica común de reproducción, plazo, compensación de RateScale, interrupción y limpieza. Stun conserva su señal independiente, sin añadirle un montage en esta corrección.
- Se actualizan el técnico de combate, el índice y el enlace desde 033. No se cambian fases, daño, crítico, carga, NA, memoria defensiva ni montages de ataque.

Las referencias de reacción siguen siendo opcionales: no hay sustitución entre ellas. La elección de clips permanece al usuario; el DA capturado tenía HitReactionMontage=None.

## Preservación

El inventario MCP encontró un DA VampsCombatSettings y ninguna derivada: `/Game/Vamps/Data/DA_VampsCombatSettings`. Se leyeron sus 14 propiedades completas y se guardó el asset antes de editar la clase; [captura](data-assets-antes.json). Se verificaron por MCP los 35 assets Vamps/abiertos, sin cambios pendientes. PIE estaba detenido.

Después del arranque limpio, get_class confirmó `/Script/vamps.VampsCombatSettings` y list_properties expuso los dos campos de montage. Las 14 propiedades anteriores coincidieron exactamente, incluidos arrays y transforms; el nuevo GuardReactionMontage quedó None. HitReactionMontage también sigue None. Se guardó únicamente el DA afectado y se confirmó is_dirty=false. [Captura final](data-assets-despues.json).

## Verificación y pendiente

Se aplicó clang-format a los tres archivos C++ modificados y se revisaron la selección de montage y el rechazo de referencias iguales. No se agregaron ni ejecutaron tests automatizados ni se simuló input. La guía [editor.md](editor.md) identifica configuración y comprobaciones fuera del código. El cierre manual del paso 3 sigue pendiente.

El editor se cerró ordenadamente por shell en 4 s, sin forzarlo; [estado](editor-close.json). build.bat compiló y enlazó correctamente con código 0 y Result: Succeeded en 52,30 s; [estado](build.json), [stdout](build.stdout.log) y [stderr](build.stderr.log). Solo después se abrió mediante editor.bat, con inicialización de 31,22 s, clase nativa y reconexión MCP verificadas; [estado de arranque](editor-launch.json) y [log](editor-validation.log). Las operaciones de procesos/build usaron la autorización del pedido de implementación; las operaciones de assets y lectura se realizaron por MCP.

No se encontraron errores nuevos de combate, Blueprint o fatales en el arranque. Unreal queda abierto y PIE detenido. No se comprobaron visualmente las dos reproducciones porque no hay clips asignados; eso no se infiere de la compilación.
