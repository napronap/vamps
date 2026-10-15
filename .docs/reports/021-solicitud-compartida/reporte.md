# 021 — Solicitud compartida y fases de una acción

## Resultado

Etapa 1 del [roadmap](../../roadmap.md#1-botón-compartido-y-fases-de-una-acción), iniciada el 15 de octubre de 2026. Se implementó el núcleo de un paso, el input compartido, la resolución conjunta de contactos, la adaptación de la IA y el debug. Se configuró y guardó el input por MCP, y se compilaron ocho Blueprints sin errores ni warnings. **Pendientes: agregar manualmente `Combat Recovery` al montage y comprobar el intercambio en PIE. La etapa no está cerrada.**

Con el MCP restablecido se creó `IA_Combat` y se modificaron ambos contextos y controllers. La corrección que desacopla las fases del blend-out compiló y aplicó un parche por Live Coding con `Success`; la consulta inicial había devuelto `NoChanges`. Las operaciones reproducibles y el paso sin soporte MCP quedan en la [guía](editor.md).

## Reglas elegidas por el usuario

- El click de RMB inicia una acción y arma una defensa hasta su final. Durante la acción, otro click puede rearmarla. No hay plazo fijo de 0,25 segundos ni acumulación de duración.
- El primer contacto entrante consume la solicitud. Los contactos posteriores requieren otra pulsación.
- Los contactos detectados en un mismo frame se resuelven juntos, con el estado previo de los participantes. Varios golpes simultáneos contra un defensor armado quedan bloqueados y consumen juntos su solicitud.
- Los tiempos pertenecen a la animación de cada personaje. Se usan notifies para delimitar contactos y ventana; la defensa puede usarse durante toda la acción propia, incluida recuperación. El final de la reproducción del montage termina su vigencia.

La continuación de cadena y NA pertenecen a la etapa 2. En esta entrega, una pulsación durante cualquier fase activa solo rearma defensa; no se guarda para iniciar otro ataque ni ejecuta un segundo paso.

## Código y responsabilidades

Rutas relativas a `Source/vamps/`; los headers están en `Public/` y las implementaciones en `Private/`.

| Archivos agregados o modificados | Responsabilidad y conexiones |
|---|---|
| Nuevo `Public/Combat/VampsCombatState.h` | Enums de fase (`Idle`, `Preparation`, `Contacts`, `ContinuationWindow`, `Recovery`), resultado de solicitud y resultado de contacto |
| `Public/Characters/VampsCharacter.h`, `Private/Characters/VampsCharacter.cpp` | `RequestCombatAction` sustituye las solicitudes separadas. Mantiene fase, defensa consumible, acción identificada, contacto habilitante y resultados; conserva GAS para daño abierto. `ResetCombatState` cancela habilidades, cierra trazas y elimina contactos pendientes del personaje |
| `Public/Combat/Abilities/GA_Attack.h`, `Private/Combat/Abilities/GA_Attack.cpp` | Inicia/finaliza la acción con la tarea de montage y conserva el ID de su reproducción. Valida un `Attack Trace`, un `Combat Recovery` y su orden antes del final del montage. El inicio del blend-out no cambia fases ni apaga trazas. Ya no excluye guardia mediante tags |
| Nuevos `Public/Combat/VampsCombatResolver.h`, `Private/Combat/VampsCombatResolver.cpp` | Subsystem del mundo que recibe contactos y los resuelve en `OnWorldPostActorTick`, después de las actualizaciones de actores/componentes. Primero filtra participantes inválidos o sin vida y evalúa todas las defensas; después consume solicitudes, retargetea y aplica resultados |
| `Private/Combat/FistHitboxComponent.cpp` | Conserva sweep del puño, obstrucción y deduplicación por acción; entrega los contactos al resolver sin aplicar daño durante el muestreo |
| `Private/Animation/ANS_AttackTrace.cpp` | Inicio del notify: preparación → contactos y activación de trazas. Final: cierre de trazas y apertura de ventana |
| Nuevos `Public/Animation/AN_CombatRecovery.h`, `Private/Animation/AN_CombatRecovery.cpp` | Notify puntual `Combat Recovery`: cierra ventana y entra en recuperación. Comparte con `Attack Trace` la validación de mesh, autoridad y reproducción propietaria mediante `OwnsCombatNotify` |
| `Public/Player/VampsPlayerController.h`, `Private/Player/VampsPlayerController.cpp` | Una propiedad `CombatAction`, Boolean, enlazada únicamente a `Started`; entrega `RequestCombatAction` sin decidir bloqueo ni daño |
| `Private/AI/VampsAIController.cpp` | Usa esa misma solicitud. Attack inicia acciones a su intervalo; Guard pulsa cuando queda sin defensa, incluso durante una acción. Como el botón es compartido, Guard también puede iniciar ofensiva. F6/cambio de comportamiento limpia el estado del rival |
| `Private/GameModes/VampsCombatTestGameMode.cpp` | R usa la limpieza central para el jugador y el rival creado por el GameMode, además de restaurar vida, posiciones y movimiento |
| `Private/UI/VampsCombatTestHUD.cpp` | Presenta fase/número de acción, disponibilidad ofensiva, defensa armada, contador/resultado de solicitudes y contactos entrantes/salientes con frame de resolución. Conserva selección, lista, vida y controles de observación |
| `Public/Combat/VampsCombatTags.h`, `Private/Combat/VampsCombatTags.cpp` | Retiran el tag de guardia mantenida; conservan Attack y Damage |

Se eliminaron `Public/Combat/Abilities/GA_Guard.h` y `Private/Combat/Abilities/GA_Guard.cpp`. No se añadieron aliases ni compatibilidad con las solicitudes antiguas. `UVampsAnimInstance` conserva su consulta existente a `IsGuarding`, que ahora indica defensa armada válida; no se modificó su fuente.

## Fases, contacto y selección

La preparación comienza al aceptar la habilidad. El estado `Attack Trace` delimita el tramo de contactos; su final abre `ContinuationWindow`. El nuevo notify `Combat Recovery` inicia `Recovery`, y el final/cancelación del montage vuelve a `Idle` y desarma defensa. La recuperación de esta etapa no es NA y permite rearmar defensa.

Las fases no dependen del inicio del blend-out ni de un límite de 0,75 s. La habilidad captura el ID de reproducción al iniciar el montage; `OwnsCombatNotify` consulta esa instancia por ID, incluso durante el blend-out, y rechaza notifies de reproducciones anteriores o canceladas. El cierre normal de trazas corresponde al final de `Attack Trace`; el final/cancelación de la acción también las cierra. El montage debe alcanzar los marcadores configurados y finalizar automáticamente. Su fin sigue siendo la frontera de la acción acordada, sin otro timer.

Cada contacto encolado conserva agresor, receptor y número de acción. Si otra acción empezó antes de resolver el frame, el contacto anterior conserva su resultado sobre el receptor y no marca como conectado el ataque nuevo. Los contactos abiertos y defendidos marcan contacto habilitante; un ataque que alcanza receptores abiertos y defendidos muestra `Mixed` sin depender del orden. El daño sigue usando `UGE_VampsDamage` y `AttackDamage`; el bloqueo todavía evita todo el daño, hasta incorporar las reglas de etapa 3.

Bloquear ya no cancela al agresor ni la acción propia. Con un agresor se lo selecciona directamente. Si hay varios bloqueados en el mismo frame, se conserva el seleccionado si pertenece al grupo; en otro caso se selecciona el primero según la lista del lock-on/HUD. Este criterio solo resuelve presentación/selección, sin dar prioridad al daño. Un contacto ofensivo contra otro rival no cambia el target.

La evaluación utiliza el estado existente al comenzar la resolución conjunta al final de las actualizaciones del frame; no reconstruye tiempos de contacto dentro de ese frame. Se mantiene el límite del detector existente: no reconstruye poses ni ventanas enteras perdidas durante un hitch. La entrega sigue siendo local con autoridad y habilidad `ServerOnly`; no incorpora la etapa de red.

## Blueprints y assets

**Hecho — Codex por MCP:**

- Nuevo `/Game/Vamps/Input/IA_Combat`: Input Action Boolean, sin triggers ni modifiers, creado mediante `DataAssetTools.create`.
- Modificados `/Game/Vamps/Input/IMC_Vamps` e `IMC_VampsCombatTest`: se retiró el mapping de LMB/IA_Attack y se sustituyó IA_Guard por IA_Combat en RMB. Quedaron seis y ocho mappings; se preservaron los modificadores de WASD, F y F6/R. `ObjectTools.set_properties/get_properties` confirmó los valores finales.
- Modificados `/Game/Vamps/Blueprints/BP_VampsPlayerController` y `BP_VampsCombatTestPlayerController`: `Combat Action = IA_Combat`. Se releyeron las referencias después de compilar/guardar; Move, CycleTarget, contextos y CameraManager conservaron sus asignaciones.
- Se compilaron con `warnings_as_errors = true` `ABP_VampsCharacter`, los dos personajes, ambos controllers, `BP_VampsAIController` y ambos GameModes: ocho Blueprints sin errores ni warnings. Se guardaron esos ocho y los tres assets de input; los once terminaron con `is_dirty = false`. La compilación/guardado no produjo cambios de contenido en los otros seis Blueprints según Git.
- `BP_VampsCharacter` y `BP_VampsCharacter_Player` conservan padres nativos Vamps, `Attack Montage = AM_BasicAttack`, daño 12, `SKM_Manny_Simple` y `ABP_VampsCharacter`. El GameMode de prueba conserva sus clases, HUD nativo y modo Passive; la IA conserva distancia 100, intervalo 1 y duración de modo mixto 4.
- Mapa actual `/Game/Vamps/Maps/L_CharacterBasic`, con override `BP_VampsCombatTestGameMode` e `is_dirty = false`; no se modificó ni guardó el mapa.

**Pendiente — usuario:** agregar `Combat Recovery` en `/Game/Vamps/Animations/AM_BasicAttack`, después del final de `Attack Trace` y antes del final del montage, conservando el contacto confirmado en etapa 0. Su posición define cuándo termina la ventana y empieza recuperación; no está limitada por el inicio del blend-out. Se releyeron duración **1 s**, Rate Scale **1**, Blend Out **0,25 s**, Blend Out Trigger Time **−1** y Auto Blend Out activo. `ObjectTools` no expone `notifies` ni `compositeSections`; su lectura fue rechazada. El catálogo tampoco ofrece una herramienta directa de edición de notifies de montage. No se usó UI ni un script de editor para suplir esa operación. El usuario confirmó que todavía no agregó el notify. Los pasos están en [editor.md](editor.md#4-delimitar-las-fases-en-el-montage).

No se crean Blueprints de habilidad, subsystem ni notify; las clases nuevas son nativas. Cámara y selección de 018–019 se conservan.

## Verificaciones y pendientes

- Revisión de referencias: no quedan consumidores de `RequestAttack`, `RequestGuard`, `AttackAction`, `GuardAction`, `UGA_Guard` o `VampsTags::Guard` en fuentes propias y configuración.
- Revisión del flujo de snapshot, consumo, retarget y daño; contactos recíprocos no se eliminan porque el primer daño del grupo reduzca HP a cero. Reset, fin de juego y pérdida de posesión descartan contactos pendientes del personaje.
- Se contrastaron las firmas de notifies, `GetActiveInstanceForMontage`, `GetMontageInstanceForID`, subsystem y delegado con headers/fuentes del motor 5.8 instalado, y los identificadores nuevos con las jerarquías e interfaces relevantes. La revisión del motor confirmó que los Branching Points siguen procesándose durante el blend-out natural; las reproducciones interrumpidas no continúan ese procesamiento.
- Se aplicó clang-format 20.1.8 con la configuración del repositorio a todos los archivos propios de C++ agregados/modificados y se revisó el resultado. `git diff --check` no informó errores de whitespace.
- Se aplicó la skill [game-feature-docs](../../../.agents/skills/game-feature-docs/SKILL.md) para contrastar alcance y reglas. Se actualizaron seguimiento del roadmap, índice y vínculo desde 020; se conservaron los cambios previos del usuario.

**Verificación nativa por MCP:** la consulta inicial de `LiveCodingToolset.LiveCodingToolset.CompileLiveCoding` devolvió `NoChanges`, y la reflexión confirmó los tipos de etapa 1 cargados. Después de retirar la dependencia del blend-out, la misma herramienta devolvió **`Result: Success`**, compilando y aplicando el parche real. Se recompilaron y guardaron de nuevo los ocho Blueprints, sin errores ni warnings y con `is_dirty = false`; el mapa también quedó sin cambios pendientes. Live Coding avisó que los cambios de tipos pueden afectar al empaquetado: la [guía](editor.md#1-compilación-nativa) deja la compilación completa con el editor cerrado para consolidarlos antes de empaquetar. No se atribuye esa compilación completa al usuario ni a Codex.

**Sin comprobar en PIE:** reproducción y orden de notifies, incluido el tramo de blend-out, contacto, consumo/rearme, retarget y simultaneidad. PIE permaneció detenido; iniciar combate sin el notify requerido provocaría el error de configuración que comprueba el código. No se crearon ni ejecutaron tests automatizados.

El cierre requiere completar la [guía del editor](editor.md) y confirmar los resultados. El cierre de etapa 0 sigue atribuido al usuario en [020](../020-roadmap-emulacion-fgoa/reporte.md#cierre-de-la-etapa-0).
