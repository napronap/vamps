# 018 — Configurar y comprobar lock-on

La [guía de 019](../019-camara-lockon/editor.md) reemplaza las indicaciones de conservar Look y cámara libre de este documento. Los pasos de selección y HUD siguen siendo aplicables.

**Pendiente para el usuario:** observar el HUD y comprobar F, su mantenimiento, recorrido con varios rivales y retarget por bloqueo. La acción, mappings y controllers ya están configurados, compilados y guardados por MCP. La adquisición, pérdida por vida cero y readquisición se comprobaron en PIE, que quedó detenido. No hay assets ni compilación adicional pendientes por esta configuración.

## 1. Compilar la clase y recargar componentes nativos

**Hecho — usuario, compilación nativa según log aportado.** Las ocho acciones finalizan con enlace de `UnrealEditor-vamps.dll` y escritura de metadata; no hay errores en el fragmento. El C4996 señala `Character.h` del motor y no requiere cambiar el lock-on. El fragmento no confirma si el editor estaba cerrado ni la recarga de sus componentes.

Para reproducir una compilación completa: detener PIE, guardar el trabajo y cerrar Unreal; en Rider o Visual Studio, compilar `vampsEditor`, configuración `Development Editor`, plataforma `Win64`, para `C:\fran\vamps\vamps.uproject`. La clase reflejada nueva y los cambios de subobjetos requieren recarga desde la compilación completa.

**Hecho — Codex por MCP, consulta actual de compilación.** `LiveCodingToolset.LiveCodingToolset.CompileLiveCoding` devolvió `NoChanges`: el motor no detectó cambios de código y no generó un parche nuevo. No se presenta como compilación/aplicación de un parche. Los componentes nativos están cargados. El renombre a `ReceiveCombatHit` no requiere migrar nodos ni assets porque no es una UFUNCTION propia. No se modificó el header del motor ni se suprimió C4996.

**Hecho — Codex por MCP, componentes y Blueprints.** `ObjectTools` confirmó el componente heredado `LockOn`, de clase `VampsLockOnComponent`, en `/Game/Vamps/Blueprints/BP_VampsCharacter` y `BP_VampsCharacter_Player`, y después en las instancias PIE. `BlueprintTools.compile_blueprint` compiló ambos con `warnings_as_errors = true`; se guardaron con `AssetTools.save_assets`. Para reproducir la consulta, abrir ambos Blueprints y comprobar el componente heredado, sin añadir otra copia desde Add Component.

No se requirió crear otro Blueprint para el componente nativo.

## 2. Crear y mapear la entrada

**Hecho — Codex por MCP.** `DataAssetTools.create`, `ObjectTools.get_properties/set_properties` y `AssetTools.save_assets/is_dirty`. Pasos reproducibles en Content Browser, carpeta `/Game/Vamps/Input`:

1. Crear una Input Action llamada `IA_CycleTarget`.
2. Abrirla y establecer **Value Type = Digital (bool)**. Mantener vacías sus listas de Triggers y Modifiers; el código utiliza `Started`, por lo que mantener F no repite el recorrido.
3. Abrir `/Game/Vamps/Input/IMC_Vamps`. Añadir un mapping de `IA_CycleTarget` con tecla **F**, sin triggers/modifiers adicionales. Guardar.
4. Repetir el mapping en `/Game/Vamps/Input/IMC_VampsCombatTest` y guardar. No eliminar los mappings existentes.

Resultado confirmado: Boolean, sin triggers/modifiers; 8 mappings en IMC_Vamps y 10 en IMC_VampsCombatTest. Los mappings anteriores se preservaron íntegramente. En las propiedades MCP de UE 5.8 se modificó `defaultKeyMappings.mappings`, no el array antiguo `mappings`.

## 3. Asignar en los controllers

**Hecho — Codex por MCP.** `ObjectTools.set_properties/get_properties`, `BlueprintTools.compile_blueprint` y `AssetTools.save_assets/is_dirty`. En los dos Blueprints siguientes, **Class Defaults → Input → Cycle Target Action** quedó asignado a `/Game/Vamps/Input/IA_CycleTarget`; ambos compilaron con warnings como errores y quedaron guardados:

- `/Game/Vamps/Blueprints/BP_VampsPlayerController`.
- `/Game/Vamps/Blueprints/BP_VampsCombatTestPlayerController`.

Conservar Move, Look, Attack, Guard y los contextos actuales. En el controller de prueba, conservar además Cycle AI Mode y Reset Combat. La nueva propiedad se hereda desde `AVampsPlayerController`; el controller de prueba no hereda sus defaults del Blueprint normal.

## 4. Comprobar el escenario y HUD

**Hecho — Codex por MCP, configuración y ejecución.** `SceneTools.get_current_level` confirmó `/Game/Vamps/Maps/L_CharacterBasic`; `ObjectTools` confirmó los defaults del GameMode de prueba: jugador, controller de prueba, rival `BP_VampsCharacter`, IA `BP_VampsAIController`, HUD `VampsCombatTestHUD` e Initial AI Mode = Passive. Los dos GameModes también compilaron con warnings como errores. `EditorAppToolset.StartPIE/StopPIE` inició y detuvo la sesión; la inspección de actores confirmó esas clases y la referencia de input del controller en ejecución. No se modificó el mapa.

**Hecho — Codex por MCP, disponibilidad del target.** Se cambió temporalmente Health del rival PIE a 0 y luego a 100 con `ObjectTools.set_properties`, leyendo `LockOn.currentTarget` entre cambios: rival → None → rival. Se restauró la vida y se detuvo PIE. Esto no verifica daño por puño, muerte o el input R. Los CDO conservaron target None. `LogsToolset.GetLogEntries` no devolvió errores/warnings de LogPlayLevel; los nueve assets y el mapa terminaron con `is_dirty = false`.

**Pendiente — usuario, presentación e inputs físicos.** Abrir el mapa y, en **World Settings → GameMode Override**, comprobar `BP_VampsCombatTestGameMode` si se cambió después de esta iteración. Las herramientas de captura no permitieron observar el HUD de juego: CaptureViewport mostró el viewport del editor y CaptureEditorImage no pudo capturar ventanas. No se automatizó la interfaz.

Iniciar PIE y comprobar:

1. El panel del jugador muestra `LOCK ON: <nombre del rival>` y el panel derecho del rival tiene franja dorada. El rival muestra su propia selección en su panel. La adquisición puede completarse en el primer Tick posterior al BeginPlay de los personajes.
2. Con un único candidato vivo, cada pulsación F conserva ese mismo objetivo. Mantener F no genera cambios repetidos.
3. WASD, mouse, LMB, RMB, F6 y R siguen funcionando. La cámara mantiene el comportamiento libre anterior; el componente todavía no la dirige.
4. Si los ataques llegan a producir contacto, reducir a cero la vida del rival: no queda seleccionado y el panel del jugador muestra `LOCK ON: None`. El panel del rival sigue visible con vida cero y sin franja dorada. R restaura la vida de ambos personajes y permite readquirir al rival.

El contacto del puño/notify seguía pendiente en 013–015. Si los golpes no quitan vida, consultar [la comprobación del montage de 013](../013-recuperacion-blueprints/editor.md#5-comprobaciones); eso impide comprobar pérdida de target por daño, pero no permite darla por aprobada.

## 5. Comprobar el recorrido con varios candidatos

**Pendiente — usuario.** Para observar selección múltiple, detener PIE y colocar temporalmente dos instancias de `/Game/Vamps/Blueprints/BP_VampsCharacter` en `L_CharacterBasic`, por ejemplo en `(0, 500, 100)` y `(0, -500, 100)`, conservando Auto Possess AI = Disabled. No necesitan controllers para figurar como candidatos. Estas instancias son adicionales al rival que crea el GameMode.

Iniciar PIE. Leer los nombres de los paneles de la derecha y pulsar F: la franja debe recorrerlos de arriba hacia abajo y volver al primero, saltando personajes con vida cero. Al perder el seleccionado, se debe elegir el siguiente elegible del mismo orden. Un bloqueo frente a un agresor distinto del seleccionado debe seleccionar al agresor; requiere un contacto real en guardia, no solo pulsar RMB.

R solo restaura jugador y rival creados por el GameMode, no estas instancias adicionales. Reiniciar PIE para restaurarlas. Al terminar, detener PIE y retirar las dos instancias temporales antes de guardar el mapa. No se realizaron estos cambios de escena por Codex.

El MCP rechazó crear actores mientras PIE estaba activo; no se crearon instancias temporales ni se cambió el mapa. La destrucción/spawn tardío y el recorrido entre varios objetivos conservan comprobación manual. No hay cambios de cámara, respawn o equipos incluidos en esta guía.
