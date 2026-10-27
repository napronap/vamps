# 049 — Revisión previa del paso 6

Decisiones posteriores: [050 — Reglas de la partida local](../050-reglas-partida-local/reporte.md), con [guía vigente](../050-reglas-partida-local/editor.md). Retira portador y resuelve parte de las preguntas de esta revisión, cuyo contenido se conserva como evidencia previa.

## Alcance y resultado — 2026-10-27

Revisión de la [partida funcional](../../game/functional.md), su [base técnica](../../game/technical.md), [roadmap](../../roadmap.md), reglas de combate/jugador, decisiones de 016–017, reportes posteriores y fuentes propios del commit `5503c3e` (`add basic hud`). El árbol de trabajo estaba limpio al iniciar.

El paso 6 sigue sin implementar: el alcance está definido, pero las preguntas de partida siguen abiertas. Movilidad, cámara y HUD ya tienen implementación sobre la que integrar equipos, bajas y puntuación. Esta revisión no crea lógica de partida ni cierra etapas anteriores.

No hubo cambios de código, configuración ni assets. Se agregan este reporte, su [guía](editor.md) y enlaces desde el índice y 048. Se conservan sin editar roadmap y especificaciones; las preguntas siguientes desarrollan las ya abiertas en partida, sin resolverlas por suposición.

## Reglas que ya están elegidas

El [funcional de partida](../../game/functional.md#alcance-de-la-base) fija equipos, puntos por bajas, respawn del mismo personaje y portador visible con recompensa mayor, sin temática Grial. No hay reservas ni sustitución de personaje. La baja individual no es derrota de equipo.

El [paso 6](../../roadmap.md#6-partida-por-equipos) exige elegibilidad enemiga tanto en selección como en contactos: los aliados no deben convertirse en objetivos ni recibir los contactos ofensivos destinados a enemigos. Atacar a un enemigo ocupado con otro aliado continúa permitido, sin implementar SUPPORT.

No se trasladan automáticamente la selección de kill leader, curación del Grial, puntos o tiempos del original. La relación entre objetivo y puntos por baja determina la duración buscada; bajar ambos proporcionalmente no alarga la partida. El primer ciclo es local: la integración de clientes pertenece a 7, y las extensiones a 9. R conserva su función de observación, separada de respawn/partida.

## Decisiones necesarias antes de implementar el ciclo completo

| Grupo | Preguntas concretas | Por qué afecta la implementación |
|---|---|---|
| 1. Formato y participantes | ¿Cuántos equipos y cuántos integrantes por equipo? ¿Cómo se asignan? En el escenario local, ¿un jugador humano y bots para los demás puestos? | Determina registro de participantes, preparación del escenario y cantidad de spawns/controladores. No implica implementar red en esta etapa. |
| 2. Puntuación y cierre | ¿Cuánto vale una baja normal y la del portador? ¿Cuál es el objetivo de puntos? ¿Hay límite de tiempo y qué decide el ganador al vencer? ¿Cómo termina un empate, incluido alcanzar el objetivo ambos equipos en el mismo lote de contactos? | Fija acumulación y condición de fin. El intercambio letal simultáneo ya permitido por combate debe conservarse; el ganador no puede depender del orden accidental de procesamiento. |
| 3. Portador | ¿Existe uno global o uno por equipo? ¿Cuándo aparece y cómo se elige? ¿Cómo se transfiere o reasigna tras morir, incluido el caso en que el receptor también muera o no quede nadie elegible? ¿Solo cambia la recompensa o tiene algún efecto adicional explícito? | Define identidad persistente, disparador de asignación, transición y presentación. No se presupone que lo recibe quien mata ni que cura. |
| 4. Respawn | ¿Cuánto tarda? ¿Usa puntos fijos por equipo u otra regla de aparición? ¿Con qué vida, guardia y energía reaparece? ¿Existe protección inicial o reaparece inmediatamente vulnerable? | Fija temporizador, datos de spawn y estado inicial. Si se requieren puntos libres, hay que definir qué hacer cuando todos estén ocupados, sin teletransporte alternativo silencioso. |
| 5. Baja y espera | ¿El personaje desaparece o permanece visible mientras espera? ¿Deja de bloquear movimiento? ¿Qué muestra/hace la cámara del jugador muerto y qué datos de espera muestra el HUD? | El código actual conserva el pawn. Cámara, colisión y presentación de baja requieren comportamiento expreso; no hace falta elegir una animación de muerte nueva para definirlo. |
| 6. Inicio y salida de partida | ¿El combate empieza al cargar el escenario o hay cuenta previa? Al finalizar, ¿se detienen acciones y respawns y queda una pantalla de resultado? ¿Cómo se inicia otra partida en esta entrega? | Completa el ciclo y permite comprobar que nadie sigue generando puntos o reapareciendo después del resultado. No exige un lobby ni matchmaking. |

Los números iniciales de puntos, segundos y recursos pueden quedar como configuración ajustable; no requieren balance definitivo. Sí deben existir valores elegidos para ejecutar y verificar el primer ciclo. No hace falta que el usuario elija clases C++, estructura interna del estado ni estrategia de replicación para responder estas reglas.

La atribución individual de una baja solo necesita una regla adicional si la transferencia del portador depende del autor. No se incorpora por anticipación un ranking individual, asistencias ni un sistema de daño ambiental/suicidios que el alcance actual no pide. Si se incluyen bajas sin atacante en esta entrega, habrá que fijar su puntuación y el destino del portador.

## Estado real de las piezas a integrar

| Pieza existente | Comprobado en fuentes | Integración necesaria en 6 |
|---|---|---|
| `AVampsCombatTestGameMode` | Crea un rival y su AI, coloca jugador/rival y configura un foco; R restaura dos personajes. | Incorporar una autoridad de partida propia con participantes, equipos, aparición, puntuación, portador y fin. Conservar separado el escenario de observación. |
| `AVampsCharacter` | GAS y estado de combate viven en el pawn. La recepción letal cancela ofensiva/movilidad, limpia reacción/NA y descarta contactos pendientes. No notifica una baja a una partida ni programa respawn. | Emitir una baja única con su contexto; cerrar participación y reaparecer según las reglas elegidas. Equipo, puntuación e identidad del participante deben sobrevivir al pawn si se reemplaza. |
| `UVampsCombatResolver` / `AVampsWeapon` | Capturan emisor, receptor y generaciones; las trazas admiten personajes vivos sin comprobar equipo. El lote se clasifica antes de aplicar consecuencias. | Aplicar elegibilidad enemiga compartida y entregar contexto para puntuar sin duplicados. Conservar contactos válidos capturados y resolver el cierre de partida después de las consecuencias del lote. |
| `UVampsLockOnComponent` | Registra todos los personajes ajenos y selecciona vivos/elegibles. No hay equipo. | Excluir aliados y participantes fuera de combate, mantener el orden de F y admitir nuevamente a un respawn válido. |
| `AVampsAIController` | Modos de observación con foco entregado externamente; el GameMode actual lo fija en el jugador. | Resolver foco y readquisición de un enemigo elegible para los participantes IA de la partida. No usar cambios de modo como sustituto de baja o reinicio global. |
| `AVampsPlayerCameraManager` | Dos encuadres, oclusión, rumbo libre y limpieza por cambio de pawn/generación. | Conectar la espera tras baja que se elija y comprobar limpieza/readquisición al reaparecer. Que exista limpieza por pawn no define por sí solo una cámara de espectador. |
| `AVampsCombatTestHUD` / controller | Vista de combate y F9; lista basada en candidatos vivos, avisos por selección/fase. La rama debug todavía consulta el GameMode de observación y su OpponentAI. | Mostrar solo enemigos en lista/amenazas y agregar equipo, puntuación, portador, tiempo/espera/resultado según las reglas. Adaptar explícitamente la parte de debug que se use en partida, sin asumir que el GameMode nuevo tiene OpponentAI. |

No se encontraron clases propias de GameState/PlayerState de partida, equipos, puntuación, portador ni respawn en los árboles Public/Private. Tener atributos replicados y habilidades ServerOnly no completa 7 ni cambia el alcance local de 6.

### Baja y simultaneidad

`ReceiveCombatHit` conserva el atacante capturado al aplicar el GameplayEffect. El callback de cambio de vida no recibe ese contexto de contacto. Puntuar desde un cambio de vida aislado perdería la atribución o podría repetir la baja por otras actualizaciones.

Además, destruir un pawn o cambiar su CombatGeneration dentro del callback letal invalidaría referencias/generaciones que todavía necesita el lote local del resolver. La implementación deberá conservar ese contexto hasta terminar el lote antes de materializar retirada/destrucción y cierre de partida. Esta es una consecuencia técnica de las reglas ya elegidas, no una nueva pregunta sobre quién tiene prioridad al atacar.

## Qué cambió respecto del roadmap

| Descripción conservada en el plan/documentos | Situación actual y consecuencia para 6 |
|---|---|
| Próxima etapa 4; movilidad referida a 037 y su impacto final | [039](../039-simplificacion-movilidad/reporte.md) y [040](../040-giro-direccional-dash/reporte.md): dos montages con root motion, giro hacia la dirección capturada, acercamiento solo inicial y sin salida especial de Guard por impacto final. No se debe restaurar la implementación anterior para preparar partida. |
| Cámara de 019, pitch fijo y sin fade/planos de acción | [042–046](../046-giro-libre-camara/reporte.md): POV propio con seguimiento/combate, transiciones, fade/restauración y giro progresivo sin target. El trabajo nuevo es integrarlo con baja/respawn y la cámara de espera elegida. |
| Solo HUD de debug y HUD de amenazas pendiente | [048](../048-hud-combate-simple/reporte.md), recibido en `5503c3e`: vista de combate, lista, proximidad, avisos direccionales y F9. Falta conectar datos de partida y filtrar aliados también en la lista, no solo en GetCurrentTarget. |
| Preguntas de equipos, victoria/empate, portador y respawn | Siguen abiertas. No se encontraron decisiones posteriores que autoricen asumir 3v3, puntos del original, kill leader o curación. |
| R de debug y red incompleta | Sigue vigente: R no es un respawn de partida y las solicitudes de clientes no están integradas. No se traslada el paso 7 al 6. |

El plan inicial incluía como entregas 1–3 el botón compartido, cadena/NA y resultados de defensa/daño, que ya están implementados y cerrados por el usuario. El último estado escrito del roadmap reconoce esos cierres, pero conserva pendientes y descripciones de 4–5 anteriores a sus cambios. El orden equipos → red → calibración → extensiones continúa siendo válido.

## Siguiente implementación y límites de la revisión

Después de responder las decisiones: autoridad/participantes/equipos; elegibilidad compartida; baja con contexto y puntuación; respawn; ciclo del portador; resultado y presentación. Las reglas de asignación del portador pueden exigir adelantar su estado antes de resolver puntuación; el orden concreto seguirá esas respuestas.

Se revisaron documentos, fuentes, historial e inventario de `/Game/Vamps`. No se modifican ni releen propiedades de Blueprints/DA por MCP: no hay herramientas directas de Unreal disponibles. El mapa existente sigue siendo `/Game/Vamps/Maps/L_CharacterBasic`; no se presenta un mapa de partida ni assets futuros como ya creados. Las conexiones de la escena de observación conservan la evidencia histórica de 045/048.

No se compila ni se inicia PIE en una revisión sin cambios de código. Tampoco se agregan/ejecutan tests automatizados ni se automatiza la interfaz. El commit del HUD acredita su presencia en el checkout, no una nueva comprobación de build o aspecto visual. Los pendientes anteriores conservan su alcance en la [guía 048](../048-hud-combate-simple/editor.md); esta revisión no los convierte en cierres nuevos.
