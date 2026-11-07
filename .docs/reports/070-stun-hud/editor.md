# Barra de guardia y aviso de STUN en Unreal

**Queda para el usuario.** Provocar una rotura de guardia con el encuadre de combate activo. Comprobar que `STUN` aparece en el centro exacto de la pantalla y que la barra roja se vacía durante el aturdimiento.

## 1. Compilar y ajustar el panel de combate

**Hecho — Codex por MCP.** `LiveCodingToolset.LiveCodingToolset.CompileLiveCoding` compiló y aplicó el parche. Unreal reinstanció las dos clases de widget y recompiló sus Blueprints.

**Hecho — Codex por MCP.** En `/Game/Vamps/UI/WBP_VampsFighter`, quitar `PanelTitle` y `StatusText` de `CombatView` con `UMGToolSet.RemoveWidget`. Mantener el fondo y los tres medidores. Colocar los textos de HP, GUARD y ENERGY en Y=18, 62 y 106, y sus barras de 10 px de alto en Y=37, 81 y 125. Se recompilaron y guardaron el panel y el indicador. Se confirmaron sus clases padre persistentes y las tres referencias a `WBP_VampsFighter` en `WBP_VampsHUD`.

**Hecho — usuario.** Reabrió Unreal e inició una ronda en `L_LocalMatch`. El log confirmó la apertura de la selección y el inicio de la ronda. La primera recarga de Live Coding había dejado referencias temporales de `VampsFighterWidget` en memoria. El Blueprint del HUD conservó las referencias persistentes en disco.

## 2. Centrar el cartel en el HUD

**Hecho — Codex por MCP.** `ObjectTools` confirmó que `/Game/Vamps/UI/WBP_VampsHUD` usa un Canvas de pantalla completa. `NoticeBackground` tiene Anchor y Alignment `(0.5, 0.5)`, offsets `(0, 0)` y Size to Content. El código muestra `STUN` en `NoticeText` con color lineal `(1, 0.28, 0.24, 1)` y el fondo oscuro existente.

**Hecho — Codex por MCP.** `LiveCodingToolset.LiveCodingToolset.CompileLiveCoding` aplicó el parche de centrado. `UMGToolSet.CompileWidgetBlueprint` confirmó la compilación del HUD. En `/Game/Vamps/UI/WBP_VampsCharacterIndicator`, `RemoveWidget` retiró `StunView` y su hijo `StunLabel`. Se compiló y guardó el indicador y se comprobaron su clase padre persistente y sus dependencias.

**Pendiente — usuario al reabrir el editor.** Con Unreal cerrado, ejecutar `C:\dev\vamps\build.bat` antes de reabrir con `C:\dev\vamps\editor.bat`. Así el binario base incorpora la retirada del enlace `StunView`. El parche ya está aplicado en la sesión actual.

## 3. Comprobar en juego

**Hecho — Codex por MCP, parcial.** `EditorAppToolset.StartPIE` inició `/Game/Vamps/Maps/L_CharacterBasic`. Se comprobaron los dos `CharacterIndicator` con `WBP_VampsCharacterIndicator_C` y el actor HUD con `WBP_VampsHUD_C`. Se detuvo PIE y se volvió a `L_LocalMatch`. La comprobación visual del HUD quedó pendiente del arranque limpio.

**Hecho — Codex por MCP.** Con el cartel centrado, `StartPIE` abrió la selección en `L_LocalMatch`. Se detuvo la sesión y `IsPIERunning` confirmó `false`.

**Pendiente — usuario.** Abrir `/Game/Vamps/Maps/L_LocalMatch`, iniciar PIE y comenzar la ronda. Acumular carga al bloquear hasta romper la guardia propia durante el encuadre de combate. Comprobar que el centro del cartel `STUN` coincide con el centro de la ventana incluso si el personaje cambia de posición en pantalla. Confirmar que el cartel desaparece al recuperar el control, que la barra vuelve al modo de carga normal y que no se superpone el aviso NA. Detener PIE al terminar.
