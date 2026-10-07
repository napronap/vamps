# MVP del duelo local

La base vigente es [004 — Un ataque y una guardia](../004-hit-y-guard/reporte.md), con su [guía del editor](../004-hit-y-guard/editor.md). Este documento conserva el estado de una iteración anterior.

Informe de la iteración anterior. Para el código vigente y las operaciones nuevas, consultar [003 — Combate animado](../003-combate-animado/reporte.md) y su [guía de editor](../003-combate-animado/editor.md). No repetir los pasos históricos que contradigan esa guía.

**Fecha:** 2026-10-01. **Alcance:** jugador contra IA, sin dash, stances, RPS ni implementación online.

## Código

Los headers están en `Source/vamps/Public/` y los `.cpp` en `Source/vamps/Private/`, con carpetas paralelas por responsabilidad. Las rutas de la tabla son relativas a esos dos árboles. Se usan clases nativas de Unreal como base, sin lógica ni Blueprints del ejemplo Third Person.

| Archivo / clase | Responsabilidad e interacción |
|---|---|
| `Combat/Data/CombatConfig.h/.cpp` — `UCombatConfig` | Data Asset compartido: tres golpes, tiempos, alcance, daño, cobertura, movimiento y montages. Lo leen personajes y habilidades. |
| `Combat/CombatRules.h/.cpp` | Buffer de una pulsación con intención y vencimiento; reloj de NA; cobertura angular. Funciones y datos usados por las habilidades, sin estado de combate paralelo. |
| `Combat/CombatTags.h/.cpp` | Tags nativos de habilidades activas y magnitud de daño. |
| `Combat/Attributes/VampsAttributeSet.h/.cpp` — `UVampsAttributeSet` | Única fuente de `Health` y `MaxHealth`; límites y notificación de replicación. |
| `Combat/Effects/GE_DuelDamage.h/.cpp` — `UGE_DuelDamage` | Gameplay Effect instantáneo; aplica a Health la magnitud negativa enviada por el atacante. |
| `Combat/Abilities/DuelGameplayAbility.h/.cpp` — `UDuelGameplayAbility` | Base GAS por personaje: activación, commit y finalización común. |
| `Combat/Abilities/GA_Attack.h/.cpp` — `UGA_Attack` | Fases, tres golpes, trayectoria fija, acercamiento inicial, contactos ya consumidos y coordinación del montage. Salir inicia NA ofensivo. |
| `Combat/Abilities/GA_Guard.h/.cpp` — `UGA_Guard` | Cobertura consumible por pulsación, continuación y salida a NA defensivo. |
| `Combat/Abilities/GA_NA.h/.cpp` — `UGA_NA` | Conserva cooldown restante y pausa reemplazable; no bloquea el movimiento. |
| `Combat/Abilities/GA_HitReaction.h/.cpp` — `UGA_HitReaction` | Reacción al daño y apertura posterior para movimiento o guardia, respetando NA. |
| `Characters/VampsDuelCharacter.h/.cpp` — `AVampsDuelCharacter` | Aloja ASC, atributos y habilidades; interpreta solicitudes, resuelve defensa/daño, mueve con colisión, mantiene objetivo y presenta resultados. |
| `Combat/DuelCombatSubsystem.h/.cpp` — `UDuelCombatSubsystem` | Reloj local común, visita límites de fase, consulta contactos y los resuelve en orden. No duplica el estado de las habilidades. |
| `AI/DuelAIController.h/.cpp` — `ADuelAIController` | Cinco modos reproducibles; solicita movimiento y pulsaciones al mismo personaje que usa el jugador. |
| `Player/DuelPlayerController.h/.cpp` — `ADuelPlayerController` | Enhanced Input, cámara libre/lock-on, cambio de IA y reinicio. Recibe acciones y mapping propios por BP. |
| `GameModes/DuelGameMode.h/.cpp` — `ADuelGameMode` | Crea el rival, comparte configuración y reinicia ambos combatientes. Selecciona personaje, controller y HUD propios. |
| `UI/DuelHUD.h/.cpp` — `ADuelHUD` | HUD nativo sencillo: vida, fases, combo, NA/pausa, resultado, modo IA y controles. No necesita Widget Blueprint. |
| `Animation/DuelAnimInstance.h/.cpp` — `UDuelAnimInstance` | Datos de presentación para el AnimBP propio: velocidad, dirección, caída, fase e índice de golpe. |

`Source/vamps/vamps.Build.cs` incorpora `GameplayAbilities`, `GameplayTags` y `GameplayTasks`. MCP confirmó que `GameplayAbilities` estaba activo y aceptó `SetPluginEnabled`, pero la revisión del archivo mostró que la declaración explícita no se persistió en `vamps.uproject`; queda indicada en la guía.

## Flujo y assets

Controller o IA → solicitud al personaje → habilidad GAS → consulta de contacto en el subsistema → defensa o Gameplay Effect → atributos y presentación. El montage sigue la fase lógica; sus notifies no deciden daño ni ventanas. BP y Data Assets asignan recursos y configuración.

La [guía del editor](editor.md) registra los assets, sus conexiones y el estado de configuración.

## Configuración completada por MCP

La configuración inicial por MCP no cambió C++ ni agregó clases. Se completaron las asignaciones y se agregaron once assets a los cuatro existentes:

| Asset / archivo | Cambio y conexión |
|---|---|
| `/Game/Duel/Input/IA_DuelMove`, `IA_DuelLook` | Input Actions Axis2D asignadas al controller propio. |
| `/Game/Duel/Input/IA_DuelShared`, `IA_DuelLock`, `IA_DuelReset`, `IA_DuelAIMode` | Input Actions Boolean para pulsación compartida, lock-on, reinicio y modo de IA. |
| `/Game/Duel/Input/IMC_Duel` | Nueve mappings: WASD, Mouse2D, click izquierdo, Tab, R y F1. Negate/Swizzle instanciados para A/W/S. Asignado a `BP_DuelPlayerController`. |
| `/Game/Duel/Blueprints/BP_DuelCharacter` | `DA_DuelCombat` y mesh `SKM_Manny_Simple` asignados. Conserva `DuelAIController`, offset y rotación nativos. |
| `/Game/Duel/Blueprints/BP_DuelGameMode` | Jugador y rival usan `BP_DuelCharacter`; controller propio, HUD nativo y configuración compartida asignados. |
| `/Game/Duel/Animations/AS_DuelAttack1`, `AS_DuelAttack2`, `AS_DuelAttack3` | Copias de las tres secuencias visuales de ataque; Root Motion desactivado, skeleton `SK_Mannequin`. Pendiente revisar notifies y conectarlas al montage propio. |
| `/Game/Duel/Maps/L_Duel` | Copia de Basic del motor, sin código/BP del ejemplo. Suelo de aproximadamente 8000 × 8000 cm en Z=0, iluminación y PlayerStart configurados; GameMode propio. |
| `Config/DefaultEngine.ini` | MCP asignó `L_Duel` como mapa inicial/de juego y `BP_DuelGameMode` como modo predeterminado. |

Los tres Blueprints compilaron. Se guardaron los quince assets del duelo y se confirmó que no quedaban cambios sin guardar. Las dependencias del personaje y del mapping apuntan a clases/recursos propios y al mesh visual permitido. Las secuencias conservan referencias de edición a sus Level Sequences visuales originales; no se copió lógica de combate del ejemplo.

## Corrección de reproducción del montage

- El usuario creó `ABP_Duel`, `BS_Duel` y `AM_DuelAttack`. MCP confirmó el AnimBP asignado al mesh, su padre `DuelAnimInstance`, `Speed → BS_Duel → DefaultSlot → Output Pose`, skeleton compatible y el montage asignado en `DA_DuelCombat`. El usuario confirmó los nueve nombres de sección; la herramienta no permite leerlos.
- Se encontró un fallo en `AVampsDuelCharacter::PlayAttackSection`: asignaba `Next Section = None` a cada fase. UE inicia el blend out al acercarse al final de una sección terminal. Con los 0,25 s de Blend Out del asset, comenzaba durante la preparación y podía ocultar el golpe.
- `Private/Combat/Abilities/GA_Attack.cpp` ahora pasa la definición completa del golpe. `Public/Characters/VampsDuelCharacter.h` y su implementación en `Private/Characters/` actualizan la firma y enlazan `Prepare → Active → Recovery → None` antes de avanzar. GAS conserva tiempos, daño y la decisión de continuar el combo; los enlaces solo evitan el desvanecimiento intermedio.
- `LogDuelAnimation` informa cuando falta una sección o `Montage_Play` rechaza la reproducción. No se agregaron clases ni se modificaron assets del usuario para esta corrección.
- Live Coding compiló y aplicó un parche real (`Success`, recarga completada y 68 funciones remapeadas). La petición HTTP agotó su espera, pero el log del editor confirmó el resultado posterior; no se repitió la compilación.
- En la sesión PIE que ya estaba abierta se leyeron contactos y un `Strike 3: Hit`, sin advertencias de `LogDuelAnimation`. Se detuvo PIE. Esto verifica ejecución del combate, no la imagen del montage; la confirmación visual queda al usuario. No se ejecutaron tests ni se automatizaron inputs.

## Validación

| Verificación | Resultado |
|---|---|
| Compilación por Live Coding | Primera versión: `Success` y parche `UnrealEditor-vamps.patch_0`. Después de la reorganización y el reinicio: `NoChanges`, sin código pendiente ni parche nuevo. Codex no ejecutó un rebuild convencional. |
| Clases y assets | Tres BP compilados y asignaciones verificadas mediante lectura de propiedades. Inputs, modificadores, mesh, GameMode y mapas guardados. |
| Arranque en PIE | `L_Duel` inició con dos instancias de `BP_DuelCharacter`; no aparecieron errores nuevos de arranque en la consulta del log. Se detuvo PIE. No se automatizaron inputs ni se verificó el combate completo. |
| Tests retirados | Se eliminó `DuelCombatTests.cpp` con sus seis pruebas y el mundo transitorio que cerró el editor. No se reemplazan ni se ejecutan otros tests. |
| Revisión estática | Enlaces locales válidos; el código nuevo no incluye clases, rutas de Blueprints ni lógica de Third Person o sus variantes. |

El usuario restableció MCP. Las operaciones del editor se realizaron mediante herramientas MCP directas, algunas agrupadas con `ProgrammaticToolset`; no se automatizó la UI. Al duplicar las secuencias, el motor emitió advertencias de carga de `AssetImportData`; el guardado y la lectura posterior de sus propiedades completaron correctamente.

## Organización y estilo

- Se movieron los 17 pares `.h/.cpp` a `Public/` y `Private/`, distribuidos en `Combat`, `Combat/Abilities`, `Combat/Attributes`, `Combat/Effects`, `Combat/Data`, `Characters`, `AI`, `Player`, `GameModes`, `Animation` y `UI`. Se actualizaron todos los includes propios.
- Las clases, propiedades reflejadas e identificadores `/Script/vamps.*` se conservaron. El cambio de carpetas no requirió recrear assets; posteriormente se completaron las asignaciones de los BP por MCP.
- HUD, mensajes y comentarios de código están en inglés. Se expandieron bloques y funciones, se separaron macros de sus declaraciones y se mejoraron nombres locales. No se cambiaron las reglas del combate en esta reorganización.
- `.clang-format` y `.editorconfig` establecen tabs de cuatro columnas, llaves en líneas separadas y límite orientativo de 120 columnas. El formato se aplicó con clang-format 19.1.5 a los 34 archivos propios.
- `AGENTS.md` fija organización, idioma y prohibición de agregar o ejecutar tests. [Convenciones C++](../../cpp-style.md) explica el criterio. El índice y las convenciones de documentación enlazan y respetan estas reglas.
- La revisión de archivos comprobó que no quedan includes a `Duel/` ni registros de tests en el código propio. Se compararon los tokens con la versión previa considerando traducciones, nombres locales, includes y llaves añadidas por el formateador; no aparecieron otros cambios de código.
- Tras recuperar MCP, Live Coding respondió `NoChanges` y el nivel inició en PIE. No se creó ni ejecutó una suite de tests.

## Configuración pendiente

- Persistir la dependencia explícita `GameplayAbilities` en `vamps.uproject`: el plugin está activo, pero la herramienta no dejó la entrada guardada.
- Revisar notifies de las copias de ataque; la lectura/edición de esos eventos no está cubierta por las herramientas disponibles.
- Comprobar visualmente la corrección del montage y revisar manualmente el duelo en PIE. **Las animaciones están conectadas; la validación visual y jugable sigue pendiente.**

Este reporte y su guía consolidan los cambios del MVP y sus pendientes. Las reglas y la arquitectura siguen documentadas en los pares funcional/técnico de combate y juego.

## Límites

La ejecución del combate es local y autorizada en el mundo del juego. La infraestructura básica de GAS/atributos no equivale a implementar predicción, RPC, historial o validación online.

La geometría se consulta con la posición disponible del frame; se subdivide el tiempo a un máximo de 1/120 s y en límites de fases. No se reconstruyen posiciones históricas durante hitches. Contactos exactamente empatados se ordenan por identificador de acción, sin ventana especial de simultaneidad.
