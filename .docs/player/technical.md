# Implementación de controles y presentación

Comportamiento: [functional.md](functional.md). Estado existente: [prototipo](../prototype/technical.md). El [núcleo de combate](../combat/technical.md) conserva autoridad sobre acciones y resultados.

## Entrada y movimiento

`AVampsPlayerController` enlaza IA_Combat por Started; mantener RMB no repite solicitudes. IA_Mobility enlaza Started para solicitar dash y Completed/Canceled para terminar la intención de sprint. Move conserva referencia al yaw de cámara y comunica dirección incluso cuando el personaje rechaza caminar; Completed/Canceled la pone en cero. Al pulsar Shift se lee también el valor actual de IA_Move para no depender del orden de callbacks.

AVampsCharacter deriva bCanDash por vida, energía, suelo y reacción; bCanMove sigue representando locomoción voluntaria. SetMobilityHeld conserva una solicitud del paso actual. UVampsCombatResolver procesa contactos, continuación, dash y liberación de locomoción en ese orden. BeginCombatStep descarta el dash anterior al avanzar; Guard/Hit/Stun, muerte y reset invalidan intención y permiso. El cierre por movilidad conserva el plazo de Recovery y la NA posterior. La salida de Guard no borra una NA ya vigente.

UVampsCombatMovementComponent ejecuta Advance y Dash mediante su ruta de movimiento con colisión. Dash fija dirección y facing iniciales y elige un montage por sector de 45 grados relativo al cuerpo: Fw, FR, R, BR, Bw, BL, L, FL. La traslación/rotación de raíz de ataque/dash se suprime para usar un solo dueño del desplazamiento; Guard/Hit conservan su alineación/empuje independiente. Comenzar ataque o materializar una reacción termina la movilidad incompatible con blend out del dash; no se cambia colisión para dar invulnerabilidad.

MobilityEnergy/MaxMobilityEnergy son atributos GAS separados de GuardLoad. El personaje paga dash al iniciar reproducción correctamente, drena sprint y recupera energía con su retraso. GetMaxSpeed aplica SprintSpeed solo cuando bIsSprinting. Los datos escalares se validan sin asserts; los montages requieren ocho referencias independientes, un slot, Auto Blend Out, velocidad constante y longitud de segmentos consistente con la del montage. Una configuración inválida cancela el dash con diagnóstico antes de abandonar Guard o cobrar energía.

La conexión vigente está en /Game/Vamps/Input/IA_Mobility, LeftShift de IMC_Vamps, MobilityAction de BP_VampsPlayerController y los ocho AM_Executioner_Dash_* de /Game/Vamps/Animations/Executioner. BS_Executioner conserva sus muestras 0/180/420 y agrega Run_Fw en 700 con RateScale=700/420. UVampsAnimInstance publica bIsDashing/bIsSprinting y el HUD de observación muestra energía, permiso y solicitud. [037](../reports/037-movilidad-combate/reporte.md) registra configuración y límites de verificación.

## Una selección para cámara y HUD

UVampsLockOnComponent conserva selección y orden de rivales mostrado por HUD. La guardia efectiva entrega el agresor como selección forzada; contacto ofensivo con otro rival no produce ese evento. Cámara y HUD reciben el mismo target. La selección inicial, reemplazo y ausencia están implementados en 018–019; equipos y respawn se conectan al incorporarlos.

AVampsCharacter_Player aporta SpringArm de 400 cm y cámara ligada a ControlRotation. AVampsPlayerCameraManager ya implementa seguimiento automático suave y tolerancia lateral, con retorno detrás del personaje sin target. La etapa 4 conserva ese comportamiento. Distancia/FOV y planos por acción se completan en 5, sin atribuirles una fórmula interna al original.

## Oclusión y señales

La selección de oclusores preparados para desvanecer modifica presentación/material local, sin alterar automáticamente colisión de personajes o límites. Su presentación se restaura al dejar de ocultar la vista relevante. El comportamiento de cámara frente a sólidos se define con el mapa; un límite invisible no requiere material de fade.

El HUD actual `AVampsCombatTestHUD` sirve a observación. La presentación de partida necesita las señales de combate, target/amenaza y puntuación descritas en el funcional; no deduce bloqueo a partir de una flecha ni crítico a partir de la animación. `UVampsAnimInstance` recibe el estado resuelto para la pose, sin decidir disponibilidad.

Las conexiones de input y animación `/Game/Vamps` se documentan en el [prototipo](../prototype/technical.md#blueprints-y-animación). Los assets futuros se crean/configuran mediante MCP directo soportado durante su implementación.

## Preguntas

1. ¿Qué tratamiento de colisión de cámara y qué materiales de los mapas cubren los oclusores visibles del alcance?
