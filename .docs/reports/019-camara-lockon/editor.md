# 019 — Configurar y comprobar la cámara automática

**Pendiente para el usuario:** compilar `vampsEditor` con Unreal cerrado para consolidar la clase reflejada, revisar los cambios sin guardar del mapa y comprobar visualmente seguimiento, F y WASD. Los assets de cámara e input ya están configurados, compilados y guardados por MCP. PIE quedó detenido. Esta guía reemplaza las instrucciones de cámara libre y Look de [018](../018-lockon-component/editor.md).

## 1. Compilación nativa

**Hecho — Codex por MCP.** Con PIE detenido, `LiveCodingToolset.LiveCodingToolset.CompileLiveCoding` compiló y aplicó los cambios con resultado `Success`. No fue `NoChanges`. La clase nueva estuvo disponible para crear el Blueprint y luego se instanció correctamente en PIE.

**Pendiente — usuario, compilación completa.** Live Coding avisó que los cambios en tipos reflejados pueden afectar el empaquetado. Guardar el trabajo que se quiera conservar, cerrar Unreal y en Rider o Visual Studio compilar el target **vampsEditor**, configuración **Development Editor**, plataforma **Win64**, del proyecto `C:\fran\vamps\vamps.uproject`. Reabrirlo al terminar. El MCP disponible permite Live Coding, pero no se dispone de una operación directa verificada para este ciclo con el editor cerrado; no se ejecutó una compilación de shell como sustituto.

## 2. Crear y configurar el manager

**Hecho — Codex por MCP.** `BlueprintTools.create/get_parent`, `ObjectTools.set_properties/get_properties`, `BlueprintTools.compile_blueprint` y `AssetTools.save_assets/is_dirty`.

Pasos reproducibles en Content Browser:

1. En `/Game/Vamps/Blueprints`, crear Blueprint Class → All Classes → `VampsPlayerCameraManager`, con nombre `BP_VampsPlayerCameraManager`.
2. Abrir Class Defaults → Lock On Camera y establecer **Follow Pitch = −15**, **Tracking Speed = 6**, **Horizontal Tolerance = 6**.
3. Compilar y guardar. No necesita lógica en Event Graph.

Se confirmó padre `/Script/vamps.VampsPlayerCameraManager` y se releyeron los valores guardados.

## 3. Asignar los controllers y conservar el SpringArm

**Hecho — Codex por MCP.** En **Class Defaults → Player Camera Manager Class** de estos dos Blueprints se asignó `BP_VampsPlayerCameraManager`:

- `/Game/Vamps/Blueprints/BP_VampsPlayerController`.
- `/Game/Vamps/Blueprints/BP_VampsCombatTestPlayerController`.

Se usaron `ObjectTools.set_properties/get_properties`, compilación de Blueprint y guardado. Debe asignarse en ambos: el controller de prueba deriva de la clase C++ de prueba y no del Blueprint normal. Cycle Target Action conserva `/Game/Vamps/Input/IA_CycleTarget`; los contextos siguen siendo respectivamente `IMC_Vamps` e `IMC_VampsCombatTest`.

**Hecho — Codex por MCP, lectura de defaults nativos.** En `/Game/Vamps/Blueprints/BP_VampsCharacter_Player`, seleccionar el componente heredado **CameraBoom** y comprobar:

| Propiedad | Valor confirmado |
|---|---|
| Target Arm Length | 400 cm |
| Target Offset | X 0, Y 0, Z 60 |
| Use Pawn Control Rotation | Activo |
| Do Collision Test | Activo |
| Enable Camera Lag | Desactivado |
| Enable Camera Rotation Lag | Desactivado |

El offset se establece en el constructor C++; no se agregó otro componente ni un override Blueprint. `FollowCamera` sigue adjunta al socket del brazo y no aplica otra ControlRotation propia.

## 4. Retirar la mirada manual y compilar assets

**Hecho — Codex por MCP.** Abrir `/Game/Vamps/Input/IMC_Vamps` y `/Game/Vamps/Input/IMC_VampsCombatTest`, retirar únicamente la entrada de `IA_Look` y guardar. Se modificó `defaultKeyMappings.mappings` con ObjectTools, preservando íntegramente los otros mappings. `IA_Look` no se eliminó del Content Browser. La propiedad Look Action ya no aparece en los controllers porque se retiró del código.

Resultado releído:

- `IMC_Vamps`: D, A, W, S, LeftMouseButton, RightMouseButton, F; siete mappings.
- `IMC_VampsCombatTest`: los mismos más F6 y R; nueve mappings.

`BlueprintTools.compile_blueprint` con `warnings_as_errors = true` compiló `BP_VampsPlayerCameraManager`, `BP_VampsCharacter`, `BP_VampsCharacter_Player`, ambos controllers y ambos GameModes: siete Blueprints, sin warnings ni errores. Se guardaron esos siete y los dos contextos con `AssetTools.save_assets`; los nueve terminaron con `is_dirty = false`.

## 5. Comprobaciones realizadas en PIE

**Hecho — Codex por MCP, verificación de estado.** En `/Game/Vamps/Maps/L_CharacterBasic`, `EditorAppToolset.StartPIE/StopPIE`, SceneTools, ActorTools y ObjectTools permitieron comprobar:

1. El controller utiliza una instancia de `BP_VampsPlayerCameraManager`.
2. Al mover temporalmente el rival seleccionado de `(200, -280, 88)` a `(200, 280, 88)`, la cámara cambia gradualmente de yaw −27,69° a +27,69°, conservando el objetivo. El rumbo al rival es ±33,69°, consistente con la tolerancia de 6°.
3. Al poner Health de los dos rivales en cero, el componente queda sin target y la cámara vuelve al yaw 0° del jugador. El pitch permanece en −15°.
4. Al restaurar Health del rival desplazado a 100, reaparecen selección y seguimiento.

Se restauraron las propiedades temporales y se detuvo PIE. Se confirmó la posición original del actor del mapa fuente. No hubo errores/warnings de LogPlayLevel en la consulta. No se simularon teclas ni se automatizó la interfaz.

## 6. Revisar mapa y encuadre

**Pendiente — usuario, mapa.** `L_CharacterBasic` terminó con cambios sin guardar (`is_dirty = true`). Codex no guardó el mapa. Revisar su contenido antes de guardar o descartar. Al iniciar la comprobación ya había un `BP_VampsCharacter` colocado además del rival que crea el GameMode; no fue creado por Codex.

**Pendiente — usuario, presentación e inputs físicos.** Tras la compilación completa, abrir el mapa y comprobar **World Settings → GameMode Override = BP_VampsCombatTestGameMode**. Iniciar PIE y observar:

1. La cámara sigue al rival con franja dorada y coincide con `LOCK ON` del HUD. El mouse no la orbita.
2. Con dos rivales vivos, F cambia selección y la cámara acompaña con transición gradual. Mantener F no recorre repetidamente la lista.
3. WASD sigue relativo a la vista; probar acercamiento, retirada, laterales y diagonales. El cuerpo puede cambiar de orientación mientras la cámara sigue al objetivo.
4. El pequeño desplazamiento lateral del rival permite tolerancia antes de corregir. Evaluar altura y distancia del encuadre con Follow Pitch −15°, brazo 400 cm y offset Z 60 cm. Ajustar los tres parámetros del manager desde su Blueprint si hace falta calibrar.
5. Cerca de paredes, el brazo retrae la cámara mediante colisión nativa y se extiende al liberar el espacio. No se implementó fade de materiales.
6. LMB, RMB, F6 y R conservan sus funciones de prototipo. El retarget por bloqueo requiere contacto real en guardia; consultar [013](../013-recuperacion-blueprints/editor.md#5-comprobaciones) si el golpe no produce contacto.

La comprobación por MCP no aprobó visualmente encuadre, colisiones o cambio por pulsación física de F. Las capturas disponibles habían mostrado el viewport del editor o fallado al capturar ventanas; estos puntos quedan como observación manual, sin alternativa por automatización de UI.
