# Combos sin límite fijo — operaciones del editor

**Queda para el usuario:** contar las secciones del montage que quiera usar, configurar una entrada de `ComboSteps` por golpe y sus notifies, y comprobar cadenas de más de tres golpes y de una sección. El cambio compiló por MCP y las 30 propiedades de cada DA se conservaron. No se conoce por MCP el número actual de secciones ni sus tiempos; no se han cambiado assets para inventar esa configuración.

## 1. Preservar la configuración actual

**Hecho — Codex por MCP.** Se descubrieron la clase `VampsCombatSettings` y sus cuatro DAs con `ObjectTools.search_subclasses` y `AssetTools.find_assets`. `ObjectTools.list_properties/get_properties` capturó las 30 propiedades de cada DA, con referencias, arrays completos y transforms; `AssetTools.save_assets` guardó los cuatro antes de compilar. [Copia actual](antes.json).

En Content Browser → `/Game/Vamps/Data/`, los DAs consultados son `DA_ExecutionerCombatSettings`, `DA_DualSwordsCombatSettings`, `DA_SwordAndShieldCombatSettings` y `DA_LancerCombatSettings`. Todos tienen actualmente tres entradas de `Combo Steps`. La captura conserva los valores del usuario; no reemplazarlos por defaults ni por configuraciones de reportes anteriores.

## 2. Compilar y verificar preservación

**Hecho — Codex por MCP.** `LiveCodingToolset.LiveCodingToolset.CompileLiveCoding` devolvió `Success` con enlace y aplicación de un parche real, no `NoChanges`: [resultado](live-coding.json). La [relectura posterior](despues.json) confirmó las 30 propiedades idénticas a la copia actual en cada DA, `preserved=true`, guardado correcto y `dirty=false` en los cuatro. No se cambiaron asignaciones ni se restauraron valores. El cambio de `GA_Attack.cpp` no agrega propiedades reflejadas ni requiere migrar los DAs.

## 3. Configurar el montage y sus pasos

**Hecho — Codex por MCP, consulta parcial.** `ObjectTools.list_properties` no expone `compositeSections` ni `notifies`; `get_properties` rechazó ambos campos. Las herramientas directas disponibles no permiten inspeccionar o editar las secciones y sus ventanas. Estas operaciones quedan para el usuario en el editor, sin automatización de interfaz.

**Pendiente — usuario.** Abrir el DA elegido y seguir su referencia `Attack Montage`. Para Lancer/Impaler apunta a `/Game/Vamps/Animations/Impaler/AM_Impaler_Combo`:

1. En el editor del montage, revisar Montage Sections y contar N secciones, en el orden de los golpes. Debe haber al menos una; la última se detecta automáticamente por su posición, sin un nombre o índice fijo.
2. En el DA → Combat → `Combo Steps`, configurar las entradas correspondientes a las N secciones. El índice 0 corresponde al primer golpe. Para ampliar una cadena de tres a N golpes, conservar los valores de las tres entradas existentes y añadir las necesarias hasta N. Definir para cada entrada `Target Alignment Distance` y `Knockback Distance` positivos, en cm, y `Contact Reaction Duration` finita y positiva, en segundos. Los valores nuevos dependen del golpe; no se rellenan automáticamente ni se exige que el último tenga mayor knockback.
3. Si el montage tiene una sola sección, se usa la primera entrada del DA; no hace falta borrar las entradas adicionales existentes. Si faltan entradas para las secciones, el ataque se cancela y `LogVampsCombat` muestra ambas cantidades.
4. En las pistas de notifies del montage, configurar los requisitos de la tabla siguiente para cada sección. No basta colocarlos únicamente en la animación de origen. Conservar velocidad de reproducción positiva y constante, y Auto Blend Out habilitado para el montage de ataque.

| Elemento | Cantidad y orden temporal |
|---|---|
| `Attack Trace` | Exactamente uno por sección: inicio de sección < inicio de traza < fin de traza < fin de sección. |
| `Combo Window` | Exactamente una en cada sección anterior a la última. En la última puede omitirse; si existe, solo una. Empieza después del inicio de la traza, termina después del fin de la traza y de su propio inicio, y cierra antes del blend-out. |
| `End Recovery` | Exactamente uno por sección, después del fin de contactos y de `Combo Window`, si existe, y antes del blend-out. Usar el límite efectivo de esa sección, no copiar un tiempo de otro golpe. |

Si ya se usa `Reset Weapon Hits`, mantenerlo estrictamente dentro de `Attack Trace` y con índices de armas válidos. El contrato de cierre por notify se detalla en [061](../061-fin-recovery-notify/editor.md#3-colocar-los-marcadores).

**Pendiente — usuario.** Guardar únicamente el montage y el DA que haya configurado. No se requieren cambios de GameMode, selector, Character Blueprint ni ABP para aumentar N.

## 4. Comprobar la cadena en juego

**Pendiente — usuario.** Con la configuración anterior lista, iniciar PIE y usar el personaje cuyo DA referencia el montage. Activar F9 si necesita observar paso, ventana y fase:

1. Con N mayor que tres, conectar cada golpe y pulsar dentro de su `Combo Window`: avanzar por las secciones en orden hasta la última. El salto debe producirse al cerrar la ventana, sin reproducir automáticamente las siguientes.
2. Iniciar un ataque y no pedir continuación. Debe cerrar ese golpe aunque el montage tenga más secciones. Repetir con un clic fuera de la ventana y con un golpe sin contacto válido: no debe avanzar la cadena.
3. Pulsar durante el último golpe: no debe intentar un paso N+1 ni saltar a otra sección. Verificar el cierre por `End Recovery`, la entrada en NA y la liberación de movimiento prevista.
4. Con un montage de una sección y su primera entrada configurada, comprobar que funciona sin `Combo Window`, termina ese único golpe y pasa por Recovery/NA. No se exige crear otra sección.
5. Si se rechaza un ataque, leer Output Log → `LogVampsCombat`: corregir la sección, notify o entrada indicada antes de repetir. La compilación del código no acredita los tiempos manuales del montage.

No se inició una sesión PIE nueva ni se activaron ataques por MCP en esta iteración. Estas comprobaciones son manuales porque las herramientas directas no activan los inputs de juego. No se automatizan clicks/teclas ni se agregan o ejecutan tests.
