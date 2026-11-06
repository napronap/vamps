# 065 — Combos sin límite fijo de secciones

La configuración del fundido continúa en [066 — Cierre por End Recovery](../066-fundido-por-end-recovery/reporte.md).

## Resultado

`UGA_Attack` acepta un montage con cualquier cantidad de secciones mayor o igual a una. El número de golpes proviene de `AttackMontage.GetNumSections()` y la última sección se identifica por ese total; desaparece el máximo fijo de tres.

Se retira también la exigencia de que el tercer golpe tenga más knockback que los anteriores. No se impone esa comparación al último golpe de una cadena más larga: cada sección conserva su valor configurado en `ComboSteps`.

## Cambio y conexiones

Se modifica únicamente `Source/vamps/Private/Combat/Abilities/GA_Attack.cpp`. La validación recorre las N secciones del montage y exige una entrada de `ComboSteps` por cada una. Si faltan entradas, el diagnóstico indica N secciones y M entradas recibidas; no completa el array ni sustituye valores.

Se conservan las validaciones de tiempos y notifies para todas las secciones: exactamente un `Attack Trace` y un `End Recovery` por sección; una `Combo Window` obligatoria antes de la última y opcional en la última, sin duplicados. El cierre de Recovery debe quedar después de contactos y de la ventana, cuando existe, y antes del blend-out.

La revisión estática de dos agentes confirmó que las piezas existentes ya usan el tamaño real: `AVampsCharacter` y la IA reconocen el final mediante `GetNumSections`; la continuación busca `GetSectionName` e incrementa el paso; `UVampsCombatResolver` consulta `ComboSteps[ComboStep - 1]` con índices `int32`. No fue necesario modificarlas. Los enlaces automáticos de todas las secciones se deshabilitan en la instancia del montage con `SetNextSectionID(..., INDEX_NONE)`: avanzar sigue requiriendo una solicitud dentro de la ventana, contacto válido y permiso de ataque. Un clic durante el último golpe no crea una sección siguiente.

No se cambian headers, structs, clases reflejadas, Blueprints, configuración ni propiedades de assets. Se guardaron los cuatro DAs actuales para preservar su configuración. Sus `ComboSteps` tienen tres entradas cada uno: [captura previa](antes.json). El montage de Lancer/Impaler referenciado es `/Game/Vamps/Animations/Impaler/AM_Impaler_Combo`; no se ha leído ni modificado su cantidad de secciones.

Se agregan este reporte y su guía al índice, con enlaces de continuación desde 024, 061 y 064. Las especificaciones funcionales y técnicas quedan fuera de esta iteración.

## Verificación y límites

- Se descubrieron por MCP `VampsCombatSettings` y sus assets, se capturaron las 30 propiedades actuales de cada uno de los cuatro DAs y se guardaron en disco antes de compilar: [antes.json](antes.json).
- Se aplicó clang-format al único archivo C++ modificado; `git diff --check` pasó. La revisión estática comprobó recorrido de N secciones, reconocimiento del final y requisitos de continuación; no equivale a observar cadenas de distintas longitudes en juego.
- `LiveCodingToolset.LiveCodingToolset.CompileLiveCoding` devolvió `Success` con enlace y aplicación de un parche real, no `NoChanges`: [resultado](live-coding.json). La [relectura posterior](despues.json) confirmó las 30 propiedades idénticas en los cuatro DAs, `preserved=true`, guardado correcto y `dirty=false` en los cuatro. No hubo restauraciones, nuevos campos ni cambios de configuración.
- `ObjectTools.list_properties` no expone `compositeSections` ni `notifies` del montage; `get_properties` rechazó ambos campos solicitados explícitamente. No hay una lectura MCP de sus secciones/ventanas que permita afirmar su número o sus tiempos, ni se modificaron mediante UI automatizada.

No se inició una sesión PIE nueva ni se activaron ataques por MCP; no hay herramienta directa para esos inputs. No se ejecutaron tests ni se automatizaron inputs. Quedan configurar manualmente montage y entradas del DA, y observar cadenas de más de tres golpes, una sección, ausencia de continuación sin input y cierre del último golpe con Recovery/NA. Pasos y límites en [editor.md](editor.md).
