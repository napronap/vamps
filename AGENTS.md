# Reglas de trabajo del repositorio

Estas reglas se aplican a todas las iteraciones del proyecto.

## Operaciones en Unreal

- Hacer mediante herramientas MCP directas las operaciones del editor que estas soporten: crear o configurar assets, consultar objetos, compilar y verificar resultados. Descubrir las herramientas y sus parámetros antes de usarlas.
- No automatizar la interfaz del editor: no usar clicks, teclado, navegación visual, computer use ni `SlateInspector` para realizar operaciones, aunque esa automatización esté expuesta por MCP.
- Si una operación no está cubierta por una herramienta MCP directa, dejarla al usuario. Documentar el paso exacto y continuar con el trabajo independiente que sea posible. No buscar una alternativa por UI ni implementar un atajo para evitar ese paso manual.
- Editar código y documentación con las herramientas de archivos habituales. Esta regla no obliga a editar archivos de texto mediante MCP.
- Para Live Coding, usar `LiveCodingToolset.LiveCodingToolset.CompileLiveCoding`. Distinguir una compilación sin cambios (`NoChanges`) de la compilación y aplicación de un parche real. Si se necesita una compilación o un reinicio que no pueda hacerse por MCP, dejar las instrucciones al usuario.
- Verificar el resultado de las operaciones antes de marcarlas como hechas. Si una herramienta falla o no está disponible, registrar qué falta y cómo resolverlo; no asumir éxito.

## Código y assets

No reutilizar, copiar ni heredar código, clases, Blueprints o Animation Blueprints del ejemplo Third Person ni de sus variantes. Se pueden usar sus modelos, esqueletos, animaciones, materiales y otros recursos visuales. Crear la lógica y los Blueprints propios sobre clases nativas de Unreal, según la [base técnica del juego](.docs/game/technical.md).

## Convenciones de código

- Escribir todo el código en inglés: identificadores, comentarios, mensajes de log y textos de interfaz incluidos en el código. La documentación interna, reportes y conversación pueden estar en español.
- Usar `Source/vamps/Public/` para los `.h` y `Source/vamps/Private/` para los `.cpp`. Organizar ambos árboles por responsabilidad, con rutas paralelas: combate, habilidades, atributos, efectos, personajes, IA, jugador, partida, animación e interfaz. No agrupar todo en una carpeta de demo.
- Usar includes relativos a la raíz `Public`, sin prefijos `Public/` ni `Private/`. El `.cpp` incluye primero su propio header; en headers reflejados, el include `.generated.h` siempre es el último.
- Seguir las convenciones de Epic adaptadas en `.clang-format` y `.editorconfig`: tabs de cuatro columnas, llaves en líneas separadas, bloques completos incluso para una sola sentencia y separación entre funciones y grupos de responsabilidades. No comprimir funciones, condiciones o declaraciones en una sola línea.
- Mantener `UPROPERTY` y `UFUNCTION` en una línea independiente de su declaración. Usar nombres descriptivos y variables locales que expliquen su propósito.
- Evitar shadowing de tipos y miembros de Unreal, incluidos los heredados: no declarar variables locales, parámetros, atributos ni métodos que oculten esos nombres. Un método con distinta firma también puede ocultar métodos y sobrecargas de las clases base. Revisar toda la jerarquía e interfaces antes de elegir identificadores. Reutilizar un nombre heredado para una función virtual solo cuando se implemente su contrato deliberadamente, con firma compatible y `override`; usar otro nombre para lógica distinta. Por ejemplo, usar `ControlledPawn` en lugar de `Character` dentro de un PlayerController, y `ReceiveCombatHit` en lugar de `ReceiveHit` para resolver contactos propios, porque `AController` y `AActor` ya declaran esos nombres. Ver la [guía C++](.docs/cpp-style.md#nombres-y-miembros-heredados).
- Aplicar el formateador a los archivos propios modificados. Revisar el resultado: el formateador no reemplaza la organización lógica ni la claridad de los nombres. Ver [convenciones C++](.docs/cpp-style.md).
- Se valora especialmente robustez, minimizar cantidad de líneas de código, evitar bloat, evitar duplicación.
- Se deben evitar fallbacks, if-else statements innecesarios (cosas como ramas de else que simplemente describan que algo esta roto, preferir que el codigo falle y tire un error para poder detectarlo y actuar acorde). Los fallbacks tienden a esconder bugs y generar bloat.
- No agregar compatibilidad con scope o backwards compatibility a menos que el usuario lo pida explicitamente.
- Asimismo, no hacer futurologia (por ejemplo, getters y setters innecesarios, bindings que "pueden ser usados en el futuro"). Mantener el scope a la tarea que se esta desarrollando actualmente. Si luego se necesitan otros bindings, se haran cuando sean necesarios.
- No agregar comentarios a menos que el usuario lo pida o pregunte mas aclaracion sobre alguna parte del código.
- No actualizar documentacion ni ningun .md (excepto reportes, obviamente) a menos que sea pedido por el usuario.

## Sin tests automatizados

- No agregar, regenerar ni ejecutar tests automatizados, suites, fixtures o arneses de pruebas para este repositorio. No usar `AutomationTestToolset` ni sustituirlo por scripts de tests.
- No condicionar una entrega a crear o aprobar tests. Verificar mediante revisión de código, compilación por MCP cuando esté disponible y comprobaciones manuales del editor según el alcance.
- Mantener los reportes y guías libres de pasos pendientes que pidan agregar o ejecutar tests. Esta decisión solo cambia por una nueva instrucción explícita del usuario.

## Entregables de cada iteración

- Crear o actualizar `.docs/reports/<NNN-iteracion>/reporte.md` con todos los cambios realizados, sus motivos, relaciones entre clases y conexiones con Blueprints y otros assets. Incluir verificaciones y pendientes relevantes.
- Mantener además `.docs/reports/<NNN-iteracion>/editor.md`: un instructivo ordenado de todas las operaciones necesarias fuera de la edición de código, incluidas las ya realizadas por MCP. Identificar claramente lo hecho por Codex, lo hecho por el usuario y lo pendiente para el usuario.
- Actualizar ambos documentos durante el trabajo o al terminar. Mantener un informe consolidado por iteración, sin un registro de cada llamada a herramientas. Crear una carpeta numerada y descriptiva por iteración; no dejar reportes sueltos en reports. Conservar las iteraciones anteriores con enlaces a la vigente. Escribir en español, de forma directa y concisa.
- Seguir las [convenciones de reportes y guías](.docs/AGENTS.md#reportes-y-guías-del-editor) y enlazar los entregables desde el índice de documentación.
