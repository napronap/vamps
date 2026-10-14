# 009 — Base definitiva de personajes

Configuración vigente de la escena: [011 — IA y GameMode de prueba](../011-ia-combate-prueba/reporte.md). Reemplaza el receptor colocado, el override del mapa y los defaults del duelo; conserva personajes, recursos visuales y las cuatro entradas normales.

Continuación: [010 — HUD de prueba](../010-hud-prueba/reporte.md) asigna DuelHUD al GameMode nuevo. Reemplaza la configuración de HUD nativo de esta iteración; los demás assets y pendientes del combate conservan su alcance.

## Alcance y resultado

Se completan `AVampsCharacter` y `AVampsCharacter_Player` para movimiento, cámara libre, un ataque con clic izquierdo y guardia mantenida con clic derecho. Los bindings pertenecen a `AVampsPlayerController` y actúan sobre el pawn que controle. El usuario confirmó la compilación con el nombre definitivo. Codex preparó diez assets por MCP y verificó el inicio de la escena en PIE; la revisión del notify y las comprobaciones de interacción/combate siguen pendientes en la [guía](editor.md).

`AVampsDuelCharacter` permanece sin cambios, únicamente como referencia. No hereda de la base nueva. Sus controladores, GameMode y HUD tampoco se migran. Las piezas compartidas de combate y animación pasan a trabajar exclusivamente con `AVampsCharacter`: el duelo anterior ya no es ejecutable con esas piezas y no se mantiene compatibilidad. El rollout de sus Blueprints queda para otra etapa.

## Código y relaciones

Rutas relativas a `Source/vamps/`; los headers están en Public y las implementaciones en Private.

| Archivos y clase | Responsabilidad y cambio |
|---|---|
| `Characters/VampsCharacter.h/.cpp` | Hereda de ACharacter e implementa IAbilitySystemInterface. Crea ASC, atributos y FistHitbox; concede GA_Attack y GA_Guard. Procesa contactos: la guardia cancela el ataque del agresor sin daño; el contacto sin guardia aplica GE_DuelDamage. Cancela habilidades y trazas al perder posesión y al terminar. Retira input, Tick, objetivo, reinicio y getters sin uso. |
| `Characters/VampsCharacter_Player.h/.cpp` | Hereda de VampsCharacter y solo añade CameraBoom y FollowCamera. Retira DoLook vacío y getters de cámara. |
| `Player/VampsPlayerController.h/.cpp` | Configura cuatro Input Actions y un Mapping Context mediante defaults de Blueprint. Move entrega movimiento relativo al yaw del controller; Look cambia su rotación. Attack y Guard llaman a la base del pawn actual. Registra y retira el contexto durante su ciclo de vida. |
| `Combat/Abilities/GA_Attack.cpp` | Usa VampsCharacter para montage y puño. Exige tipo de avatar, montage y socket válidos. Conserva el AbilityTask y sus callbacks de finalización, cancelación y blend-out. |
| `Combat/FistHitboxComponent.h/.cpp` | Dueño y receptores son VampsCharacter. Valida dueño/socket y mantiene el barrido del puño, exclusión del dueño, deduplicación por ataque y comprobación de paredes. |
| `Animation/ANS_AttackTrace.cpp` | Enciende y apaga las trazas sobre VampsCharacter. Conserva la comprobación de instancia de montage para ignorar notifies de un ataque cancelado. |
| `Animation/DuelAnimInstance.cpp` | Lee locomoción y guardia de VampsCharacter, también para su subclase jugador. Se conserva el nombre reflejado de la clase. |

El controller pasa de AVampsPlayerControllerNew a AVampsPlayerController: se renombran header, implementación, include generado, definiciones y referencias de bindings. El tipo reflejado definitivo es /Script/vamps.VampsPlayerController; no se conserva la clase anterior ni se agrega un redirect. La búsqueda en Source confirmó que no existía otra clase con ese nombre. El comportamiento y las propiedades de input permanecen iguales.

GA_Guard, VampsAttributeSet, GE_DuelDamage y CombatTags se reutilizan sin modificaciones. El ASC registra automáticamente el AttributeSet creado como subobjeto del personaje; no se vuelve a agregar. Las habilidades se conceden antes de Super::BeginPlay para que el evento BeginPlay de Blueprint pueda solicitarlas. La vida inicial es 100 y el daño inicial 12. Ataque y guardia se excluyen mediante los tags existentes; no se agrega cola de inputs. Un clic solicita un ataque, mantenerlo no repite. Guard usa Started para activar y Completed/Canceled para terminar.

No hay una cadena Move/DoMove ni Look/DoLook. Attack y Guard del controller resuelven el pawn actual y cruzan a otra clase, que conserva la responsabilidad de GAS; no cachean el personaje ni necesitan renovar bindings al cambiar posesión. La ausencia temporal de pawn se ignora; un tipo de pawn o una configuración de input incorrectos provocan una comprobación explícita.

La base no tiene cámara ni Tick de actor; CharacterMovement y la animación siguen actualizándose mediante sus componentes. GAS actualiza su información de actor al ganar/perder posesión para conservar el controller actual. La pose y los huesos se actualizan fuera de pantalla para conservar la posición del socket. Cámara del jugador: brazo de 400 cm, sigue ControlRotation y no orienta directamente la cápsula. Movimiento inicial: cápsula de 34 × 88 cm, velocidad 420 cm/s y rotación hacia el desplazamiento.

## Assets y documentación

Se agregan diez assets en Content/Vamps mediante herramientas MCP directas. No hay cambios nuevos de C++ en esta etapa, ni cambios en Config o clases del ejemplo Third Person. No se heredan Blueprints ni Animation Blueprints del ejemplo.

| Assets creados | Conexiones verificadas |
|---|---|
| `/Game/Vamps/Blueprints/BP_VampsCharacter` y `BP_VampsCharacter_Player` | Padres nativos VampsCharacter y VampsCharacter_Player. Manny Simple, Anim Class ABP_Duel_C, Attack Montage AM_BasicAttack y daño 12. Cámara solo en el jugador. |
| `/Game/Vamps/Blueprints/BP_VampsPlayerController` | Padre VampsPlayerController; referencias al contexto y las cuatro acciones nuevas. |
| `/Game/Vamps/Blueprints/BP_VampsGameMode` | Padre GameModeBase; personaje jugador y controller nuevos, HUD nativo. |
| `/Game/Vamps/Input/IA_Move`, `IA_Look`, `IA_Attack`, `IA_Guard` | Move/Look = Axis2D; Attack/Guard = Boolean. Sin triggers adicionales. |
| `/Game/Vamps/Input/IMC_Vamps` | Siete mappings: WASD, Mouse2D, LMB y RMB. Modificadores propios de Negate/Swizzle, con valores y referencias verificados. |
| `/Game/Vamps/Maps/L_CharacterBasic` | Copia del mapa propio L_Duel con suelo/iluminación nativos. Override BP_VampsGameMode, PlayerStart en (-220, 0, 100) y receptor base en (220, 0, 88,964), Yaw 180°. L_Duel permanece intacto. |

Recursos existentes reutilizados sin modificaciones: `/Game/Characters/Mannequins/Meshes/SKM_Manny_Simple`, `/Game/Duel/Animations/ABP_Duel`, `BS_Duel`, `AS_DuelAttack1` y `AM_BasicAttack`. ABP_Duel tiene padre DuelAnimInstance; su graph conecta BS_Duel mediante Speed, DefaultSlot y Output Pose, sin casts al personaje anterior. No contiene una pose de guardia implementada en esta etapa.

Mesh y montage comparten SK_Mannequin. FistHit ya existía en middle_metacarpal_r con transform local identidad; Codex verificó el socket y no lo creó ni modificó. El montage tiene un solo segmento AS_DuelAttack1 de un segundo, Rate/Loop Count = 1 y Auto Blend Out habilitado. El notify Attack Trace sigue sin verificar: los toolsets descubiertos no ofrecen operaciones de montage/notifies y ObjectTools rechazó la lectura de Notifies.

Las operaciones se hicieron mediante el protocolo MCP del endpoint local configurado en .codex/config.toml, con descubrimiento previo de toolsets y esquemas. ProgrammaticToolset solo orquestó llamadas a esas herramientas. No se usó SlateInspector, automatización de UI ni APIs de editor ajenas a las herramientas MCP. Guardado selectivo y consulta is_dirty confirmaron los diez assets guardados. El mapa nuevo quedó abierto, con PIE detenido.

Se agregan este reporte y editor.md, con enlaces desde el índice y los reportes anteriores relevantes. Se conservan las nuevas reglas de AGENTS.md agregadas por el usuario; Codex añade únicamente la regla solicitada de evitar shadowing de tipos y miembros de Unreal, incluidos los heredados. No se actualizan especificaciones funcionales ni técnicas.

## Verificación y pendientes

- Revisión estática de bindings, cambio de posesión, activación/exclusión de habilidades, cancelación al bloquear y recorrido montage → notify → barrido → daño.
- APIs de BindAction con parámetros adicionales, posesión, AddMovementInput, registro automático de AttributeSets, despacho del BeginPlay de Blueprint y actualización de pose contrastadas con el código instalado de UE 5.8.
- Config/DefaultInput.ini ya selecciona EnhancedPlayerInput y EnhancedInputComponent. No se cambió configuración.
- clang-format aplicado a los once archivos C++ modificados. `git diff --check` sin errores de whitespace.
- El usuario obtuvo C4458 en Move, Attack y Guard: las variables locales Character ocultaban AController::Character. Se renombraron las tres a ControlledPawn; el usuario confirmó la compilación después de esa corrección y nuevamente tras el cambio de nombre del controller.
- Los cuatro Blueprints nuevos compilaron mediante BlueprintTools.compile_blueprint y sus padres fueron consultados. Configuración de input y defaults verificados por lectura después de la escritura y también, para los componentes del personaje, en PIE.
- PIE inició por MCP: se encontraron jugador, receptor y controller nuevos. GAS concedió GA_Attack y GA_Guard a nivel 1 en ambos personajes; vida inicial 100/100, habilidades inactivas y sin tags activos. Cámara solo en el jugador; brazo 400 cm, pose actualizada fuera de pantalla, puño con radio 12 cm y NoCollision. LogPlayLevel no devolvió errores ni warnings. Se detuvo PIE y se confirmó IsPIERunning = false.
- El primer intento de instanciar modificadores no produjo referencias válidas; se corrigió usando refPath de clase y se verificaron los objetos creados y sus ejes antes de la entrega. La captura de viewport durante PIE falló y no se cuenta como verificación visual.
- No se compiló C++ nuevamente ni se ejecutó Live Coding, porque no hubo nuevos cambios de código. No se ejecutaron tests automatizados ni se agregó un arnés de inputs.

Pendiente: revisar el notify y su ventana de contacto, el ajuste visual del socket y comprobar movimiento, mirada, ataque, daño y bloqueo según [editor.md](editor.md). El MCP no ofrece entrega directa de estos inputs de juego ni acceso al notify; esos pasos quedan para el usuario. El alcance es ejecución local; no se implementa flujo de input o inicialización de GAS para multijugador.
