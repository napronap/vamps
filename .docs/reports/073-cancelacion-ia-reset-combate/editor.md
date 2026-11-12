# Editor — Cancelación de acciones de IA y reset de combate

**Pendiente — usuario.** Compilar con Unreal cerrado, abrir el editor y comprobar el cambio de conducta, el reset de entrenamiento y el respawn. Las herramientas MCP de Unreal y Live Coding no estaban disponibles en esta sesión.

1. **Pendiente — usuario.** Detener PIE y cerrar Unreal. En Visual Studio, compilar el target `vampsEditor` para `Win64 Development` (`Development Editor | Win64` en la solución). Confirmar que termina sin errores y abrir `vamps.uproject`.
2. **Pendiente — usuario.** Abrir `/Game/Vamps/Maps/L_CharacterBasic` y arrancar PIE. Pulsar **F6** hasta `Mixed`. Durante un cambio de conducta, comprobar que el ataque o la solicitud de defensa anterior se cancela. Cargar guardia o provocar stun antes de un cambio. La carga debe seguir su recuperación habitual y el stun debe vencer por su timer.
3. **Pendiente — usuario.** Golpear al bot cuando cambia de conducta. El contacto debe conservarse y resolverse según el orden de los ataques. Pulsar **R** durante un ataque o encuadre de combate. Los personajes vuelven a sus posiciones con los valores de inicio y la cámara limpia el encuadre anterior. Comprobar que pueden volver a atacar y que la cámara vuelve a entrar en encuadre de combate.
4. **Pendiente — usuario.** Abrir `/Game/Vamps/Maps/L_LocalMatch`, iniciar una ronda y esperar el respawn tras la muerte del personaje local. Comprobar que conserva la vista de muerte hasta reaparecer y que, al reaparecer, puede moverse, atacar y hacer dash.
