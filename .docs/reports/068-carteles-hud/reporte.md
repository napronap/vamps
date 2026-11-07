# 068 — Carteles del HUD y depuración en PIE

- `UVampsHUDWidget` controla el texto y la visibilidad de los carteles de inicio, reaparición y equipo. `/Game/Vamps/UI/WBP_VampsHUD` envuelve los tres textos existentes en Borders con el mismo fondo oscuro semitransparente del marcador y tamaño ajustado al contenido. Los dos avisos quedan centrados y la etiqueta conserva su anclaje inferior izquierdo. El fondo de equipo cubre `TEAM A` y `TEAM B`.
- `UVampsFighterWidget` reserva el último contacto y las fases de ataque y recuperación para depuración. Los paneles normales conservan las barras, las reacciones, la movilidad y el indicador `DOWN`. En `/Game/Vamps/UI/WBP_VampsFighter`, `ContactText` pasa de `CombatView` a `DebugView`, y `StatusText` parte vacío.
- `AVampsPlayerController` registra F9 únicamente en mundos `EWorldType::PIE`. `UVampsHUDWidget` aplica la misma restricción a la activación y al aviso de pantalla. `/Game/Vamps/UI/BP_VampsHUD` instancia `WBP_VampsHUD`, que usa `WBP_VampsFighter` para los paneles del jugador y de los rivales.

## Verificación

- Unreal compiló ambos Widget Blueprints. Se releyeron la jerarquía, los fondos, el ajuste al contenido y los valores iniciales de visibilidad. Se guardaron los dos paquetes y se confirmó `dirty=false` en ambos.
- Se aplicó clang-format al C++ modificado y pasó `git diff --check`. Live Coding aplicó el parche final. `L_LocalMatch` inició en PIE y el log confirmó que abrió la selección. Se detuvo la sesión y se confirmó el cierre. La [guía del editor](editor.md) recoge las comprobaciones de ronda y Standalone pendientes.
