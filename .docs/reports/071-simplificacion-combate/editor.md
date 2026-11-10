# Editor — Simplificación del combate y la presentación

**Pendiente — usuario.** Abrir `/Game/Vamps/Maps/L_LocalMatch`, iniciar PIE, elegir un personaje y pulsar **START**. No hay asignaciones de assets ni cambios de Blueprint que configurar.

1. Durante la selección, cambiar de personaje y confirmar que el marcador y **START** se actualizan. Comprobar los cambios de vista durante el countdown, la pelea y el resultado de la ronda.
2. En combate, cambiar de objetivo con **F** y dejar que el objetivo muera. Confirmar que el HUD pasa al nuevo objetivo o muestra que no hay uno. Tras el respawn, confirmar que el panel del jugador vuelve a mostrar sus atributos restaurados. Al jugar otra ronda, comprobar que se actualiza con el pawn nuevo.
3. Iniciar un ataque y bloquear otro. Confirmar la entrada de cámara al encuadre de combate en ambos casos y la salida al terminar la cadena. Comprobar ataque aislado y combo encadenado, con el cierre de Recovery y el inicio de NA en el marcador previsto.
4. Pulsar dash cuando el personaje reciba un impacto en el mismo frame. El impacto debe impedir que el dash consuma energía. Confirmar aparte que un dash válido en reposo reproduce su montage y aplica el coste, y que durante Recovery se rechaza.

## Compilación

**Hecho — Codex por shell con autorización para esta iteración.** Con Unreal cerrado, ejecutar `C:\dev\vamps\build.bat`. El script compila y enlaza `vampsEditor Win64 Development` con UE 5.8. UBT devolvió código 0 y `Result: Succeeded`.

## Arranque y selector

**Hecho — Codex por shell con autorización para esta iteración.** Tras compilar, ejecutar `C:\dev\vamps\editor.bat` y esperar a que cargue `/Game/Vamps/Maps/L_LocalMatch`. El editor temporal cargó el mapa y Map Check devolvió 0 errores y 0 advertencias.

**Hecho — Codex por MCP.** Usar `EditorToolset.EditorAppToolset.StartPIE` con `PlayMode_InViewPort`. El log registró `character selection opened`. Se ejecutó `StopPIE`, `IsPIERunning` devolvió `false` y se cerró el editor temporal.
