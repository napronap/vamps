# 060 — Salida de combat framing antes de NA

La anticipación temporal de esta iteración se sustituye por [061 — AN_EndRecovery](../061-fin-recovery-notify/reporte.md). El usuario confirmó la salida en ataques intermedios y señaló la recuperación larga del tercero; la configuración nueva usa un cierre manual en el montage.

## Resultado

La salida del encuadre de combate se adelanta dentro de la recuperación final de la cadena para completar el regreso al encuadre general al comenzar NA. Se conserva la duración configurada de NA, 0,5 s, y su lógica de inicio/finalización. La transición de cámara sigue configurada en 0,3 s; si queda menos recuperación, solo se acorta esa transición visual.

## Código y coordinación

| Clase / archivos | Cambio |
|---|---|
| `AVampsCharacter`, `Source/vamps/Public/Characters/VampsCharacter.h` y `Source/vamps/Private/Characters/VampsCharacter.cpp` | Agrega la marca privada `bComboContinuationClosed` y `GetPendingNAStartTime()`. Informa `CombatStepEndsAt`, el mismo plazo que usa la espera de recuperación previa a NA, únicamente durante Recovery y cuando la continuación ya está cerrada o la animación fue liberada para locomoción. |
| `AVampsPlayerCameraManager`, `Source/vamps/Public/Player/VampsPlayerCameraManager.h` y `Source/vamps/Private/Player/VampsPlayerCameraManager.cpp` | Consulta ese plazo para iniciar la salida dentro de los últimos 0,3 s de recuperación. `TransitionLength` guarda la duración efectiva de cada transición. La salida interpola también el yaw y termina con la composición general, evitando prolongarla con el filtrado angular/espacial habitual. |

### Evitar salidas falsas

- Cada paso reinicia la marca de cierre. En el último paso ya no existe otro golpe ofensivo; en los anteriores, la marca se activa al cerrar su ventana de continuación.
- Mientras haya una continuación solicitada, el getter no entrega un plazo. El resolver existente procesa contactos y continuaciones antes de locomoción. Si la continuación es válida, `BeginCombatStep` inicia el siguiente paso, vuelve a Preparation y reinicia la marca; la cámara conserva framing.
- Con ventana aún disponible o continuación pendiente no se inicia la salida por anticipación. La mera entrada en Recovery no basta para abandonar el framing.
- La liberación visual por movimiento/dash conserva su camino existente, sin modificar sus plazos ni exigir movimiento para anticipar la cámara.
- Cancelación, fin de NA y reset limpian la marca. Se conservan la validación de generación/montage, muerte y cambio de ViewTarget del manager.

### Completar el regreso

La salida arranca cuando la recuperación comprometida entra en el margen configurado de transición. Si la confirmación llega con menos margen, la duración visual se limita al tiempo disponible. Un cambio de target durante esa salida respeta el mismo límite. Si el manager ya recibe el estado NA, completa la composición general en esa actualización; no añade 0,3 s después del inicio del bloqueo.

Durante esta salida, posición, interés, FOV y yaw usan la misma transición acotada. La cámara conserva su comprobación de colisión; un obstáculo todavía puede acercarla físicamente. Los filtros habituales siguen aplicándose al seguimiento ordinario y las entradas a combat framing.

El framing solicitado por un bloqueo continúa siguiendo la cadena del atacante correspondiente. Se aplica el mismo criterio de fin confirmado de esa cadena.

## Assets y configuración

No se crean, modifican ni guardan assets en esta iteración. Se mantienen las conexiones de `BP_VampsPlayerCameraManager` con los controllers existentes, el montage y los widgets de [058](../058-ui-widgets/reporte.md)/[059](../059-indicadores-personaje/reporte.md).

La clase y las estructuras del Data Asset no cambian. Su valor y referencia relevantes se releyeron sin diferencias: [antes](na-antes.json), [después](na-despues.json). `NARecoveryDuration=0.5` y `AM_Executioner_Combo` permanecen iguales. También se conservaron las variables de cámara: [antes](camara-antes.json), [después](camara-despues.json).

## Verificación y límites

- Formateador aplicado a los cuatro archivos C++ modificados y revisión del diff.
- Revisión de los caminos de golpe final, ventana abierta, continuación pendiente/aceptada/rechazada, liberación para locomoción, interrupción y reinicio. La marca nueva solo se consulta para presentación; no condiciona solicitudes ni temporizadores del combate.
- `LiveCodingToolset.LiveCodingToolset.CompileLiveCoding` devolvió `Success` y aplicó un parche real: [salida](live-coding.json).
- PIE inició en `L_LocalMatch`, se encontró `BP_VampsPlayerCameraManager_C_0` y se releyó la cámara en selección, con framing desactivado y sin fuente: [lectura](camara-pie.json). La consulta de `LogVampsCombat` no mostró errores de cámara/configuración.
- No se automatizaron inputs ni se crearon/ejecutaron tests. La observación temporal de combos y del primer instante de NA sigue pendiente del usuario en la [guía](editor.md); no se da por demostrada por una lectura en selección.

Se actualizan el índice y las referencias históricas pertinentes. No se editan las especificaciones funcionales/técnicas del juego.
