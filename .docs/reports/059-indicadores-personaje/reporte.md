# 059 — Indicadores sobre personajes

Coordinación posterior con la cámara: [060 — Salida de combat framing antes de NA](../060-salida-framing-antes-na/reporte.md), sin cambiar la duración de la barra o el bloqueo.

## Resultado

Se agregan una flecha grande sobre el objetivo de lock-on del jugador local y una barra de NA sobre su propio personaje. La barra muestra `CAN'T ATTACK OR DEFEND` y se vacía de 100 a 0 según el tiempo real restante. Ambos viven en un Widget Component del personaje; se conserva la UI de [058](../058-ui-widgets/reporte.md).

## Código y conexiones

| Archivo / clase | Responsabilidad |
|---|---|
| `Source/vamps/Public/Characters/VampsCharacter.h` y `Private/Characters/VampsCharacter.cpp` | Agregan `CharacterIndicator`, un `UWidgetComponent` nativo unido a la raíz. Lo configuran en Screen, tamaño 240×80, pivot (0,5;1), posición relativa (0,0,128), sin colisión. En BeginPlay inicializan el widget, validan su tipo y le entregan el personaje observado. |
| `Source/vamps/Public/UI/VampsCharacterIndicatorWidget.h` y `Private/UI/VampsCharacterIndicatorWidget.cpp` | Nueva clase `UVampsCharacterIndicatorWidget`. Lee la selección del LockOn del jugador local y el plazo `CombatState.NARecoveryEndsAt`. Mantiene la lógica de presentación en C++, con solo tres controles enlazados por `BindWidget`. |
| `/Game/Vamps/UI/WBP_VampsCharacterIndicator` | Widget Blueprint propio con diez widgets. `LockOnMarker` compone una flecha amarilla hacia abajo con tres Borders; `NAView` contiene fondo, texto y Progress Bar. No requiere imágenes ni graphs de lógica. |
| `/Game/Vamps/Blueprints/BP_VampsCharacter` y `BP_VampsCharacter_Player` | Asignan el widget nuevo en el componente heredado `CharacterIndicator`. Con ello lo reciben bots y jugador. |

La flecha aparece únicamente sobre el objetivo seleccionado. NA aparece únicamente sobre el jugador local: porcentaje = `clamp((NARecoveryEndsAt - Now) / NARecoveryDuration, 0, 1)`. Al agotarse el plazo se pone en cero y se oculta. La UI no crea otro temporizador ni modifica la duración, el bloqueo de acciones o la selección.

Ambos indicadores se ocultan si muere el jugador observado/local, y durante selección, indisponibilidad o resultado. Se readmiten con las condiciones normales de respawn y ronda nueva. En el modo de observación sin MatchGameMode se conservan las condiciones de personaje vivo y target válido.

Screen mantiene orientación y tamaño legibles mediante Slate, sin render target de UI en el mundo; también implica que el indicador no queda ocluido por paredes. La posición sigue la cápsula, sin depender de sockets o animaciones.

## Preservación y verificación

- Compilación C++ mediante `LiveCodingToolset.LiveCodingToolset.CompileLiveCoding`: `Success`, con parche real aplicado. [Resultado](live-coding.json).
- El componente nativo se encontró por MCP en ambos Blueprints después de la recarga. El widget y los dos personajes compilaron con `BlueprintTools.compile_blueprint`, `warnings_as_errors=true`.
- Propiedades y slots descubiertos antes de editar, leídos y comparados después. [Componente final y guardado](componentes.json), [jerarquía](widget.json). Los tres assets afectados quedaron guardados con `dirty=false`.
- Se capturaron ajustes relevantes de ambos personajes antes del cambio de clase: [antes](personajes-antes.json), [después](personajes-despues.json). CombatSettings, daño, vida inicial, mesh, materiales, AnimBP y transform del mesh coincidieron sin diferencias.
- No se modifican clases/estructuras ni valores de Data Assets; NARecoveryDuration solo se consulta. No se reutiliza lógica Third Person.
- PIE inició con los cuatro personajes vivos y sus cuatro componentes apuntando al widget correcto, en Screen, 240×80 y Z=128. [Lectura runtime](pie.json). La consulta de `LogVampsCombat` no mostró errores de inicialización del indicador.
- Una primera captura mostraba el Designer y no se usó como evidencia del juego. Después se releyó `RoundPhase=Active`, con Executioner seleccionado, y se capturó el viewport: [flecha y NA visible](indicadores.png). Se observan la flecha amarilla sobre el target y la barra sobre el jugador con relleno parcial.
- **Comprobación manual — usuario, 2026-10-29:** confirmó «Se ven ambos correctamente» al consultar por la flecha sobre el objetivo y la barra de NA vaciándose sobre su personaje. Se registra su confirmación, sin atribuirla a MCP ni extenderla a los casos de muerte/respawn y fin de ronda de la [guía](editor.md).
- No se automatizan inputs ni se agregan/ejecutan tests.

Se agrega este par de reportes y su enlace desde el índice. Se mantienen las especificaciones de juego sin cambios, conforme al alcance solicitado.

## Diferenciar enemigos y aliados

El usuario precisó que quiere distinguir equipos, no identificar rivales individuales. La consulta de los materiales del mesh `SKM_Manny_Simple` confirmó dos slots, con `MI_Manny_01_New` y `MI_Manny_02_New`; ambos exponen el parámetro Vector `Paint Tint`.

La opción simple es crear instancias dinámicas por personaje para esos dos materiales y asignar un tinte por equipo: por ejemplo, azul para el equipo local y rojo para el rival. Debe decidirse por equipo, incluyendo al bot aliado; no por ser jugador o bot. Esta iteración no implementa colores de equipo ni modifica los materiales compartidos. Un contorno requeriría trabajo adicional de material de postproceso/Custom Depth y no hace falta para este primer contraste.
