# Editor — Pausa y salida

**Pendiente — usuario.** Cerrar Unreal, compilar `vampsEditor` en `Win64 Development` y reabrir el proyecto para cargar los cambios de C++ en un proceso nuevo. Abrir `/Game/Vamps/Maps/L_LocalMatch` en **Standalone Game** para probar Escape. Escape es el atajo de detención habitual en PIE.

1. En el selector, pulsar Escape y comprobar que el menú de pausa sigue cerrado. Comprobar que **Quit Game** cierra el juego desde el selector y volver a abrirlo.
2. Iniciar la ronda y pulsar Escape durante la cuenta regresiva y durante una pelea. Comprobar que el reloj queda fijo y el cursor permite usar el menú.
3. Reanudar una vez con **Resume** y otra con Escape. Confirmar que vuelve el control del personaje y que sostener Escape al abrir la pausa no la cierra.
4. Abrir la pausa y usar **Quit Game**. Confirmar que se cierra la partida.

**Hecho — Codex por MCP.** `DataAssetTools.create` y `AssetTools.save_assets` crearon y guardaron `/Game/Vamps/Input/IA_Pause` como Boolean. `ObjectTools.set_properties` agregó Escape a `/Game/Vamps/Input/IMC_Vamps`. La relectura confirmó las ocho entradas, incluidos los modifiers previos. [Captura previa](input-antes.json) · [Captura final](input-despues.json).

**Hecho — Codex por MCP.** `LiveCodingToolset.LiveCodingToolset.CompileLiveCoding` aplicó el parche. Se asignó `IA_Pause` a `PauseAction` en `/Game/Vamps/Blueprints/BP_VampsMatchPlayerController`, se compiló el Blueprint y se guardó. La [relectura del controller](controller-despues.json) coincidió con lo esperado. `StartPIE` completó `BeginPlay` en `L_LocalMatch` y `StopPIE` cerró la sesión.
