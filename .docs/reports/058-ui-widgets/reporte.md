# 058 — Migración de la UI a UMG

El selector continúa en [064 — Opciones desde Data Assets](../064-selector-desde-data-assets/reporte.md), con [guía del editor](../064-selector-desde-data-assets/editor.md). Reemplaza el botón fijo por una lista de opciones reutilizables; conserva las vistas de combate, debug y resultado.

Ampliación posterior: [059 — Flecha de lock-on y barra de NA sobre personajes](../059-indicadores-personaje/reporte.md). Conserva la UI de esta iteración y agrega un Widget Component por personaje.

## Resultado

Selección, HUD de combate, enemigos/amenazas, vista técnica F9 y resultado pasan a Widget Blueprints propios. Se conservan textos, colores, posiciones y acciones del HUD anterior; no se incorporan imágenes ni se modifican reglas de combate o partida.

El HUD anterior de 727 líneas queda como un propietario de 49 líneas. La lógica de presentación se distribuye entre tres widgets C++; no se traslada a graphs de Blueprint. La reducción del archivo HUD no representa eliminar toda esa lógica: se conserva la información técnica y se comparte la presentación de personajes.

## Archivos y responsabilidades

Los headers y fuentes están en los árboles paralelos `Source/vamps/Public/UI/` y `Source/vamps/Private/UI/`.

| Clase / archivo | Responsabilidad y relación |
|---|---|
| `AVampsCombatTestHUD` | Crea `HUDWidgetClass`, lo agrega al viewport, entrega F9 y lo retira al finalizar. Conserva la clase que usan los controllers existentes. |
| `UVampsHUDWidget` | Coordina selección, combate/debug y resultado. Los botones invocan `SelectExecutioner`, `StartLocalRound` y `OpenCharacterSelection` del GameMode existente. Lee marcador, plazos y participantes; administra las listas de widgets. |
| `UVampsFighterWidget` | Reutiliza el panel de jugador, objetivo y personajes del debug. Carga valores iniciales y se suscribe a los seis delegates de atributos de GAS: vida/capacidad, guardia/capacidad y energía/capacidad. Retira los delegates al cambiar el observado o destruirse. Valida valores finitos y capacidades positivas antes de calcular porcentajes. |
| `UVampsEnemyWidget` | Presenta distancia, selección, ataque entrante, dirección relativa a cámara y aviso central de amenaza. Las flechas se componen con widgets Border, sin dibujo Canvas. |
| `vamps.Build.cs` | Agrega la dependencia explícita de `SlateCore` para layout de UMG. |

Barras y valores numéricos se actualizan por delegates existentes. Estados, temporizadores y direcciones continúan con una actualización visual simple en C++; no se introduce un sistema de eventos adicional en las clases de combate ni bindings de Blueprint por frame.

El layout normal conserva la escala mínima respecto a 1280×720. `LayoutScale` y `DebugScale` son Scale Boxes: aplican escala de layout y permiten rasterizar el texto con su escala efectiva. Reemplazan el escalado por Render Transform inicial tras el aviso del usuario sobre pixelado. El debug conserva sus dimensiones en píxeles y el ancho máximo de 380 del HUD anterior, independientemente del DPI de UMG.

## Assets

Todos los assets nuevos están en `/Game/Vamps/UI`:

| Asset | Padre / conexión |
|---|---|
| `WBP_VampsFighter` | `VampsFighterWidget`; grupos `CombatView` y `DebugView`, textos y Progress Bars reutilizados. |
| `WBP_VampsEnemy` | `VampsEnemyWidget`; fila y amenaza direccional. |
| `WBP_VampsHUD` | `VampsHUDWidget`; selección, combate, debug, partida y resultado. Referencia los dos widgets anteriores. |
| `BP_VampsHUD` | `VampsCombatTestHUD`; `HUDWidgetClass = WBP_VampsHUD_C`. |

`BP_VampsMatchGameMode` y `BP_VampsCombatTestGameMode` asignan `BP_VampsHUD_C` como HUD. La comparación de los valores leídos antes/después encontró únicamente ese cambio: [partida antes](partida-antes.json), [después](partida-despues.json), [observación antes](observacion-antes.json), [después](observacion-despues.json). No se cambian mapas, personajes, input ni configuración de Data Assets. No se reutiliza lógica de los ejemplos Third Person.

[Referencias y dependencias finales](conexiones.json) releídas con PIE detenido: clases nativas propias, UMG/Slate y los widgets propios. La consulta de dependencias no funciona durante PIE; se repitió correctamente después de detenerlo y se volvió a iniciar la sesión.

Las conexiones entre C++ y widgets usan nombres `BindWidget` comprobados al compilar. El diseño se edita en el Designer; los botones y la actualización de datos están en las clases nativas. Las imágenes futuras pueden asignarse a las propiedades visuales de estos widgets.

## Verificación

- Compilación por `LiveCodingToolset.LiveCodingToolset.CompileLiveCoding` con `Success` y aplicación de parches reales: [resultado](live-coding.json).
- Los cuatro assets nuevos y los dos GameModes compilaron mediante `BlueprintTools.compile_blueprint`, con `warnings_as_errors=true`.
- Propiedades y slots se descubrieron, leyeron y verificaron por MCP después de editarlos. [Jerarquías y guardado](assets-ui.json): 20 widgets en Fighter, 12 en Enemy y 40 en HUD; los seis assets afectados estaban guardados (`dirty=false`). Las dos referencias a clases de widgets se conservaron después de la última recarga de C++.
- PIE inició con `BP_VampsHUD_C`; la selección apareció en una captura del editor. No se encontraron diagnósticos nuevos de configuración del HUD en la consulta de `LogVampsCombat`.
- Capturar el editor requiere que su ventana esté visible; con la ventana oculta/minimizada la herramienta falla. La captura del viewport de edición no acredita la presentación de PIE.
- No se automatizaron clicks/teclas ni se crearon o ejecutaron tests automatizados.

El usuario confirmó el funcionamiento de selección/START y F9, y señaló pixelado. La corrección con Scale Boxes compiló con `Success`, el widget principal volvió a compilar con advertencias como errores y PIE se reinició. Falta comparar visualmente la nitidez corregida; no se acredita todavía el recorrido completo de muerte/respawn y resultado/revancha con la nueva presentación. La comprobación se consolida en la [guía](editor.md).

Se actualizan el índice y las referencias históricas de 048, 051 y 055. Se conservan las especificaciones funcionales y técnicas existentes, fuera del alcance solicitado.
