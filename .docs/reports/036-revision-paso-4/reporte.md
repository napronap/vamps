# 036 — Revisión del paso 4

Implementación posterior: [037 — Movilidad integrada al combate](../037-movilidad-combate/reporte.md), con [configuración y pendientes actuales](../037-movilidad-combate/editor.md). Esta revisión conserva las decisiones y lecturas previas.

## Alcance y resultado — 2026-10-19

Revisión del acercamiento ofensivo, evasión y sprint solicitada por el usuario, contrastando el roadmap, las especificaciones, los fuentes actuales, el cierre de 3 en [035](../035-revision-paso-3/reporte.md) y la investigación conservada en [028](../028-contraste-fgoa/analisis.md). Incluye las decisiones posteriores del usuario sobre recursos, dash defensivo, NA, blending, prioridad de continuación y dirección de retroceso. El paso 4 sigue pendiente de implementación; se prepara su prompt de ejecución en la conversación.

No hubo cambios de código, configuración ni assets. Se agregan este reporte y su [guía de editor](editor.md), y se enlazan desde el índice. Los cambios documentales que ya había en Git pertenecen al trabajo previo y se conservan.

## Qué sigue vigente

- Acercamiento ofensivo limitado: el ataque puede empezar lejos, avanzar, quedar corto y golpear al aire. La geometría y las trazas reales deciden el contacto.
- Evasión direccional; Shift sin dirección retrocede como adaptación explícita de Vamps. Mantener Shift con dirección permite sprint.
- Evasión al final del paso para cerrar la cadena, sin cancelar libremente en cualquier frame.
- NA permite locomoción y dash sin levantar su prohibición de atacar/defender. Guard habilita dash al registrar el último impacto enemigo definido abajo; Hit/Stun conservan sus restricciones. Lock-on, orientación corporal y dirección de viaje son conceptos separados.
- Cámara de acciones, zoom, oclusión y HUD de partida pertenecen a 5. Aquí corresponde mantener el seguimiento existente y exponer lo necesario para observar movilidad.

## Cambios relevantes desde el roadmap inicial

### Reacción y permisos

El paso 3 ya implementa Guard/Hit/Stun, ReactionEndsAt, memoria defensiva por atacante/paso/generación, cancelación ofensiva y resultados simultáneos. No corresponde repetir las carencias históricas de 028 como si siguieran presentes.

En AVampsCharacter, SetCombatPhase deriva bCanMove de vida y ausencia de reacción; AddMovementInput lo consulta para jugador e IA. No hay permiso separado de evasión. UGA_Attack, las secciones y el resolver ya ofrecen fronteras para continuación y liberación de locomoción. El resolver aplica contactos antes de continuaciones y locomoción; la solicitud de evasión debe integrarse con un orden explícito.

La [sección 6 de 028](../028-contraste-fgoa/analisis.md#6-en-guard-se-escucha-input-pero-se-autorizan-acciones-distintas), apoyada por N05 y las exportaciones locales de C388F0/C7B9E0/C7BE80/C7C050/C3BE90, distingue Step de caminar y atacar: Guard permite evaluarlo durante freeze con requisitos adicionales. Entrar en Step limpia el calendario defensivo adicional y desplazamientos anteriores. Esto respalda una salida defensiva propia; no demuestra que Hit, Stun y NA deban permitirla ni que toda pulsación tenga éxito.

### Recurso y ruptura

La carga/barra de guardia y la energía de movilidad son sistemas distintos. GuardLoad aumenta al bloquear y rompe al llegar a capacidad; no se reutiliza para sprint ni dash. El usuario corrigió expresamente la confusión introducida por llamar a ambos «el recurso». Los atributos propios actuales son Health/MaxHealth y GuardLoad/MaxGuardLoad; la energía de movilidad todavía no está implementada.

028 registra gasto del campo actor+3530 en guardia, carrera y esquiva. Esa evidencia de coste al bloquear no identifica ese campo con la barra de ruptura ni demuestra que agotarlo provoque guard break. Al corregir la documentación, distinguir energía, coste de una acción, carga de guardia y stun, conservando los límites de la evidencia nativa. La separación de recursos para Vamps está decidida; quedan los parámetros y reglas de consumo/recuperación de movilidad.

### Avance y orientación

028/V05 respalda avance limitado y fallo, sin determinar el algoritmo de persecución. N07 distingue seguimiento ofensivo de giro defensivo; el ejemplo de 3 grados por frame y 30 acumulados pertenece a un evento concreto, no a una regla universal para copiar.

El movimiento actual procesa root motion y alineación/empuje del receptor. TargetAlignmentDistance pertenece a ese desplazamiento posterior al contacto: no es alcance de persecución del atacante. El avance necesita datos y límites propios. La base del jugador gira hacia el movimiento; la IA configura orientación hacia su controller. Cualquier política nueva debe coordinar esas rutas, no aplicar yaw desde varios dueños.

## Estado comprobado por MCP

Lecturas directas de AssetTools y ObjectTools; sin iniciar PIE, compilar, modificar ni guardar assets:

- BP_VampsPlayerController referencia IMC_Vamps, IA_Move, IA_Combat e IA_CycleTarget. defaultKeyMappings contiene WASD, RMB y F, sin Shift. La propiedad mappings devuelve un array vacío; se consultó también defaultKeyMappings para leer la configuración real.
- AM_Executioner_Combo contiene cinco segmentos de AttackCombo7: Stage1, Stage1_Back, Stage2, Stage2_Back y Stage3, con duración total aproximada de 7,3667 s y RateScale=1. El número de segmentos no equivale al de pasos de cadena. Se verificó bEnableRootMotion=true en Stage1; no se midió la traslación de su raíz.
- ABP_Executioner usa RootMotionFromMontagesOnly y depende de BS_Executioner. El BlendSpace tiene muestras Idle=0, Walk_Fw=180 y Run_Fw=420, con eje Speed de 0 a 420. Es locomoción hacia adelante; publicar Direction en C++ no agrega muestras laterales.
- AssetTools.find_assets encontró ocho Dash direccionales en /Game/GreatSwordAnimV1/Animation/RM. La revisión de archivos también encontró Dodge laterales/traseros y variantes FacingFw. Son candidatos visuales, no una elección aprobada.
- Dash_Fw_RM tiene duración 1 s, bEnableRootMotion=false y skeleton del pack GreatSwordAnimV1. Se debe comprobar compatibilidad y movimiento real; el sufijo RM no prueba que la extracción esté habilitada.
- DA_VampsCombatSettings conserva alineación/empuje 130/100, 130/100 y 150/250 cm, reacción de 1 s por paso, NA=0,5 s, capacidad/carga=100/25, recuperación=20/s tras 1 s y stun=2 s.

Límites: BlueprintTools.list_graphs devuelve AnimGraph/EventGraph de ABP_Executioner, pero read_graph_dsl devolvió texto vacío para AnimGraph. No se acreditó su topología interna. compositeSections no está expuesto por ObjectTools y su lectura fue rechazada; los pasos existentes se contrastaron con fuentes y 035, sin atribuir una lectura nueva de secciones. Las referencias originales de investigación en D: no están disponibles en esta ubicación; se usaron el análisis y las exportaciones preservadas dentro del repositorio. No se volvió a ejecutar el binario de FGOA ni a observar los clips de video en esta iteración.

## Partes que se tocarían

| Pieza existente | Cambio previsto y conexión |
|---|---|
| AVampsPlayerController | Leer dirección vigente y Shift presionado/soltado; distinguir intención de evasión y sprint sin decidir permisos. |
| AVampsCharacter / FVampsCombatState | Autorizar movilidad según fase/reacción; resolver evasión pendiente en la frontera acordada, salida de Guard y limpieza por contacto, muerte, R y cambio de posesión. Conservar NA y memoria según reglas elegidas. |
| UVampsCombatMovementComponent | Ejecutar avance/evasión con colisión y límite de recorrido; coordinar root motion, giro y empuje para que no se sumen movimientos incompatibles. Sprint usa locomoción del mismo componente. |
| UGA_Attack / UVampsCombatResolver | Conectar inicio/fin del paso con avance y evasión; decidir prioridad frente a continuación y contactos capturados. No introducir una segunda resolución de daño. |
| UVampsCombatSettings / FVampsCombatStepSettings | Configurar únicamente parámetros necesarios de avance y movilidad. Antes de cambiar clase/struct: localizar todos los DA y derivados por MCP, capturar valores completos, guardar y verificar restauración después de compilar. |
| UVampsAttributeSet | Incorporar energía de movilidad separada de GuardLoad conforme a las reglas de consumo/recuperación; conservar la ruptura vigente por carga de guardia. |
| UVampsAnimInstance, ABP_Executioner, BS_Executioner | Presentar evasión y sprint; ampliar locomoción si se elige cuerpo orientado al rival y desplazamiento lateral. La presentación no concede permisos. |
| IMC_Vamps y BP_VampsPlayerController | Agregar/asignar la entrada de movilidad y su binding. No utilizar Blueprints de lógica del ejemplo Third Person. |
| AM_Executioner_Combo y futuros montages propios de evasión | Autoría de intervalos de avance/giro y clips elegidos; conservar ventanas de contacto/continuación existentes salvo ajuste necesario y documentado. |
| AVampsAIController / escena de observación | Reutilizar solicitudes/permisos compartidos y verificar que enfoque y umbral AttackDistance no oculten el nuevo avance. Ampliar observación solo cuando sea necesario; no construir otra IA de combate. |
| Cámara/lock-on/HUD de observación | Mantener selección y seguimiento; mostrar estado/recurso necesario para verificar movilidad. El cambio de planos, fade y HUD completo siguen en 5. |

Las clases propias conservan headers/cpp en Public/Private con organización paralela. No se crean clases nuevas durante esta revisión.

## Cómo implementarlo después de decidir reglas

1. Aplicar las decisiones funcionales de abajo y completar los detalles pendientes sin reabrirlas. Distinguir solicitud pendiente, evasión en ejecución y sprint sostenido; bCanMove no alcanza para expresarlos.
2. Confirmar clips y root motion. Se recomienda aprovechar el movimiento de raíz ya utilizado, con presupuesto máximo por paso e intervalos explícitos de avance/giro en el componente existente. Medir los clips antes de elegir definitivamente el mecanismo; no mantener varios mecanismos alternativos por anticipación ni sumar un avance adicional al root motion sin contabilizarlo.
3. Separar distancia deseada respecto del rival, máximo de avance y alcance físico del arma. Mantener la animación aunque quede corto. Definir qué seguimiento se permite si el rival se mueve, cambia F, desaparece o deja de ser elegible.
4. Incorporar Shift y conservar dirección de intención aunque caminar esté prohibido. El usuario aceptó iniciar evasión al presionar y permitir sprint después de ella si Shift y dirección siguen vigentes. Shift sin dirección retrocede alejándose del target, no según el yaw de cámara o cuerpo.
5. Integrar el cierre ofensivo en la frontera elegida. La continuación válida del siguiente ataque gana frente al dash solicitado. Conservar el calendario de recuperación/NA; no reutilizar sin más CancelCombatOffense, cuya interrupción por Guard evita crear NA. Evitar un dash residual ejecutado después de perder esa prioridad, un hit o reset.
6. Implementar la salida especial de Guard con permiso propio después de registrar el último impacto del ataque enemigo, el golpe de cierre con knockback. No habilitarla entre subimpactos ni exigir esperar además el vencimiento de ReactionEndsAt para admitirla. Al ejecutarla, terminar la reacción y su reproducción y limpiar el empuje anterior. ResetCombatMotion actual detiene todos los montages: revisar su uso para no detener el montage de evasión recién iniciado.
7. Aplicar sprint, costes y presentación; mantener un único dueño de cada transición/plazo. No agregar invulnerabilidad: el dash evita golpes por desplazamiento y geometría. Ataque/defensa están permitidos durante dash/sprint con blending adecuado, cuando las otras restricciones de combate lo permitan. No omitir NA ni borrar contactos válidos ya capturados.
8. Compilar por MCP cuando el cambio lo permita, restaurar/verificar los DA y observar manualmente los casos del editor. No se agregan ni ejecutan tests automatizados.

## Decisiones incorporadas y detalles pendientes

Las decisiones del usuario están integradas en las secciones anteriores. Recursos separados; dash desde Guard tras el impacto final enemigo; dash durante NA; ataque/defensa durante movilidad con blending y permisos previos; Shift inmediato y luego sprint sostenido; continuación ofensiva prioritaria; ausencia de invulnerabilidad; retroceso relativo al target.

La lectura actual de FVampsCombatStepSettings y FVampsPendingContact confirma que no existe metadata de impacto final. Los tres pasos tienen KnockbackDistance positivo: detectar empuje no identifica el golpe habilitante. La implementación debe autorar/capturar esa señal por ataque/subimpacto, sin inferirla del tercer paso de combo, del daño o del fin visual del montage. Es una regla de Vamps definida por el usuario, no un descubrimiento nuevo del binario original.

Para ejecutar en una sola iteración, el prompt deberá autorizar decisiones técnicas y parámetros iniciales configurables, identificar esas elecciones en el reporte y preservar las reglas acordadas. No están resueltos por los comentarios:

- Coste de dash, drenaje de sprint, recuperación e insuficiencia de energía de movilidad.
- Identificación concreta del impacto final en cada ataque/multihit y tratamiento del permiso frente a otros agresores/contactos posteriores.
- Ventana de admisión de dash durante ataque, frontera autorada y recuperación conservada tras salir.
- Referencia de WASD, fijación/corrección direccional, giro ofensivo y comportamiento sin target. Solo el retroceso sin dirección quedó fijado relativo al target.
- Clips, compatibilidad y parámetros iniciales de distancias, velocidad y duración.

No bloquean esta etapa las preguntas de equipos, portador, red, cartas o extensiones propias. Las preguntas de target inicial/ausencia que aún figuran en player deben contrastarse con lo implementado en 018–019, en vez de reabrir decisiones ya cerradas. player/technical también conserva referencias anteriores a la solicitud compartida y al seguimiento automático; se detecta su desactualización sin modificar las especificaciones en esta revisión.
