# 072 — Revisión de código

## Controlador de IA

- `AVampsAIController::OnPossess` usa el `VampsCharacter` válido que le entregan los GameModes y orienta al bot según el foco del controller. Se retira el respaldo de los flags de rotación porque el respawn mantiene la posesión y el cierre de ronda destruye el pawn al desposeerlo. `SetMode` arma el timer de decisión solo con un `AVampsCharacter` poseído. `OnUnPossess` cancela el timer y consume el input antes de que Unreal desvincule el pawn. El reset de combate queda en `AVampsCharacter::UnPossessed`.
- En partida, el controlador toma el objetivo de `UVampsLockOnComponent`. En el mapa de prueba, usa el foco que le asigna `AVampsCombatTestGameMode`. Si falta objetivo o el bot está muerto, el controlador espera. La comprobación de permisos de movimiento y acción queda en el personaje.
- `AVampsCharacter::CanRequestCombatContinuation` centraliza el permiso, la ventana y la existencia de un paso siguiente. La IA consulta esa función y conserva sus decisiones propias de esperar un contacto, evitar solicitudes duplicadas, acercarse y espaciar ataques. Se retiró el `default` del `switch` y el log de configuración de `SetMode`. Se conserva la guarda de tiempos y el reset al cambiar de comportamiento en `Mixed`.

`/Game/Vamps/Blueprints/BP_VampsAIController` conserva su padre nativo y sus defaults. `BP_VampsCombatTestGameMode` selecciona el controller del rival en `L_CharacterBasic`. `AVampsMatchGameMode` instancia el controller nativo para los bots de `L_LocalMatch`. El cambio se limita a C++.

## Notifies de combate

`UAN_EndRecovery` obtiene al dueño del mesh en la condición del `if` y entrega el payload a `AVampsCharacter` cuando el dueño es un combatiente. Se retira el log del wrapper. El personaje pausa el montage y programa el cierre del paso. Después de los contactos y la posible continuación, comprueba si el paso sigue en Recovery y entra en NA.

`End Recovery`, `Reset Weapon Hits`, `Attack Trace` y `Combo Window` conservan `bIsNativeBranchingPoint = true`. Se retira de sus constructores la asignación de `bShouldFireInEditor`, que la ruta directa de Branching Points de UE 5.8 no consulta.

## AnimInstance

`UVampsAnimInstance` consulta al dueño del mesh directamente como `AVampsCharacter`. El preview de animación usa un actor genérico que queda fuera por el cast. Si el viewport de un Blueprint instancia un `VampsCharacter`, la instancia puede leer sus datos de locomoción. En inicialización limpia la referencia anterior y toma `LocomotionBlendSpace` si el personaje tiene `CombatSettings`. Un Blend Space vacío se copia como `nullptr`, por lo que se retira su rama de error. La actualización solo calcula `Speed` desde la velocidad horizontal.

Los paquetes guardados de `/Game/Vamps/Animations/ABP_VampsCharacter` y `/Game/Vamps/Animations/Executioner/ABP_Executioner` son los dos derivados encontrados. El primero usa `LocomotionBlendSpace` y `Speed`, y el segundo usa `Speed` con su Blend Space fijo. La búsqueda en C++ y en paquetes guardados no encontró consumidores de `Direction`, `bInAir`, `bIsGuarding`, `bIsHitReacting`, `bIsStunned`, `CombatReactionRemaining`, `bIsDashing` ni `bIsSprinting` de la instancia. Se retiran esos campos y sus cálculos. Los estados de dash y sprint de `FVampsCombatState` siguen en uso.

## VampsCharacter

- `AVampsCharacter` usa `FObjectInitializer` para que `ACharacter` cree su componente nativo de movimiento con la clase `UVampsCombatMovementComponent`. Se retira el ajuste redundante de tick y se conserva `AutoPossessAI = Disabled` porque ambos GameModes asignan sus controllers. GAS inicializa el ASC como componente y el pawn refresca el contexto del controller al cambiar de posesión. El getter del ASC queda en el header.
- `BeginPlay` equipa armas y concede habilidades en autoridad si la inicialización tiene éxito. Se retiran las validaciones repetidas de reacción y movilidad y el log genérico de inicialización. El modo de partida conserva su preflight de perfiles. Las armas creadas durante un equipamiento parcial se destruyen en `EndPlay`, junto con las de un equipamiento completo. El indicador de personaje se enlaza en instancias con presentación. `EndPlay` también cancela timers y habilidades y descarta el trabajo del resolver.
- `RefreshCombatPermissions` actualiza permisos al cambiar fase, vida, defensa, ventana de combo o ronda. El personaje se suscribe a `OnRoundPhaseChanged` del GameMode. Para reaparecer, el GameMode libera `RespawnAt` antes de restaurar vida para que el callback de GAS vea al participante habilitado. Los dos controles de objetivo se conservan: lock-on puede seleccionar a un enemigo con protección de spawn y el arma consulta si puede golpearlo.
- `GA_Defend` da vigencia a la solicitud defensiva. El receptor usa el `ContactReactionDuration` del contacto para bloquear ataque, movimiento, dash y sprint durante Hit o Guard. Cada impacto que determina la reacción del frame reinicia el plazo. `FinishCombatReaction` limpia el estado y refresca los permisos al vencer el timer. Stun y NA usan sus propios plazos. El HUD usa los tiempos de stun y NA. `Tick` integra la carga de guardia y la energía con sus plazos privados.
- La elegibilidad del dash se consulta al pulsar y al consumir la solicitud con datos actuales de suelo y energía. Un impacto puede descartar la solicitud antes de gastar energía. Sprint conserva el consumo gradual mientras hay input, suelo y permiso de locomoción. Caminar por un desnivel puede dejar al personaje en el aire aunque el juego carezca de salto.
- Los notifies de ataque conservan el filtro por instancia y sección. Se retira la vigilancia del montage por frame y su consulta al resolver. El resolver intenta la continuación antes del cierre de Recovery y usa el número de acción para descartar el cierre de un paso anterior.
- El resolver revalida actores y generaciones al aplicar daño. El personaje conserva la guarda del spec de GAS y el vencimiento de la reacción. El ataque se cancela si desaparece un arma mientras se habilita su traza. Se retiran logs de contacto, reacción y trazado del personaje. La variante de muerte por bloqueo conserva su montage del perfil. La cápsula pierde colisión y movimiento al morir. Al cerrar la ronda se congelan los personajes vivos mientras los montages de muerte pueden terminar.

Los Blueprints de personaje, armas, cámara e indicador conservan sus asignaciones. La búsqueda en paquetes guardados no encontró consumidores de los tres plazos retirados ni de `bCanDash`.

Se documentan en el `.cpp` las decisiones de recuperación, movilidad, solicitud de combate y permisos. El resolver descarta un dash invalidado antes de llamar al personaje. El inicio de paso usa las armas equipadas al inicializar el luchador. Los notifies conservan autoridad, identidad de reproducción y sección del combo como condiciones de entrada.

## Lock-on

`UVampsLockOnComponent` usa al `AVampsCharacter` que lo crea como subobjeto nativo. Se retiran el diagnóstico de tipo de dueño y el modo de depuración de lock-on. `AVampsCombatTestPlayerController` retira el binding nativo de F7 y su callback. `CycleTarget` recorre candidatos elegibles, `SelectTarget` recibe un objetivo concreto, `RegisterCandidate` registra personajes y `SetCurrentTarget` publica los cambios al HUD. El cambio se limita a C++.

El componente se queda sin tick. Cada luchador llama al método público `InitializeCandidates` después de `Super::BeginPlay()` para registrarse con los demás. `SelectTarget` selecciona al atacante elegible tras un bloqueo confirmado y deja el registro a la inicialización. El header de `RefreshTarget` identifica las llamadas directas, los delegates y quién consume la selección. Si falta un objetivo elegible, busca el siguiente del ciclo o limpia la selección. Conserva al luchador muerto como candidato para el respawn y retira al que sale de juego.

En partida, `InitializeCandidates` registra solo rivales. El GameMode ya asignó los equipos al llegar a `BeginPlay`. `IsEligible` verifica que haya un candidato vivo y que la ronda esté activa cuando hay modo de partida. Si el objetivo sale de juego, `HandleCandidateEndPlay` lo retira y reanuda el ciclo en su posición anterior. La cámara usa `Participants` en partida para incluir aliados y lee los candidatos en el mapa de prueba.

## GA_Attack

Se retira `ValidateAttackConfiguration` de cada activación. `GA_Attack.h` documenta el perfil, las armas, las secciones del montage y el orden de los Branching Points que requiere el ataque. `AVampsCharacter::ResetCombatWeaponHits` comprueba el índice al acceder al array de armas equipadas.

`GA_Attack` se activa por GAS y omite `CommitAbility` porque la habilidad nativa carece de costo y cooldown y el proyecto no consume el evento de commit. Se suscribe a las señales de fin de la tarea de montage. Si la tarea termina durante el arranque, la activación retorna antes de iniciar el estado del personaje. La instancia deshabilita sus enlaces automáticos entre secciones. El resolver autoriza los saltos de combo y el personaje usa el ID de reproducción para reconocer sus notifies.

`ResolveCombatRecoveryEnd` limpia la reproducción e inicia NA antes de cerrar GAS. Un notify de traza fuera de fase, una reproducción perdida al continuar o una traza que falla al habilitarse pasan por `CancelCombatOffense`. Si una reproducción sigue registrada al llegar a `GA_Attack::EndAbility`, la habilidad llama al método público `CancelCombatOffense(false)` para limpiar el ataque mientras GAS completa el cierre.

## GA_Dash

`GA_Dash` usa el luchador que recibió la habilidad. `GA_Dash.h` documenta por qué hay que asignar el montage, darle root motion y habilitar Auto Blend Out. Al evadir sin dirección, se orienta hacia el objetivo cuando hay separación horizontal. Si no hay objetivo o ambos comparten posición horizontal, retrocede según la orientación actual. Este cambio reemplaza el rechazo que describe la [especificación de movilidad](../../player/functional.md#movimiento-y-evasión).

`CanStartCombatDash` comprueba la energía antes de activar GAS. El dash omite `CommitAbility` y descuenta `DashEnergyCost` directamente en el atributo cuando arranca la reproducción. `ReadyForActivation` puede cancelar la tarea de forma síncrona. La guarda posterior evita iniciar el estado de dash y cobrar energía en ese caso. `FinishDash` termina la habilidad directamente porque la tarea de montage ya verifica que siga activa antes de emitir sus delegates.

## VampsCombatSettings

Se elimina `VampsCombatSettings.cpp`, que solo implementaba las validaciones de reacción, muerte y movilidad. Se retiran sus declaraciones nativas de `VampsCombatSettings.h` y las llamadas de `AVampsMatchGameMode::CreateParticipants`. El GameMode sigue comprobando las referencias al Blend Space, montage de ataque y armas que requiere para crear los participantes. El header conserva la clase reflejada y los campos que usan los cuatro Data Assets del selector.

## GE_VampsDamage

Se comenta el constructor del efecto instantáneo. El modificador suma a `Health` la magnitud que `ReceiveCombatHit` escribe con signo negativo bajo `VampsTags::Damage` en el spec de cada golpe.

## VampsCombatMovementComponent

El componente lee la velocidad de sprint del perfil y calcula el avance con variables separadas para distancia al objetivo, margen de frenado y límite de avance. El resolver entrega al componente el golpe del atacante y ya comprueba autoridad y vigencia del destinatario. El desplazamiento usa la cápsula que `ACharacter` asigna como `UpdatedComponent` al crearse. Las distancias de alineación y retroceso quedan a cargo de la configuración de cada paso.

El tick de animación conserva el filtro de root motion antes y después de actualizar la pose. Un notify puede terminar el ataque durante esa actualización, con una muestra de root motion ya extraída. El reset detiene los montages cuando hay una instancia de animación y limpia el desplazamiento. El header documenta las responsabilidades del componente, sus métodos y los puntos de entrada de Unreal.

## VampsCombatResolver

El personaje valida la entrada de `ComboSteps` al abrir la ventana de traza. Si falta, registra el perfil y cancela la ofensiva antes de habilitar el arma. El arma comprueba objetivo, obstáculos y deduplicación. `QueueWeaponContact` captura el crítico, el daño y la geometría en el momento del contacto y le entrega el resultado al resolver.

`BeginCombatAction` registra el montage de ataque en orden de inicio. `ANS_AttackTrace` habilita los golpes de ese paso. Cuando el barrido encuentra al receptor con su propia traza habilitada, el contacto captura la reproducción para resolver el cruce. Las continuaciones del mismo montage conservan el orden de inicio.

El resolver recorre ese registro para aceptar los contactos. Si el receptor tenía la traza habilitada y su ataque empezó antes, descarta el golpe. Un contacto aceptado puede interrumpir los golpes salientes del receptor en ese lote, incluidos los dirigidos a terceros. El resolver confirma Guard cuando el primer contacto aceptado encuentra al receptor con defensa armada.

El filtro al inicio de `ResolvePendingContacts` descarta los contactos encolados según la validez, la vida y la generación de cada luchador. Un reset o `EndPlay` invalida el trabajo capturado de una generación anterior. La interrupción o muerte limpia las intenciones del personaje.

Una defensa armada cubre un impacto y se consume al bloquear. Si varios impactos coinciden en un frame, se bloquea el primero del grupo y los demás entran abiertos. El contacto que llena la carga todavía bloquea y causa ruptura. Se clasifican y acreditan los contactos antes de aplicar daño. Si un callback de GAS elimina o reinicia al receptor, termina su grupo. El cierre del frame ejecuta los callbacks pendientes y actualiza la partida.

`AVampsCharacter::ReactToCombatResolution` confirma defensa, acredita el paso, aplica el daño y ejecuta la reacción elegida con su desplazamiento. El personaje procesa el clic con sus permisos internos. La IA usa esa misma entrada para atacar o continuar y solicita defensa de forma explícita en modo `Guard`.

Al cerrar `ANS_ComboWindow` con un clic guardado, el personaje programa `TryContinueCombatCombo` en la misma cola de callbacks que usan `AN_EndRecovery` y el dash. Después de resolver los contactos del frame, el personaje comprueba la generación, el paso, la solicitud y el golpe acreditado antes de avanzar. Si avanza, el número de paso invalida el cierre de Recovery anterior. `FinishAttackRecovery` inicia NA para un cierre vigente. `StartRequestedDash` activa `GA_Dash` si la solicitud sigue vigente y hay permiso de movilidad. La habilidad controla el montage y descuenta la energía al arrancar.

## GA_Defend

El personaje concede `UGA_Defend` desde C++. Un clic válido arma defensa e intenta iniciar el ataque si los permisos del personaje lo permiten. Durante Hit y Guard, cada RMB arma o renueva una sola solicitud por `DefenseRequestDuration`. Con una reproducción ofensiva registrada, la solicitud se conserva hasta bloquear o cerrar el ataque. Cada bloqueo consume la solicitud y actualiza el plazo de reacción con el `ContactReactionDuration` del impacto. Si el montage de Guard anterior sigue reproduciéndose, el bloqueo lo conserva. Se retiraron `DefendedAttacker` y `DefendedActionNumber` de `FVampsCombatState`. La búsqueda por nombre en los paquetes guardados no encontró consumidores.

Al vencer el timer de Hit o Guard, el personaje limpia la reacción y la solicitud defensiva y recalcula sus permisos. El inicio del ataque depende de los permisos internos en cualquier clic, también después de reaparecer. El lock-on dirige el avance y la cámara. El orden de inicio se aplica al resolver los contactos. Una NA que ya estaba activa mantiene su propio plazo.

## Retiro del HUD de depuración

- `AVampsPlayerController` pierde el binding nativo de F9 y su `ToggleCombatHUD`. También se elimina el método de reenvío en `AVampsCombatTestHUD`. Esa clase sigue creando el widget del HUD que usan los dos modos. R y F6 siguen en el controller de observación.
- `UVampsHUDWidget` pierde `ToggleCombatDebug`, `bShowCombatDebug`, el aviso F9, el ajuste de DPI de debug, los paneles técnicos y el texto de controles de prueba. La lista normal de rivales queda en `RefreshEnemies`. Se retira `FighterWidgetClass`, que instanciaba los paneles técnicos. Los dos paneles normales provienen del Widget Blueprint.
- `UVampsFighterWidget` elimina `RefreshDebugDetails`, los bindings y las copias de barras técnicas. `ObserveFighter` recibe solo el personaje y comprueba el ASC y los atributos que usan los medidores. El HUD normal sigue consultando el tiempo de reacción para la barra de stun y el indicador sobre el personaje consulta el tiempo de NA.
- `FVampsCombatState` pierde `RequestCount`, `LastRequest`, `LastOutgoingContact`, `LastIncomingContact`, `LastDamageReceived` y `bLastIncomingCritical`, además de los dos enums de diagnóstico. `RequestCombatAction` activa GAS sin el check de retorno que escribía el resultado de F9. Desaparece el registro de `Miss` en el cierre de Recovery. La acreditación del contacto llega a `ReactToCombatResolution`, que marca `bActionConnected` para el paso registrado. La tirada crítica queda local al cálculo del daño que se encola.
- `CanStartCombatDash` y `bCombatMotionBlocked` pasan a ser privados, pues sus consumidores restantes son sus propias clases. `ActionNumber` y `bActionConnected` pasan a la categoría `Combat` en reflexión.
- Se retiran `bIsRecovering` y `ReactionSource` de `FVampsCombatState` y las asignaciones del personaje. La revisión de C++ y la búsqueda por nombre en los assets guardados no encontraron lectores. Los permisos siguen consultando la fase de Recovery o NA y el resolver conserva el atacante de cada contacto.

Por MCP se retiraron `DebugView` y sus hijos de `WBP_VampsFighter`, y `DebugScale`, sus hijos y `DebugHint` de `WBP_VampsHUD`. Ambos widgets se compilaron y guardaron. Se compararon los árboles antes y después y se confirmó que los nodos conservados mantuvieron sus padres y slots. `CombatView` mantuvo `Self Hit Test Invisible`. Los dos paquetes quedaron guardados.

## Verificación

Se revisaron las rutas de posesión, destrucción, foco, lock-on, solicitud de ataque, inicialización de animación, permisos y resolución de contactos. Se aplicó `clang-format` y pasó `git diff --check`.

PIE arrancó en `L_LocalMatch` hasta la selección de personajes y en `L_CharacterBasic` se comprobó la creación de `BP_VampsHUD`. Queda comprobar visualmente los medidores durante una ronda y probar las teclas. El editor quedó en `L_LocalMatch`, con PIE detenido. El binario cargado ya incorpora el retiro del HUD nativo. UHT se ejecutó antes de que se guardara el retiro de `bIsRecovering` y `ReactionSource` de `FVampsCombatState`, por lo que esa limpieza todavía requiere compilar en frío. [Guía del editor](editor.md).

Para la iniciativa y defensa se revisaron los puntos de entrada, los cruces del mismo frame, la transición de vida en respawn y los permisos durante el timer de reacción. Se aplicó `clang-format` y pasó `git diff --check`. Quedan la compilación y las comprobaciones de combate en PIE. [Guía del editor](editor.md).
