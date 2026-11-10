# 070 — Barra de guardia y aviso de STUN

La iteración continúa en [071 — Simplificación del combate y la presentación](../071-simplificacion-combate/reporte.md).

- `UVampsFighterWidget` muestra HP, GUARD y ENERGY en el panel normal. Durante un stun, el relleno rojo de GUARD representa `(ReactionEndsAt − tiempo actual) / StunDuration`, limitado al rango de 0 a 1. En ese intervalo la etiqueta dice `GUARD`. Al terminar, el panel recupera el color azul y vuelve a mostrar la carga real. El cambio se aplica tanto al jugador como al objetivo observado. El panel de depuración conserva sus diagnósticos.
- `UVampsHUDWidget` muestra `STUN` para el personaje local vivo mientras dura el aturdimiento durante la pelea. Usa `NoticeText` y `NoticeBackground` de `/Game/Vamps/UI/WBP_VampsHUD`, anclados en `(0.5, 0.5)` con alineación central y desplazamiento cero. El aviso comparte el fondo oscuro del countdown y queda fijo al centro de la pantalla durante el encuadre de combate.
- `UVampsCharacterIndicatorWidget` omite la barra de NA durante el aturdimiento. Si aún queda tiempo de NA al salir del stun, se vuelve a mostrar su barra. Se retiró el cartel anterior del indicador proyectado sobre el personaje.
- `/Game/Vamps/UI/WBP_VampsFighter` retira `PanelTitle` y `StatusText` de `CombatView` y reparte las tres filas en el mismo espacio. `/Game/Vamps/UI/WBP_VampsHUD` sigue referenciando ese Blueprint para jugador, objetivo y panel de depuración.

## Verificación

- Live Coding aplicó un parche real. Se recompilaron los dos Widget Blueprints y se verificaron en el registro de assets sus padres nativos, dependencias y guardado. `WBP_VampsHUD` conserva su dependencia de `WBP_VampsFighter` en disco. En memoria, el compilador del HUD informó que `PlayerPanel`, `TargetPanel` y `DebugPlayerPanel` seguían esperando el tipo temporal de Live Coding. El paquete del HUD quedó sin guardar y `dirty=false`.
- `L_CharacterBasic` inició PIE y se confirmaron las referencias de los indicadores en los dos personajes y la clase de widget del actor HUD. Se detuvo la sesión y se volvió a `L_LocalMatch`. El usuario reabrió Unreal y el log registró que inició una ronda. Su observación del aviso fuera del centro motivó moverlo al HUD.
- Tras centrar el aviso, Live Coding aplicó el parche y `WBP_VampsHUD` volvió a compilar. Se verificó el anclaje central y que el árbol guardado de `WBP_VampsCharacterIndicator` conserva los indicadores de objetivo y NA y ya no contiene `StunView`. PIE abrió la selección en `L_LocalMatch` y se detuvo. Queda observar un stun durante el encuadre de combate.
- Se aplicó clang-format al C++ propio. Pasó `git diff --check`.

[Guía del editor](editor.md).
