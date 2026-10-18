# 033 — Resultados de daño y defensa

La presentación de Guard se corrige posteriormente en [034](../034-montage-guardia/reporte.md): GuardReactionMontage independiente de HitReactionMontage. Este reporte conserva las verificaciones y configuración de la implementación inicial.

## Resultado — 2026-10-18

Se implementaron Guard, reacción a daño abierto, residual letal, acumulación de guardia, ruptura con stun y crítico. Se conserva la cadena de 2: montage por secciones, continuación por ventana/contacto, NA y trazas independientes por arma con AN_ResetWeaponHits. La cancelación por reacción no genera NA y los contactos capturados conservan sus datos.

El build completo fue correcto, el editor se cerró/reabrió por los scripts autorizados, el DA se preservó y se verificó el arranque de los dos personajes en PIE. Los ajustes finales de IA/movimiento y del montage opcional de daño fueron compilados y aplicados mediante parches reales de Live Coding. **El paso 3 queda pendiente de cierre manual de los intercambios y presentación**, no de compilación ni de restaurar el DA. Unreal queda abierto y PIE detenido. [Guía y pendientes](editor.md).

## Reglas y decisiones

Las reglas completas se integraron en [combate funcional](../../combat/functional.md) y sus responsabilidades en el [técnico](../../combat/technical.md). Se usó la skill game-feature-docs. Las decisiones menores autorizadas quedan resueltas así:

- Solicitud defensiva con vencimiento configurable; una interacción recordada por emisor, paso y generación. Cada nuevo paso requiere admisión, mientras armas y reinicios del mismo paso comparten identidad. Salir de Guard, recibir abierto, stun o reset limpia memoria.
- Una pulsación puede admitir una sola instancia nueva del lote, prefiriendo una distinta de la recordada. Entre candidatas se prioriza target, orden de candidatos y nombre del actor; no el valor del ID. Bloquear B reemplaza memoria de A, sin quitar los ataques de A.
- El lote se clasifica antes de daño/cancelaciones. Se proyecta la carga para que el golpe que llena la barra siga bloqueado y cause stun; los posteriores quedan abiertos. Una reacción y un desplazamiento por receptor, con prioridad ruptura > abierto > bloqueo. En simultaneidad, el contacto elegido determina fuente, plazo y movimiento.
- Se conservan contactos capturados al interrumpir. Pueden producir intercambio letal del mismo frame; no se aplica daño adicional a un receptor ya eliminado. Destrucción o reset invalida los contextos correspondientes. No se habilita otra generación con un contacto antiguo.
- Cada contacto aceptado fuera de simultaneidad renueva la reacción desde su tiempo capturado. Guard/Hit comparten duración total por paso; Stun usa su duración propia y no se prolonga por recibir otros golpes. Reacciones restringen ataque y movimiento voluntario; mantienen el desplazamiento impuesto con colisión.
- GuardLoad suma por contacto bloqueado, incluidas armas independientes. Recupera fuera de reacción, tras el retraso desde el último bloqueo. Al terminar stun se limpia a cero. El crítico se tira por contacto de arma/receptor deduplicado y no modifica carga, reacción ni las reglas de bloqueo.

No se agregaron stats sin uso, compatibilidad con la propiedad retirada, rutas de recuperación, cartas, movilidad de 4, equipos/respawn ni infraestructura de red. Las cantidades iniciales son decisiones editables de esta implementación, no mediciones del original.

## Clases y archivos

Rutas de código relativas a Source/vamps/; los headers están en Public y los .cpp en Private.

| Clase / archivos | Cambio y conexiones |
|---|---|
| Combat/Data/VampsCombatSettings.h y nuevo .cpp — UVampsCombatSettings / FVampsCombatStepSettings | ContactReactionDuration sustituye el adicional. Agrega parámetros de crítico/guardia y validación no fatal HasValidReactionSettings. El resolver captura los datos del atacante y del defensor desde sus DA. |
| Combat/VampsCombatState.h — FVampsCombatState / EVampsCombatReaction | Agrega None/Guard/Hit/Stun, plazo, permisos, solicitud, memoria/fuente y último daño/crítico. Fase ofensiva y NA permanecen separadas. |
| Characters/VampsCharacter.h/.cpp — AVampsCharacter | Tick autoritativo finaliza reacción/solicitud y recupera carga. CancelCombatOffense invalida montage y continuación antes de cancelar la habilidad. Recepción aplica daño capturado mediante GE y carga mediante ASC. ApplyCombatReaction detiene movimiento voluntario y refleja el resultado. Reset y baja limpian su contexto. |
| Combat/VampsCombatResolver.h/.cpp — UVampsCombatResolver / FVampsPendingContact | Snapshot de daño, crítico, reducción, carga, tiempo, distancias y generaciones. Clasifica antes de consecuencias, conserva independencia por arma y resuelve daño/reacción antes de continuación y locomoción. |
| Combat/Abilities/GA_Attack.cpp — UGA_Attack | Valida settings nuevos y duración total positiva. Conserva el montage, sus notifies, ventanas y cierre normal. La interrupción por reacción no alcanza la ruta que crea NA porque el personaje ya invalidó la reproducción. |
| Combat/Attributes/VampsAttributeSet.h/.cpp — UVampsAttributeSet | Agrega GuardLoad y MaxGuardLoad, con límites y replicación; mantiene Health/MaxHealth. El personaje configura capacidad desde su DA. GE_VampsDamage sigue siendo el efecto instantáneo de vida sin cambios. |
| Combat/VampsCombatMovementComponent.h/.cpp — UVampsCombatMovementComponent | Conserva alineación/empuje con colisión. Giro hacia la fuente también durante empuje; suprime root motion de reacción. La reproducción opcional de HitReactionMontage pasa al personaje para ajustar su velocidad al plazo. |
| Animation/VampsAnimInstance.h/.cpp — UVampsAnimInstance | bIsGuarding refleja Guard efectivo. Agrega bIsHitReacting, bIsStunned y CombatReactionRemaining para presentación. |
| AI/VampsAIController.cpp — AVampsAIController | Guard distingue solicitud armada de interacción recordada, para poder emitir otra pulsación contextual. Sigue solicitando mediante RequestCombatAction y usa los permisos compartidos. |
| UI/VampsCombatTestHUD.cpp — AVampsCombatTestHUD | Agrega barra de carga, reacción/plazo, permiso de movimiento, memoria y daño/crítico. No decide resultados. |
| Documentación | Actualiza combate, roadmap, índice y enlaces desde 031–032; repara destinos de secciones resueltas en las guías históricas 016–017. Crea reporte, guía y evidencias 033. |

No se modificaron clases padre ni grafos de BP/AnimBP. Los personajes `/Game/Vamps/Blueprints/BP_VampsCharacter_Player` y `BP_VampsCharacter` conservan sus conexiones y la misma BP_VampsGreatSword equipada. Los ANS y AN de reinicio conservan su autoría en AM_Executioner_Combo.

## DA preservado y valores nuevos

MCP encontró una clase nativa de settings, sin derivadas, y un asset: `/Game/Vamps/Data/DA_VampsCombatSettings`. Se capturaron las cinco propiedades completas y se guardó el asset antes de modificar el struct. Evidencia: [antes](data-assets-antes.json), [reapertura](data-assets-reapertura.json) y [después](data-assets-despues.json).

Se conservan Weapons[0] = BP_VampsGreatSword, HandGrip_R, transform identidad; AttackMontage = `/Game/Vamps/Animations/Executioner/AM_Executioner_Combo`; NA = 0,5 s y HitReactionMontage = None. Los tres pasos mantienen alineación/empuje 130/100, 130/100 y 150/250 cm, en el mismo orden. No se agregó ni quitó un arma ni una sección.

| Parámetro nuevo | Valor guardado | Uso |
|---|---:|---|
| ContactReactionDuration, pasos 1–3 | 0,5 s cada uno | Duración total de Guard/Hit. No es el antiguo adicional ni sección más 0,5. |
| CriticalDamage / CriticalChancePercent | 24 / 10 % | Daño crítico plano y probabilidad del atacante. AttackDamage sigue en el personaje; ambos personajes vivos conservaron 12. |
| BlockDamageReduction | 10 | Resta plana del defensor: abierto normal/crítico 12/24, defendido 2/14 con esta configuración. |
| GuardCapacity / GuardLoadPerHit | 100 / 25 | Capacidad del defensor y carga por contacto bloqueado del atacante. |
| GuardRecoveryDelay / GuardRecoveryPerSecond | 1 s / 20 | Retraso desde el último bloqueo y reducción por segundo fuera de reacción. |
| StunDuration | 2 s | Stun del defensor; salida con carga cero. |
| DefenseRequestDuration | 0,5 s | Vigencia de la pulsación no consumida. |

Después del arranque limpio, las referencias y distancias ya coincidían; se reaplicaron expresamente arrays/configuración preservada y campos nuevos por MCP. Las 14 propiedades finales coincidieron exactamente con lo esperado, incluyendo arrays anidados y transforms. Solo se guardó el DA afectado. get_class devolvió `/Script/vamps.VampsCombatSettings` e is_dirty=false; la relectura después de PIE y Live Coding mantuvo igualdad.

## Compilación, editor y observación

- La consulta global de dirty assets falló al encontrar la referencia inexistente `/Game/__ExternalObjects__/ThirdPerson/Lvl_ThirdPerson/0/TQ/1UP2MCOIYXC0GTW985R4K0`. No se supuso éxito global ni se repitió el mismo barrido. Se comprobó por MCP el conjunto de 35 assets de Vamps y abiertos, todos sin cambios pendientes.
- Cierre por shell autorizado: PID 58916, identificado por línea de comandos del proyecto, salió ordenadamente en 3 s. No se forzó ni se cerraron otras sesiones. [Estado](editor-close.json).
- Build completo por build.bat: código 0, `Result: Succeeded`, 46,14 s; UHT procesó las nuevas estructuras, 12 acciones de compilación/enlace. Límite 15 min, sin alcanzarlo. [Estado](build-1.json), [stdout](build-1.stdout.log), [stderr](build-1.stderr.log).
- editor.bat se ejecutó solo después de comprobar el éxito. PID nuevo 64744; L_CharacterBasic cargado, inicialización 13,96 s y MCP reconectado dentro de 5 min. La sesión MCP anterior requirió reinicialización automática; después las consultas funcionaron. [Estado](editor-launch.json).
- StartPIE/StopPIE por MCP: dos personajes Idle, Reaction=None, bCanMove/bCanStartAction=true, HP 100/100, GuardLoad 0/100, una espada cada uno y referencia al DA preservado. [Observación](observacion-pie.json). No se simuló input ni se forzaron contactos, estados o atributos.
- Ajuste de IA y eliminación de dos asignaciones duplicadas de giro: CompileLiveCoding devolvió Success y aplicó UnrealEditor-vamps.patch_0. [Salida](live-coding.log). La revisión posterior corrigió un detalle concreto del montage opcional: detener la reproducción anterior al sustituir reacción y compensar RateScale para respetar el plazo. La compilación final devolvió Success y aplicó patch_1; [salida final](live-coding-final.log). Ambos fueron parches reales, no NoChanges.
- Editor/PIE sin errores LogVampsCombat, Blueprint, PIE o fatales en la [captura del log](editor-validation.log). PIE quedó detenido. Tres compilaciones en total; no hubo reintentos por fallo ni timeouts. Las operaciones de shell tuvieron escalación revisada y aprobada; assets/PIE/configuración se realizaron por MCP.
- clang-format aplicado a los archivos propios modificados y revisado. Se revisaron nombres frente a las clases base, salidas normal/interrumpida, captura por generación, límites, exclusión de daño tras baja y uso de permisos. No se agregaron comentarios, asserts fatales ni tests automatizados. Se revisaron diff, whitespace y enlaces.

## Límites y siguiente comprobación

El arranque y la configuración están verificados; los intercambios Guard/Hit, multihit, ruptura/stun, residual letal, críticos, A→B→A y las transiciones visuales requieren observación manual. No se presenta revisión estática ni compilación como prueba de esos resultados. La escena observada conserva un solo rival y no se agregó un fixture para varios agresores.

HitReactionMontage sigue vacío por preservación. El código admite su asignación opcional y expone Guard/Hit/Stun al AnimBP, pero no se eligieron clips ni se autoraron poses/transiciones nuevas. No se usó UI ni scripts del editor como atajo. Los pasos concretos y los parámetros que conviene calibrar están en la [guía](editor.md).
