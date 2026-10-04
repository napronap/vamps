# MVP del duelo: pasos del editor

**Pendiente para el usuario:** comprobar visualmente la reproducción del ataque después de la corrección de enlaces del montage (paso 9), revisar notifies de las copias de ataque (paso 7) y persistir Gameplay Abilities en el proyecto (paso 1). El usuario ya creó y conectó `ABP_Duel`, `BS_Duel` y `AM_DuelAttack`; sus referencias y el Slot se verificaron por MCP.

MCP está conectado. La corrección del montage compiló y se aplicó por Live Coding (`Success`). Los tres Blueprints compilaron durante la configuración y PIE inició con dos personajes propios. Se incluyen instrucciones manuales equivalentes para reproducir lo hecho por MCP. Proyecto: `C:\dev\vamps\vamps.uproject`. Todos los assets nuevos van en `/Game/Duel/`.

## 1. Gameplay Abilities

**Hecho — Codex por MCP, consulta de disponibilidad. Pendiente — usuario, persistir la dependencia explícita.** `PluginToolset.IsEnabled` confirmó Gameplay Abilities activo y `SetPluginEnabled` aceptó la solicitud, pero la entrada no quedó guardada en `vamps.uproject`. `ModelContextProtocol`, `AllToolsets` y `LiveCodingToolset` ya estaban disponibles.

Con Unreal cerrado, agregar a la lista `Plugins` de `vamps.uproject` la entrada `{ "Name": "GameplayAbilities", "Enabled": true }` y guardar. Después del arranque, confirmar Gameplay Abilities habilitado en `Edit > Plugins`. Esto evita depender de los toolsets del editor para habilitar GAS.

## 2. Compilar

**Hecho — usuario, reinicio de MCP. Hecho — Codex por MCP, consulta de Live Coding y compilación de los tres Blueprints.**

La primera compilación aplicó `UnrealEditor-vamps.patch_0`. Tras la reorganización y el reinicio, `LiveCodingToolset.CompileLiveCoding` respondió `NoChanges`: no encontró código pendiente ni produjo otro parche. `BlueprintTools.compile_blueprint` compiló `BP_DuelCharacter`, `BP_DuelPlayerController` y `BP_DuelGameMode` después de sus asignaciones. Codex no ejecutó un rebuild convencional. Los tests fueron eliminados y no deben ejecutarse ni recrearse.

**Corrección del montage — hecha por Codex mediante MCP:** Live Coding devolvió `Success` en el log y completó la recarga del código. La petición HTTP agotó su espera antes del final, por lo que se confirmó el resultado con `LogsToolset.GetLogEntries`. El parche está aplicado en la sesión actual; basta volver a Play para comprobarlo.

Para reproducir desde un proyecto sin binarios actualizados:

1. Con Unreal cerrado, compilar el target `vampsEditor`, configuración `Development Editor`, plataforma `Win64`, desde Rider. Esperar un resultado correcto antes de abrir los assets guardados con las clases nuevas.
2. Abrir `vamps.uproject` con UE 5.8.3.
3. En `Output Log`, campo `Cmd`, ejecutar `ModelContextProtocol.StartServer`.
4. Confirmar la reconexión. Para cambios posteriores, Codex puede invocar Live Coding por MCP.

Si Rider sigue mostrando la carpeta antigua `Duel`, recargar el proyecto para actualizar su índice. Las clases y sus propiedades reflejadas conservan sus nombres: no hay que recrear ni reasignar Blueprints como consecuencia de esta reorganización.

## 3. Assets ya creados

**Hecho — Codex por MCP.** `DataAssetTools.create`, `BlueprintTools.create` y `AssetTools.save_assets` crearon y guardaron:

| Asset | Clase nativa / finalidad |
|---|---|
| `/Game/Duel/Data/DA_DuelCombat` | `CombatConfig`; parámetros compartidos |
| `/Game/Duel/Blueprints/BP_DuelCharacter` | `VampsDuelCharacter`; ambos combatientes |
| `/Game/Duel/Blueprints/BP_DuelPlayerController` | `DuelPlayerController`; controles y cámara |
| `/Game/Duel/Blueprints/BP_DuelGameMode` | `DuelGameMode`; preparación y reinicio |

Para reproducir: crear un Data Asset de clase `CombatConfig` y tres Blueprint Classes con las clases padre indicadas. No duplicar Blueprints del ejemplo. Compilar y guardar cada BP.

## 4. Input Actions y Mapping Context

**Hecho — Codex por MCP.** `DataAssetTools.create` creó los siete assets. `ObjectTools.set_properties` configuró tipos, mappings, modificadores y referencias del controller; se verificaron con `get_properties` y se guardaron con `AssetTools.save_assets`.

En `/Game/Duel/Input/`, crear estos Input Actions y un Input Mapping Context `IMC_Duel`:

| Asset | Value Type | Mappings en `IMC_Duel` |
|---|---|---|
| `IA_DuelMove` | Axis2D | D: eje X positivo; A: `Negate`; W: `Swizzle Input Axis Values`, orden YXZ; S: `Negate` y luego `Swizzle`, orden YXZ |
| `IA_DuelLook` | Axis2D | `Mouse XY 2D-Axis`, sin modificadores |
| `IA_DuelShared` | Digital (bool) | Left Mouse Button |
| `IA_DuelLock` | Digital (bool) | Tab |
| `IA_DuelReset` | Digital (bool) | R |
| `IA_DuelAIMode` | Digital (bool) | F1 |

No agregar triggers de repetición. El código enlaza ataque/defensa con `Started`: mantener el botón no repite. `Move` y `Look` usan `Triggered`; soltar movimiento lo limpia mediante `Completed`/`Canceled`.

En UE 5.8, las nueve entradas están en `Default Key Mappings > Mappings`; el array antiguo `Mappings` está deprecado. Los `Negate` de A y S tienen X activado, Y/Z desactivados. Los `Swizzle` de W y S usan YXZ. Para crear modificadores instanciados por MCP, usar `refPath` con su clase y después editar el subobjeto devuelto; el campo `instance` mencionado por la descripción de la herramienta no creó los modificadores. Se verificó que no quedaron referencias vacías.

En `BP_DuelPlayerController > Class Defaults > Input`, asignar `Duel Mapping = IMC_Duel` y las seis propiedades `Move Action`, `Look Action`, `Shared Action`, `Lock Action`, `Reset Action`, `AI Mode Action` a sus assets. Compilar y guardar.

**Ya verificado en configuración:** `DefaultPlayerInputClass = EnhancedPlayerInput` y `DefaultInputComponentClass = EnhancedInputComponent` en `Config/DefaultInput.ini`; conservar esos valores.

## 5. Configurar personaje y GameMode

**Hecho — Codex por MCP.** `ObjectTools.set_properties` asignó configuración, mesh y clases. `BlueprintTools.compile_blueprint` y `AssetTools.save_assets` compilaron y guardaron los BP; la lectura posterior confirmó las referencias y las transformaciones.

1. En `BP_DuelCharacter`, asignar `Combat Config = DA_DuelCombat`.
2. En su componente `Mesh`, asignar `/Game/Characters/Mannequins/Meshes/SKM_Manny_Simple`. Conservar Location `(0,0,-88)`, Rotation `(0,-90,0)` y colisión desactivada, ya definidos por la clase nativa.
3. En `BP_DuelGameMode > Class Defaults`, asignar `Default Pawn Class = BP_DuelCharacter`, `Opponent Class = BP_DuelCharacter`, `Player Controller Class = BP_DuelPlayerController`, `HUD Class = DuelHUD`, `Shared Config = DA_DuelCombat`.
4. Conservar `Player Spawn`: Location `(-220,0,100)`, Rotation `(0,0,0)`; `Opponent Spawn`: Location `(220,0,100)`, Rotation `(0,180,0)`; escala `(1,1,1)` en ambos.
5. Compilar y guardar ambos BP. No colocar otro rival en el mapa: lo crea el GameMode.

`DuelHUD` es nativo; no requiere Widget Blueprint. `DuelAIController` ya está asignado por la clase del personaje y se usa al crear el rival.

## 6. Animation Blueprint propio

**Hecho — usuario; verificado por Codex mediante MCP.** Existen `ABP_Duel` y `BS_Duel`. El AnimBP hereda de `DuelAnimInstance`, recibe `Speed` y conecta `BS_Duel` → `Slot DefaultSlot` → `Output Pose`. El mesh usa `AnimationBlueprint` y `ABP_Duel_C`. El Blend Space creado es de dos ejes, con todas las muestras en Y=0 y el pin Y fijado en 0; para esta locomoción solo se usa Speed en X. La creación especializada sigue sin herramienta MCP directa.

1. Crear en `/Game/Duel/Animations/` un Animation Blueprint llamado `ABP_Duel`, con skeleton `/Game/Characters/Mannequins/Meshes/SK_Mannequin` y clase padre `DuelAnimInstance`. Si el diálogo no ofrece la clase padre, elegirla después en `Class Settings`.
2. Usar el Blend Space existente `/Game/Duel/Animations/BS_Duel`, sobre ese skeleton. Eje X `Speed`: mínimo `0`, máximo `420`. Muestras verificadas: `/Game/Characters/Mannequins/Anims/Unarmed/MM_Idle` a `0`, `Walk/MF_Unarmed_Walk_Fwd` a `180` y `Jog/MF_Unarmed_Jog_Fwd` a `420`, todas con Y=0. Son solo recursos de animación; no copiar el AnimBP del ejemplo.
3. En el AnimGraph de `ABP_Duel`, conectar un Blend Space Player de `BS_Duel`, con X alimentado por la variable heredada `Speed` y Y=0, a un nodo `Slot` con `DefaultGroup.DefaultSlot`; conectar ese Slot a `Output Pose`.
4. Activar looping del Blend Space Player. Dejar Root Motion desactivado para la locomoción. El MVP mueve la cápsula desde código.
5. Compilar y guardar el Blend Space y el AnimBP.
6. En `BP_DuelCharacter > Mesh`, establecer `Animation Mode = Use Animation Blueprint`, `Anim Class = ABP_Duel`. Compilar y guardar.

Esta locomoción inicial muestra avance/idle; las variables heredadas `Direction`, `bInAir`, `Phase` y `StrikeIndex` permiten ampliar después la presentación sin cambiar las reglas.

## 7. Montage de tres golpes

**Hecho — Codex por MCP:** las tres copias de Animation Sequences y Root Motion desactivado. **Hecho — usuario:** creación y asignación de `AM_DuelAttack`, con nueve secciones cuyos nombres confirmó. MCP verificó skeleton, `DefaultSlot`, los tres clips y la referencia en `DA_DuelCombat`. **Pendiente — usuario:** revisar/retirar notifies y comprobar el resultado visual. Las herramientas disponibles no exponen la lectura/edición de Montage Sections ni `Notifies` de las secuencias.

1. **Ya hecho:** `AssetTools.duplicate` copió únicamente las Animation Sequences `MM_Attack_01`, `MM_Attack_02` y `MM_Attack_03`, de `/Game/Characters/Mannequins/Anims/Unarmed/Attack/`, a `/Game/Duel/Animations/AS_DuelAttack1`, `AS_DuelAttack2` y `AS_DuelAttack3`. No volver a duplicarlas. No se copió `AM_ComboAttack`.
2. **Parcial:** `ObjectTools.set_properties` dejó `Enable Root Motion = false` en las tres copias y `AssetTools.save_assets` las guardó. El skeleton verificado es `SK_Mannequin`. Abrir cada copia y retirar cualquier notify de lógica de gameplay que exista; guardar. El daño se resuelve por colisión lógica, nunca desde un notify.
3. Crear `AM_DuelAttack` sobre el mismo skeleton. En el slot `DefaultGroup.DefaultSlot`, colocar las tres secuencias completas una detrás de otra, con Loop Count `1` y Play Rate `1`.
4. Crear nueve secciones. Para cada secuencia, usar como punto de partida el `0%`, `40%` y `60%` de su duración, sumados a la posición donde empieza el segmento:

| Segmento | Secciones, en ese orden |
|---|---|
| AS_DuelAttack1 | `Hit1_Prepare`, `Hit1_Active`, `Hit1_Recovery` |
| AS_DuelAttack2 | `Hit2_Prepare`, `Hit2_Active`, `Hit2_Recovery` |
| AS_DuelAttack3 | `Hit3_Prepare`, `Hit3_Active`, `Hit3_Recovery` |

5. Eliminar la sección `Default` si queda duplicando el inicio. En ejecución, el código configura `Prepare → Active → Recovery → None` para cada golpe antes de avanzar; calcula las velocidades y ordena los saltos según la duración lógica. No enlaza recovery al golpe siguiente. Los `Next Section = None` guardados anteriormente no requieren edición manual para aplicar esta corrección; los enlaces de cada golpe se reemplazan al reproducirlo.
6. Previsualizar y ajustar los cortes 40%/60% al movimiento de cada clip si hace falta. Conservar los nombres exactos de las secciones.
7. Guardar y asignar `DA_DuelCombat > Attack Montage = AM_DuelAttack`. Los nueve nombres ya vienen configurados en `Strikes`.
8. `Guard Montage` y `Hit Montage` son referencias de presentación opcionales. Si se dispone de clips adecuados, crear montages propios en el mismo slot y asignarlos. Sin ellos, HUD y cápsulas de diagnóstico muestran los resultados; no hay animación específica de bloqueo o reacción al daño.

La ventana inicial de parry está dentro de la preparación del primer golpe. Recovery no inmoviliza: aunque el clip continúe, el personaje puede moverse o continuar según sus reglas.

**Diagnóstico del montage:** se comprobaron `Attack Montage`, `Anim Class`, skeleton y Slot correctos. El código anterior cerraba cada sección y, con `Blend Out = 0,25 s`, provocaba un desvanecimiento durante la preparación. La corrección mantiene enlazadas las fases intermedias. Si persiste el problema, revisar `Output Log` filtrando `LogDuelAnimation`: ahora indica una sección faltante o un montage que no pudo iniciarse. No reducir timings de combate para compensar una configuración visual.

## 8. Mapa y modo de juego

**Hecho — Codex por MCP.** `AssetTools.duplicate` creó `L_Duel` desde `/Engine/Maps/Templates/Template_Default`, la plantilla Basic del motor. `SceneTools.load_level` abrió la copia después de guardarla. `ActorTools.set_actor_transform`, `ObjectTools.set_properties` y `ConfigSettingsToolset.SetSectionProperties` ajustaron escenario, GameMode y mapas predeterminados. Todos los assets quedaron guardados.

1. Crear un nivel `Basic` del motor, sin Third Person, y guardarlo como `/Game/Duel/Maps/L_Duel`.
2. Conservar suelo e iluminación del nivel Basic. En `Floor`, establecer Location `(0,0,0)` y Scale `(8,8,8)`: su mesh `SM_Template_Map_Floor` ocupa aproximadamente `8000 × 8000 cm`, con superficie superior en `Z=0` y colisión `QueryAndPhysics / BlockAllDynamic`. Colocar `PlayerStart` en `(-220,0,100)` con yaw `0`.
3. En `World Settings`, asignar `GameMode Override = BP_DuelGameMode`.
4. En `Project Settings > Maps & Modes`, asignar `Default GameMode = BP_DuelGameMode`, `Editor Startup Map = L_Duel` y `Game Default Map = L_Duel`.
5. Guardar el mapa. No colocar personajes del ejemplo ni agregar sus Blueprints de lógica al nivel.

## 9. Revisar el duelo manualmente

**Hecho — Codex por MCP, arranque básico:** `EditorAppToolset.StartPIE` inició `L_Duel` y `SceneTools.find_actors` confirmó dos instancias de `BP_DuelCharacter`. La consulta del log no mostró errores nuevos de arranque; `StopPIE` cerró la sesión. **Pendiente — usuario:** las interacciones siguientes y el ajuste visual, después de conectar las animaciones. No se automatizaron inputs ni se ejecutaron tests.

Después del parche del montage, se consultó la sesión que el usuario ya tenía abierta: había contactos y un tercer golpe acertado, sin advertencias de `LogDuelAnimation`. PIE quedó detenido. Volver a Play y comprobar visualmente preparación, golpe y retorno; no hace falta cambiar las referencias ni los nombres de sección para aplicar el parche.

1. Abrir `L_Duel` y comenzar PIE con un jugador. Deben aparecer dos personajes, dos barras de vida, las fases y el modo de IA. Confirmar primero que WASD, mouse, Tab, R y F1 responden.
2. En modo `Passive`, probar tres pulsaciones, dejar vencer una cadena y continuar al aire. El tercer golpe cierra en NA; moverse durante recovery corta la cadena y conserva ese NA.
3. En `Combo`, recibir un golpe y responder con movimiento o una pulsación defensiva al abrir la oportunidad. Verificar que no hay defensa durante NA.
4. En `Reactive`, observar parry/bloqueos y la diferencia de recuperación. La IA puede continuar después de recibir un parry; cada bloqueo posterior necesita otra pulsación.
5. En `Sidestep`, comprobar que los golpes segundo y tercero conservan la dirección inicial aunque siga activo el lock-on.
6. Observar `NA` y `Pause` en el HUD al recibir golpes bajo NA. `R` debe limpiar ambos y restaurar vida y posiciones. Los textos del juego están en inglés: `PLAYER`, `OPPONENT`, `VICTORY` y `DEFEAT`.
7. Llevar una vida a cero y verificar victoria/derrota y reinicio. Revisar los cortes del montage en movimiento y guardar los ajustes visuales.

La política vigente excluye tests automatizados. Esta revisión manual tampoco valida el futuro modo online.
