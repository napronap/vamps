# 075 — Barrido de caja para armas

`AVampsWeapon` ejecuta un `SweepMultiByObjectType` contra `ECC_Pawn` por tick activo. Los marcadores `TraceStart` y `TraceEnd` fijan el recorrido en la pose del tick. El componente nativo `TraceBox` aporta sus semiextensiones escaladas y su rotación. Queda bajo `WeaponRoot` para que la escala visual de la malla no multiplique el tamaño de la caja. El valor inicial de `Box Extent` es `(8, 8, 8)` cm y cada Blueprint lo puede editar.

Se retiran `TraceRadius`, el transform del tick anterior y los barridos interpolados. Se conservan la deduplicación por arma, la comprobación de paredes y el envío del contacto a `AVampsCharacter::QueueWeaponContact`. Los notifies existentes siguen controlando la ventana de daño y el reinicio de víctimas para impactos múltiples.

Los seis Blueprints derivados heredan `TraceBox`. Sus marcadores y referencias desde los perfiles de combate se conservan. Los paquetes guardados contienen `TraceBox`. La calibración del volumen queda en cada Blueprint y esta limpieza de C++ conserva los cambios de assets que ya estaban en el workspace.

`AVampsCharacter` llama a `InitializeCombatTrace` después de equipar el arma y aplicar su transform. En ese punto se validan la malla asignada y la geometría editable, se guarda una referencia débil al luchador y se registra la dependencia del tick respecto de su malla. Se retiran las comprobaciones de componentes nativos y la validación repetida al activar. El personaje habilita la traza en autoridad cuando abre la ventana de daño. El tick usa la referencia guardada y se desactiva si dejó de ser válida.

## Verificación

Se revisaron los puntos de entrada de la traza y se confirmó una sola llamada al barrido de caja por tick activo. Se aplicó `clang-format` y pasó `git diff --check`. Quedan la compilación y la comprobación en PIE. [Guía del editor](editor.md).
