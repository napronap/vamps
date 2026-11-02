# 063 — Operaciones de locomoción configurable

**Queda para el usuario:** comprobar visualmente idle/caminar/correr de Executioner y crear/asignar los BS de los otros tres personajes. El campo, el código, el ABP compartido y las asignaciones existentes ya están compilados y guardados. Los montages, armas y marcadores pendientes siguen en [062](../062-data-assets-personajes/editor.md) y [061](../061-fin-recovery-notify/editor.md).

## 1. Preservar los cuatro DAs

**Hecho — Codex por MCP.** Con PIE detenido, `ObjectTools.search_subclasses`, `AssetTools.find_assets` y `ObjectTools.list_properties/get_properties` localizaron los cuatro DAs de `/Game/Vamps/Data/` y leyeron todos sus valores, arrays, referencias y transform del arma. Se guardó [antes.json](antes.json) y `AssetTools.save_assets` confirmó su guardado en disco antes de cambiar la clase.

## 2. Compilar el código

**Hecho — Codex por MCP.** `LiveCodingToolset.LiveCodingToolset.CompileLiveCoding` devolvió `Success` y aplicó un parche real. Tras completarse la recarga se releyeron los cuatro DAs: las propiedades previas coincidían con la captura, sin necesidad de restaurarlas. Se confirmó el campo nuevo en el DA y en `VampsAnimInstance`.

No se realizó un reinicio ni una compilación completa fuera del editor. Si posteriormente se necesita una compilación completa, cerrar Unreal y ejecutar `C:\dev\vamps\build.bat`; reabrir con `C:\dev\vamps\editor.bat`. No guardar referencias transitorias de una recarga incompleta.

## 3. Asignar el BS por DA

**Hecho — Codex por MCP.** Abrir cada DA en Content Browser → `/Game/Vamps/Data/` → Animation → **Locomotion Blend Space**:

| DA | Valor guardado |
|---|---|
| `DA_ExecutionerCombatSettings` | `/Game/Vamps/Animations/Executioner/BS_Executioner` |
| `DA_DualSwordsCombatSettings` | None |
| `DA_SwordAndShieldCombatSettings` | None |
| `DA_LancerCombatSettings` | None |

Se asignó Executioner con `ObjectTools.set_properties`; los otros campos nuevos conservaron `None`. Se guardaron los cuatro con `AssetTools.save_assets` y se compararon sus valores finales con la captura completa previa.

## 4. Conectar el ABP compartido

**Hecho — Codex por MCP.** En `/Game/Vamps/Animations/ABP_VampsCharacter` → AnimGraph:

1. En el BlendSpace Player, exponer **Blend Space** como pin desde Details. MCP modificó `showPinForProperties` del nodo con `ObjectTools.set_properties` y confirmó el pin expuesto.
2. Crear **Get Locomotion Blend Space**, propiedad de `VampsAnimInstance`, y conectar su salida al pin **BlendSpace**. Retirar el valor fijo del pin.
3. Conservar **Get Speed → X**, **Y=0**, Play Rate=1, Loop activo y la cadena **BlendSpace Player → DefaultSlot → OutputPose**.
4. Compilar y guardar. `BlueprintTools.create_node/connect_pins/set_pin_value/compile_blueprint` y `AssetTools.save_assets` realizaron esos pasos; la compilación con advertencias como errores terminó correctamente.

## 5. Usar el mismo ABP en los personajes existentes

**Hecho — Codex por MCP.** En `BP_VampsCharacter` y `BP_VampsCharacter_Player` → Mesh → Animation:

- Animation Mode = **Use Animation Blueprint**.
- Anim Class = `/Game/Vamps/Animations/ABP_VampsCharacter.ABP_VampsCharacter_C`.
- Combat Settings del personaje = `DA_ExecutionerCombatSettings`.

Se consultaron los componentes a través de `BlueprintTools.get_default_object` y `ActorTools.get_components`; `ObjectTools.set_properties` cambió el jugador. Ambos personajes compilaron con `warnings_as_errors=true` y se guardaron. No se cambió su mesh ni el BS de Executioner.

## 6. Comprobar en el juego

**Hecho — Codex por MCP, comprobación parcial.** Se inició PIE en `L_LocalMatch` y se confirmaron cuatro personajes con el ABP común y el DA de Executioner. No se registraron errores de inicialización de locomoción. También arrancó `L_CharacterBasic`. Se detuvieron ambas sesiones y se restauró `L_LocalMatch`.

MCP no permite leer `AnimScriptInstance` del mesh; tampoco dispone de los inputs necesarios para comprobar visualmente caminar y correr. No se utilizó automatización de interfaz.

**Pendiente — usuario.** Iniciar PIE en `L_LocalMatch`, seleccionar Executioner, pulsar START y esperar la preparación. Comprobar idle, desplazamiento con WASD y sprint con movimiento + Shift. Debe conservar las animaciones de `BS_Executioner` y los montages sobre `DefaultSlot`. El ataque mantiene el pendiente independiente de `End Recovery`.

## 7. Completar los BS nuevos

**Pendiente — usuario.** Crear los BS de Dual Swords, Sword and Shield y Lancer con sus animaciones compatibles con el skeleton del juego. Mantener **Speed en X y las muestras en Y=0**, como el BS actual. Asignarlos a **Animation → Locomotion Blend Space** del DA correspondiente y guardar. Ajustar muestras, velocidades y mezcla según cada animación.

Los personajes que se preparen para esos perfiles usarán `ABP_VampsCharacter` y su propio DA. La referencia se toma al inicializar la instancia de animación; comprobar la asignación en una nueva sesión PIE.
