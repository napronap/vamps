# Selector desde Data Assets — operaciones del editor

**Queda para el usuario:** comprobar START y las muertes normal y bloqueada de Warden o Impaler usando su montage compartido. El usuario avanzó la configuración de los perfiles; [la captura actual](muerte-compartida-antes.json) documenta sus asignaciones. Catálogo, widgets y ajuste de muerte compartida están compilados; las 30 propiedades actuales de cada DA se conservaron y los cuatro assets están guardados. No se ha verificado una ronda con la configuración actual.

## 1. Preservar los Data Assets y compilar

**Hecho — Codex por MCP.** Con PIE detenido se descubrieron la clase `VampsCombatSettings`, sus derivadas y sus cuatro assets mediante `ObjectTools.search_subclasses` y `AssetTools.find_assets`. `ObjectTools.list_properties/get_properties` leyó todas las propiedades previas; [antes.json](antes.json) conserva referencias, escalares, arrays y transforms. `AssetTools.save_assets` guardó los cuatro DAs antes del cambio de clase.

Para reproducir la revisión, abrir los cuatro DAs de `/Game/Vamps/Data/` enumerados en [el reporte](reporte.md#assets) y comparar sus grupos de propiedades con la captura antes de modificar la clase. No sustituir esos valores por defaults.

**Hecho — Codex por MCP.** `LiveCodingToolset.LiveCodingToolset.CompileLiveCoding` compiló y aplicó un parche real con resultado `Success`. La relectura de los cuatro DAs confirmó la clase nativa y preservación exacta de los valores previos: [tras-compilar.json](tras-compilar.json). No se necesitó restaurar propiedades. El aviso general de Live Coding sobre empaquetado no constituye una comprobación de build empaquetado.

**Hecho — Codex por MCP.** La [compilación final](live-coding-final.json) de `MobilityPressed`, que ignora la entrada de movilidad mientras no existe pawn durante la selección, también devolvió `Success` con enlace y aplicación de un parche real. Su salida no incluyó el aviso de tipos. Este ajuste no cambia clases reflejadas ni requiere guardar nuevamente los assets.

## 2. Asignar los nombres

**Hecho — Codex por MCP.** `ObjectTools.set_properties` configuró `Character Name` en cada DA de `/Game/Vamps/Data/`:

| DA | Character Name aplicado inicialmente |
|---|---|
| `DA_ExecutionerCombatSettings` | `Executioner` |
| `DA_DualSwordsCombatSettings` | `Dual Swords` |
| `DA_SwordAndShieldCombatSettings` | `Sword and Shield` |
| `DA_LancerCombatSettings` | `Lancer` |

Reproducción: abrir cada asset desde Content Browser, editar el campo en Details y guardar. Mantener todos sus demás valores; no hay retratos ni propiedades nuevas de recursos visuales.

**Hecho — usuario.** Actualizó recursos y cambió `Character Name` de `DA_SwordAndShieldCombatSettings` a `Warden` y de `DA_LancerCombatSettings` a `Impaler`. La captura actual conserva estos valores; no volver a aplicar los nombres iniciales de la tabla.

## 3. Configurar el catálogo del GameMode

**Hecho — Codex por MCP.** En `/Game/Vamps/Blueprints/BP_VampsMatchGameMode`, padre `VampsMatchGameMode`, se configuraron mediante `ObjectTools.set_properties`:

1. Class Defaults → Match → `Available Characters`: cuatro entradas, en el orden de la tabla anterior y sin duplicados.
2. `Start Players as Spectators = true`, para abrir el selector sin crear un combatiente antes de elegir.
3. Se conservaron Default Pawn Class `BP_VampsCharacter_Player`, Bot Character Class `BP_VampsCharacter`, Player Controller Class `BP_VampsMatchPlayerController` y HUD Class `BP_VampsHUD`. También se conservaron puntuación y duraciones.

El catálogo exige al menos cuatro referencias distintas con nombres no vacíos. Con estas cuatro entradas, el jugador recibe su elección y los tres bots reciben los DAs restantes en orden aleatorio. No hay que asignar cada DA a un Blueprint diferente.

## 4. Crear la opción y adaptar el selector

**Hecho — Codex por MCP.** Herramientas directas de `UMGToolSet` crearon y modificaron los árboles; `ObjectTools.list_properties/get_properties/set_properties` descubrió, configuró y releyó controles y slots.

En Content Browser → `/Game/Vamps/UI`:

1. `WBP_VampsCharacterOption`, padre `VampsCharacterOptionWidget`: raíz `SizeBox` llamada `OptionSize`, Width Override 320 y Height Override 48. Contiene `CharacterButton` (`Button`) y, dentro, `CharacterLabel` (`TextBlock`). El texto usa Roboto Regular 10; conservar el estilo del selector. El nombre proviene del DA y el clic se conecta en C++.
2. Abrir `WBP_VampsHUD`, padre `VampsHUDWidget`. En Designer → `SelectionPanel`, retirar `SelectButton` y sus hijos `SelectButtonContent`/`SelectedText`; agregar un `VerticalBox` llamado `CharacterOptions`, vacío en el Designer. Canvas Slot: posición (40,65), tamaño 320×224.
3. Mantener `SelectionPanel` centrado con tamaño 400×390 y posición (-200,-195). Colocar `StartButton` en (40,310), tamaño 320×48. Cada opción generada ocupa 48 px de alto con 8 px de separación.
4. En Class Defaults → UI del HUD, asignar `Character Option Widget Class = WBP_VampsCharacterOption_C`. Conservar `Enemy Widget Class = WBP_VampsEnemy_C` y `Fighter Widget Class = WBP_VampsFighter_C`.

Conservar los nombres y tipos de los controles requeridos por `BindWidget`; no se necesita lógica en el Graph. [Árboles y referencias verificados](despues.json).

## 5. Compilar, guardar y comprobar el arranque

**Hecho — Codex por MCP.** `BlueprintTools.compile_blueprint`, con `warnings_as_errors=true`, compiló `WBP_VampsCharacterOption`, `WBP_VampsHUD` y `BP_VampsMatchGameMode`. `AssetTools.save_assets` guardó esos tres assets y los cuatro DAs; la relectura confirmó únicamente el nombre nuevo en los DAs y `dirty=false` para los siete: [guardado.json](guardado.json). Las [dependencias guardadas](dependencias.json) se consultaron con PIE detenido y conservaron referencias estables.

Reproducción: Compile en cada Blueprint y guardar exclusivamente los siete assets afectados. No se modifican mapas ni Project Settings.

**Hecho — Codex por MCP, parcial.** `EditorAppToolset.StartPIE`, `SceneTools.find_actors` y `ObjectTools.get_properties` confirmaron en `/Game/Vamps/Maps/L_LocalMatch` la fase `Selecting`, el catálogo de cuatro DAs, selección vacía y ningún combatiente creado todavía. La instancia transitoria del HUD no es legible mediante `ObjectTools`. La captura obtenida mostró el Designer; no acredita la apariencia ni los clics del selector en juego.

## 6. Permitir el montage de muerte compartido

**Hecho — Codex por MCP.** Antes de modificar `HasValidDeathMontages`, se descubrieron de nuevo cuatro DAs de la clase y ninguna derivada; `ObjectTools.list_properties/get_properties` capturó sus 30 propiedades actuales y `AssetTools.save_assets` guardó los cuatro assets: [muerte-compartida-antes.json](muerte-compartida-antes.json).

**Hecho — usuario.** En `/Game/Vamps/Data/DA_SwordAndShieldCombatSettings`, `Death Montage` y `Guard Death Montage` comparten `/Game/Vamps/Animations/Warden/AM_Warden_HitDeath`. En `DA_LancerCombatSettings`, ambos campos comparten `/Game/Vamps/Animations/Impaler/AM_Impaler_HitDeath`. Codex conserva estas asignaciones y el resto de su configuración.

**Hecho — Codex por MCP.** `LiveCodingToolset.LiveCodingToolset.CompileLiveCoding` compiló el cambio con `Success`, enlace y aplicación de un parche real: [resultado](muerte-compartida-live-coding.json). Se elimina únicamente la exigencia de que esos dos campos sean distintos. Se mantienen presencia, duración y rate positivos, Auto Blend Out desactivado y separación de los montages de ataque, reacciones y dash. La [relectura posterior](muerte-compartida-despues.json) confirmó las 30 propiedades de cada DA idénticas a la captura previa, guardado correcto y `dirty=false` en los cuatro. No se restauraron ni cambiaron asignaciones; no se inició PIE ni se activó START para esta comprobación.

**Pendiente — usuario.** Abrir `/Game/Vamps/Maps/L_LocalMatch` y Play:

1. Elegir un personaje y pulsar START. Comprobar que no se rechaza el inicio por compartir los dos montages de muerte. Si aparece otro diagnóstico de `LogVampsCombat`, registrar el DA y el requisito concreto; esta corrección no acredita el resto de la configuración.
2. Observar una muerte normal y otra por daño bloqueado de Warden o Impaler. Ambas deben reproducir el montage compartido asignado al DA y completar el flujo de muerte habitual.

Estas interacciones se dejan al usuario porque las herramientas directas disponibles no activan las acciones de juego. No se automatizan clicks ni teclas del editor.
