# 072 — Revisión de código

## Controlador de IA

- `AVampsAIController::OnPossess` usa el `VampsCharacter` válido que le entregan los GameModes y orienta al bot según el foco del controller. Se retira el respaldo de los flags de rotación porque el respawn mantiene la posesión y el cierre de ronda destruye el pawn al desposeerlo. `SetMode` arma el timer de decisión solo con un `AVampsCharacter` poseído. `OnUnPossess` cancela el timer y consume el input antes de que Unreal desvincule el pawn. El reset de combate queda en `AVampsCharacter::UnPossessed`.
- En partida, el controlador toma el objetivo de `UVampsLockOnComponent`. En el mapa de prueba, usa el foco que le asigna `AVampsCombatTestGameMode`. Si falta objetivo o el bot está muerto, el controlador espera. La comprobación de permisos de movimiento y acción queda en el personaje.
- `AVampsCharacter::CanRequestCombatContinuation` centraliza el permiso, la ventana y la existencia de un paso siguiente. La IA consulta esa función y conserva sus decisiones propias de esperar un contacto, evitar solicitudes duplicadas, acercarse y espaciar ataques. Se retiró el `default` del `switch` y el log de configuración de `SetMode`. Se conserva la guarda de tiempos y el reset al cambiar de comportamiento en `Mixed`.

`/Game/Vamps/Blueprints/BP_VampsAIController` conserva su padre nativo y sus defaults. `BP_VampsCombatTestGameMode` selecciona el controller del rival en `L_CharacterBasic`. `AVampsMatchGameMode` instancia el controller nativo para los bots de `L_LocalMatch`. El cambio se limita a C++.

## Notifies de combate

`UAN_EndRecovery` obtiene al dueño del mesh en la condición del `if` y entrega el payload a `AVampsCharacter` cuando el dueño es un combatiente. Se retira el log del wrapper. En los montages de ataque, el marcador sigue encolando el fin de Recovery. El resolver procesa primero la posible continuación y, si la cadena termina, inicia NA.

`End Recovery`, `Reset Weapon Hits`, `Attack Trace` y `Combo Window` conservan `bIsNativeBranchingPoint = true`. Se retira de sus constructores la asignación de `bShouldFireInEditor`, que la ruta directa de Branching Points de UE 5.8 no consulta.

## AnimInstance

`UVampsAnimInstance` consulta al dueño del mesh directamente como `AVampsCharacter`. El preview de animación usa un actor genérico que queda fuera por el cast. Si el viewport de un Blueprint instancia un `VampsCharacter`, la instancia puede leer sus datos de locomoción. En inicialización limpia la referencia anterior y toma `LocomotionBlendSpace` si el personaje tiene `CombatSettings`. Un Blend Space vacío se copia como `nullptr`, por lo que se retira su rama de error. La actualización solo calcula `Speed` desde la velocidad horizontal.

Los paquetes guardados de `/Game/Vamps/Animations/ABP_VampsCharacter` y `/Game/Vamps/Animations/Executioner/ABP_Executioner` son los dos derivados encontrados. El primero usa `LocomotionBlendSpace` y `Speed`, y el segundo usa `Speed` con su Blend Space fijo. La búsqueda en C++ y en paquetes guardados no encontró consumidores de `Direction`, `bInAir`, `bIsGuarding`, `bIsHitReacting`, `bIsStunned`, `CombatReactionRemaining`, `bIsDashing` ni `bIsSprinting` de la instancia. Se retiran esos campos y sus cálculos. Los estados de dash y sprint de `FVampsCombatState` siguen en uso.

## VampsCharacter

- `AVampsCharacter` usa `FObjectInitializer` para que `ACharacter` cree su componente nativo de movimiento con la clase `UVampsCombatMovementComponent`. Se retira el ajuste redundante de tick y se conserva `AutoPossessAI = Disabled` porque ambos GameModes asignan sus controllers. GAS inicializa el ASC como componente y el pawn refresca el contexto del controller al cambiar de posesión. El getter del ASC queda en el header.
- `BeginPlay` equipa armas y concede habilidades en autoridad si la inicialización tiene éxito. Se retiran las validaciones repetidas de reacción y movilidad y el log genérico de inicialización. El modo de partida conserva su preflight de perfiles. Las armas creadas durante un equipamiento parcial se destruyen en `EndPlay`, junto con las de un equipamiento completo. El indicador de personaje se enlaza en instancias con presentación. `EndPlay` también cancela timers y habilidades y descarta el trabajo del resolver.
- `RefreshCombatPermissions` actualiza permisos al cambiar fase, vida, defensa, ventana de combo o ronda. El personaje se suscribe a `OnRoundPhaseChanged` del GameMode. Para reaparecer, el GameMode libera `RespawnAt` antes de restaurar vida para que el callback de GAS vea al participante habilitado. Los dos controles de objetivo se conservan: lock-on puede seleccionar a un enemigo con protección de spawn y el arma consulta si puede golpearlo.
- Defensa, reacción y NA vencen con timers del personaje. `UVampsCombatResolver` usa el estado de defensa y conserva la clasificación de contactos antes de aplicar daño, incluido el intercambio letal simultáneo. Se retiran los plazos públicos y `ContactTime`. El HUD, la barra de stun y el indicador de NA consultan duración restante para dibujar sus medidores. `Tick` integra la carga de guardia y energía con sus plazos privados. Unreal procesa los timers antes del lote de contactos. El frame exacto de vencimiento puede variar por el orden de tick.
- La elegibilidad del dash se consulta al pulsar y al consumir la solicitud con datos actuales de suelo y energía. Un impacto puede descartar la solicitud antes de gastar energía. Sprint conserva el consumo gradual mientras hay input, suelo y permiso de locomoción. Caminar por un desnivel puede dejar al personaje en el aire aunque el juego carezca de salto.
- Los notifies de ataque conservan el filtro por instancia y sección. Se retira la vigilancia del montage por frame y su consulta al resolver. El resolver intenta la continuación antes del cierre de Recovery y usa el número de acción para descartar el cierre de un paso anterior. El panel F9 muestra el resultado de los contactos de entrada y salida sin almacenar su frame global.
- El resolver revalida actores y generaciones al aplicar daño. El personaje conserva la guarda del spec de GAS y el vencimiento de la reacción. El ataque se cancela si desaparece un arma mientras se habilita su traza. Se retiran logs de contacto, reacción y trazado del personaje. La variante de muerte por bloqueo conserva su montage del perfil. La cápsula pierde colisión y movimiento al morir. Al cerrar la ronda se congelan los personajes vivos mientras los montages de muerte pueden terminar.

Los Blueprints de personaje, armas, cámara e indicador conservan sus asignaciones. La búsqueda en paquetes guardados no encontró consumidores de los tres plazos retirados ni de `bCanDash`.

Se documentan en el `.cpp` las decisiones de recuperación, movilidad, solicitud de combate y permisos. El resolver descarta un dash invalidado antes de llamar al personaje. El inicio de paso usa las armas equipadas al inicializar el luchador. Los notifies conservan autoridad, identidad de reproducción y sección del combo como condiciones de entrada.

## Lock-on

`UVampsLockOnComponent` usa al `AVampsCharacter` que lo crea como subobjeto nativo. Se retiran el diagnóstico de tipo de dueño y el modo de depuración de lock-on. `AVampsCombatTestPlayerController` retira el binding nativo de F7 y su callback. `CycleTarget` recorre candidatos elegibles, `SelectTarget` recibe un objetivo concreto, `RegisterCandidate` registra personajes y `SetCurrentTarget` publica los cambios al HUD. El cambio se limita a C++.

El componente se queda sin tick. Cada luchador llama al método público `InitializeCandidates` después de `Super::BeginPlay()` para registrarse con los demás. `SelectTarget` selecciona al atacante elegible tras un bloqueo confirmado y deja el registro a la inicialización. El header de `RefreshTarget` identifica las llamadas directas, los delegates y quién consume la selección. Si falta un objetivo elegible, busca el siguiente del ciclo o limpia la selección. Conserva al luchador muerto como candidato para el respawn y retira al que sale de juego.

En partida, `InitializeCandidates` registra solo rivales. El GameMode ya asignó los equipos al llegar a `BeginPlay`. `IsEligible` verifica que haya un candidato vivo y que la ronda esté activa cuando hay modo de partida. Si el objetivo sale de juego, `HandleCandidateEndPlay` lo retira y reanuda el ciclo en su posición anterior. La cámara y el HUD de F9 usan `Participants` en partida para seguir incluyendo aliados. En el mapa de prueba siguen leyendo los candidatos.

## GA_Attack

Se retira `ValidateAttackConfiguration` de cada activación. `GA_Attack.h` documenta el perfil, las armas, las secciones del montage y el orden de los Branching Points que requiere el ataque. `AVampsCharacter::ResetCombatWeaponHits` comprueba el índice al acceder al array de armas equipadas.

`GA_Attack` se activa por GAS y omite `CommitAbility` porque la habilidad nativa carece de costo y cooldown y el proyecto no consume el evento de commit. Se suscribe a las señales de fin de la tarea de montage. Si la tarea termina durante el arranque, la activación retorna antes de iniciar el estado del personaje. La instancia deshabilita sus enlaces automáticos entre secciones. El resolver autoriza los saltos de combo y el personaje usa el ID de reproducción para reconocer sus notifies.

`ResolveCombatRecoveryEnd` limpia la reproducción e inicia NA antes de cerrar GAS. Un ataque que completa Recovery sin conectar registra `Miss`. Un notify de traza fuera de fase, una reproducción perdida al continuar o una traza que falla al habilitarse pasan por `CancelCombatOffense`. Los abortos conservan el último resultado de contacto registrado. Si una reproducción sigue registrada al llegar a `GA_Attack::EndAbility`, la habilidad llama al método público `CancelCombatOffense(false)` para limpiar el ataque mientras GAS completa el cierre.

## GA_Dash

`GA_Dash` usa el luchador que recibió la habilidad. `GA_Dash.h` documenta por qué hay que asignar el montage, darle root motion y habilitar Auto Blend Out. Al evadir sin dirección, se orienta hacia el objetivo cuando hay separación horizontal. Si no hay objetivo o ambos comparten posición horizontal, retrocede según la orientación actual. Este cambio reemplaza el rechazo que describe la [especificación de movilidad](../../player/functional.md#movimiento-y-evasión).

`CanStartCombatDash` comprueba la energía antes de activar GAS. El dash omite `CommitAbility` y descuenta `DashEnergyCost` directamente en el atributo cuando arranca la reproducción. `ReadyForActivation` puede cancelar la tarea de forma síncrona. La guarda posterior evita iniciar el estado de dash y cobrar energía en ese caso. `FinishDash` termina la habilidad directamente porque la tarea de montage ya verifica que siga activa antes de emitir sus delegates.

## VampsCombatSettings

Se elimina `VampsCombatSettings.cpp`, que solo implementaba las validaciones de reacción, muerte y movilidad. Se retiran sus declaraciones nativas de `VampsCombatSettings.h` y las llamadas de `AVampsMatchGameMode::CreateParticipants`. El GameMode sigue comprobando las referencias al Blend Space, montage de ataque y armas que requiere para crear los participantes. El header conserva la clase reflejada y los campos que usan los cuatro Data Assets del selector.

## Verificación

Se revisaron las rutas de posesión, destrucción, foco, lock-on, solicitud de ataque, inicialización de animación, permisos y resolución de contactos. Se aplicó `clang-format` y pasó `git diff --check`. Quedan la compilación en frío por el cambio de propiedades reflejadas y la comprobación en Unreal. Ver la [guía del editor](editor.md).
