# 021 — Solicitud compartida y fases de una acción

Continuación vigente: [022 — Cadena por secciones y NA](../022-cadena-y-na/reporte.md), con su [guía del editor](../022-cadena-y-na/editor.md). Conserva este cierre de etapa 1 y agrega la implementación de etapa 2, todavía pendiente de configuración y comprobación jugable.

## Resultado de la revisión — 2026-10-16

La etapa 1 del [roadmap](../../roadmap.md#1-botón-compartido-y-fases-de-una-acción) está **completada por confirmación del usuario el 2026-10-16**. Se contrastaron las fuentes del commit `91db881`, los assets cargados y la aclaración del usuario: **un solo ANS delimita el ataque; la defensa termina en AttackEnd y Recovery ocupa el resto del montage. RMB no inicia otro ataque durante Recovery.** Con esta regla, no faltan ContinuationWindow ni AN_CombatRecovery para esta etapa y no se encontró una brecha nativa en esas fronteras.

No hubo cambios de código, configuración ni assets. Se consolidaron este reporte y su [guía del editor](editor.md), y se actualizaron el roadmap y el índice con el cierre y las fronteras aclaradas. No se modificaron las especificaciones funcionales/técnicas.

## Cierre de la etapa 1

**Hecho — usuario, aceptación de la etapa, 2026-10-16.** El usuario indicó «ok ya podes marcar este punto como terminado» después de aclarar los tiempos y revisar la explicación del sistema de ataque/defensa. Se registra esa aceptación como cierre; no se atribuye a Codex una ejecución nueva de PIE ni al usuario un desglose de comprobaciones que no comunicó. La [guía](editor.md) conserva los pasos para reproducir la configuración y el intercambio. La siguiente etapa es cadena y NA; no se inició su implementación.

## Reglas acordadas

- Cada click de RMB inicia una acción desde Idle y arma defensa desde el inicio hasta AttackEnd, salvo consumo previo. Otro click antes de AttackEnd puede rearmarla; no acumula duración ni inicia un segundo ataque.
- El primer contacto entrante consume defensa. Los contactos del mismo frame se evalúan juntos con el estado previo y consumen juntos la solicitud.
- Un único ANS_AttackTrace define AttackStart y AttackEnd. Preparation va desde el inicio hasta AttackStart; Contacts, desde AttackStart hasta AttackEnd; Recovery, desde AttackEnd hasta MontageEnd. La defensa es válida durante Preparation y Contacts. El comienzo del blend-out no determina esas fases.
- Recovery no permite ataque ni defensa, no es NA y no limita locomoción. La animación restante puede blendear con otras acciones; específicamente RMB espera a que termine el montage para iniciar otro ataque. El código actual descarta los clicks durante Recovery, sin buffer ni ataque automático al finalizar.
- Bloquear retargetea al agresor y conserva las acciones de ambos participantes. Cadena y NA pertenecen a la etapa 2.

## Qué está presente

Rutas relativas a `Source/vamps/`; no se modificaron estas clases durante la revisión.

| Clases y archivos | Responsabilidad y conexiones comprobadas en código |
|---|---|
| `Public/Combat/VampsCombatState.h` | Fases Idle, Preparation, Contacts y Recovery; disponibilidad, solicitud, resultados y contadores visibles para el HUD. Suficientes para esta acción de un paso |
| `Public/Characters/VampsCharacter.h`, `Private/Characters/VampsCharacter.cpp` | `RequestCombatAction` activa `GA_Attack` o rearma defensa antes de Recovery. Gestiona reproducción, fases, trazas y recepción de daño por GAS. Desarma defensa en AttackEnd y rechaza RMB durante Recovery |
| `Public/Combat/Abilities/GA_Attack.h`, `Private/Combat/Abilities/GA_Attack.cpp` | Exige exactamente un Attack Trace y Auto Blend Out; reproduce el montage y finaliza al completar, interrumpir o cancelar. No usa OnBlendOut para terminar la acción ni necesita Combat Recovery |
| `Private/Animation/ANS_AttackTrace.cpp` | Branching Point nativo; entrega inicio/final de contactos al personaje. Su final conduce directamente a Recovery |
| `Private/Combat/FistHitboxComponent.cpp` | Muestrea/sweepea el puño, descarta obstrucciones y deduplica por acción; encola contactos en el resolver |
| `Public/Combat/VampsCombatResolver.h`, `Private/Combat/VampsCombatResolver.cpp` | Subsystem de mundo; evalúa contactos en OnWorldPostActorTick, determina bloqueos antes de consumir defensa y aplica retarget/resultados después. Los contactos recíprocos no se eliminan por el primer daño del lote |
| `Private/Player/VampsPlayerController.cpp` | Enlaza CombatAction Boolean a Started y entrega RequestCombatAction; mantener RMB no genera solicitudes nuevas desde este binding |
| `Private/AI/VampsAIController.cpp` | Attack y Guard entregan la misma solicitud. Guard también puede iniciar ofensiva y respeta el rechazo de solicitudes durante Recovery |
| `Private/UI/VampsCombatTestHUD.cpp` | Presenta fase, número de acción, disponibilidad, defensa, solicitud y resultados con frame. Usa el mismo lock-on que la cámara |
| `Private/GameModes/VampsCombatTestGameMode.cpp` | R limpia combate, contactos pendientes, vida, posiciones y movimiento del jugador y rival creado por el GameMode |
| `Private/Animation/VampsAnimInstance.cpp` | Presenta bCanDefend como estado de guardia; no decide bloqueo ni daño |
| `Combat/VampsCombatTags.*`, `Combat/Effects/GE_VampsDamage.*` | No hay habilidad/tag de guardia mantenida; daño abierto conserva GAS. Recibir bloqueo no cancela al agresor |

No quedan consumidores de RequestAttack, RequestGuard, AttackAction, GuardAction ni GA_Guard en Public/Private. El resolver no requiere un Blueprint ni un actor colocado en el mapa.

## Límites de la verificación de Codex

1. **Notifies del montage.** ObjectTools expone duración, segmento y blends, pero no Notifies ni CompositeSections. El catálogo no ofrece edición directa de notifies de montage. Codex no verificó sus tiempos; la guía conserva su revisión manual como referencia reproducible. No se requiere Combat Recovery.
2. **Intercambio en PIE.** Codex no inició PIE ni observó consumo/rearme, rechazo de RMB durante Recovery, retarget o simultaneidad. El código respalda las reglas; el cierre se apoya en la aceptación del usuario, no en una ejecución MCP de esos casos.
3. **Locomoción y blend.** La fase no cambia el modo de movimiento ni bloquea el input de locomoción. Codex no observó la transición visual del montage. Las acciones futuras se integrarán cuando existan; no se agregan ahora bindings para ellas.

Se retiran los hallazgos previos sobre falta de ventana/notify y defensa demasiado corta: se basaban en una regla sustituida por la aclaración del usuario. El control por ID de reproducción existe dentro de SetCombatContacts, sin necesitar el helper OwnsCombatNotify. La ausencia de las clases citadas en el reporte anterior no acredita pérdida de implementación: no son necesarias con la regla actual. No se atribuye a esta sesión el parche Success mencionado anteriormente.

## Assets y verificaciones MCP actuales

- Editor Unreal 5.8.3; mapa actual `/Game/Vamps/Maps/L_CharacterBasic`; PIE detenido.
- `/Game/Vamps/Input/IA_Combat`: Boolean, triggers y modifiers vacíos.
- `/Game/Vamps/Input/IMC_Vamps` e `IMC_VampsCombatTest`: seis y ocho entradas en **defaultKeyMappings.mappings**, sin overrides de perfil. RMB corresponde a IA_Combat; no hay mapping de LMB/IA_Attack ni IA_Guard. Se conservan WASD/F y F6/R en el contexto de prueba. La propiedad antigua mappings aparece vacía y no significa que falten las entradas.
- `/Game/Vamps/Blueprints/BP_VampsPlayerController` y `BP_VampsCombatTestPlayerController`: CombatAction=IA_Combat y sus respectivos contextos; Move, CycleTarget y CameraManager asignados. El de prueba conserva CycleAIMode y ResetCombat.
- `/Game/Vamps/Blueprints/BP_VampsCharacter`: AttackMontage=AM_BasicAttack, AttackDamage=12. La reflexión de CombatState coincide con la estructura actual de cuatro fases.
- `/Game/Vamps/Animations/AM_BasicAttack`: duración 1 s, Rate Scale 1, un segmento AS_BasicAttack1 en DefaultSlot, una repetición; Auto Blend Out activo, Blend Out 0,25 s y Blend Out Trigger Time -1. Secciones y notifies sin verificar.
- `LiveCodingToolset.LiveCodingToolset.CompileLiveCoding`: **NoChanges**. No hubo compilación ni aplicación de un parche nuevo; tampoco acredita una compilación completa desde cero.
- `BlueprintTools.compile_blueprint` con warnings_as_errors=true completó las ocho compilaciones sin errores: ABP_VampsCharacter, ambos personajes, ambos controllers, BP_VampsAIController y ambos GameModes. La consulta de log no devolvió errores/warnings de Blueprint. Los ocho y el montage quedaron con is_dirty=false; no fue necesario guardar assets.
- No se ejecutaron tests automatizados. Git estaba limpio antes de actualizar estos reportes.

## Continuidad

No quedan tareas de cierre para la etapa 1. La [guía](editor.md) queda como referencia reproducible. El roadmap y el índice reflejan el cierre y el uso de un único ANS. La cadena, sus ventanas y NA corresponden a la etapa 2, todavía pendiente. La etapa 0 conserva su cierre atribuido al usuario en [020](../020-roadmap-emulacion-fgoa/reporte.md#cierre-de-la-etapa-0).
