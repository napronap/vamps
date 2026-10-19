# 040 — Giro direccional al iniciar dash

## Resultado — 2026-10-20

El usuario confirmó que el dash con root motion de 039 funciona, pero el giro gradual impedía orientar una evasión lateral a tiempo. Esta iteración aplica giro instantáneo únicamente al iniciar un dash aceptado. Sustituye la regla de 039 que usaba siempre el frente previo del cuerpo. [Guía y comprobación visual pendiente](editor.md).

WASD sin Shift conserva su giro gradual. Al pulsar Shift se captura el vector actual de IA_Move con la misma referencia al yaw de cámara que la locomoción. La GA orienta instantáneamente el personaje hacia ese vector justo después de confirmar que su montage comenzó y antes del avance de root motion. A+Shift permite salir hacia la izquierda incluso si el cuerpo no alcanzó a girar. Se admiten cardinales y diagonales con el mismo montage frontal.

La dirección pertenece a esa pulsación y se conserva si el resolver espera al cierre de ComboWindow. Si una continuación válida descarta el dash, no hay giro. Si falla configuración, recurso o permiso, tampoco se aplica el giro. Sin input se mantiene el montage de retirada alejándose del target. No cambia la ventana de admisión ni se agrega invulnerabilidad.

## Archivos y conexiones

- Source/vamps/Private/Player/VampsPlayerController.cpp: MobilityPressed lee IA_Move vigente, convierte su valor a dirección horizontal respecto de cámara y la entrega con Shift. Move mantiene su ruta de locomoción y giro existentes.
- Source/vamps/Public/Characters/VampsCharacter.h y Private/Characters/VampsCharacter.cpp: SetMobilityHeld recibe la dirección de la pulsación, valida que sea finita y conserva un único RequestedDashDirection. Elimina bDashBackward, que ahora se deriva del vector vacío. ClearMobilityIntent limpia la captura; el resolver conserva su orden y prioridad.
- Source/vamps/Private/Combat/Abilities/GA_Dash.cpp: deriva frontal/retirada de esa captura. Tras validaciones y comienzo confirmado del montage, aplica SetActorRotation una sola vez hacia la dirección solicitada o hacia el target para retirada.
- No se modifican Data Assets, clases/structs de sus propiedades, montages, inputs, Blueprints ni graphs. Siguen las dos referencias Fw/Bw, los clips propios con root motion, controllers y ABP de 039. No hay parámetros nuevos en el DA.
- Se agregan este reporte, la guía y capturas de compilación/arranque; el índice y 039 enlazan la configuración vigente. Se conservan los cambios anteriores del árbol de trabajo.

## Verificaciones

- clang-format aplicado a los cuatro fuentes modificados; git diff --check sin errores. Revisión de captura, continuación, falta de input, cancelación y orden de giro/reproducción. Nombres revisados contra la jerarquía nativa.
- LiveCodingToolset.LiveCodingToolset.CompileLiveCoding aplicó un parche [Success](live-coding.json), no NoChanges. El resultado advirtió cambios de tipos; no se guardaron assets durante esa recarga ni se atribuye un build completo a esta iteración.
- PIE iniciado por MCP en L_CharacterBasic. Jugador y rival vivos, Idle, bCanDash=true, bIsDashing=false, sin errores/warnings LogVampsCombat en la consulta. [Lectura](pie-estado.json). PIE detenido y confirmado false.
- No se simularon inputs ni se ejecutaron tests automatizados. La comprobación de arranque no acredita el giro visual ni la eficacia de esquivar entre ataques. Queda la observación manual de la guía; no se marca cerrado el paso 4.
