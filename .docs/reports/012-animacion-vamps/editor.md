# 012 — Reparentar y mover el AnimBP

Esta guía queda reemplazada por [013 — Configuración recuperada](../013-recuperacion-blueprints/editor.md). No repetir el movimiento desde Duel ni las asignaciones antiguas; las rutas actuales ya están configuradas y guardadas.

**Pendiente para el usuario:** con Unreal cerrado, compilar `vampsEditor / Development Editor / Win64` para `C:\dev\vamps\vamps.uproject` desde Rider/Visual Studio y reabrir el proyecto. Avisar al terminar para continuar por MCP. AGENTS.md reserva al usuario las compilaciones completas y reinicios sin soporte MCP directo. La corrección de previews está escrita pero todavía no compilada; no dar por migrado el AnimBP.

[Reporte](reporte.md) · [Configuración de combate vigente](../011-ia-combate-prueba/editor.md).

## 1. Incorporar la clase nativa

**Hecho — Codex por MCP, versión inicial.** `LiveCodingToolset.LiveCodingToolset.CompileLiveCoding` aplicó patch_4 y expuso VampsAnimInstance. Posteriormente el editor se cerró durante el movimiento del asset.

**Pendiente — usuario.** Compilar con el editor cerrado y reabrir para incorporar la versión corregida de NativeUpdateAnimation, que excluye mundos de preview. No abrir ni guardar un Blueprint reparentado a una clase de un parche que todavía no se haya consolidado.

## 2. Reparentar el AnimBP existente

**Pendiente — Codex por MCP.** En `/Game/Duel/Animations/ABP_Duel`, cambiar Parent Class a `VampsAnimInstance`, compilar y guardar antes de moverlo. Herramientas: BlueprintTools.set_parent, compile_blueprint con warnings_as_errors, get_parent y AssetTools.save_assets/is_dirty.

Equivalente manual reproducible: abrir ABP_Duel → Class Settings → Parent Class = VampsAnimInstance → Compile → Save. El intento anterior confirmó el padre y compilación en memoria, pero no se guardó antes del cierre.

## 3. Mover y renombrar

**Pendiente — Codex por MCP.** Usar AssetTools.move desde `/Game/Duel/Animations/ABP_Duel` hasta `/Game/Vamps/Animations/ABP_Vamps`. Confirmar resultado y existencia del destino, padre y graphs. No repetir a ciegas si aparece un error: consultar primero ambas rutas.

Equivalente manual: en Content Browser, mover ABP_Duel a Vamps/Animations y renombrarlo ABP_Vamps. Si Unreal propone guardar referencias afectadas, revisar BP_VampsCharacter, BP_VampsCharacter_Player y BP_DuelCharacter. No duplicar el AnimBP ni crear una lógica nueva sobre el ejemplo Third Person.

## 4. Asignaciones y guardado

**Pendiente — Codex por MCP.** En ambos `/Game/Vamps/Blueprints/BP_VampsCharacter` y `BP_VampsCharacter_Player`, componente Mesh:

- Animation Mode = Use Animation Blueprint.
- Anim Class = `/Game/Vamps/Animations/ABP_Vamps.ABP_Vamps_C`.

Compilar el AnimBP y los Blueprints afectados, guardar y consultar is_dirty = false. Confirmar los referencers del destino y revisar posibles redirectors producidos por el movimiento. No se agregan CoreRedirects de clase. BS_Duel y AM_BasicAttack conservan sus rutas; moverlos queda fuera de este pedido.

## 5. Verificar ejecución

**Pendiente — Codex por MCP.** Iniciar PIE en L_CharacterBasic, consultar Mesh/AnimInstance del jugador y rival para confirmar ABP_Vamps_C y sus datos; revisar logs de animación/Blueprint. Detener PIE y dejar assets guardados. No simular inputs ni automatizar UI.

**Pendiente — usuario.** Tras completar la migración, observar idle, movimiento y ataque en el viewport. Deben conservar la presentación previa. El graph actual no consume bIsGuarding para una pose de guardia; este pedido no añade esa animación.
