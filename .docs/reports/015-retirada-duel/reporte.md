# 015 — Retirada de Duel

Documentación vigente: [017 — Base FGOA y extensiones Vamps](../017-base-fgoa-y-extensiones/reporte.md). No cambia ni confirma los pendientes operativos de esta limpieza.

## Alcance y estado

El usuario confirmó compilación completa y funcionamiento de PIE después de 014 y autorizó eliminar el grupo histórico Duel. Se retiran sus fuentes y los assets que el MCP pudo borrar. Vamps conserva personajes, IA, GameModes, animación, HUD, efecto y tags nuevos.

**Limpieza pendiente de cierre manual:** quedan físicamente `Content/Duel/Data/DA_DuelCombat.uasset` y `Content/Duel/Maps/L_Duel.umap`. El DataAsset no carga porque su clase CombatConfig ya no existe. Para el mapa, MCP retiró la entrada del registro y devolvió éxito en el borrado individual, pero el archivo permanece en disco. No se marca ninguno como borrado. Se solicitó al usuario cerrar Unreal, retirar la carpeta residual y consolidar las clases eliminadas mediante compilación completa.

## Fuentes eliminadas

Headers Public y cpp Private, rutas relativas a Source/vamps:

- Characters/VampsDuelCharacter.h/.cpp.
- Player/DuelPlayerController.h/.cpp.
- AI/DuelAIController.h/.cpp, incluido EDuelAIMode.
- GameModes/DuelGameMode.h/.cpp.
- Animation/DuelAnimInstance.h/.cpp.
- UI/DuelHUD.h/.cpp.
- Combat/Effects/GE_DuelDamage.h/.cpp.
- Combat/CombatTags.h/.cpp, namespace y definiciones nativas DuelTags.

Son 16 archivos. No se modificó la lógica nueva ni se agregaron redirects, aliases o compatibilidad. Source, Config y vamps.uproject ya no contienen referencias textuales Duel ni al header Combat/CombatTags.h anterior. Los archivos nuevos de 014 siguen en su ubicación original.

## Assets

El registro inicial tenía 14 assets bajo `/Game/Duel`, sin referencers externos. Antes de borrar, el mapa abierto era L_CharacterBasic y el HUD de prueba era VampsCombatTestHUD.

Eliminados por MCP y confirmados ausentes en disco:

- Blueprints: BP_DuelCharacter, BP_DuelPlayerController y BP_DuelGameMode.
- Input: IMC_Duel, IA_DuelShared, IA_DuelReset, IA_DuelMove, IA_DuelLook, IA_DuelLock e IA_DuelAIMode.
- Animations: ABP_Duel y AM_DuelAttack.

Se usó AssetTools.delete por subcarpetas, después del rechazo del borrado global por el DataAsset inválido. No se abrieron sus editores ni se consultaron defaults de los Blueprints anteriores. El motor cargó paquetes internamente durante su eliminación.

Se conservan `/Game/Vamps` y los recursos visuales compartidos de Characters/Mannequins. No se borraron documentos históricos, archivos de configuración ni artefactos generados de build. Los reportes anteriores documentan las iteraciones realizadas; el índice apunta a esta limpieza.

## Verificaciones

- LiveCodingToolset.CompileLiveCoding devolvió Success y aplicó patch_0 tras retirar las fuentes. No fue NoChanges.
- Siete Blueprints Vamps y ABP_VampsCharacter compilaron con warnings_as_errors = true, sin errores ni warnings de compilación.
- PIE arrancó con jugador y rival VampsCharacter, VampsAIController y VampsCombatTestHUD. LogPlayLevel no devolvió errores/warnings. PIE detenido y assets comprobados sin cambios pendientes.
- Revisión textual de Source, Config y vamps.uproject sin Duel. Inspección de disco confirmó los dos residuos descritos; el registro MCP por sí solo no certifica su borrado.
- No se ejecutaron tests automatizados ni se automatizó la UI.

La compilación completa y reapertura después de esta eliminación todavía no se han confirmado. Deben retirar las clases/tags anteriores del binario principal, sin depender del estado en memoria de Live Coding. Pasos y atribución en la [guía](editor.md).

La base conservada y sus conexiones se documentan en [014](../014-copias-nomenclatura-vamps/reporte.md) y [013](../013-recuperacion-blueprints/reporte.md).
