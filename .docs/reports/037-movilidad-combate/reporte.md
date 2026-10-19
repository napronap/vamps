# 037 — Movilidad integrada al combate

## Alcance y estado

Implementa el paso 4: acercamiento ofensivo limitado, dash/evasión y sprint sobre el combate de 3. La etapa permanece **pendiente de cierre** hasta completar autoría y observación en juego. [Guía y pendientes del editor](editor.md). Reemplaza la planificación de [036](../036-revision-paso-4/reporte.md), conservando su evidencia previa; [035](../035-revision-paso-3/reporte.md) sigue siendo el cierre de 3.

No se incorporan cámara de acciones, oclusión, equipos, red ni extensiones. No se ejecutan tests automatizados ni se automatiza UI.

## Cambios y relaciones

| Archivos/clases propias, bajo Source/vamps/Public y Private | Responsabilidad y cambio |
|---|---|
| Characters/VampsCharacter, Combat/VampsCombatState | Permisos, intención y solicitud de dash por paso, salida de Guard por impacto final y gasto/recuperación de energía. NA conserva su plazo. Continuación válida descarta el dash; contacto, muerte, reset y cambio de posesión limpian intención. |
| Combat/VampsCombatMovementComponent | Advance y Dash usan el movimiento con colisión existente. Reemplaza raíz ofensiva por avance controlado; mantiene empuje/alineación del receptor separados. Elige montage direccional y detiene movilidad incompatible con blend out. Sprint modifica GetMaxSpeed. |
| Combat/Data/VampsCombatSettings | Parámetros de avance por paso, arma/índice del impacto final, ocho montages y escalares de movilidad. Valida energía/distancias/duraciones sin asserts. |
| Combat/Attributes/VampsAttributeSet | MobilityEnergy/MaxMobilityEnergy con clamp y replicación, independientes de GuardLoad. No cambia la regla de ruptura. |
| Combat/VampsWeapon | Índice de subimpacto independiente por arma: cero al comenzar sección, incremento con ResetImpactHits. No cuenta aciertos contra víctimas. |
| Combat/VampsCombatResolver | Captura bFinalImpact según arma/subimpacto autorados. Conserva clasificación/prioridades del lote. Resuelve contactos → continuación → dash → liberación de locomoción; no descarta contactos capturados por evadir. |
| Combat/Abilities/GA_Attack | Valida avance y final autorado: arma existente e índice coincidente con su último reinicio en la sección. El golpe final requiere knockback positivo. |
| Player/VampsPlayerController | IA_Mobility Started/Completed/Canceled; dirección actual de IA_Move leída desde EnhancedPlayerInput al pulsar y puesta en cero al finalizar. No depende del orden de callbacks ni del valor almacenado del frame anterior. No decide permisos de combate. |
| Animation/VampsAnimInstance | Publica bIsDashing y bIsSprinting; conserva pose/reacción resueltas por combate. |
| UI/VampsCombatTestHUD | Muestra energía de movilidad, permiso final de Guard, disponibilidad/solicitud de dash y sprint, separados de carga de guardia. Amplía espacio de paneles e instrucciones de Shift. |

No se agregan clases ni se copia lógica de Third Person. Cámara, lock-on, IA y GameMode conservan sus clases. La IA usa el mismo acercamiento al solicitar su ataque; su umbral de inicio existente no garantiza un acierto.

## Reglas elegidas para completar los comentarios

- WASD sigue relativo a cámara. Dash fija dirección y facing al pulsar/ejecutar; el clip se selecciona por sector de 45 grados relativo al cuerpo. Sin dirección se aleja del target; sin target/dirección válida se rechaza con diagnóstico.
- El avance toma destino y dirección al comenzar cada paso; cambios de F, pérdida o movimiento posterior del rival no generan persecución. Sin target, el golpe se ejecuta sin acercamiento. Se suprime la raíz del ataque para no sumar otro avance; giro limitado a 500 grados/s.
- Guard habilita dash al registrar el final autorado de su instancia entrante, sin esperar ReactionEndsAt o el fin del empuje. Una reacción nueva reemplaza el permiso. Si el final falla, sigue el plazo ordinario; ruptura/Hit/Stun/muerte/reset no habilitan la salida.
- NA permite dash/sprint pero sigue impidiendo ataque/defensa. En Idle, ataque durante dash inicia la ofensiva y mezcla su presentación sustituyendo la evasión. La defensa sigue requiriendo solicitud válida; no se concede guardia permanente.
- Durante ataque, dash se admite en ventana de continuación o Recovery; espera el cierre de contactos/ventana. Una continuación válida gana y descarta el pedido; la salida conserva Recovery restante y NA. Un contacto cancela intención sostenida y requiere otra pulsación.
- No hay invulnerabilidad. El coste se cobra solo tras iniciar el montage de dash. Insuficiencia rechaza dash; agotamiento durante sprint termina el sprint sin stun/guard break. Se conserva locomoción ordinaria y se requiere nueva pulsación para sprint otra vez. Energía se recupera fuera de dash/sprint tras su retraso, incluso en reacciones.

Estas elecciones adicionales son decisiones iniciales de Vamps autorizadas para la iteración, no mediciones del original. La energía nativa citada en 028 y un coste de bloquear no identifican por sí solos una barra de ruptura.

## Assets y configuración

Rutas dentro de /Game/Vamps:

- Input/IA_Mobility: Boolean, sin triggers/modifiers, duplicada de IA_Combat por MCP. LeftShift agregado a IMC_Vamps y a IMC_VampsCombatTest, conservando mappings y modificadores anteriores.
- Blueprints/BP_VampsPlayerController y BP_VampsCombatTestPlayerController: MobilityAction=IA_Mobility; ambos compilados y guardados. El controller de observación tiene clase/mapping propios y requiere asignación expresa.
- Animations/Executioner/AM_Executioner_Dash_Fw, FR, R, BR, Bw, BL, L y FL: assets propios duplicados de AM_Executioner_Block y configurados con el clip Dash_*_RM correspondiente de GreatSwordAnimV1. DefaultSlot, RateScale=1, Auto Blend Out, Blend In=0,06 s y Blend Out=0,08 s. Los ocho clips fuente duran 1 s y tienen extracción de root motion desactivada.
- El usuario completó el recálculo de duración que MCP no soporta; la lectura final de los ocho montages confirma longitud 1 s, segmentos correctos y guardado en disco. [Captura MCP](dash-montages-mcp.json). No acredita por sí sola reproducción visual.
- Animations/Executioner/BS_Executioner: conserva Idle/Walk/Run en 0/180/420 y agrega Run_Fw en 700 con RateScale=1,6667; eje Speed hasta 700. No cambia la topología del AnimBP.
- Data/DA_VampsCombatSettings: conserva armas, montages, tres entradas, transforms, daño y guardia anteriores. Agrega ocho montages en orden Fw/FR/R/BR/Bw/BL/L/FL y los parámetros siguientes.

| Parámetro nuevo | Valor inicial |
|---|---:|
| AdvanceDistance / AdvanceDuration / AdvanceStopDistance, cada paso | 250 cm / 0,25 s / 130 cm |
| FinalImpactWeaponIndex / FinalImpactIndex, cada paso | 0 / 0, sujeto a verificar autoría de multihit |
| MobilityEnergyCapacity / DashEnergyCost | 100 / 25 |
| SprintEnergyPerSecond | 20/s |
| MobilityRecoveryPerSecond / MobilityRecoveryDelay | 25/s / 1 s |
| DashDistance / DashDuration / SprintSpeed | 350 cm / 0,3 s / 700 cm/s |

La autoría inicial de final=arma 0/subimpacto 0 representa un único impacto. Si una sección tiene reinicios, UGA_Attack rechaza ese índice e informa el número exigido. Se debe verificar que el arma/subimpacto elegido representa visualmente el cierre con knockback. No se infirió del tercer paso.

## Preservación de Data Assets

La búsqueda por MCP en todas las raíces y search_subclasses encontró un único DA y solo UVampsCombatSettings. Se capturaron todos sus valores en [data-assets-antes.json](data-assets-antes.json) y se guardó el asset antes de editar clase/struct.

La primera compilación Live Coding fue Success y aplicó un parche real. La recarga estructural mostró los tres pasos reemplazados por defaults en memoria; [captura](data-assets-live-coding.json). No se guardó ese estado. El intento posterior de restaurar en memoria no llegó a ejecutarse porque el endpoint MCP ya no estaba disponible durante el cierre del editor.

**Hecho — usuario:** cerró sin guardar los assets afectados, compiló Development Editor/Win64 y reabrió el proyecto, confirmado en la conversación. Tras el arranque, MCP volvió a leer los valores originales conservados en disco, asignó los campos nuevos y verificó todos los campos previos contra la captura. [data-assets-despues.json](data-assets-despues.json) registra cero diferencias y referencias estables; se guardó únicamente el DA afectado y los Blueprints configurados. is_dirty=false confirmado para DA y ambos controllers. El [objetivo completo](data-assets-objetivo.json) permite reproducir la configuración.

## Verificaciones y límites

- Revisión de código de permisos, prioridades, cancelación, energía, root motion y contactos capturados; formateador aplicado a los archivos propios modificados y git diff --check sin errores de whitespace.
- Primera compilación MCP: [Success estructural](live-coding-1.log). Después del build/arranque del usuario, [NoChanges](live-coding-final.json): confirma ausencia de cambios para compilar en ese momento, no otro parche. Los ajustes posteriores de validación, input y supresión de raíz en el último frame se compilaron/aplicaron sin cambios de tipos reflejados; [compilación vigente](live-coding-5.json).
- MCP confirmó conexiones de entrada, campos nuevos, preservación del DA y muestras del BlendSpace. No se atribuye a esta lectura una observación visual de blending o gameplay.
- Arranque de PIE realizado por MCP, con jugador y rival creados: ambos vivos, Idle, energía 100/100, carga de guardia 0, bCanDash=true y sin permiso final de Guard. Consulta LogVampsCombat sin errores/warnings; no hubo solicitudes ni contactos en la captura. [Estado](pie-estado.json). PIE detenido y confirmado false al terminar. No se simularon inputs ni se consideran observados dash, sprint o multihit.
- No se modificaron ni se automatizaron los graphs del AnimBP. La referencia 036 conserva el límite de lectura de AnimGraph.

## Pendientes para cierre

1. Verificar el impacto final autorado de cada sección, incluyendo reinicios por arma si existen. La configuración inicial 0/0 no se presenta como una revisión visual de los subgolpes.
2. Observar las transiciones, dirección, blending, multihit y los casos de movilidad de la guía. El recálculo manual de duración ya fue verificado por MCP; no se marca completado gameplay por estar compilado o tener propiedades correctas.

El índice y las especificaciones de combate/jugador/juego/roadmap se actualizan para estas reglas. 028 aclara terminología de energía frente a barra de guardia sin cambiar la evidencia nativa. 036 enlaza esta implementación.
