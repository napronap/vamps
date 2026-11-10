# Editor — Revisión de código

**Pendiente — usuario.** Abrir Unreal con PIE detenido para habilitar la compilación por MCP. El editor estaba cerrado durante esta revisión. Los Blueprints conservan su configuración.

1. **Pendiente — Codex por MCP.** Con el editor conectado, compilar mediante `LiveCodingToolset.LiveCodingToolset.CompileLiveCoding` y confirmar el resultado.
2. **Pendiente — usuario.** Abrir `/Game/Vamps/Maps/L_CharacterBasic` y arrancar PIE. Pulsar **F6** para recorrer `Passive`, `Attack`, `Guard`, `Sidestep` y `Mixed`. Comprobar que el rival conserva el foco sobre el jugador, se acerca en `Attack` y encadena después de conectar. Confirmar que `Mixed` alterna comportamientos y que **R** reinicia el duelo.
3. **Pendiente — usuario.** Abrir `/Game/Vamps/Maps/L_LocalMatch`, arrancar una ronda y observar que los bots adquieren un enemigo al entrar en combate. Cuando un objetivo muere, el bot debe cambiar a otro elegible o esperar. Finalizar la ronda y volver a selección para comprobar que se retiran los bots anteriores.
