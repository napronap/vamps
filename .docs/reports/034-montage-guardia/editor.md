# Editor — Montages de Guard y Hit

**Para el usuario:** asignar dos montages diferentes en el DA y observar bloqueo y daño abierto. No se eligieron clips ni se sustituyó una reacción por el montaje de la otra. Esta corrección no cierra la etapa 3.

## 1. Captura previa

**Hecho — Codex por MCP.** AssetTools.find_assets y ObjectTools.search_subclasses encontraron un único DA, sin derivadas. list_properties/get_properties capturaron las 14 propiedades de `/Game/Vamps/Data/DA_VampsCombatSettings`, incluidos arrays y transforms; save_assets lo guardó antes del cambio. Se comprobaron 35 assets Vamps/abiertos, todos sin cambios pendientes. PIE estaba detenido.

## 2. Configurar las reacciones

**Hecho — Codex por shell, dentro de la autorización del trabajo del paso 3.** Cierre ordenado, build completo correcto y reapertura del proyecto. **Hecho — Codex por MCP.** Relectura de la clase nativa y 15 propiedades: las 14 anteriores intactas y GuardReactionMontage=None; guardado selectivo del DA y comprobación de is_dirty=false. El editor está abierto, PIE detenido; no hace falta otro build para configurar los campos.

**Pendiente — usuario.** Abrir `/Game/Vamps/Data/DA_VampsCombatSettings` y desplegar Reaction:

1. Asignar en Hit Reaction Montage el montage de recepción de daño abierto.
2. Asignar en Guard Reaction Montage otro montage para bloqueo.
3. Ambos deben ser diferentes entre sí y de Attack Montage, compatibles con skeleton/slot del mesh, con Auto Blend Out, RateScale constante positivo y sin Time Stretch Curve.
4. Guardar únicamente el DA afectado. Si se asigna el mismo asset a ambos campos, la validación informa el DA y el montage y rechaza la configuración.

La duración de Guard y Hit sigue viniendo de Contact Reaction Duration del atacante, no de la longitud del clip. El código ajusta velocidad, detiene la reacción anterior y suprime root motion para conservar un solo desplazamiento impuesto. Dejar una referencia vacía no reproduce la otra.

## 3. Comprobar en PIE

**Pendiente — usuario.** En L_CharacterBasic, observar un bloqueo y un daño abierto con los clips elegidos. Deben reproducir Guard y Hit respectivamente. Verificar el cambio de uno a otro bajo contactos sucesivos y que ambos terminen de acuerdo con Reaction Time. R debe limpiar la reproducción activa. Las señales bIsGuarding/bIsHitReacting y el HUD siguen mostrando el estado real.

Compilación, arranque y conservación de valores: [reporte](reporte.md).
