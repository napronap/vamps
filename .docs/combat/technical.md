# Implementación del combate

Reglas: [combate](functional.md). Configuración: [guía de la iteración 006](../reports/006-golpe-de-puno/editor.md).

## Arquitectura de la primera versión

Solo hay dos habilidades, ambas heredan directamente de `UGameplayAbility`:

| Pieza | Para qué sirve |
|---|---|
| `UGA_Attack` | Reproduce un único montage mediante la tarea nativa `UAbilityTask_PlayMontageAndWait`. Al finalizar o cancelarse, termina la habilidad y detiene las trazas |
| `UGA_Guard` | Permanece activa desde que se presiona guardia hasta que se suelta |
| `AVampsDuelCharacter` | Aloja GAS, recibe input y resuelve `ReceiveHit`: cancelar ataque o aplicar daño |
| `UANS_AttackTrace` | Un Notify State que enciende y apaga las trazas en el tramo del golpe |
| `UFistHitboxComponent` | Es una esfera sobre el socket del puño; barre su movimiento y evita aplicar el mismo golpe varias veces al mismo personaje |
| `UVampsAttributeSet` | Guarda Health y MaxHealth; limita la vida a su rango |
| `UGE_DuelDamage` | Efecto instantáneo que resta vida |

No hay base de habilidades propia, Data Asset de combate, enums de fase/resultado, dispatcher de ventanas ni subsystem de combate. La [IA básica](../game/technical.md#ia-para-observar-el-combate) usa las mismas solicitudes del personaje, sin añadir reglas a las habilidades.

## Cómo leer el código

1. `RequestAttack` pide al AbilitySystemComponent que active la habilidad concedida en BeginPlay.
2. `UGA_Attack::ActivateAbility` reproduce el montage. GAS agrega el tag Attack mientras la habilidad está activa.
3. El notify `Attack Trace` habilita el componente de trazas.
4. El componente detecta al receptor y llama a `ReceiveHit`.
5. `ReceiveHit` consulta si el receptor tiene el tag Guard. Si lo tiene, llama a `CancelAttack` del agresor y retorna. Si no, aplica `UGE_DuelDamage` a su vida.
6. Soltar RMB cancela `UGA_Guard`; GAS retira su tag.

Los tags Attack y Guard son la fuente del estado, no una máquina de estados adicional. Ambos bloquean la activación de la otra habilidad. Damage identifica el valor que se pasa al Gameplay Effect. Los handles solo permiten pedir activación/cancelación a GAS.

## Animación y contactos

`AttackMontage` y `AttackDamage` se configuran directamente en BP_DuelCharacter. El montage contiene **un clip y un golpe**, a velocidad normal, sin navegación de secciones ni duración calculada en C++.

`UANS_AttackTrace` va en el montage. Usa branching points y comprueba la instancia de reproducción para que una señal de un montage cancelado no afecte a otro. No almacena datos por personaje.

FistHitbox hereda de USphereComponent y se adjunta al skeletal mesh del personaje, en SocketName = FistHit. Sphere Radius = 12 cm inicialmente. El usuario coloca ese socket en el puño del skeleton. No se necesita una mesh adicional ni dos sockets.

Durante el tramo activo, hace un sphere sweep desde la posición anterior del socket hasta la actual, usando GetScaledSphereRadius. La primera muestra consulta la posición actual sin barrer desde una pose anterior al notify. Tick del componente corre después del skeletal mesh; fuera del ataque está desactivado. No genera overlaps ni bloquea físicamente: la esfera define el volumen consultado por las trazas. Un conjunto de receptores evita repetir daño dentro del mismo ataque. Una consulta Visibility impide golpear a un receptor oculto por una pared.

UGA_Attack comprueba que exista el socket antes de reproducir el montage. Si falta, el componente deja un aviso en Output Log al comenzar la partida. El radio se ajusta en el componente; el centro se ajusta moviendo el socket. BeginPlay alinea la esfera al socket, sin offsets adicionales del componente.

Cancelar el ataque cierra sus trazas inmediatamente y la tarea GAS detiene el montage con su blend-out. La guardia no necesita montage ni notify: el HUD la muestra y `UDuelAnimInstance::bIsGuarding` permite añadir una pose visual sin cambiar sus reglas.

El muestreo sigue siendo una aproximación entre poses; no reconstruye frames ni ventanas completas perdidas durante un hitch.

## Movimiento y presentación

Move aplica `AddMovementInput` desde el callback de input, sin almacenamiento ni restricciones de combate. El Tick del personaje solo mantiene orientación hacia el lock-on; PlayerTick solo sigue el objetivo con la cámara.

El HUD consulta Health, MaxHealth e IsAttacking/IsGuarding. No mantiene fases, resultados ni contadores propios. La copia bIsGuarding del AnimInstance es exclusivamente de presentación.

## Alcance de GAS

El AbilitySystemComponent concede dos habilidades. El AttributeSet conserva la vida y el Gameplay Effect aplica daño. Esos son los conceptos usados en esta base.

La ejecución continúa siendo local y con autoridad, con habilidades ServerOnly. Se usa ahora la tarea estándar de montage de GAS, pero no se agregó envío de input por red ni predicción. La replicación básica de atributos no equivale a un duelo online terminado. No se incorpora infraestructura adicional hasta necesitarla.
