# UI con widgets — operaciones del editor

Queda para el usuario comparar la nitidez después del cambio a Scale Boxes y comprobar muerte/respawn y resultado/PLAY AGAIN. El usuario ya confirmó selección/START y F9. Los assets ya existen y están conectados; no hay que recrearlos ni importar imágenes.

## 1. Preparación y compilación

**Hecho — usuario:** servidor MCP abierto; ventana de Unreal restaurada para una captura de PIE.

**Hecho — Codex por MCP:** descubrimiento de herramientas/schemas, PIE detenido antes de compilar y parches reales con `LiveCodingToolset.LiveCodingToolset.CompileLiveCoding` (`Success`). Las clases nativas nuevas se comprobaron con `ObjectTools.search_subclasses`.

No hace falta instalar plugins nuevos. La dependencia `SlateCore` queda declarada en `vamps.Build.cs`.

## 2. Widgets de personaje y enemigo

**Hecho — Codex por MCP.** `UMGToolSet.CreateWidgetBlueprint/AddWidget`, `ObjectTools.list_properties/get_properties/set_properties` y compilación estricta con `BlueprintTools.compile_blueprint`.

En Content Browser → `/Game/Vamps/UI`:

1. `WBP_VampsFighter`, padre `VampsFighterWidget`: raíz Canvas, `CombatView` de 300×160 y `DebugView` para el panel técnico de 380×425. El compacto coloca título en (12,8), barras de 276×6 bajo HP/GUARD/ENERGY y los textos de estado/último contacto. El debug conserva las dos barras y la información anterior. Los nombres de campos requeridos constan en [la captura de jerarquías](assets-ui.json).
2. `WBP_VampsEnemy`, padre `VampsEnemyWidget`: `EnemyRow` anclado arriba/derecha y `ThreatMarker` al centro. Dos Borders forman cada flecha; no hay texturas ni materiales nuevos.
3. Los Progress Bars usan estilo plano, relleno de izquierda a derecha, sin animación de relleno y fondo `(0.15,0.15,0.18,1)`. Vida `(0.25,0.8,0.65,1)`, guardia `(0.3,0.6,1,1)`, energía `(1,0.75,0.15,1)`; la lógica aplica rojo al stun.

Para reproducir o ajustar, abrir cada asset con doble click, editar el Designer y compilar/guardar. Conservar los nombres `BindWidget` y las clases de los controles. No se necesita editar el Graph.

## 3. Pantallas y botones

**Hecho — Codex por MCP**, con las mismas herramientas de UMG/objetos y compilación estricta.

`WBP_VampsHUD`, padre `VampsHUDWidget`, contiene:

- `SelectionView`: fondo opaco, selector de 400×240 centrado, botones de 320×48 EXECUTIONER y START.
- `CombatView`: panel de jugador a 20 px del borde izquierdo y 186 del inferior; objetivo a 320 px del borde derecho y 20 del superior; enemigos desde Y=200.
- `DebugView`: panel izquierdo y lista de rivales; F9 mantiene el tamaño en píxeles del debug anterior.
- `MatchView`: marcador de 480×36, equipo, protección y respawn.
- `ResultView`: panel centrado de 480×280, ganador, estadísticas y PLAY AGAIN de 240×36.

Texto Roboto Regular de tamaño 10; títulos de selección/resultado de 12,5. Fondo habitual `(0.025,0.025,0.04,0.86)`, selector opaco y resultado con alpha 0,96. La escala normal se calcula respecto a 1280×720. `LayoutScale` envuelve `Layout`; `DebugScale` envuelve `DebugView`. Ambos Scale Boxes usan Stretch = User Specified y su contenido se alinea Fill en ambos ejes. C++ ajusta su escala; los Render Transforms permanecen con escala (1,1), para no redimensionar el texto después de rasterizarlo.

En Class Defaults → UI: `EnemyWidgetClass = WBP_VampsEnemy_C`, `FighterWidgetClass = WBP_VampsFighter_C`. Las acciones de botones se conectan en C++ a los métodos existentes del GameMode.

## 4. HUD y GameModes

**Hecho — Codex por MCP.** `BlueprintTools.create`, propiedades descubiertas/leídas/editadas, `compile_blueprint` con `warnings_as_errors=true` y `AssetTools.save_assets`.

1. `BP_VampsHUD`, padre `VampsCombatTestHUD`, tiene `HUDWidgetClass = WBP_VampsHUD_C` en Class Defaults → UI.
2. En `/Game/Vamps/Blueprints/BP_VampsMatchGameMode` y `BP_VampsCombatTestGameMode`, Class Defaults → Classes → HUD Class = `BP_VampsHUD`.
3. Compilar y guardar esos seis assets concretos. La relectura confirmó únicamente el cambio de HUD en los GameModes y `dirty=false` para todos. No guardar globalmente otros assets.

`AssetTools.get_dependencies` y las referencias finales se verificaron con PIE detenido; esa consulta devuelve un error durante PIE. [Conexiones verificadas](conexiones.json). Después se volvió a iniciar `L_LocalMatch`.

No cambiar `DA_VampsCombatSettings`, mapas, clases de personajes ni input para esta migración.

## 5. Comprobación en PIE

**Hecho — Codex por MCP, parcial:** `StartPIE`, `SceneTools.find_actors`, captura del editor y consulta de logs confirmaron el arranque en `L_LocalMatch`, una instancia de `BP_VampsHUD_C` y la pantalla de selección. Después del aviso sobre pixelado, `WrapWidgets/RenameWidget` incorporaron los Scale Boxes, se releyeron propiedades y clases, se compiló/guardó el widget y se reinició PIE con el parche C++ aplicado.

**Hecho — usuario:** confirmó que funcionan EXECUTIONER → START y F9. Señaló pixelado; comparar el resultado después del cambio a escala de layout.

**Pendiente — usuario:**

1. En `L_LocalMatch`, Play: elegir EXECUTIONER; START debe habilitarse e iniciar los 5 s de preparación. No debe haber controles de combate activos durante la selección.
2. Durante la partida, revisar marcador, plazos, vida/guardia/energía, objetivo y F para cambiarlo. Comprobar distancias, flechas y avisos de ataque entrante.
3. Alternar F9 y volver: conservar toda la información técnica, selección y combate. Repetir con una ventana de otra proporción.
4. Observar muerte, cámara libre, cuenta de respawn, restauración de barras y protección de aparición.
5. Al terminar, comprobar ganador/empate y estadísticas. PLAY AGAIN debe volver al selector conservando la elección; START debe iniciar una ronda nueva con sus valores restablecidos.

Estos inputs se dejan al usuario porque las herramientas directas disponibles no activan las acciones de juego; no se utiliza automatización de la interfaz como sustituto. La captura MCP de la aplicación requiere mantener visible la ventana de Unreal.
