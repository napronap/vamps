# 023 — Armas por Blueprint y trazas independientes

Continuación vigente al cierre de la ventana: [024 — Cierre de Combo Window](../024-cierre-ventana-combo/reporte.md), con su [guía](../024-cierre-ventana-combo/editor.md). Esta iteración conserva la configuración y evidencia de armas.

## Resultado — 2026-10-17

Se reemplazó UFistHitboxComponent por AVampsWeapon, una clase nativa para crear un Blueprint por arma. Cada instancia tiene mesh, marcadores TraceStart/TraceEnd, TraceRadius y registro propio de enemigos golpeados por sección. El DA del personaje incorpora un array de armas con clase, socket y transform de montaje.

El código compiló mediante Live Coding con parches reales y el usuario realizó después la compilación completa/reapertura, confirmada con «listo». Se creó, configuró, compiló y guardó BP_VampsGreatSword como arma inicial. El DA quedó asignado y guardado con una entrada en HandGrip_R. Ambos personajes crearon su arma y la adjuntaron a su propia mesh en PIE; al detener la sesión no quedaron armas en el mapa. Falta la calibración visual y la observación de impactos con las animaciones definitivas.

Esta iteración continúa [022 — Cadena y NA](../022-cadena-y-na/reporte.md) y reemplaza su detección del puño. El cierre de la etapa 2 sigue dependiendo de preparar las animaciones y observar el combate. No se modificaron especificaciones funcionales/técnicas ni el roadmap.

## Comportamiento

- Un Blueprint de AVampsWeapon define WeaponMesh, TraceStart, TraceEnd y TraceRadius. WeaponRoot representa el agarre; la mesh puede ajustar su posición, rotación y escala sin desplazar ese origen. Los marcadores son hijos de la mesh. No hay un ActorComponent propio adicional ni una caja de colisión.
- El DA contiene FVampsWeaponSettings: WeaponClass, AttachSocket y RelativeTransform. El personaje crea cada arma al comenzar la partida, asigna owner/instigator, la monta en su mesh y conserva las instancias en Weapons. Las armas se destruyen al terminar el personaje. No hay cambio de equipo ni inventario.
- El array admite dos instancias para armas duales, incluso del mismo Blueprint, montadas en HandGrip_R y HandGrip_L. Cada arma mantiene su TSet de enemigos alcanzados: cada una puede dañar una vez al mismo rival durante una sección. El contacto de una no consume el de la otra. Cada contacto abierto aplica el AttackDamage actual del personaje, sin repartirlo entre armas.
- BeginCombatStep limpia el registro y la historia de todas las armas. ANS_AttackTrace conserva la ventana única: Contacts habilita las trazas y AttackEnd las detiene. Cerrar/reabrir detección dentro de una sección no limpia enemigos alcanzados. R, cambio de modo de IA y salida del personaje detienen detección y limpian registros.
- Las trazas ejecutan en autoridad, en TG_PostPhysics y después del mesh del personaje. La pose actual se consulta con un barrido de esfera sobre el segmento completo. Entre actualizaciones se distribuyen puntos a lo largo del segmento según longitud/radio y se barren sus trayectorias; el giro usa subdivisiones adaptativas e interpolación de quaternion. Al abrir una ventana se descarta el recorrido anterior, evitando trazar a través de un salto de sección.
- TraceRadius está en centímetros del arma y acompaña la escala del actor. La escala local de WeaponMesh cambia la geometría y los marcadores; no multiplica por sí sola el radio. El muestreo calcula sus divisiones desde las posiciones efectivas de los marcadores.
- Las consultas ignoran al propietario y a todas sus armas. Solo buscan objetos Pawn y entregan personajes vivos al resolver. Se conserva la comprobación de pared por Visibility entre atacante y receptor. Las meshes de las armas tienen NoCollision y no generan overlaps.
- CombatResolver conserva todos los contactos de armas distintas y su resolución defensiva por lote. Una conexión de cualquiera habilita la continuación. El arbitraje previo de presentación sigue seleccionando un desplazamiento por receptor/frame; esto no elimina el daño de los demás contactos. Dos armas bloqueadas en el mismo frame usan la regla conjunta de defensa existente.
- Configuración ausente o inválida produce un error explícito: array vacío, clase abstracta/nula, socket inexistente, mesh ausente, radio inválido o marcadores coincidentes. No se conserva una ruta al puño ni se sustituye un arma faltante silenciosamente.

## Código y relaciones

Rutas relativas a Source/vamps/.

| Archivos | Responsabilidad y cambio |
|---|---|
| Public/Combat/VampsWeapon.h y Private/Combat/VampsWeapon.cpp | Nueva clase AVampsWeapon: componentes editables, barridos del filo, historial temporal, exclusiones y registro de impactos independiente. Envía cada contacto al CombatResolver existente |
| Public/Combat/Data/VampsCombatSettings.h | Agrega FVampsWeaponSettings y Weapons. Conserva montages, NA y distancias por paso |
| Public/Characters/VampsCharacter.h y Private/Characters/VampsCharacter.cpp | Elimina el subobjeto FistHitbox; crea/monta/destruye las armas y coordina ventana, cambio de sección y limpieza |
| Private/Combat/Abilities/GA_Attack.cpp | Reemplaza la validación del socket FistHit por la exigencia de armas equipadas; conserva las validaciones del montage y sus ventanas |
| Public/Combat/FistHitboxComponent.h y Private/Combat/FistHitboxComponent.cpp | Eliminados. No quedan referencias en las fuentes propias |

No se modificaron CombatResolver, los ANS, la IA, el AnimBP ni el movimiento de combate para repartir daño por arma: la cola existente ya admite contactos separados del mismo atacante al mismo receptor.

## Assets

- **Creado por Codex mediante MCP:** /Game/Vamps/Blueprints/Weapons/BP_VampsGreatSword, derivado de /Script/vamps.VampsWeapon. Usa únicamente el recurso visual /Game/GreatSwordAnimV1/DEMO/StaticMesh/Sword_10.Sword_10; no se copió lógica del pack.
- **Configuración inicial del arma:** WeaponMesh con Location=(60,0,0), Rotation=(Pitch=90,Yaw=0,Roll=0), Scale=(30,30,30); TraceStart=(0,0,0.8), TraceEnd=(0,0,-3.239877), relativos a la mesh; TraceRadius=8 cm. La mesh importada mide aproximadamente 6,51 unidades en Z, por eso se escaló. El thumbnail y sus bounds se consultaron por MCP. Los valores son un punto de partida, pendiente de calibración visual con las animaciones.
- **Consultado, sin edición:** SKM_Manny_Simple conserva HandGrip_R sobre hand_r y HandGrip_L sobre hand_l; ambos sockets ya existían. Manny y ABP_VampsCharacter siguen compartidos.
- **Asignado y guardado por Codex mediante MCP tras el arranque limpio:** /Game/Vamps/Data/DA_VampsCombatSettings, Weapons con una entrada inicial BP_VampsGreatSword → HandGrip_R → transform identidad. Se conservaron AttackMontage=AM_BasicAttack, HitReactionMontage vacío, NA=0,5 y distancias 100/60, 100/60 y 100/180 cm. La configuración dual se reproduce agregando otra entrada con HandGrip_L; no se cambió el personaje inicial a dual sin elegir esas animaciones.
- **Sin edición de contenido ajeno:** las importaciones GreatSwordAnimV1 y las modificaciones previas del mapa corresponden al usuario. No se guardaron sus animaciones ni el mapa.

## Verificación y pendientes

- Aplicado clang-format a los seis archivos C++ agregados/modificados. Git diff --check no informó problemas. Se revisaron los nombres nuevos contra Actor, Pawn, Character, UObject, DataAsset e interfaces relevantes.
- LiveCodingToolset.LiveCodingToolset.CompileLiveCoding devolvió Success y aplicó parches reales, incluido el ajuste del origen de agarre. UHT registró AVampsWeapon y FVampsWeaponSettings. El log advirtió sobre tipos recargados y packaging.
- ActorTools.get_components confirmó tras el arranque limpio que ninguno de los CDO de BP_VampsCharacter y BP_VampsCharacter_Player tiene FistHitbox.
- BP_VampsGreatSword compiló con warnings_as_errors=true, se guardó con save_assets=true y quedó is_dirty=false. Se releyeron mesh, transform y referencias a los marcadores.
- ObjectTools.list_properties encontró inicialmente WeaponClass con tipo transitorio de Live Coding. Tras la compilación completa y reapertura realizadas por el usuario, confirmó Class@/Script/vamps.VampsWeapon; la referencia transitoria desapareció. No hay herramienta MCP directa para esa compilación completa/reapertura.
- Después de reabrir se releyeron y conservaron mesh, transform de componentes, TraceStart/End y TraceRadius del Blueprint. BP_VampsGreatSword, ambos personajes y ABP_VampsCharacter compilaron con warnings_as_errors=true, sin errores ni advertencias. Se guardaron arma, personajes y DA; los cuatro paquetes quedaron is_dirty=false.
- StartPIE/StopPIE en L_CharacterBasic confirmó dos personajes en Idle, ComboStep=0 y bCanDealDamage=false. Cada Weapons contenía una instancia diferente de BP_VampsGreatSword. ActorTools.get_parent_component confirmó WeaponRoot adjunto al CharacterMesh0 del personaje correspondiente; TraceRadius=8 y bStartWithTickEnabled=false en ambas armas. Después de StopPIE, find_actors no encontró armas.
- ObjectTools no permite leer los campos privados Owner ni AttachSocketName. La asociación se comprobó mediante Weapons y get_parent_component; el socket HandGrip_R se releyó en el DA, y la asignación del owner/validación del enganche está en el código nativo ejecutado. No se añadió una vía alternativa para acceder a esos campos.
- La consulta del log no encontró asserts ni errores de Blueprint/PIE; solo una sesión MCP vencida durante la reconexión. PIE quedó detenido. No se solicitaron ataques en estas comprobaciones de arranque.
- Pendientes: calibración del agarre/filo y observación de impactos, duales y cadenas según la [guía](editor.md), con las animaciones elegidas por el usuario. La creación de dos armas por personaje y el daño de ambas no se marcaron como observados.
- No se agregaron ni ejecutaron tests automatizados. La interpolación de trazas aproxima las poses entre actualizaciones; no reevalúa la animación original entre frames. La calibración debe incluir los giros rápidos reales.
