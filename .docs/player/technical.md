# Implementación de controles y presentación

Comportamiento: [functional.md](functional.md). Estado existente: [prototipo](../prototype/technical.md). El [núcleo de combate](../combat/technical.md) conserva autoridad sobre acciones y resultados.

## Entrada y movimiento

`AVampsPlayerController` pasa de Attack/Guard separados a una solicitud contextual por `Started`. Mantener RMB no genera repetición. La ventana y vigencia defensiva se evalúan en combate; cambiar el mapping por sí solo no implementa la regla. Evasión/sprint y selección necesitan sus entradas y condiciones propias.

La base actual aplica Move relativo al yaw y Look libre. Se debe coordinar la referencia direccional con el encuadre de lock-on, la orientación del personaje y el desplazamiento de ataque/evasión, conservando la distinción entre dirección de viaje y target. No se limita todo movimiento por un tag de ataque ni se confunde NA con inmovilidad.

## Una selección para cámara y HUD

El controller conserva selección y orden de rivales mostrado por HUD. La guardia efectiva entrega el agresor como selección forzada; contacto ofensivo con otro rival no produce ese evento. Cámara y HUD reciben el mismo target para evitar selecciones independientes por cercanía. La elegibilidad se conecta con equipos, muerte y respawn.

`AVampsCharacter_Player` aporta actualmente SpringArm de 400 cm y cámara ligada a ControlRotation; eso no implementa seguimiento de target. La cámara de referencia necesita calcular la relación visual jugador/target y aplicar tolerancia/corrección, distancia visual y presentación por acción. La elección entre FOV y distancia se hace al calibrarla, sin atribuirle una fórmula interna al original.

## Oclusión y señales

La selección de oclusores preparados para desvanecer modifica presentación/material local, sin alterar automáticamente colisión de personajes o límites. Su presentación se restaura al dejar de ocultar la vista relevante. El comportamiento de cámara frente a sólidos se define con el mapa; un límite invisible no requiere material de fade.

El HUD actual `AVampsCombatTestHUD` sirve a observación. La presentación de partida necesita las señales de combate, target/amenaza y puntuación descritas en el funcional; no deduce bloqueo a partir de una flecha ni crítico a partir de la animación. `UVampsAnimInstance` recibe el estado resuelto para la pose, sin decidir disponibilidad.

Las conexiones de input y animación `/Game/Vamps` se documentan en el [prototipo](../prototype/technical.md#blueprints-y-animación). Los assets futuros se crean/configuran mediante MCP directo soportado durante su implementación.

## Preguntas

1. Tras definir pérdida y ausencia de target, ¿cómo se representa la selección y se mantiene coherente entre controller, cámara y HUD?
2. ¿Qué tratamiento de colisión de cámara y qué materiales de los mapas cubren los oclusores visibles del alcance?
