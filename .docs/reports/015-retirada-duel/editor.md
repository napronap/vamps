# 015 — Finalizar la retirada de Duel

**Queda para el usuario:** cerrar Unreal, eliminar `C:\dev\vamps\Content\Duel`, compilar vampsEditor / Development Editor / Win64 y reabrir con MCP habilitado. La carpeta solo conserva dos archivos que las operaciones MCP no pudieron eliminar físicamente. No hace falta rehacer los Blueprints Vamps.

[Reporte](reporte.md) · [Setup conservado](../014-copias-nomenclatura-vamps/editor.md).

## 1. Preparación

**Hecho — usuario.** Confirmar compilación completa y funcionamiento de PIE de la migración 014. Reabrir/habilitar el servidor MCP para esta limpieza.

**Hecho — Codex por MCP.** IsPIERunning = false, mapa abierto `/Game/Vamps/Maps/L_CharacterBasic`, HUD VampsCombatTestHUD. AssetTools.find_assets/get_referencers confirmó 14 assets Duel y ninguna referencia desde fuera de `/Game/Duel`.

## 2. Retirar assets históricos

**Hecho — Codex por MCP.** AssetTools.delete eliminó las subcarpetas Blueprints, Input y Animations de `/Game/Duel`; se confirmó su ausencia en registro y disco. El intento del directorio completo falló por DA_DuelCombat. No se abrieron editores de los Blueprints anteriores.

**Pendiente — usuario.** Con Unreal cerrado, eliminar mediante el Explorador la carpeta literal `C:\dev\vamps\Content\Duel`. Sus únicos archivos residuales son:

- Data\DA_DuelCombat.uasset: Unreal rechaza cargarlo porque falta CombatConfig y su herramienta de eliminación requiere cargarlo.
- Maps\L_Duel.umap: MCP lo retiró del registro, pero el archivo permanece en disco.

No eliminar `Content\Vamps` ni `Content\Characters`. AGENTS.md exige dejar al usuario la operación que MCP no logra completar; Codex no sustituye la eliminación por un script de archivos ni recrea clases temporales para cargar el DataAsset roto.

## 3. Retirar fuentes y consolidar

**Hecho — Codex, edición de archivos.** Eliminados los 16 headers/cpp históricos enumerados en el reporte. Source/Config/uproject no conservan referencias Duel. No se agregaron redirects.

**Hecho — Codex por MCP.** CompileLiveCoding devolvió Success y creó patch_0. Esto verifica la compilación de la sesión, pero no sustituye el binario principal sin las clases retiradas.

**Pendiente — usuario.** Después del paso 2, con Unreal cerrado, compilar `vampsEditor / Development Editor / Win64` para `C:\dev\vamps\vamps.uproject` desde Rider/Visual Studio. Reabrir después del éxito, activar MCP y avisar para la comprobación final. No hay herramienta MCP directa de compilación con el editor cerrado.

## 4. Verificaciones

**Hecho — Codex por MCP, antes de consolidar.** Compilar/guardar los siete Blueprints de Vamps y ABP_VampsCharacter. PIE con jugador, rival, IA y HUD nuevos; LogPlayLevel sin errores/warnings, PIE detenido y assets guardados.

**Pendiente — Codex por MCP y lectura de archivos.** Después de la reapertura, confirmar carpeta Duel ausente en disco y registro, tags Duel sin registrar, padres y referencias Vamps correctos y arranque de PIE. No se realiza otra edición de assets si la comprobación confirma el estado previsto.
