# 004 — Un ataque y una guardia

**Continuación:** [005 — IA básica](../005-ia-basica/reporte.md) agrega nuevamente un controlador simple y F1. Este reporte conserva el alcance de 004; para configurar el estado actual, seguir la [guía 005](../005-ia-basica/editor.md).

**Fecha:** 2026-10-05. Cambios de código y documentación; assets sin editar.

Esta iteración reemplaza la configuración de [003](../003-combate-animado/reporte.md). La [guía vigente](editor.md) prepara un único ataque y una guardia mantenida. No seguir los pasos de combos, guardia animada por ventanas ni NA de las iteraciones anteriores.

## Resultado

`AVampsDuelCharacter::ReceiveHit` contiene las dos reglas:

1. Receptor en guardia: cancelar la habilidad de ataque del agresor, incluyendo su montage y trazas; no aplicar daño.
2. Receptor sin guardia: aplicar el Gameplay Effect de daño.

La guardia cubre todas las direcciones y no se consume. Un ataque no produce una cadena. Recibir daño no empuja ni interrumpe al receptor. No se agregó muerte ni fin de partida.

Se conservan movimiento, cámara, lock-on, reinicio y HUD de vida/estado. El HUD consulta GAS para mostrar IDLE, ATTACK o GUARD; no guarda un enum ni una copia del estado del combate.

## Código

Las rutas siguientes son relativas a `Source/vamps/Public/` y `Private/`, con pares .h/.cpp salvo indicación.

| Archivo / clase | Cambio |
|---|---|
| `Characters/VampsDuelCharacter` | Solo concede dos habilidades, recibe solicitudes y resuelve ReceiveHit. AttackMontage y AttackDamage están directamente en el personaje. Conserva objetivo, cámara, movimiento y reinicio |
| `Combat/Abilities/GA_Attack` | Ahora hereda directamente de UGameplayAbility. Usa UAbilityTask_PlayMontageAndWait; termina al finalizar o interrumpirse la animación. EndAbility detiene trazas |
| `Combat/Abilities/GA_Guard` | Hereda directamente de UGameplayAbility. Su tag dura desde presionar hasta soltar; sin montage, timers ni ventanas |
| `Combat/CombatTags` | Solo Attack, Guard y Damage. Los dos primeros indican habilidades activas y bloquean su activación simultánea |
| `Combat/WeaponTraceComponent` | Propiedades locales de sockets/radio, barridos y receptores ya alcanzados. Un mismo ataque daña una sola vez a cada receptor; un bloqueo corta los barridos restantes |
| `Animation/ANS_AttackTrace` — nuevo | Reemplaza el notify genérico. Solo enciende/apaga trazas y descarta señales de una reproducción anterior |
| `Animation/DuelAnimInstance` | Retira Phase/StrikeIndex; conserva locomoción y expone bIsGuarding exclusivamente para una pose visual opcional |
| `Player/DuelPlayerController` | RMB Started activa; Completed/Canceled termina guardia. Retira F1/IA. Conserva lock-on y cámara; movimiento directo desde input |
| `GameModes/DuelGameMode` | Conserva dos personajes y reinicio; retira SharedConfig y la secuencia de IA |
| `UI/DuelHUD.cpp` | Conserva barras de vida y estado actual. Retira fases, combo, resultados, NA, pausa y victoria/derrota |

Sin cambios en AttributeSet ni Gameplay Effect: continúan almacenando vida y aplicando el daño mediante GAS.

**Eliminados:** pares `GA_NA`, `GA_HitReaction`, `DuelGameplayAbility`, `CombatRules`, `Combat/Data/CombatConfig`, `AI/DuelAIController` y `Animation/ANS_DuelWindow`: 14 archivos. También desaparecen enums de fase/resultado/modo IA, strikes, contadores de contactos, log de última acción, ventanas de combo/guardia/respuesta, pausas, ángulos defensivos y empujes. El subsystem y buffer ya habían sido eliminados en 003 y no se reintroducen.

No se modificó ni se reutilizó código de las variantes Third Person.

## Conexiones de assets

Los .uasset permanecen intactos. La revisión corresponde a la [guía](editor.md):

- `/Game/Duel/Blueprints/BP_DuelCharacter`: conserva padre y componentes; debe asignar AttackMontage y AttackDamage directamente. Resetear referencias a la IA eliminada.
- `/Game/Duel/Blueprints/BP_DuelPlayerController`: conserva Move, Look, Lock y Reset. No hay AI Mode ni Shared Action.
- `/Game/Duel/Blueprints/BP_DuelGameMode`: conserva clases de personajes/controller/HUD; deja de tener SharedConfig.
- `/Game/Duel/Animations/ABP_Duel`: reparar usos de Phase/StrikeIndex si existen. bIsGuarding puede controlar una pose; no decide defensa.
- `/Game/Duel/Animations/AM_DuelAttack`: no debe seguir reproduciendo tres golpes. La guía propone un montage nuevo con una sola secuencia, sin editar este asset automáticamente.
- `/Game/Duel/Data/DA_DuelCombat`: ya no tiene una clase C++ vigente ni lo utiliza el código. Retirar referencias antes de borrarlo manualmente.
- `/Game/Duel/Input/IMC_Duel`: puede limpiar IA_DuelAIMode/IA_DuelShared; conservar IA_DuelLock.
- `/Game/Duel/Maps/L_Duel`: escenario sin modificaciones; el segundo personaje es estático.

## Documentación

Combate funcional/técnico, juego funcional/técnico, roadmap, convenciones y el índice se ajustaron a esta base en 004. Los reportes anteriores se conservan como historial en sus carpetas. La explicación actual para empezar a leer está en [Cómo leer el código del prototipo](../../prototype/technical.md#cómo-leer-el-código). [017](../017-base-fgoa-y-extensiones/reporte.md) separa ese prototipo de las especificaciones objetivo.

## Verificación

Revisión de referencias a símbolos eliminados y firmas de las tareas GAS contra la instalación local de UE 5.8. Se aplicó clang-format 20 a los 19 archivos C++ modificados/nuevos que permanecen en el árbol; git diff --check no señaló errores de whitespace. Se revisaron 86 enlaces locales de archivos, sin destinos faltantes. No quedan reportes sueltos en reports.

La búsqueda en Public/Private no encontró referencias a los tipos y funciones retirados. Las dos implementaciones de habilidades tienen 62 líneas (Attack) y 22 (Guard), incluido el ciclo nativo de GAS; no necesitan la base personalizada anterior.

No se ejecutaron tests automatizados. No se compiló ni se ejecutó PIE: no hay herramientas MCP directas de Unreal expuestas en esta sesión. No se afirma validación jugable, Live Coding ni aplicación de parche. La guía deja compilación completa y configuración manual pendientes.

El código permanece orientado al prototipo local: no se agregó RPC de input ni predicción. Las trazas aproximan el movimiento entre poses y no reconstruyen ventanas perdidas durante hitches.
