# 063 — BlendSpace configurable y ABP compartido

## Resultado

Cada `UVampsCombatSettings` tiene ahora `LocomotionBlendSpace`. `UVampsAnimInstance` lo toma del DA del personaje al inicializarse y `/Game/Vamps/Animations/ABP_VampsCharacter` lo usa como entrada de su BlendSpace Player. Jugador y bots usan ese mismo ABP, manteniendo `DefaultSlot` para los montages.

`DA_ExecutionerCombatSettings` conserva `/Game/Vamps/Animations/Executioner/BS_Executioner.BS_Executioner`. En `DA_DualSwordsCombatSettings`, `DA_SwordAndShieldCombatSettings` y `DA_LancerCombatSettings` el campo queda en `None`, para que el usuario cree y asigne sus BS. Los cuatro DAs están en `/Game/Vamps/Data/`; todos sus valores anteriores se conservaron exactamente.

## Código y conexiones

| Archivo o asset | Cambio y relación |
|---|---|
| `Source/vamps/Public/Combat/Data/VampsCombatSettings.h` | Agrega referencia editable `UBlendSpace` en la categoría Animation. Mantiene intactos los structs y las propiedades anteriores. |
| `Source/vamps/Public/Animation/VampsAnimInstance.h` y `Private/Animation/VampsAnimInstance.cpp` | Agregan la referencia transitoria visible al ABP y `NativeInitializeAnimation`. Lee `AVampsCharacter::CombatSettings` al inicializar la animación, limpia la referencia anterior y diagnostica personaje/configuración/BS requeridos ausentes en juego. Los mundos de previsualización y la ausencia inicial de pawn no ejecutan esa configuración. |
| `/Game/Vamps/Animations/ABP_VampsCharacter` | Expone el pin `BlendSpace` y lo conecta a `Get Locomotion Blend Space`. Retira el asset fijo del nodo. Mantiene `Speed → X`, `Y=0`, reproducción a 1× en loop y `BlendSpace Player → DefaultSlot → OutputPose`. |
| `/Game/Vamps/Blueprints/BP_VampsCharacter_Player` | Cambia Anim Class de `ABP_Executioner` a `ABP_VampsCharacter`. Conserva mesh y DA de Executioner. |
| `/Game/Vamps/Blueprints/BP_VampsCharacter` | Conserva el ABP común; se compiló y guardó con las referencias actualizadas. |
| Los cuatro DAs de `/Game/Vamps/Data/` | Se guardaron con el nuevo campo; solo Executioner tiene un BS asignado. |

`BS_Executioner` conserva sus muestras originales: idle a 0, walk a 180, run a 420 y run a 700 con rate scale 1,666667; eje X `Speed`, muestras en Y=0. No se modificó ese asset. La lectura del DA se realiza al inicializar/reinicializar la instancia, no en cada frame. No hay un BS alternativo cuando falta la configuración.

No se modificaron montages, GameModes, mapas, lógica de combate ni los paquetes importados por el usuario. El asset `ABP_Executioner` permanece en el proyecto; los dos Blueprints activos usan el compartido.

## Preservación y verificación

- Antes de editar/compilar se descubrieron por MCP `VampsCombatSettings` y sus derivadas: había cuatro assets. Se leyeron todas sus propiedades y se guardaron en disco. [Captura completa](antes.json) y [gráficos/asignaciones previos](graficos-antes.json).
- Se aplicó clang-format a los tres archivos C++ modificados y se revisaron nombres contra la jerarquía nativa. `git diff --check` pasó.
- `LiveCodingToolset.CompileLiveCoding` devolvió `Success` y aplicó `UnrealEditor-vamps.patch_0`, con recarga completa de las dos clases. Fue un parche real, no `NoChanges`: [resultado](live-coding.json). La relectura mostró clases nativas y todos los valores previos conservados; no se observaron referencias transitorias ni errores de incompatibilidad estructural en esas lecturas. El log incluyó el aviso general de Live Coding sobre empaquetar tipos modificados.
- El ABP común y ambos Blueprints de personaje compilaron por MCP con `warnings_as_errors=true`. Se guardaron únicamente esos tres Blueprints y los cuatro DAs afectados.
- La [lectura posterior al guardado](despues.json) confirma preservación de todas las propiedades previas, BS de Executioner asignado, los otros tres vacíos y ausencia de cambios pendientes en los DAs y el ABP. Las dependencias del ABP ya no incluyen un BS concreto; el gráfico contiene la conexión al campo de la instancia.
- PIE en `L_LocalMatch` arrancó con cuatro personajes, todos con el DA de Executioner y `ABP_VampsCharacter`, sin diagnósticos de inicialización de locomoción: [lectura](pie-partida.json). También arrancó PIE en `L_CharacterBasic`.
- `ObjectTools` no expone la instancia interna `AnimScriptInstance` del mesh y rechazó su lectura explícita; el catálogo de `SkeletalMeshTools` tampoco ofrece esa consulta. [Límite](limite-lectura-instancia.json). Se verificaron las asignaciones y el gráfico compilado, pero no se afirma una relectura del BS interno ni una comprobación visual de caminar/correr.

Se detuvieron ambas sesiones PIE y se volvió a `L_LocalMatch`. No se agregaron ni ejecutaron tests, ni se automatizaron inputs. Pasos reproducibles y pendientes visuales en [editor.md](editor.md).

## Continuación

El usuario creará y asignará los BS de los tres perfiles nuevos, junto con sus montages y armas pendientes de [062](../062-data-assets-personajes/reporte.md). La creación de esos recursos no forma parte de esta entrega. El pendiente de `End Recovery` de [061](../061-fin-recovery-notify/editor.md) continúa vigente.
