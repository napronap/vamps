# Indicadores sobre personajes — operaciones del editor

El usuario confirmó la flecha y la barra de NA vaciándose correctamente. Queda observar su retirada/reaparición en muerte, respawn y fin de ronda. Los assets ya existen, están configurados y guardados; no hay que crearlos otra vez.

## 1. Compilar las clases nativas

**Hecho — Codex por MCP.** PIE estaba detenido. `LiveCodingToolset.LiveCodingToolset.CompileLiveCoding` aplicó un parche real (`Success`). Se comprobó que `CharacterIndicator` existía en los CDO de `BP_VampsCharacter` y `BP_VampsCharacter_Player`.

El componente se crea en C++, no mediante Add Component del Blueprint. No agregar una segunda copia. El nuevo widget hereda de `VampsCharacterIndicatorWidget`, clase propia derivada de `UserWidget`.

## 2. Widget Blueprint

**Hecho — Codex por MCP.** `UMGToolSet.CreateWidgetBlueprint/AddWidget` y `ObjectTools.list_properties/get_properties/set_properties` construyeron `/Game/Vamps/UI/WBP_VampsCharacterIndicator`.

Para reproducir o ajustar en el Designer:

1. Raíz `IndicatorSize`, Size Box con Width Override 240 y Height Override 80; contiene `IndicatorCanvas`.
2. `LockOnMarker`, Canvas de 240×80: `ArrowStem` y dos Borders girados ±45° forman una flecha amarilla grande hacia abajo. Color `(1,0.75,0.15,1)`. Su visibilidad inicial es Collapsed.
3. `NAView`, Canvas de 240×46 situado en (0,30): fondo `(0.025,0.025,0.04,0.9)`, `NALabel` centrado con `CAN'T ATTACK OR DEFEND`, Roboto Regular 10, y `NAProgress` en (10,28), tamaño 220×10. Fondo de barra `(0.15,0.15,0.18,1)`, relleno amarillo, Left to Right / Scale, sin animación de relleno. Visibilidad inicial Collapsed.
4. Conservar los nombres y tipos de `LockOnMarker`, `NAView` y `NAProgress`: son los tres enlaces `BindWidget` requeridos. La lógica se resuelve en C++; no hay que conectar Tick ni funciones en el Graph.

`BlueprintTools.compile_blueprint`, con `warnings_as_errors=true`, confirmó la compilación. [Jerarquía leída](widget.json).

## 3. Configurar el componente en personajes

**Hecho — Codex por MCP.** En los dos Blueprints `/Game/Vamps/Blueprints/BP_VampsCharacter` y `BP_VampsCharacter_Player`, seleccionar `CharacterIndicator` en Components:

- Widget Class: `WBP_VampsCharacterIndicator`.
- Space: Screen.
- Draw Size: X=240, Y=80; Draw at Desired Size desactivado.
- Pivot: X=0.5, Y=1.
- Transform relativo: Location (0,0,128), Rotation (0,0,0), Scale (1,1,1).
- Collision: No Collision, configurado en la clase nativa.

Los valores se releyeron tras editar/compilar y se guardaron esos dos Blueprints y el widget con `AssetTools.save_assets`. [Configuración final](componentes.json). La comparación de mesh, animación, materiales y parámetros de combate anteriores no encontró cambios.

Screen mantiene el indicador de frente y con tamaño estable, y no lo ocluye con paredes. Para sustituir la flecha por una imagen más adelante, modificar su grupo visual conservando `LockOnMarker`.

## 4. PIE y observación

**Hecho — Codex por MCP, parcial.** `StartPIE`, `SceneTools.find_actors` y `ObjectTools.get_properties` verificaron cuatro personajes vivos con componentes correctamente configurados, rotación cero, escala uno y NoCollision. La inicialización del widget no produjo errores en la consulta de `LogVampsCombat`. La captura inicial del Designer no se usó como evidencia; una captura posterior con `RoundPhase=Active` muestra [la flecha y la barra de NA parcialmente vacía](indicadores.png).

**Hecho — usuario, 2026-10-29:** confirmó que ambos indicadores se ven correctamente, en respuesta a la consulta sobre flecha del objetivo y barra de NA vaciándose. Pasos reproducibles de esa comprobación:

1. Abrir `L_LocalMatch`, iniciar PIE y elegir EXECUTIONER → START.
2. Comprobar una sola flecha sobre el objetivo local actual. F debe trasladarla al nuevo enemigo. No debe señalar al bot aliado ni al propio jugador.
3. Terminar un ataque/cadena para entrar en NA. Sobre el jugador debe aparecer `CAN'T ATTACK OR DEFEND` y la barra debe vaciarse hasta desaparecer al finalizar el plazo. El NA vigente es el del DA; no se cambió para facilitar la observación.

**Pendiente — usuario:** verificar que ambos desaparecen al morir, en selección y al terminar la ronda. Al respawn deben volver solo cuando correspondan. Revisar legibilidad con movimiento, dash y cambios de cámara; estos casos no se dan por comprobados a partir de la confirmación general de visibilidad y vaciado.

Estos inputs requieren intervención manual porque las herramientas directas disponibles no los activan; no se usa automatización de la interfaz. Los materiales por equipo quedan como una consulta separada, sin operaciones adicionales necesarias para estos indicadores.
