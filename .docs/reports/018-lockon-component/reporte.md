# 018 — Componente de lock-on y HUD de debug

Continuación vigente: [019 — Cámara automática sobre lock-on](../019-camara-lockon/reporte.md) y su [guía](../019-camara-lockon/editor.md). Reemplaza la cámara libre y la configuración de Look descritas en esta iteración; conserva el componente y el recorrido de selección.

## Alcance y resultado

Se implementa `UVampsLockOnComponent` como componente nativo de `AVampsCharacter`. Conserva la selección compartida, elige inicialmente el primer candidato válido, permite recorrerlos con una solicitud de input y selecciona al agresor tras una guardia efectiva. Al perder el objetivo recorre desde su posición hacia el siguiente válido, con vuelta al comienzo; sin candidatos elegibles limpia la selección.

El HUD de prueba muestra `LOCK ON: <actor>` en cada panel y marca con una franja dorada al objetivo del jugador. Sus paneles de rivales usan la misma lista y orden del componente que recorre F. Los personajes con vida cero permanecen visibles para observar el estado, pero no pueden seleccionarse.

**Configuración completada por MCP y comprobación parcial en PIE.** El usuario compiló la base y abrió el MCP. `LiveCodingToolset.LiveCodingToolset.CompileLiveCoding` devolvió `NoChanges`: no detectó cambios de código ni creó un parche nuevo. Los componentes nativos están cargados; seis Blueprints compilaron con warnings como errores y los assets quedaron guardados. En PIE se confirmó adquisición, pérdida por vida cero y readquisición. Quedan la pulsación física de F, presentación visual y casos con varios rivales de [editor.md](editor.md).

## Código

Rutas relativas a `Source/vamps/`; cada header está en Public y cada implementación en Private.

| Archivos/clase | Responsabilidad y cambio |
|---|---|
| `Characters/VampsLockOnComponent.h/.cpp`, nuevos | `UVampsLockOnComponent`, derivado directamente de `UActorComponent`. Selección, elegibilidad y lista ordenada con referencias débiles. Consulta inicial del mundo y registro de nuevos actores mediante `FOnActorSpawned`; retira el handler al terminar |
| `Characters/VampsCharacter.h/.cpp` | Crea el subobjeto nativo `LockOn`, compartido por el jugador y los demás personajes. `ReceiveCombatHit` solicita selección del agresor solamente en la rama de guardia efectiva |
| `Combat/FistHitboxComponent.cpp` | Actualiza la recepción a `ReceiveCombatHit`, evitando ocultar `AActor::ReceiveHit` |
| `Player/VampsPlayerController.h/.cpp` | Añade la propiedad requerida `CycleTargetAction`, de tipo Boolean, y su binding `Started` a `LockOn->CycleTarget()` del pawn controlado |
| `UI/VampsCombatTestHUD.h/.cpp` | Muestra selección de cada personaje, destaca el objetivo del jugador y obtiene la lista del componente; elimina su iteración independiente de actores |

El componente consulta la vida de `UVampsAttributeSet`. Es elegible un `AVampsCharacter` distinto del dueño, válido, con BeginPlay completado, sin destrucción iniciada y con Health > 0. La lista registra personajes sin ordenarlos por distancia; mantiene el orden de registro, agrega nuevos al final y elimina referencias inválidas conservando el orden relativo.

Tick valida la selección y limpia referencias; no vuelve a buscar todos los actores del mundo. La selección siguiente se calcula antes de eliminar la referencia del actor perdido, comparando identidad con `HasSameIndexAndSerialNumber`: la igualdad normal de referencias débiles considera iguales dos referencias inválidas y podría perder el lugar en el recorrido. Se mantienen las entradas de vida cero para permitir recuperación de elegibilidad al restaurar vida, incluido R en los dos personajes del escenario de prueba.

No se agregaron clases, bindings ni eventos para el CameraManager. La cámara, movimiento y foco de la IA conservan su ejecución actual. El retarget por guardia se conecta al contacto existente; no modifica la cancelación del ataque ni incorpora las demás reglas del combate FGOA.

### Corrección de nombre heredado

`AVampsCharacter::ReceiveHit(AVampsCharacter*)` ocultaba `AActor::ReceiveHit`, el evento de colisión física declarado en `GameFramework/Actor.h:2303` del motor instalado. Se renombra a `ReceiveCombatHit` en declaración, definición y llamada de `UFistHitboxComponent`, sin wrappers ni compatibilidad con el nombre anterior. No es una UFUNCTION propia, por lo que no requiere migración de nodos Blueprint o assets.

Se revisaron los métodos propios sin `override` de las clases de combate, personaje, controller, componentes, IA, GameMode y HUD contra sus jerarquías del motor, incluidas las bases de UObject y las interfaces relevantes. No se encontraron otros conflictos de métodos en esa revisión. Esto es distinto del C4996 de `GetMovementBase` del motor, que el renombre no corrige.

Se amplía la regla en `AGENTS.md` y en [la guía C++](../../cpp-style.md#nombres-y-miembros-heredados): incluye explícitamente métodos con distinta firma, jerarquía completa, interfaces y distinción entre ocultamiento y un `override` intencional. El ejemplo de clase del documento pasa a `AVampsCharacter`.

## Blueprints y assets

Los componentes heredados `LockOn` se confirmaron en los defaults y en las instancias PIE. Sus referencias pertenecen a las clases nativas de:

- `/Game/Vamps/Blueprints/BP_VampsCharacter`, padre `AVampsCharacter`.
- `/Game/Vamps/Blueprints/BP_VampsCharacter_Player`, padre `AVampsCharacter_Player`, que hereda el componente de la base.

El HUD sigue siendo `AVampsCombatTestHUD` en `/Game/Vamps/Blueprints/BP_VampsCombatTestGameMode`. Se confirmaron sus defaults y su instancia en PIE. El controller de prueba hereda la nueva propiedad del controller normal.

| Asset | Cambio realizado y verificado por MCP |
|---|---|
| `/Game/Vamps/Input/IA_CycleTarget` | Creado mediante `DataAssetTools.create` con clase `/Script/EnhancedInput.InputAction`; Boolean, sin triggers ni modifiers |
| `/Game/Vamps/Input/IMC_Vamps` | Mapping F → IA_CycleTarget en `defaultKeyMappings.mappings`, de 7 a 8 entradas; las siete anteriores y sus modifiers conservan sus valores |
| `/Game/Vamps/Input/IMC_VampsCombatTest` | Mapping F → IA_CycleTarget, de 9 a 10 entradas; las nueve anteriores, incluidos F6/R, conservan sus valores |
| `/Game/Vamps/Blueprints/BP_VampsPlayerController` | `cycleTargetAction` asignada a IA_CycleTarget; resto de acciones y contexto conservados |
| `/Game/Vamps/Blueprints/BP_VampsCombatTestPlayerController` | Misma asignación en su CDO independiente; contexto de prueba y acciones anteriores conservados |

Estos son los cinco `.uasset` nuevos/modificados en disco. También se compilaron `BP_VampsCharacter`, `BP_VampsCharacter_Player`, `BP_VampsGameMode` y `BP_VampsCombatTestGameMode`; no cambiaron sus archivos respecto de Git. Se guardaron explícitamente los nueve assets involucrados. `is_dirty` confirmó false en los nueve y en `L_CharacterBasic`, que no se modificó.

Se usaron `ObjectTools.list_properties/get_properties/set_properties`, `BlueprintTools.compile_blueprint` y `AssetTools.save_assets/is_dirty`. En UE 5.8 los mappings activos consultados están en `defaultKeyMappings.mappings`; el array antiguo `mappings` está vacío y no se utilizó. El código comprueba la presencia y tipo de las cinco Input Actions; la asignación se completó antes de iniciar PIE.

## Verificaciones y límites

- Lectura de código propio y firmas del motor instalado UE 5.8: spawn handlers, vida de actores y comparación de referencias débiles. Se revisaron selección inicial, recorrido con vuelta, pérdida del objetivo antes de compactar, lista vacía, vida cero/restaurada y spawn tardío.
- Formato aplicado a los nueve archivos propios mediante clang-format 20.1.8, incluido el consumidor de `ReceiveCombatHit`; comprobación posterior del formato sin incidencias.
- Revisión de includes, nombres frente a miembros heredados, limpieza del handler y referencias débiles. `git diff --check` sin incidencias.
- Enlaces locales y anclas de los entregables/índice revisados, sin destinos ausentes.
- Compilación nativa anterior confirmada mediante el log del usuario, con enlace de la DLL y metadata. Consulta actual por Live Coding: `NoChanges`, sin parche nuevo.
- Seis Blueprints compilaron con `warnings_as_errors = true`, sin errores ni warnings. Props, mappings y referencias se volvieron a leer tras modificarlos.
- PIE arrancó con jugador y rival Vamps, controller de prueba, input IA_CycleTarget y HUD VampsCombatTestHUD. Ambos componentes eligieron al otro personaje como target.
- En la instancia PIE del rival se cambió temporalmente Health de 100 a 0 mediante propiedades: el jugador pasó a `currentTarget = None`. Restaurar Health = 100 produjo readquisición del rival. Esto comprueba elegibilidad y Tick; no confirma daño por contacto ni la cadena GAS de muerte.
- PIE se detuvo. Los CDO de ambos componentes conservaron `currentTarget = None`; los cambios de vida fueron exclusivamente de ejecución. LogPlayLevel no devolvió errores/warnings de la sesión y los assets quedaron guardados.
- La pulsación física de F, mantenimiento de tecla, apariencia de paneles/franja, retarget por bloqueo y recorrido con varios candidatos no se verificaron. No se agregaron ni ejecutaron tests automatizados ni se automatizó la UI.

### Límites de las herramientas del editor

`SceneTools.add_to_scene_from_asset` rechazó crear actores durante PIE; no se crearon personajes temporales ni se modificó el mapa. La aparición tardía y la pérdida entre varios candidatos conservan la comprobación manual de la guía. No se añadieron funciones de debug, bindings auxiliares ni scripts para eludir esa limitación.

`CaptureViewport` devolvió una vista del editor sin el HUD de juego; `CaptureEditorImage` no pudo capturar ventanas. Por tanto, se confirmó la clase del HUD y sus fuentes de datos, no su apariencia visual. Las capturas de trabajo permanecen fuera del repositorio y no se entregan como evidencia de la presentación del HUD.

### Warning C4996 del motor

El log repite el mismo diagnóstico en `GameFramework/Character.h:798`, dentro del motor instalado. La revisión local confirma que `APawn::GetMovementBase` está marcado `UE_DEPRECATED(5.8, ...)` en `Pawn.h:59–60` y que `ACharacter` conserva su override final en `Character.h:797–798`, también marcado obsoleto. El diagnóstico señala ese override, no una llamada de Vamps. [C4996 identifica el uso de un símbolo obsoleto en MSVC](https://learn.microsoft.com/en-us/cpp/error-messages/compiler-warnings/compiler-warning-level-3-c4996?view=msvc-170).

La búsqueda de `GetMovementBase` en todo `Source` no encontró llamadas ni overrides propios. Los targets ya usan `BuildSettingsVersion.V7` y `EngineIncludeOrderVersion.Unreal5_8`. No se necesita migrar una API del lock-on para resolver este diagnóstico: corresponde a la declaración interna del motor y se repite en las unidades de compilación que la incluyen. No se modificaron headers del motor, código del juego ni opciones para suprimir warnings. La compilación aportada terminó pese al aviso; la eliminación del warning en el motor no se marca resuelta.

La implementación corresponde al escenario local existente. No hay equipos en el código: todos los otros `AVampsCharacter` vivos son candidatos. No se agregan RPC, predicción ni replicación del target; la integración online y el filtro de equipos se abordarán con sus reglas. La IA actual conserva su propio Focus, independiente del componente; mostrar su lock-on no cambia a quién decide atacar.

No se actualizaron las especificaciones generales en esta iteración de código. Este reporte registra el traslado de la selección al componente del personaje respecto de la distribución inicial de responsabilidades de 017. [La guía](editor.md) identifica lo que queda para compilar, configurar y observar.
