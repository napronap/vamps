# 061 — Fin de Recovery por Anim Notify

## Resultado

`AN_EndRecovery` define el cierre normal de cada golpe. Al recibirlo se termina Recovery, empieza NA con su duración configurada de 0,5 s, se libera el movimiento y la cámara inicia su transición normal al plano general. El instante se elige manualmente en el montage; la cámara ya no anticipa NA ni calcula recuperación restante.

Hasta ese cierre, Preparation, Contacts y Recovery bloquean caminar, sprint y dash. Se conserva la representación existente: Recovery empieza al terminar `ANS_AttackTrace`; en los primeros golpes puede contener todavía una ventana de combo abierta. `ANS_ComboWindow` decide la continuación y `AN_EndRecovery` debe colocarse después de su cierre.

El código está implementado y compiló por Live Coding. Tras un cierre del editor al intentar PIE sobre una recarga estructural, el usuario realizó compilación y reapertura limpias; el nuevo arranque por MCP fue correcto. Queda colocar los notifies y comprobar su respuesta en combate. Los pasos se consolidan en [editor.md](editor.md). La colocación de notifies es manual porque MCP no expone esa operación.

## Diagnóstico del ataque bloqueado

Revisión del 2026-11-01: el clic derecho alcanza `UGA_Attack::ActivateAbility`, pero la validación cancela el ataque antes de reproducir el montage. El log de la sesión registra `missing AN_EndRecovery` para `BP_VampsCharacter_Player_C_0` y los bots que comparten `DA_VampsCombatSettings`. La primera sección rechazada es `Default`, de 0 a 1,933333 s; la validación se detiene ahí y no confirma el estado de los marcadores de las otras secciones.

La configuración manual pendiente sigue siendo la causa observada. `ObjectTools.list_properties` confirmó nuevamente que los eventos del montage no están expuestos; el catálogo MCP no dispone de edición de notifies. La lectura por `ObjectTools.get_properties` confirmó `BlendOut=0.25`, `BlendOutTriggerTime=-1`, `RateScale=1` y auto blend out activo. En `Default`, el marcador debe quedar después del cierre de contactos/combo y estrictamente antes de 1,683333 s. Los tiempos de esos cierres no se pueden leer por MCP; deben verse en la pista del editor.

No se modificaron código ni assets para este diagnóstico, ni se recompiló o inició una sesión de juego. Se conserva el contrato de cierre por notify; la solución pendiente y su comprobación están en [editor.md](editor.md#3-colocar-los-marcadores).

## Código y conexiones

| Clase / archivos | Cambio y relación |
|---|---|
| `UAN_EndRecovery`, `Source/vamps/Public/Animation/AN_EndRecovery.h` y `Private/Animation/AN_EndRecovery.cpp` | Notify nativo de tipo branching point, nombre visible `End Recovery`. Valida el dueño del mesh y llama a `AVampsCharacter::EndCombatRecovery`. No requiere Blueprint ni dispara lógica al previsualizar en el editor. |
| `AVampsCharacter`, headers y fuentes de `Characters/VampsCharacter` | Valida la identidad del notify, la fase y la ventana cerrada; solicita el cierre al resolver. `ResolveCombatRecoveryEnd` inicia NA y cancela la habilidad de ataque. Retira `GetPendingNAStartTime`, `TryCalculateCombatSectionDuration`, `CombatStepEndsAt`, `ActionRecoveryTimer`, `bComboContinuationClosed` y `ResolveCombatLocomotion`. Un final de ataque sin el evento requerido genera diagnóstico y cancela la acción; no inicia NA por la longitud del clip. |
| `UVampsCombatResolver`, headers y fuentes de `Combat/VampsCombatResolver` | Sustituye solicitudes de liberación por locomoción por cierres de Recovery identificados por personaje y número de acción. Orden: contactos, continuaciones, cierres, dash. Un cierre de un golpe anterior se descarta al avanzar válidamente el combo. Reset elimina solicitudes pendientes. |
| `UGA_Attack`, `Private/Combat/Abilities/GA_Attack.cpp` | Exige exactamente un `AN_EndRecovery` por sección del montage, después de contactos y cierre de combo y antes del blend out automático. Informa sección y tiempos de configuración inválidos; no sustituye un notify faltante por el final del clip. |
| `UGA_Dash`, `Private/Combat/Abilities/GA_Dash.cpp` | Retira la cancelación de Recovery por movilidad. La habilitación del personaje impide iniciar dash antes del notify. |
| `FVampsCombatState`, `Public/Combat/VampsCombatState.h`; `UVampsCombatMovementComponent`, `Private/Combat/VampsCombatMovementComponent.cpp`; `UVampsFighterWidget`, `Private/UI/VampsFighterWidget.cpp` | Eliminan `bRecoveryVisualReleased`, sus excepciones de movimiento/rotación y la línea `RELEASED` del panel F9. El widget sigue mostrando fase y disponibilidad de movimiento. |
| `AVampsPlayerCameraManager`, headers y fuentes de `Player/VampsPlayerCameraManager` | Retira predicción de NA, duración variable y salida especial ajustada al plazo. Al pasar la fuente a NA deja combat framing y usa la transición normal configurada, actualmente 0,3 s. Conserva seguimiento, colisión, oclusión y descarte de fuentes inválidas. |

El notify pausa el montage hasta la resolución del mismo frame: así no consume la cola ni termina automáticamente antes de decidir una continuación pendiente. Si esa continuación se acepta, se salta y reanuda la sección siguiente; si no, se inicia NA y se cancela el ataque con su blend out. No se añade un temporizador ni un plazo calculado para esa decisión.

## Assets

El montage conectado es `/Game/Vamps/Animations/Executioner/AM_Executioner_Combo.AM_Executioner_Combo`, desde `/Game/Vamps/Data/DA_VampsCombatSettings.DA_VampsCombatSettings`. Lo usan los personajes existentes `BP_VampsCharacter` y `BP_VampsCharacter_Player`; no cambian sus asignaciones, AnimBP, widgets ni clase de cámara.

No se modifican clases ni estructuras pertenecientes a propiedades de Data Assets. Las lecturas [anterior](na-antes.json), [posterior al parche](na-despues.json) y [tras la carga limpia](na-carga-limpia.json) conservan `NARecoveryDuration=0.5` y la referencia al montage. Codex no creó ni guardó assets en esta iteración, tampoco durante la recarga estructural. `FVampsCombatState` pertenece al personaje y pierde su campo de liberación visual; no forma parte de las propiedades del Data Asset.

## Verificación y pendientes

- Revisión de código y formateador de los trece archivos C++ afectados. Se retiraron las referencias a la liberación visual y los cálculos anteriores de cierre.
- `LiveCodingToolset.LiveCodingToolset.CompileLiveCoding` devolvió `Success` y aplicó un parche real: [resultado](live-coding.json). La consulta posterior de subclases confirmó `/Script/vamps.AN_EndRecovery` cargada: [clase](clase-notify.json). No fue una compilación `NoChanges`.
- El log de recarga señaló `Struct Property CombatState has a struct type mismatch` con `LIVECODING_VampsCombatState_0`. Se intentó iniciar PIE en `L_LocalMatch` sobre esa recarga; Unreal terminó con el assert interno `Ret->IsA(T::StaticClass())`, en `Class.h:4627`, antes de completar el arranque. MCP perdió conexión. [Extracto del log](cierre-editor.log). No se guardaron Blueprints reinstanciados ni se añadieron redirects o compatibilidad para ocultar la incompatibilidad.
- **Hecho por el usuario:** compilación completa con Unreal cerrado y reapertura, confirmadas con «Recompilé y reabrí; MCP listo». **Verificado por Codex por MCP:** clase nativa cargada, mismo Data Asset y arranque correcto de PIE en `L_LocalMatch`, con [cuatro personajes](personajes-pie.json). Se detuvo PIE y se confirmó `IsPIERunning=false` para dejar editar el montage. La carga limpia eliminó el fallo de arranque observado en la sesión anterior; no se modificó código después de la compilación correcta.
- El [log de la sesión limpia](logs-pie.json) contiene rechazos explícitos por `missing AN_EndRecovery` en la sección `Default` [0, 1,933333]. Esto confirma la validación de configuración faltante; no demuestra todavía que se hayan colocado los notifies o ejecutado su cierre en combate.
- Pendiente del usuario: colocar los tres notifies, guardar el montage y comprobar ataque aislado, combo, movimiento, dash e interrupciones según [la guía](editor.md). El usuario decidió configurar los marcadores después; no se marca ese paso ni la comprobación de combate como hechos.
- No se automatizaron inputs ni se agregaron o ejecutaron tests. La revisión de código no demuestra todavía la respuesta visual en combate.

Esta implementación sustituye la anticipación temporal de [060](../060-salida-framing-antes-na/reporte.md). Se mantienen los indicadores de [059](../059-indicadores-personaje/reporte.md).

La configuración de personajes continúa en [062 — Data Assets por personaje](../062-data-assets-personajes/reporte.md): el DA pasó a llamarse `DA_ExecutionerCombatSettings`, conservando sus valores y el mismo montage. Las referencias al nombre anterior de esta iteración documentan el estado previo al renombrado; el pendiente de `End Recovery` continúa vigente.
