# 071 — Simplificación del combate y la presentación

La revisión de código continúa en [072](../072-revision-codigo/reporte.md).

Un impacto del mismo frame conserva prioridad sobre el dash. Los contactos pueden cancelar la solicitud antes de que GAS active la habilidad y consuma energía. El cierre del combo y de Recovery conserva el orden previo.

| Código | Cambio |
|---|---|
| `AVampsCharacter`, `UGA_Dash` | `SetCombatPhase` cambia la fase. `RefreshCombatPermissions` actualiza permisos y solo toca las armas cuando cambia el permiso de daño. La solicitud de dash conserva la dirección y se descarta al iniciar otro paso, reaccionar o resetear. La elegibilidad se revalida justo antes de activar GAS y la habilidad ejecuta el dash. Se agrupan las declaraciones por responsabilidad y se privatizan las operaciones internas del resolver. |
| `UVampsCombatResolver`, `AVampsWeapon`, `UGA_Attack` | El arma valida la elegibilidad una vez antes de la traza de visibilidad. La habilidad valida el daño base y la duración de los pasos antes de reproducir el montage. El resolver exige un receptor con combate inicializado, conserva las comprobaciones de vigencia al consumir la cola y separa el lote de contactos del orden del frame. Aplica contactos, continuaciones, cierres de Recovery y dash antes de consolidar la partida. |
| `UANS_AttackTrace`, `UANS_ComboWindow`, `UAN_ResetWeaponHits` | Los callbacks nativos declaran el dueño del mesh en el `if`, con un ternario completo. Se retiran las ramas comentadas. El payload sigue identificando la instancia y la sección del montage. |
| `AVampsPlayerCameraManager`, `AVampsCharacter` | El personaje emite un evento al iniciar su ataque o confirmar una defensa. La cámara escucha al personaje observado y toma la cadena como fuente para el encuadre. El seguimiento, la interpolación y la colisión siguen por frame. |
| `UVampsLockOnComponent`, `AVampsMatchGameMode`, `UVampsHUDWidget` | El HUD escucha cambios de objetivo, fase y selección. Los atributos conservan los delegates de GAS. Las vistas con tiempo restante y los cálculos de layout siguen actualizándose por frame. |

Los Blueprints existentes conservan sus clases y conexiones nativas. `BP_VampsPlayerCameraManager` conserva su configuración de cámara. `/Game/Vamps/UI/WBP_VampsHUD` y sus paneles conservan sus bindings de widgets. Los montages de combate conservan `Attack Trace`, `Combo Window`, `Reset Weapon Hits` y `End Recovery`. No se editaron assets ni estructuras de los Data Assets.

## Verificación

- Se aplicó `clang-format` a los fuentes editados y pasó `git diff --check`.
- `build.bat` compiló y enlazó `vampsEditor Win64 Development` con Unreal 5.8. UBT devolvió código 0 y `Result: Succeeded` en 35,44 s.
- `editor.bat` abrió `L_LocalMatch`. Map Check informó 0 errores y 0 advertencias. Por MCP se inició PIE y el log confirmó que se abrió el selector. Se detuvo PIE por MCP, se confirmó `IsPIERunning = false` y se cerró la instancia temporal.

Quedan las comprobaciones jugables de prioridad del dash, encuadre, cambios de objetivo y avance de combo. Ver la [guía del editor](editor.md).
