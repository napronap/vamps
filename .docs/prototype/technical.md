# Implementación existente del prototipo

Comportamiento: [functional.md](functional.md). Fuentes propias en `Source/vamps/Public/` y `Source/vamps/Private/`, con rutas paralelas. Los cambios requeridos por la referencia se especifican en [combate](../combat/technical.md), [jugador](../player/technical.md) y [partida](../game/technical.md).

## Clases y responsabilidades

| Clase existente | Responsabilidad |
|---|---|
| `AVampsCharacter` | `ACharacter` con GAS, atributos y hitbox; concede habilidades con autoridad y resuelve `ReceiveHit` |
| `AVampsCharacter_Player` | Añade SpringArm de 400 cm y cámara ligada a ControlRotation |
| `AVampsPlayerController` | Requiere Mapping Context y cuatro Input Actions asignadas: Move, Look, Attack y Guard |
| `UGA_Attack` | Montage único mediante `UAbilityTask_PlayMontageAndWait`; final/cancelación cierran trazas |
| `UGA_Guard` | Habilidad activa hasta liberar guardia |
| `UFistHitboxComponent` / `UANS_AttackTrace` | Sweep del puño y ventana del montage |
| `UVampsAttributeSet` / `UGE_VampsDamage` | Health/MaxHealth y efecto instantáneo de daño |
| `AVampsAIController` | Solicita movimiento, ataque y guardia según `EVampsAIMode`; no resuelve daño |
| `AVampsCombatTestGameMode` | Crea rival e IA explícitamente; reinicia vida, posiciones, acciones y foco del rival |
| `AVampsCombatTestPlayerController` | Añade CycleAIMode y ResetCombat al controller normal |
| `AVampsCombatTestHUD` / `UVampsAnimInstance` | HUD de vida/estado/IA y datos de locomoción/guardia para presentación |

`AVampsCharacter` desactiva Tick y orienta locomoción mediante CharacterMovement. AutoPossessAI está Disabled; el GameMode de prueba crea el controller del rival y lo posee explícitamente. No hay seguimiento de target en el controller normal ni en la cámara.

## Cómo leer el código

1. `AVampsPlayerController` enlaza Attack a `Started` y Guard a pulsación/liberación. Move usa yaw de ControlRotation; Look aplica yaw/pitch libres.
2. `AVampsCharacter::RequestAttack` solicita la habilidad concedida en BeginPlay mediante GAS.
3. `UGA_Attack` comprueba montage/socket, limpia receptores previos y reproduce un montage a velocidad normal. `VampsTags::Attack` y `Guard` se excluyen mutuamente.
4. `UANS_AttackTrace` enciende/apaga el componente durante la reproducción propietaria; señales de otro montage no deben afectar a la acción.
5. `UFistHitboxComponent` detecta receptores y llama a `ReceiveHit`. Con Guard cancela la habilidad del agresor y retorna; sin Guard aplica `UGE_VampsDamage` con SetByCaller `VampsTags::Damage`.
6. Soltar guardia cancela `UGA_Guard`. Cancelación/final de ataque cierra trazas; la tarea GAS controla el montage.

Los tags representan las dos acciones actuales. No hay Data Asset de combate, fases/cadenas ni coordinación de contactos simultáneos. Las habilidades son `ServerOnly`; las solicitudes exigen autoridad. La replicación de atributos no implementa entrega de input ni predicción online.

## Animación y contacto

`AttackMontage` y `AttackDamage` se configuran en los personajes. El setup documentado utiliza `AM_BasicAttack`, un clip y un golpe en DefaultSlot, sin navegación de secciones ni duración calculada en C++.

La esfera `FistHitbox`, inicialmente de radio 12 cm, se adjunta al socket `FistHit` del skeletal mesh. Se comprueba la existencia del socket. No genera overlaps ni colisión física: consulta Pawns mediante sphere sweep entre muestras del socket, después de actualizar el mesh. La primera muestra consulta la posición actual. Un conjunto evita repetir receptores por ataque; una consulta Visibility impide contacto a través de paredes.

Fuera del trazado, Tick del componente está apagado. El muestreo no reconstruye poses o ventanas perdidas durante un hitch. La guardia actual no necesita montage ni notify. `UVampsAnimInstance` calcula Speed, Direction, bInAir y bIsGuarding; este último es presentación, no un segundo estado de combate.

## IA y reinicio

`AVampsAIController::Decide` se ejecuta cada 0,1 segundos en modos activos. Mixed alterna tres comportamientos según `MixedModeDuration`. Tick entrega `MoveDirection` a CharacterMovement; la decisión no aplica daño. Al poseer, la IA usa orientación deseada por controller; al liberar restaura las opciones anteriores.

`SetMode` limpia timer, acciones y movimiento, y programa la espera inicial de 0,75 segundos. Passive no programa decisiones. `ResetCombat` restaura personajes y ControlRotation, fija foco al jugador y vuelve a aplicar el modo. No hay Behavior Tree, State Tree ni navegación por obstáculos en esta base.

## Blueprints y animación

Conexiones documentadas en [013](../reports/013-recuperacion-blueprints/reporte.md), actualizadas por [014](../reports/014-copias-nomenclatura-vamps/reporte.md) y [015](../reports/015-retirada-duel/reporte.md):

| Asset bajo `/Game/Vamps` | Conexión |
|---|---|
| `Blueprints/BP_VampsCharacter` | Padre `AVampsCharacter`, montage y AnimBP propios; rival |
| `Blueprints/BP_VampsCharacter_Player` | Padre `AVampsCharacter_Player`, cámara nativa, montage y AnimBP propios; jugador |
| `Blueprints/BP_VampsPlayerController` | Padre normal, cuatro acciones y `IMC_Vamps` |
| `Blueprints/BP_VampsCombatTestPlayerController` | Padre de prueba, acciones anteriores más CycleAIMode/ResetCombat y `IMC_VampsCombatTest` |
| `Blueprints/BP_VampsAIController` | Padre `AVampsAIController`, modos y parámetros de observación |
| `Blueprints/BP_VampsGameMode` | Jugador/controller normales, HUD nativo |
| `Blueprints/BP_VampsCombatTestGameMode` | Jugador, controller de prueba, rival/IA Vamps y `AVampsCombatTestHUD` |
| `Animations/ABP_VampsCharacter` | Padre `UVampsAnimInstance`; BlendSpace → DefaultSlot → OutputPose, EventGraph vacío |
| `Animations/BS_VampsCharacter` | Locomoción Speed 0–420 con recursos visuales compartidos |
| `Animations/AM_BasicAttack` | Segmento de `AS_BasicAttack1`; existen también `AS_BasicAttack2/3`, sin cadena implementada |
| `Maps/L_CharacterBasic` | Override de GameMode de prueba |

Las fuentes se revisaron en la iteración documental [017](../reports/017-base-fgoa-y-extensiones/reporte.md); no se consultaron nuevamente los defaults de assets en Unreal. El contacto y el notify del montage permanecen sin confirmación según 013–015. El estado de compilación completa y limpieza residual después de 015 se conserva en su [guía](../reports/015-retirada-duel/editor.md); no se marca resuelto por esta revisión.
