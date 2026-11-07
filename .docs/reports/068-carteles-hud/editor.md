# Carteles y depuración del HUD en Unreal

**Queda para el usuario.** Comprobar los carteles al iniciar una ronda y al reaparecer. Para probar F9 en Standalone, generar primero el binario base con `build.bat` y reabrir Unreal.

## 1. Configurar y guardar los Widget Blueprints

**Hecho — Codex por MCP.** En `/Game/Vamps/UI/WBP_VampsHUD`, envolver `NoticeText`, `RespawnText` y `TeamText` en `NoticeBackground`, `RespawnBackground` y `TeamBackground`. Usar el brush de `ScoreBackground`, color lineal `(0.025, 0.025, 0.04, 0.86)`, Size to Content y visibilidad inicial `Collapsed`. Dar a los avisos padding horizontal 14 y vertical 6. Centrarlos con offsets `(0, 0)` y `(0, 35)`. Dar al equipo padding horizontal 10 y vertical 4, con anclaje inferior izquierdo y offset `(20, -235)`. Dejar `DebugHint` y `DebugView` inicialmente en `Collapsed`.

**Hecho — Codex por MCP.** En `/Game/Vamps/UI/WBP_VampsFighter`, mover `ContactText` a `DebugView` con posición `(12, 332)` y tamaño `356 × 20`. Mover `DebugContact` a `(12, 352)` con tamaño `356 × 68` y vaciar el texto inicial de `StatusText`. Se usaron `UMGToolSet` y `ObjectTools` para editar y releer ambos árboles. `CompileWidgetBlueprint` compiló los dos y `AssetTools.save_assets` los guardó. `is_dirty` confirmó el guardado.

## 2. Aplicar el C++ al editor abierto

**Hecho — Codex por MCP.** `LiveCodingToolset.LiveCodingToolset.CompileLiveCoding` compiló y aplicó un parche en el editor.

## 3. Comprobar en juego

**Hecho — Codex por MCP.** Se inició PIE en `/Game/Vamps/Maps/L_LocalMatch` con `EditorAppToolset.StartPIE`. El log confirmó la apertura de la selección. Se cerró con `StopPIE` y `IsPIERunning` devolvió `false`.

**Pendiente — usuario.** En `/Game/Vamps/Maps/L_LocalMatch`, iniciar PIE, elegir personaje y pulsar **START**. Comprobar el fondo de `MATCH STARTS IN` y `TEAM A`. Durante la pelea, comprobar que los paneles normales no muestran `READY`, la fase de ataque ni `LAST`. Al morir, comprobar que `RESPAWN IN` aparece con fondo y que el cartel entero se oculta al reaparecer. Pulsar F9 durante la ronda para abrir y cerrar el panel técnico. Detener PIE.

**Pendiente — usuario.** Cerrar Unreal, ejecutar `C:\dev\vamps\build.bat` y reabrir con `C:\dev\vamps\editor.bat`. En el menú de Play elegir **Standalone Game**, iniciar una ronda y comprobar que el aviso de F9 está ausente y que la tecla no abre el HUD de depuración. Cerrar la sesión de Standalone.
