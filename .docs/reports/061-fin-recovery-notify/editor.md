# AN_EndRecovery — operaciones del editor

**Ruta actual del DA:** `/Game/Vamps/Data/DA_ExecutionerCombatSettings`, renombrado y verificado en [062](../062-data-assets-personajes/reporte.md). Las capturas de esta guía conservan el nombre anterior; el montage a editar sigue siendo `AM_Executioner_Combo`.

**Queda para el usuario:** agregar un notify `End Recovery` en cada una de las tres secciones de `AM_Executioner_Combo`, guardar y comprobar el combate. La compilación completa y reapertura ya las realizó el usuario; Codex verificó el nuevo arranque y dejó PIE detenido. Hasta configurar los tres, la validación rechaza iniciar ataques e indica la sección que requiere el notify. MCP no dispone de edición de esos eventos; se deja ese paso manual conforme a `AGENTS.md`.

**Diagnóstico confirmado el 2026-11-01:** los intentos del jugador y los bots llegan a `UGA_Attack`, pero se cancelan por `missing AN_EndRecovery` en `Default`. Completar el paso 3 para habilitar los ataques. La validación recorre todas las secciones antes de iniciar el montage, por lo que hay que revisar los tres marcadores aunque el primer error solo nombre `Default`.

## 1. Preparación

**Hecho — Codex por MCP.** Se descubrieron las herramientas del editor, propiedades y Live Coding. PIE estaba detenido. `ObjectTools.list_properties/get_properties` confirmó en `/Game/Vamps/Data/DA_VampsCombatSettings` la referencia a `AM_Executioner_Combo` y `NARecoveryDuration=0.5`. El catálogo no ofrece edición de notifies y el schema del montage no expone `Notifies` ni `CompositeSections`.

## 2. Compilación

**Hecho — Codex por MCP.** `LiveCodingToolset.LiveCodingToolset.CompileLiveCoding` compiló y aplicó un parche real con `Success`: [resultado](live-coding.json). `ObjectTools.search_subclasses` confirmó la clase nativa `/Script/vamps.AN_EndRecovery`. La relectura del Data Asset conservó NA y la referencia al montage.

**Hecho — Codex por MCP, comprobación fallida.** Se intentó iniciar PIE en `L_LocalMatch`. El editor terminó con un assert interno de clase después de advertir incompatibilidad del struct `CombatState` durante la recarga. PIE no completó el arranque y MCP quedó desconectado. No se guardaron assets reinstanciados. [Diagnóstico](cierre-editor.log).

**Hecho — usuario.** Compilación y carga limpias, confirmadas con «Recompilé y reabrí; MCP listo». Procedimiento reproducible:

1. Mantener Unreal cerrado y cerrar el Crash Reporter si está abierto.
2. Ejecutar `C:\fran\vamps\build.bat`. Compila `vampsEditor Win64 Development` con Unreal 5.8 y `-NoHotReload`. Esperar un resultado correcto.
3. Ejecutar `C:\fran\vamps\editor.bat`. Esperar que abra el proyecto y vuelva a estar disponible MCP.
4. No guardar ni restaurar versiones transitorias `LIVECODING_*` de los assets. No se necesita recrear personajes, widgets ni la cámara.

**Hecho — Codex por MCP.** Tras la reapertura se confirmó la clase nativa, la configuración del Data Asset y el arranque correcto de PIE en `L_LocalMatch`, con cuatro personajes. Se detuvo la sesión y se confirmó `IsPIERunning=false`. El [log](logs-pie.json) muestra el rechazo por notify faltante en `Default`, coherente con el siguiente paso pendiente. El notify aparece como `End Recovery`; no crear un Blueprint para él.

## 3. Colocar los marcadores

**Pendiente — usuario.** El usuario decidió configurar los marcadores después. Con PIE detenido:

1. Abrir `/Game/Vamps/Animations/Executioner/AM_Executioner_Combo` desde Content Browser.
2. En una pista de **Notifies del montage**, agregar **Add Notify → End Recovery** (`AN_EndRecovery`). Agregar un notify simple, no un Notify State ni un evento del Skeleton.
3. Dejar exactamente uno en cada sección de ataque. En las secciones primera y segunda, ubicarlo estrictamente después del final de `ANS_AttackTrace` y de `ANS_ComboWindow`. En la tercera, ubicarlo después del final de `ANS_AttackTrace` y también después de `ANS_ComboWindow` si esa sección tiene una.
4. Elegir visualmente cuánto Recovery conservar antes del marcador. Ese instante libera movimiento, inicia NA y solicita el regreso de cámara. Colocarlo antes del blend out automático de la sección; con la configuración actual de 0,25 s y velocidad 1×, dejar más de 0,25 s hasta su final. No usar el último frame ni superponerlo con el cierre de otro notify.
5. Guardar el montage. No cambiar `NA Recovery Duration` del Data Asset, ni las secciones, clips, contactos o ventanas para aplicar este cambio.

**Lectura actual confirmada por Codex por MCP, 2026-11-01:** `BlendOut=0.25`, `BlendOutTriggerTime=-1`, `RateScale=1` y auto blend out activo. El error sitúa `Default` entre 0 y 1,933333 s: su `End Recovery` debe quedar estrictamente antes de 1,683333 s y después del cierre de contactos y combo. No es una posición sugerida para el marcador, sino su límite superior de validación. Revisar los intervalos de las otras dos secciones en el montage.

Los tiempos exactos de contactos y ventanas no están expuestos por MCP; esta guía no inventa posiciones numéricas. Los mensajes de `LogVampsCombat` identifican la sección y los límites si un marcador falta, está duplicado o queda fuera del intervalo permitido. Después de guardarlo, repetir el ataque y comprobar que desaparece `attack rejected`; si el mensaje señala otra sección, corregir el marcador de esa sección antes de evaluar el flujo de combate.

## 4. Comprobar el flujo

**Pendiente — usuario.** Iniciar PIE en `L_LocalMatch`, seleccionar EXECUTIONER y START:

1. Hacer un golpe sin continuar. Mantener WASD durante el ataque: no debe liberar locomoción antes de `End Recovery`. Al marcador empieza NA, se puede caminar y la cámara inicia su transición al plano general.
2. Encadenar dos y tres golpes, incluyendo un input cerca del cierre de ventana. No debe iniciarse NA ni una salida de cámara entre continuaciones válidas. Al fallar o no solicitar continuación, el marcador del golpe actual cierra Recovery.
3. En el tercer golpe, mover el marcador permite calibrar su recuperación sin esperar toda la cola del clip. La barra de NA debe seguir durando 0,5 s.
4. Comprobar Shift: no inicia dash en Preparation, Contacts o Recovery; vuelve a estar disponible durante NA si se cumplen energía, suelo y ausencia de reacción.
5. Comprobar bloqueo, interrupción, muerte y siguiente ronda: no deben aplicar el marcador de una acción anterior. Las reacciones y sus restricciones siguen teniendo prioridad sobre moverse.

La transición de cámara conserva sus 0,3 s y empieza al resolver el notify; ya no se anticipa para terminar antes de NA. Las comprobaciones interactivas quedan al usuario por falta de herramientas MCP directas para esos inputs.
