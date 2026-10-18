# 032 — Comprobación real de build y arranque del editor

El ciclo posterior con cierre, build estructural, restauración del DA y código del paso 3 se registra en [033](../033-resultados-combate/reporte.md). Esta comprobación conserva su autorización y resultados originales.

## Resultado — 2026-10-18

Con autorización del usuario limitada a esta comprobación, Codex ejecutó `C:\dev\vamps\build.bat` y, únicamente después de verificar su éxito, `C:\dev\vamps\editor.bat`. El build completo de vampsEditor Win64 Development terminó con código 0 y `Result: Succeeded`; el editor cargó L_CharacterBasic y MCP reconectó. Unreal queda abierto, sin PIE.

No se modificaron código, scripts, clases, Blueprints, Data Assets ni configuración del juego. El paso 3 no se implementó ni se cerró. Se agregan este reporte, su guía, logs y estados de ejecución, con enlaces desde el índice y la iteración 031.

## Comprobaciones

| Operación | Evidencia y resultado |
|---|---|
| Estado previo | Consulta de procesos por PowerShell: no había UnrealEditor, UnrealEditor-Cmd ni procesos de compilación activos. No fue necesario cerrar una sesión ni guardar assets. |
| Build por shell | Inicio 14:01:18 JST; final 14:02:14 JST. UBT informó 54,80 s, 19 acciones, compilación de C++ y enlace de UnrealEditor-vamps.dll/lib. No fue Live Coding ni un build sin cambios. Límite efectivo de 15 min; no se alcanzó. |
| Arranque por shell | `editor.bat` se lanzó a las 14:02:49 JST, después del código 0 y del resultado de UBT. Launcher código 0; proceso UnrealEditor PID 58916 con `C:\dev\vamps\vamps.uproject` en su línea de comandos. |
| Inicialización | Log de esta sesión: `Engine is initialized`, carga de `/Game/Vamps/Maps/L_CharacterBasic` y `Engine Initialization Total time: 31.98 seconds`. Límite de comprobación de arranque/reconexión de 5 min; verificado dentro del plazo. |
| MCP | `describe_toolset` volvió a responder. ObjectTools.get_class del DA devolvió `/Script/vamps.VampsCombatSettings`, sin referencia a una clase transitoria. |
| Clase compilada | ObjectTools.list_properties/get_properties sobre `/Script/vamps.Default__AN_ResetWeaponHits` confirmaron `weaponIndices=[0]` y `bShouldFireInEditor=false`. |
| Estado final | EditorAppToolset.IsPIERunning devolvió false. El editor se conserva abierto; no se inició gameplay. |

La primera consulta de procesos desde el sandbox no tuvo acceso a CIM. Se repitió con escalación revisada automáticamente, y las operaciones de build y arranque también se ejecutaron con escalación aprobada. No hubo rechazo de aprobación. Esto comprueba el flujo bajo los permisos concedidos en esta ejecución, no garantiza permisos futuros.

Antes del arranque y durante los primeros segundos MCP no estaba disponible. Al reconectar, el log registró una sesión anterior desconocida y luego una nueva sesión inicializada automáticamente; las consultas posteriores funcionaron. No se encontraron errores LogVampsCombat, errores de Blueprint ni errores fatales en la captura de arranque. No se cambió configuración del conector ni se automatizó la interfaz.

## Archivos de evidencia

- [Estado de build](build-status.json), [stdout de build](build.stdout.log) y [stderr de build](build.stderr.log); stderr vacío.
- [Estado de arranque](editor-launch-status.json), [stdout del launcher](editor-launch.stdout.log) y [stderr del launcher](editor-launch.stderr.log); ambas salidas vacías.
- [Captura del log del editor](editor-startup.log), copiada de Saved/Logs/vamps.log al terminar la verificación. El log vivo puede continuar creciendo.
- [Guía reproducible](editor.md).

## Alcance y límites

No se ensayó el cierre automático: el editor ya estaba cerrado. Tampoco se ejecutaron restauración de propiedades tras cambios estructurales, PIE ni tests automatizados. La lectura del DA confirma disponibilidad y clase, no es una comparación completa de valores ni reemplaza una futura captura antes de modificar su estructura.

No quedan operaciones necesarias para esta comprobación. La autorización de shell fue específica de este pedido; una futura ejecución de implementación/reinicio debe contar con su propia autorización. Las reglas y el cierre de 2 permanecen en [031](../031-cierre-paso-2-y-reglas-paso-3/reporte.md); la implementación de 3 sigue pendiente.
