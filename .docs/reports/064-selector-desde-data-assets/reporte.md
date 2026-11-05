# 064 — Selección desde Data Assets y reparto 2vs2

Continuación: [065 — Combos sin límite fijo de secciones](../065-combos-sin-limite-fijo/reporte.md), con [configuración del montage y pasos por DA](../065-combos-sin-limite-fijo/editor.md). No modifica el catálogo ni sus asignaciones; permite que cada perfil use N golpes según su montage.

## Resultado

El selector genera una opción por cada DA de `AvailableCharacters` del GameMode. Cada `UVampsCombatSettings` incorpora únicamente `CharacterName`, de tipo `FText`; no se agregan retratos ni una clase de definición nueva. La selección guarda la referencia al DA en `SelectedCharacter`.

START asigna el DA elegido al jugador y sortea los otros tres sin reemplazo para los bots. Con los cuatro assets configurados, cada ronda usa los cuatro perfiles una sola vez: jugador y aliado en A contra dos bots en B. La revancha conserva la selección y vuelve a crear los cuatro participantes; el respawn normal conserva su actor y DA.

El usuario avanzó la configuración de los perfiles; sus valores actuales están en [muerte-compartida-antes.json](muerte-compartida-antes.json). Sword and Shield se muestra ahora como Warden y Lancer como Impaler. Ambos tienen el mismo montage asignado a muerte normal y bloqueada. La validación permite compartirlo entre esos dos campos y conserva los demás requisitos del montage. No se ha verificado el inicio ni una ronda completa con la configuración actual.

## Código y relaciones

| Archivos | Responsabilidad y cambio |
|---|---|
| `Source/vamps/Public/Combat/Data/VampsCombatSettings.h` | Agrega `CharacterName` editable. Conserva las propiedades y estructuras de combate existentes. |
| `Source/vamps/Private/Combat/Data/VampsCombatSettings.cpp` | `HasValidDeathMontages` permite que `DeathMontage` y `GuardDeathMontage` referencien el mismo asset. Mantiene presencia, duración y rate positivos, Auto Blend Out desactivado y separación respecto de ataque, reacciones y dash. |
| `Source/vamps/Public/GameModes/VampsMatchGameMode.h` y `Private/GameModes/VampsMatchGameMode.cpp` | Sustituyen el bool y la función exclusivos de Executioner por catálogo, selección y validación de referencias únicas con nombres no vacíos. El arranque abre selección sin combatientes. START valida los recursos, reparte cuatro DAs distintos y crea los participantes. |
| `Source/vamps/Public/UI/VampsHUDWidget.h` y `Private/UI/VampsHUDWidget.cpp` | Crean una opción por DA en `CharacterOptions`, usando `CharacterOptionWidgetClass`. START depende de `SelectedCharacter`; desaparece el botón fijo de Executioner. |
| Nuevos `Source/vamps/Public/UI/VampsCharacterOptionWidget.h` y `Private/UI/VampsCharacterOptionWidget.cpp` | Muestran `CharacterName`, entregan el DA al GameMode mediante `SelectCharacter` y resaltan la opción seleccionada. El diseño permanece en un Widget Blueprint. |
| `Source/vamps/Private/Player/VampsPlayerController.cpp` | `MobilityPressed` no solicita movilidad cuando el controller todavía no tiene pawn durante la selección. Conserva el diagnóstico si existe un pawn inválido o falta configuración de entrada. |

El GameMode conserva `BP_VampsCharacter_Player` para el jugador y `BP_VampsCharacter` para los bots. Crea cada actor mediante spawn diferido, asigna `CombatSettings` antes de `FinishSpawning` y reinicializa la animación para que el ABP compartido lea ese DA. Cancela y limpia la creación si falla la inicialización o posesión. No se modifica lógica de Third Person ni sus variantes.

## Assets

Los cuatro DAs conservan la clase nativa `VampsCombatSettings` y sus valores anteriores:

| Asset | Nombre / configuración aplicada |
|---|---|
| `/Game/Vamps/Data/DA_ExecutionerCombatSettings` | `CharacterName = Executioner`. |
| `/Game/Vamps/Data/DA_DualSwordsCombatSettings` | `CharacterName = Dual Swords`. |
| `/Game/Vamps/Data/DA_SwordAndShieldCombatSettings` | Nombre inicial `Sword and Shield`; el usuario lo cambió a `Warden`. Comparte `AM_Warden_HitDeath` entre muerte normal y bloqueada. |
| `/Game/Vamps/Data/DA_LancerCombatSettings` | Nombre inicial `Lancer`; el usuario lo cambió a `Impaler`. Comparte `AM_Impaler_HitDeath` entre muerte normal y bloqueada. |
| `/Game/Vamps/Blueprints/BP_VampsMatchGameMode` | `AvailableCharacters` en el orden de la tabla; `Start Players as Spectators = true`. Conserva clases, HUD, puntuación y duraciones. |
| `/Game/Vamps/UI/WBP_VampsHUD` | Padre `VampsHUDWidget`; sustituye el botón fijo por `CharacterOptions`, amplía el panel y referencia el nuevo widget de opción. |
| Nuevo `/Game/Vamps/UI/WBP_VampsCharacterOption` | Padre `VampsCharacterOptionWidget`; `OptionSize` de 320×48 contiene `CharacterButton` y `CharacterLabel`, con Roboto Regular 10 y el estilo del selector. |

Codex no modificó mapas, configuración de arranque, Blueprints de personajes, armas, montages ni BS. Las conexiones y jerarquías del selector constan en [despues.json](despues.json). El ajuste de muerte compartida conserva todas las asignaciones actuales del usuario, incluida su nueva arma de lanza; no configura recursos adicionales.

Se agregan este reporte y su guía al índice, con enlaces de continuación desde los reportes 055, 058, 062 y 063. Las especificaciones funcionales y técnicas existentes quedan fuera de esta actualización.

## Preservación y verificación

- Antes de editar o compilar se descubrieron por MCP los assets de la clase y sus derivadas. Se capturaron las 29 propiedades previas de los cuatro DAs, incluidos arrays completos, referencias y transforms, y se guardaron los cuatro assets: [antes.json](antes.json).
- Se aplicó clang-format a los nueve archivos C++ y se revisaron nombres y jerarquías nativas. `git diff --check` pasó.
- `LiveCodingToolset.LiveCodingToolset.CompileLiveCoding` devolvió `Success` con enlace y aplicación de un parche real; no fue `NoChanges`. Recargó las clases modificadas e incorporó la clase de opción. El log incluyó el aviso general sobre tipos modificados y empaquetado; no se realizó una compilación de empaquetado: [resultado](live-coding.json).
- El ajuste final de `MobilityPressed` también compiló con `Success`, enlace y aplicación de un parche real; su salida no incluyó el aviso de tipos: [compilación final](live-coding-final.json). No cambia reflexión ni requiere volver a guardar los assets.
- La relectura posterior confirmó todas las propiedades anteriores conservadas y los DAs asociados a `/Script/vamps.VampsCombatSettings`: [tras-compilar.json](tras-compilar.json). No fue necesario reconstruir valores perdidos.
- Ambos Widget Blueprints y el GameMode compilaron por MCP con `warnings_as_errors=true`. Se guardaron únicamente los siete assets afectados. La [relectura guardada](guardado.json) confirma nombres, preservación y `dirty=false`; las [dependencias](dependencias.json), consultadas con PIE detenido, usan rutas estables sin referencias transitorias.
- PIE en `L_LocalMatch` confirmó `Selecting`, cuatro DAs disponibles, `SelectedCharacter=None` y ausencia de participantes antes de START: [pie.json](pie.json). `ObjectTools` no permite leer el campo transitorio de la instancia del HUD; la captura disponible mostró el Designer y no acredita la presentación del selector durante PIE.
- Antes del ajuste de muerte compartida se volvieron a descubrir por MCP la clase y sus assets: cuatro DAs y ninguna derivada. Se capturaron sus 30 propiedades actuales y se guardaron los cuatro assets en disco: [muerte-compartida-antes.json](muerte-compartida-antes.json). El ajuste compiló con `Success`, enlace y aplicación de un parche real: [compilación](muerte-compartida-live-coding.json). La [relectura posterior](muerte-compartida-despues.json) confirmó las 30 propiedades idénticas en cada DA, guardado correcto y `dirty=false` en los cuatro; no fue necesario restaurar ni cambiar su configuración. No se inició PIE ni se activó START para esta comprobación.

No se automatizaron inputs ni se agregaron o ejecutaron tests. Queda comprobar START y las muertes normal y bloqueada con el montage compartido, según [editor.md](editor.md).
