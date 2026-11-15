# Editor — Instrucciones en el selector

**Pendiente — usuario.** En PIE en `/Game/Vamps/Maps/L_LocalMatch`, abrir **遊び方**. Confirmar que inicia en japonés y alterna con **English** y **日本語**. En ambos idiomas debe verse el párrafo sobre pulsar RMB repetidamente y el consejo de tomar distancia al acabar el ataque. Verificar que el fondo impide usar el selector mientras el modal está abierto, que se cierra con **閉じる**, **Close** o Escape y que luego se puede elegir personaje y pulsar START.

**Hecho — Codex por MCP.** `ObjectTools.get_properties` confirmó la vista de selección en el Widget Blueprint y las teclas en `IMC_Vamps`.

**Hecho — Codex por MCP.** `LiveCodingToolset.LiveCodingToolset.CompileLiveCoding` devolvió `Success` y aplicó los textos actualizados. Durante la integración, `EditorAppToolset.StartPIE` inició `L_LocalMatch` hasta `BeginPlay`. `StopPIE` cerró la sesión e `IsPIERunning` confirmó que se detuvo.
