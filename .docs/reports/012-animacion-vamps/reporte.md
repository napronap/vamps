# 012 — AnimInstance y AnimBP con nomenclatura Vamps

Estado reemplazado por [013 — Recuperación del setup Vamps](../013-recuperacion-blueprints/reporte.md). El usuario regeneró el personaje y movió los recursos; Codex reconstruyó y guardó la configuración en las rutas actuales. Los pendientes de migración de este reporte corresponden al cierre anterior.

## Alcance y estado

Se crea `UVampsAnimInstance` directamente sobre `UAnimInstance`, sin heredar de `DuelAnimInstance`. El objetivo es reparentar el AnimBP propio existente y moverlo a `/Game/Vamps/Animations/ABP_Vamps`, actualizando sus referencias.

**Entrega parcial:** el reparentado se verificó en memoria y el AnimBP compiló sin errores ni warnings. Al mover el asset, Unreal se cerró durante la generación de la miniatura de BP_DuelCharacter. No llegó a confirmarse el movimiento ni el guardado; ABP_Duel sigue en su ruta original en disco y no hay ABP_Vamps guardado. Falta retomar las operaciones por MCP después de la compilación completa y reapertura del usuario.

## Código y relaciones

- Nuevos `Source/vamps/Public/Animation/VampsAnimInstance.h` y `Source/vamps/Private/Animation/VampsAnimInstance.cpp`. La clase actualiza Speed, Direction, bInAir y bIsGuarding desde VampsCharacter. Conserva los nombres de los datos que consume el graph actual; no añade bindings, estado de combate independiente ni métodos pasamanos.
- NativeUpdateAnimation omite mundos de preview y ausencia de pawn. En mundos de juego exige VampsCharacter mediante CastChecked. La exclusión del preview corrige el cierre durante las miniaturas del editor, sin dar soporte de juego al personaje viejo de Duel.
- DuelAnimInstance y su código permanecen sin modificaciones.
- El AnimGraph existente conecta Speed → BS_Duel → DefaultSlot → OutputPose. El EventGraph contiene UpdateAnimation y TryGetPawnOwner sin lógica conectada. No se modifica la locomoción ni se añade una pose de guardia.
- Referencias originales de ABP_Duel: BP_VampsCharacter, BP_VampsCharacter_Player y BP_DuelCharacter. Las referencias de los dos personajes nuevos deben terminar en ABP_Vamps_C. El movimiento también afecta la referencia histórica del Blueprint viejo; no se migra su lógica ni se lo integra al setup nuevo.

No hay cambios confirmados de assets en disco en esta iteración. No se agregan redirects de clase. El estado de posibles redirectors de assets deberá consultarse después del movimiento exitoso.

## Verificaciones y bloqueo

1. LiveCodingToolset.CompileLiveCoding devolvió Success, generó patch_4 e incorporó VampsAnimInstance. No fue NoChanges.
2. BlueprintTools.set_parent devolvió y get_parent confirmó `/Script/vamps.VampsAnimInstance`. compile_blueprint con warnings_as_errors = true terminó sin error.
3. AssetTools.move cerró el editor. Saved/Logs/vamps.log y el CrashContext registran el cast fallido de BP_DuelCharacter_C a VampsCharacter en NativeUpdateAnimation, durante SavePackage de una miniatura. No se atribuye el cierre al reparentado ni a un bug de MCP.
4. Se corrigió el código para excluir previews y se aplicó clang-format a ambos archivos nuevos. **La corrección todavía no está compilada**, porque Unreal y el servidor MCP quedaron cerrados.
5. La inspección de archivos y git confirmó ABP_Duel sin cambios en disco y ausencia de ABP_Vamps guardado. El reparentado exitoso en memoria no se considera persistido.

Pendientes: compilar y reabrir, repetir reparentado con guardado inmediato, mover, verificar y guardar referencias, compilar los Blueprints afectados, consultar estado final y comprobar las instancias en PIE. Pasos en la [guía del editor](editor.md). No se ejecutaron tests automatizados.

La configuración de IA, GameMode y HUD sigue documentada en [011](../011-ia-combate-prueba/reporte.md). Esta iteración solo reemplazará la asignación de AnimBP cuando se complete el guardado.
