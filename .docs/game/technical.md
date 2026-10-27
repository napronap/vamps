# Base técnica del juego

Se usa Unreal Engine 5.8 y GAS. Reglas: [juego y partida](functional.md). Dependencias: [combate](../combat/technical.md), [jugador](../player/technical.md) y [base existente](../prototype/technical.md).

## Código y recursos

Las clases y Blueprints propios heredan de clases nativas de Unreal. No se copia ni hereda lógica del ejemplo Third Person ni de sus variantes; solo se reutilizan sus recursos visuales. Los archivos propios se organizan por responsabilidad en `Source/vamps/Public/` y `Source/vamps/Private/`.

`AVampsCharacter` aloja GAS y combate; `AVampsCharacter_Player` añade cámara; `AVampsPlayerController` interpreta entrada. `AVampsAIController` solicita acciones compartidas. `AVampsCombatTestGameMode`, su controller y HUD forman la escena local de observación, no la partida por equipos. Sus [relaciones y assets actuales](../prototype/technical.md) son el punto de partida.

## Estado de partida y participantes

La implementación del paso 6 incorporará una autoridad de partida propia, separada del GameMode de observación. Mantendrá equipos A/B, participantes, estados de preparación/activo/terminado, plazos autoritativos, puntuación y estadísticas por participante. Equipo, kills, muertes y racha pertenecen a la identidad del participante y sobreviven al reemplazo de su pawn. La primera configuración local es 2v2 con Executioner; no necesita un selector de personaje ni coordinación de aceptación entre clientes.

La puntuación se deriva de la [recompensa por racha](functional.md#puntuación), sin estado de portador ni estadísticas duplicadas para calcularla. Los parámetros de partida conservan responsabilidad separada de las propiedades del personaje.

## Contactos, baja y puntuación

Una regla compartida de elegibilidad distinguirá aliados, enemigos, eliminados y participantes fuera de una partida activa. Selección, lista/amenazas del HUD, foco de IA y trazas/resolver deberán usarla de forma coherente. La protección de respawn bloquea el contacto ofensivo, sin inferir un bloqueo de guardia ni dar continuación por un contacto rechazado.

`UVampsCombatResolver` ya captura atacante, receptor y generaciones. `AVampsCharacter::ReceiveCombatHit` aplica el GameplayEffect con ese contexto; el callback de Health solo limpia el combate al morir y no conserva por sí solo el autor de la baja. La nueva notificación deberá registrar una baja única por vida del participante con el atacante válido del contacto letal.

Las recompensas usan una captura de rachas anterior a las consecuencias del lote. Retirada/destrucción, cambio de generación y cierre de partida se materializan después de resolver sus contactos válidos, para conservar los intercambios letales simultáneos. Las kills/muertes y puntos se consolidan antes de evaluar objetivo y K/D; una víctima que también mata conserva la kill acumulada pero no una racha sobreviviente.

El K/D de equipo se calcula desde la suma de kills y muertes, sin promediar los ratios individuales. La comparación contempla explícitamente cero muertes y puede hacerse mediante productos cruzados para ratios finitos, sin redondear el valor del HUD para decidir el ganador.

## Respawn y recursos del personaje

La autoridad mantendrá los spawn points por equipo y los plazos de reaparición/protección del [funcional](functional.md#respawn-y-protección). La protección se valida en la ruta autoritativa de contacto antes de registrar daño, reacción, carga o continuación; el HUD solo muestra su estado. Se retira al aceptar el inicio de un ataque, antes de abrir sus contactos, no al recibir cualquier click. La baja/fin invalidan temporizadores que ya no correspondan.

`UVampsAttributeSet` ya declara Health/MaxHealth, GuardLoad/MaxGuardLoad y MobilityEnergy/MaxMobilityEnergy. Su constructor inicializa capacidades en 100; `ResetCombatState` limpia guardia y restaura movilidad, y el reset de observación restaura Health desde MaxHealth. Eso no acredita que hoy exista una configuración de vida por personaje: el paso 6 debe permitir inicializar MaxHealth desde los stats propios del personaje y usar ese máximo al reaparecer, sin convertirlo en un valor global de partida.

No se modificará una clase o estructura de DA existente sin la captura/restauración previa por MCP exigida en AGENTS.md. Esta especificación no implica haber editado ni preservado ya esos assets.

`AVampsCombatTestGameMode::ResetCombat` restaura vida/posición para observar el prototipo; no implementa muerte ni respawn. Se conserva esa responsabilidad separada de las reglas de partida que se incorporen, sin duplicar la resolución del combate dentro del GameMode.

## Montages de muerte y cámara de espera

`UVampsCombatSettings` deberá incorporar dos referencias requeridas a UAnimMontage: `DeathMontage` y `GuardDeathMontage`. Todavía no existen en su declaración. Se validarán antes de admitir al personaje a la partida; una referencia ausente/incompatible se diagnosticará sin sustituirla por otro montage ni por la variante de muerte restante.

La selección se capturará desde `FVampsPendingContact::bBlocked` del contacto que causa la baja, antes de aplicar el GameplayEffect: el callback actual de Health limpia la reacción y no conserva esa distinción. La presentación de muerte se iniciará una vez, después de cerrar los contactos válidos del lote y cancelar las acciones anteriores. No dependerá de `bCanDefend` ni del estado de guardia ya limpiado. La pose final debe mantenerse durante la espera y la cápsula eliminada dejar de bloquear movimiento.

La cámara libre de espera se controlará localmente y se separará del pawn eliminado y de su lock-on, manteniendo el HUD/countdown. El tipo de control de esa cámara está en las [preguntas funcionales](functional.md#preguntas); la implementación no cambiará la cámara ni los controles del personaje vivo. El respawn y el fin de partida cerrarán el control de espera según su transición correspondiente.

La ampliación del DA requiere buscar clase y derivadas, capturar propiedades completas y guardar assets antes de editar la clase. Esa operación está pendiente de MCP directo, no se reemplaza por leer defaults del código ni por las capturas históricas de otra iteración.

## HUD, resultado y nueva partida

La vista de combate de `AVampsCombatTestHUD` y la alternancia F9 existen en 048. La implementación de partida añadirá puntuación/estadísticas, countdown de preparación y respawn, reloj restante y panel de resultado con botón local de nueva partida. Los contadores se derivarán de plazos de la autoridad, sin acumular tiempo desde el dibujo del HUD. El controller entregará la solicitud de revancha a la autoridad; el HUD no decidirá ganadores ni modificará estadísticas directamente.

El resultado detendrá acciones, movimiento, IA y respawns por estado de partida, además de mostrar el panel. Una nueva partida limpiará datos y contexto de la anterior antes de crear/rehabilitar participantes. La futura aceptación de todos y la selección de personaje no se anticipan con bindings o infraestructura en esta entrega. El flujo de entrada del botón y su restitución al juego se resolverán dentro de esta implementación.

## Autoridad y red

La base existente ejecuta habilidades `ServerOnly` y solicitudes con autoridad. Replicar atributos no equivale a tener combate online completo. La reproducción de la referencia requiere diseñar entrega de solicitudes, estado autoritativo y presentación para varios participantes cuando llegue la integración de red.

Los videos no determinan predicción, compensación de latencia ni arbitraje. La elección de esos mecanismos no recupera automáticamente modelos retirados de reportes anteriores. No se agrega infraestructura de red o compatibilidad con cartas por anticipación.

## Dependencias de implementación

El [roadmap](../roadmap.md) prioriza núcleo, movilidad/presentación y partida antes de [extensiones Vamps](../extensions/technical.md). Equipos, recompensa por racha y respawn dependen de daño/muerte resueltos y selección coherente para cámara/HUD. La segunda parte de 6 añadirá selección de personaje; eso no adelanta skills ni perfiles de 9. Las comprobaciones se hacen mediante revisión de código, compilación por MCP directo cuando corresponda y observación manual del editor.

## Decisiones pendientes

El tipo de control de cámara libre está en las [preguntas funcionales](functional.md#preguntas). El detalle de sincronización entre clientes se aborda en 7, después del ciclo local.
