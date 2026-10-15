# Base técnica del juego

Se usa Unreal Engine 5.8 y GAS. Reglas: [juego y partida](functional.md). Dependencias: [combate](../combat/technical.md), [jugador](../player/technical.md) y [base existente](../prototype/technical.md).

## Código y recursos

Las clases y Blueprints propios heredan de clases nativas de Unreal. No se copia ni hereda lógica del ejemplo Third Person ni de sus variantes; solo se reutilizan sus recursos visuales. Los archivos propios se organizan por responsabilidad en `Source/vamps/Public/` y `Source/vamps/Private/`.

`AVampsCharacter` aloja GAS y combate; `AVampsCharacter_Player` añade cámara; `AVampsPlayerController` interpreta entrada. `AVampsAIController` solicita acciones compartidas. `AVampsCombatTestGameMode`, su controller y HUD forman la escena local de observación, no la partida por equipos. Sus [relaciones y assets actuales](../prototype/technical.md) son el punto de partida.

## Partida, baja y respawn

La autoridad de partida registra equipos, bajas, puntuación, portador y condiciones de fin. La derrota individual termina acciones y participación del personaje antes de reaparecer con el mismo tipo; no consume una reserva de clases o personajes. La presentación recibe el resultado de partida y la condición de portador.

El portador modifica el valor de una baja. Su selección, transferencia y posibles efectos dependen de las [decisiones de partida](functional.md#preguntas). No se agrega curación por analogía con el Grial ni se crean datos de reservas.

`AVampsCombatTestGameMode::ResetCombat` restaura vida/posición para observar el prototipo; no implementa muerte ni respawn. Se conserva esa responsabilidad separada de las reglas de partida que se incorporen, sin duplicar la resolución del combate dentro del GameMode.

## Autoridad y red

La base existente ejecuta habilidades `ServerOnly` y solicitudes con autoridad. Replicar atributos no equivale a tener combate online completo. La reproducción de la referencia requiere diseñar entrega de solicitudes, estado autoritativo y presentación para varios participantes cuando llegue la integración de red.

Los videos no determinan predicción, compensación de latencia ni arbitraje. La elección de esos mecanismos no recupera automáticamente modelos retirados de reportes anteriores. No se agrega infraestructura de red o compatibilidad con cartas por anticipación.

## Dependencias de implementación

El [roadmap](../roadmap.md) prioriza núcleo, movilidad/presentación y partida antes de [extensiones Vamps](../extensions/technical.md). Equipos y portador dependen de daño/muerte resueltos y de una selección coherente de enemigos para cámara y HUD. Las comprobaciones de cada implementación se hacen mediante revisión de código, compilación por MCP directo cuando corresponda y observación manual del editor.

## Preguntas

1. Con las condiciones funcionales de partida elegidas, ¿qué estado autoritativo y qué presentación necesitan compartir los participantes?
