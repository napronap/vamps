# Implementación del combate de referencia

Reglas: [functional.md](functional.md). La implementación de cadena/NA y los resultados del paso 3 usan clases propias sobre Unreal/GAS. [033](../reports/033-resultados-combate/reporte.md) conserva código, configuración, compilación y límites de su observación inicial; [035](../reports/035-revision-paso-3/reporte.md) registra la configuración vigente y el cierre aprobado por el usuario.

## Responsabilidades

| Pieza | Responsabilidad |
|---|---|
| `AVampsCharacter` / `FVampsCombatState` | Fase ofensiva y NA, reacción separada, memoria defensiva, solicitud con vencimiento y permisos derivados. Tick autoritativo finaliza reacción y recupera guardia. |
| `UGA_Attack` | Un montage por cadena y secciones ordenadas. El cierre normal conserva NA; CancelCombatOffense invalida la reproducción antes de cancelar la habilidad para que su callback no programe NA. |
| `AVampsWeapon` / `UANS_AttackTrace` / `UAN_ResetWeaponHits` | Un ANS por sección, trazas y víctimas independientes por arma, con reinicios opcionales. Interrumpir desactiva trazas; un reinicio no cambia identidad ofensiva. |
| `UVampsCombatResolver` / `FVampsPendingContact` | Captura datos del contacto, clasifica el lote antes de consecuencias, registra contacto habilitante y aplica daño/reacción antes de continuación y locomoción. |
| `UVampsCombatSettings` / `FVampsCombatStepSettings` | Duración total por paso y parámetros de crítico, bloqueo, acumulación, recuperación, stun y vigencia de solicitud. HasValidReactionSettings diagnostica configuración inválida sin cerrar el editor. |
| `UVampsAttributeSet` / `UGE_VampsDamage` | Health/MaxHealth y GuardLoad/MaxGuardLoad replicados. Daño instantáneo vía GameplayEffect; acumulación/recuperación por el ASC. |
| `UVampsCombatMovementComponent` | Movimiento impuesto y root motion con colisión; elimina root motion de reacción y orienta hacia la fuente del desplazamiento. |
| Movilidad de 4 | Advance/Dash en el componente existente, energía GAS independiente y permisos en el personaje; [implementación de jugador](../player/technical.md). |
| `UVampsAnimInstance` / `AVampsCombatTestHUD` | Exponen reacción efectiva, duración, permisos, guardia, daño y crítico sin decidir disponibilidad. |

Jugador e IA usan RequestCombatAction y AddMovementInput del personaje. No se añadió otra ruta de input, daño o validación ni infraestructura de red, cartas o extensiones posteriores.

## Reacción, permisos y animación

Reaction y ReactionEndsAt son el único plazo de la reacción; bOffenseRestricted deriva de ese estado. Guard/Hit detienen locomoción voluntaria y ataque. Stun y NA tienen sus restricciones funcionales. La solicitud defensiva usa su propio vencimiento porque representa intención, no tiempo de bloqueo.

CancelCombatOffense cierra trazas, reproducción y continuación sin borrar la cola de contactos. Si existía NA, conserva su plazo; recibir un golpe no la crea ni la cancela. Reset incrementa CombatGeneration, limpia cola, habilidades, montages, movimiento, memoria y guardia. La recepción letal detiene la ofensiva y movimiento, sin implementar respawn.

HitReactionMontage y GuardReactionMontage son referencias independientes en la categoría Reaction del DA. Hit elige el primero y Guard el segundo; no pueden apuntar al mismo asset. Su asignación es opcional y ninguno se usa como sustituto del otro. Cuando están asignados se reproducen desde el personaje, con velocidad ajustada a la duración configurada y compensación de RateScale. Se validan animación, longitud, velocidad constante positiva, Auto Blend Out y separación de AttackMontage. Sustituir reacción detiene su reproducción anterior; su root motion no añade desplazamiento. [Corrección y configuración 034](../reports/034-montage-guardia/reporte.md).

AnimInstance publica bIsGuarding, bIsHitReacting, bIsStunned y CombatReactionRemaining. bIsGuarding ya no equivale a bCanDefend. El HUD de observación muestra el estado incluso sin clips de reacción autorados. Las poses y transiciones visuales del AnimBP que carezcan de MCP directo quedan como pasos manuales en la guía 033.

## Contexto y resolución del contacto

La cola captura emisor, receptor, ActionNumber, generaciones, tiempo, daño normal/crítico elegido, reducción del defensor, carga de guardia y settings completos del paso, además de origen/dirección. El crítico se decide con autoridad una vez por contacto deduplicado, no por muestra del barrido. La recepción no vuelve a leer daño de una acción posterior.

La memoria defensiva usa emisor, ActionNumber y generación. ActionNumber cambia por paso: otro paso requiere nueva admisión; armas y reinicios del mismo paso conservan identidad. La memoria se limpia con la salida de Guard, daño abierto, stun o reset. La [política funcional del lote](functional.md#guardia-daño-y-reacción) define orden, una admisión nueva por pulsación y prioridades de reacción; no usa el valor del ID para elegir ganador.

FinalImpactWeaponIndex y FinalImpactIndex se autoran por paso. AVampsWeapon mantiene un índice de subimpacto: cero al empezar sección e incremento en ResetImpactHits. QueueCombatContact recibe arma/índice y captura bFinalImpact. UGA_Attack valida que el arma exista y que el índice coincida con su último reinicio autorado dentro del ANS. Es identidad del subgolpe seleccionado, no contador de impactos acertados ni un final inferido por número de combo.

El resolver conserva la prioridad de reacción. Solo Guard puede recibir bGuardDashReady al incluir el impacto final bloqueado de la instancia que determina esa reacción. Una reacción posterior sustituye ese permiso; Hit/Stun/reset/muerte lo limpian. Un golpe final fallido no crea señal y Guard vence por su plazo normal. Los contactos capturados siguen resolviéndose antes de la solicitud de dash.

AdvanceDistance, AdvanceDuration y AdvanceStopDistance pertenecen al atacante. BeginOffensiveAdvance captura el target actual y un destino acotado por paso; no persigue cambios posteriores. El componente reemplaza la raíz ofensiva por ese avance controlado, con giro limitado a 500 grados/s y detención ante colisión. TargetAlignmentDistance y KnockbackDistance siguen siendo desplazamientos del receptor y no se usan como máximo de avance.

Los contactos capturados sobreviven a una interrupción ordinaria y pueden producir intercambio letal del mismo frame. Reset invalida generaciones; destrucción y receptor ya eliminado impiden aplicación. El registro habilitante se realiza antes de materializar interrupciones y solo corresponde al paso capturado. Una única reacción/desplazamiento por receptor evita sumar empujes por orden accidental de barridos.

## Datos y configuración

ContactReactionDuration sustituye a AdditionalOffensiveRestrictionDuration. Es duración total, sin cálculo por longitud de sección ni mínimo del clip. No se conserva la propiedad retirada ni se agrega compatibilidad. La configuración anterior se captura antes de cambiar el struct y se restaura por MCP tras el arranque limpio, incorporando explícitamente los valores nuevos; [captura y comparación 033](../reports/033-resultados-combate/reporte.md).

AttackDamage sigue en el personaje. CriticalDamage, CriticalChancePercent y los parámetros de guardia están en el DA; crítico y daño normal usan la misma resta plana del defensor. GuardLoadPerHit pertenece al atacante; capacidad, recuperación y StunDuration al defensor. No se suman stats sin función ni fórmulas de escalado. Los valores iniciales elegidos y los límites de verificación se conservan en el reporte.
