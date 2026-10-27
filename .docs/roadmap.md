# Roadmap hacia la emulación de FGOA

El objetivo es reproducir la **[base FGOA adaptada](game/functional.md#alcance-de-la-base)**: intercambio, movilidad, selección, cámara y partida por equipos, usando Executioner para todos en la primera entrega local. La selección de personaje se incorpora en la segunda parte de 6; después de cerrar la base se agregan las [extensiones propias de Vamps](extensions/functional.md). Emular significa conseguir los comportamientos acordados, no reconstruir algoritmos internos del original ni copiar sus números de balance.

Las reglas viven en [combate](combat/functional.md), [jugador](player/functional.md) y [partida](game/functional.md), con sus documentos técnicos. El [análisis 016](reports/016-analisis-fgo-arcade/analisis.md) y sus [escenas](reports/016-analisis-fgo-arcade/escenas.md) sirven para comparar resultados. No hace falta reabrir la investigación ni pedir nuevas grabaciones para avanzar.

## Estado disponible

| Área | Disponible | Brecha hacia la referencia |
|---|---|---|
| Personaje y GAS | Clases propias, cadena/NA, armas, reacciones, movilidad y presentación integradas | Calibración integral en 8 |
| Ataque y defensa | RMB compartido, memoria defensiva por paso, Guard que cancela ofensiva propia, residual, acumulación/ruptura/stun y crítico; cierre de 3 en 035 | Calibración integral en 8 |
| Lock-on y HUD | Selección enemiga compartida, F y retarget por bloqueo; recursos, amenazas, proximidad, marcador y resultado; F9 alterna debug | Integración de clientes en 7 |
| Cámara | Seguimiento/combate, distancia/FOV, transiciones, oclusores y rumbo sin target; cámara libre durante respawn | Calibración integral en 8 |
| Escenario e IA | L_LocalMatch con selector Executioner, START y jugador más tres bots; L_CharacterBasic conserva observación R/F6/F7 | Participantes humanos y bots en red en 7 |
| Partida y red | Ciclo local 2v2 con bajas, rachas, respawn, protección, resultado y revancha; atributos replicados y habilidades ServerOnly | Entrega de solicitudes y estado compartido de clientes en 7 |

Los reportes 039–048, 051 y 055 describen movilidad, presentación, partida y selector vigentes. El [prototipo](prototype/technical.md) conserva el escenario de observación y su contexto histórico.

## Orden de ejecución

Este archivo lleva el estado del plan; los reportes conservan la evidencia y quién confirmó cada cierre. Al completar una etapa se actualizan aquí su estado, fecha y referencia al reporte. No se mantiene otro archivo con una copia del avance.

**Estado al 27 de octubre de 2026:** etapas 0–6 completadas, incluidas 6.1 y 6.2. Cierres locales anteriores en [053](reports/053-cierre-local-y-documentacion/reporte.md); selector implementado en [055](reports/055-selector-executioner/reporte.md), con compilación y funcionamiento confirmados por el usuario. **Siguiente etapa: 7, varios jugadores y autoridad de red**; calibración integral y extensiones siguen en 8–9. Este cierre no representa una nueva compilación o ejecución de PIE por Codex.

**Cierre de 3:** implementación en [033](reports/033-resultados-combate/reporte.md), montages independientes en [034](reports/034-montage-guardia/reporte.md) y configuración/intercambios revisados en [035](reports/035-revision-paso-3/reporte.md). El usuario aprobó el cierre el 2026-10-19; no se atribuye a una nueva ejecución de PIE o compilación por Codex.

| Etapa | Estado | Entrega | Dependencia y salida |
|---|---|---|---|
| 0 | Completada | Consolidar el escenario actual | Cierre manual confirmado el 2026-10-15; [registro](reports/020-roadmap-emulacion-fgoa/reporte.md#cierre-de-la-etapa-0) |
| 1 | Completada | Botón compartido y fases de una acción | Cierre confirmado por el usuario el 2026-10-16; [registro 021](reports/021-solicitud-compartida/reporte.md#cierre-de-la-etapa-1) |
| 2 | Completada | Cadena de hasta tres pasos y NA | Cierre confirmado por el usuario el 2026-10-18; [registro 031](reports/031-cierre-paso-2-y-reglas-paso-3/reporte.md) |
| 3 | Completada | Guardia, residual, reacción, stun y crítico | Cierre confirmado por el usuario el 2026-10-19; [registro 035](reports/035-revision-paso-3/reporte.md#cierre-del-paso-3) |
| 4 | Completada | Acercamiento, evasión y sprint | Configuración 039–040; cierre confirmado por el usuario el 2026-10-27 en 053 |
| 5 | Completada | Cámara de acciones, oclusión y HUD de combate | Cámara 042–046 y HUD 048; cierre confirmado por el usuario el 2026-10-27 en 053 |
| 6.1 | Completada | Equipos, bajas, rachas, respawn, resultado y revancha local 2v2 | Implementación/evidencia 051; comprobación final del usuario el 2026-10-27 en 053 |
| 6.2 | Completada | Selector Executioner, START y revancha al selector | Compilación y funcionamiento confirmados por el usuario el 2026-10-27; cierre en 055 |
| 7 | Pendiente | Integración de varios jugadores en red | Reglas locales de 1–6; clientes pueden jugar el mismo ciclo bajo autoridad |
| 8 | Pendiente | Calibración y cierre de la base | Comparación integral local y en red; sin brechas de comportamiento acordadas |
| 9 | Pendiente | Extensiones Vamps | Base cerrada en 8; identidades, dos skills, parry y ultimate propias |

### 0. Consolidar lo implementado

**Completada — usuario, 2026-10-15.** El usuario confirmó «punto 0 esta OK», cerrando la compilación completa, revisión del mapa, observación de cámara/WASD/F y comprobación de contacto/daño que agrupaba esta etapa. La [guía 019](reports/019-camara-lockon/editor.md) y la [comprobación de contacto de 013](reports/013-recuperacion-blueprints/editor.md#5-comprobaciones) quedan como instrucciones reproducibles. El [reporte 020](reports/020-roadmap-emulacion-fgoa/reporte.md#cierre-de-la-etapa-0) registra la atribución del cierre; no representa una nueva ejecución MCP.

Conservar selección inicial, recorrido y pérdida implementados en 018, y seguimiento automático de 019. No reconstruir estos sistemas como parte del siguiente cambio. Separar correcciones necesarias de la calibración fina, que continúa en 5 y 8.

**Cierre:** en el escenario local se ve un golpe que alcanza y resta vida, uno fuera de alcance que no lo hace, F que recorre varios rivales y una cámara que acompaña al seleccionado. La pérdida/readquisición ya observadas por propiedades deben ser coherentes con el HUD y la vista. Esto no aprueba las reglas antiguas de guardia como emulación.

### 1. Botón compartido y fases de una acción

**Completada — usuario, 2026-10-16.** El usuario confirmó el cierre después de aclarar las fronteras y revisar la explicación de la resolución ataque/defensa. Solicitud compartida, fases, resolver, IA y debug están presentes; input y ocho Blueprints fueron verificados por MCP. Live Coding devolvió NoChanges en esta revisión. El cierre se atribuye a la confirmación del usuario y no a una nueva ejecución de PIE por Codex; [reporte 021](reports/021-solicitud-compartida/reporte.md#cierre-de-la-etapa-1) y [guía reproducible](reports/021-solicitud-compartida/editor.md).

Las [reglas acordadas](reports/021-solicitud-compartida/reporte.md#reglas-acordadas) usan un único ANS_AttackTrace: Preparation desde el inicio hasta AttackStart; Contacts hasta AttackEnd; Recovery hasta MontageEnd. La defensa se arma con el click, se consume por el primer lote de contactos y puede rearmarse antes de AttackEnd. Recovery no admite ataque ni defensa y permite locomoción y mezcla con otras acciones; RMB no inicia otro ataque hasta finalizar el montage. No se necesita AN_CombatRecovery ni una ventana separada en esta etapa. La resolución conjunta usa el estado al final del frame, sin buffer ni prioridades por identificador.

1. Distinguir solicitud, acción aceptada, preparación, contactos y recuperación de un único paso. La ventana de cadena se incorpora en 2.
2. Sustituir Attack/Guard separados por pulsaciones individuales de RMB. Cambiar el mapping acompaña al cambio del núcleo; no lo reemplaza.
3. Permitir defensa válida durante preparación y contactos, hasta AttackEnd, con consumo y rearme por click. Conservar retarget por guardia efectiva y las ofensivas de ambos participantes al bloquear.
4. Adaptar la IA de observación a las mismas solicitudes y mostrar fase, disponibilidad y resultado necesarios en el debug. Ningún controller decide daño o bloqueo.

**Cierre aceptado:** sostener RMB no repite acciones ni mantiene guardia permanente; una solicitud válida puede defender durante preparación y contactos. AttackEnd termina defensa y contactos; Recovery rechaza RMB hasta MontageEnd sin limitar locomoción. Bloquear cambia al agresor como target sin cancelar su ofensiva. Los contactos del mismo frame se evalúan juntos antes de consumir defensa o aplicar daño.

### 2. Cadena y recuperación NA

**Completada — usuario, 2026-10-18.** El usuario confirmó «Marca paso 2 como completado porque lo esta». El cierre se registra en [031](reports/031-cierre-paso-2-y-reglas-paso-3/reporte.md), sin atribuir una nueva compilación, ejecución de PIE o edición de assets a Codex. La implementación y comprobaciones anteriores permanecen en 022–030.

Se incorporaron hasta tres pasos por secciones de un montage, ventana y contacto habilitante, cierre por fallo, por no continuar y después del tercer paso, y recuperación NA. Un ANS_AttackTrace por sección y AN_ResetWeaponHits opcionales permiten varios impactos con registros independientes por arma. La frontera de fin de paso será también la que use evasión en 4. La IA usa las mismas solicitudes y ventanas.

**Cierre aceptado:** un click temprano se descarta; un contacto abierto, bloqueado o contra otro rival permite encadenar sin forzar retarget ofensivo. Un paso fallido corta aunque haya pulsaciones. NA impide atacar/defender y permite locomoción. El paso 3 reemplaza la conservación de la ofensiva propia al bloquear por una reacción Guard que la cancela sin crear NA; esa revisión de comportamiento no reabre el cierre de 2.

### 3. Resultados de daño y defensa

**Completada — usuario, 2026-10-19.** El usuario aprobó documentar el cierre tras la revisión de estado; [registro 035](reports/035-revision-paso-3/reporte.md#cierre-del-paso-3). [033](reports/033-resultados-combate/reporte.md) conserva implementación y compilación, y [034](reports/034-montage-guardia/reporte.md) la separación de montages. Las [reglas de combate](combat/functional.md#guardia-daño-y-reacción) definen Guard que cancela la ofensiva propia, reacción común de bloqueo/daño, memoria A→B→A, contactos capturados, carga/ruptura/stun y daño normal/crítico plano.

La barra y las reacciones ya tienen atributos/estados y parámetros configurados. La identidad defensiva cambia por paso; el crítico se tira por contacto deduplicado; el coeficiente plano pertenece al DA del defensor. Las decisiones de simultaneidad están incorporadas en el funcional. Calibrar cantidades y tiempos mediante observación, sin tratarlos como valores deducidos del video.

El HUD presenta el resultado resuelto y AnimInstance expone las señales de reacción. Guard y Hit tienen asignados AM_Executioner_Block y AM_Executioner_Hit; su duración lógica actual es 1 s por paso. La IA usa los mismos permisos. La [guía 035](reports/035-revision-paso-3/editor.md) conserva las comprobaciones reproducibles y distingue la aprobación del usuario de las lecturas MCP.

**Cierre aceptado:** daño abierto, daño defendido, guard break, reacción, stun y NA diferenciados; guardia 360°, residual letal, acumulación/recuperación de guardia, memoria A→B→A y señal de crítico. Bloquear cancela la ofensiva propia; las reacciones vencen según el último contacto y no por la longitud del montage. Bloquear o recibir daño no crea NA automáticamente. La baja y reaparición completas se integran en 6.

### 4. Movimiento integrado al combate

**Completada — usuario, 2026-10-27.** Comprobación general confirmada en [053](reports/053-cierre-local-y-documentacion/reporte.md). Implementación vigente en [039](reports/039-simplificacion-movilidad/reporte.md) y [040](reports/040-giro-direccional-dash/reporte.md): dos montages con root motion, giro hacia la dirección capturada y energía independiente para dash/sprint. Guard conserva su duración ordinaria; se retiró el permiso por impacto final. Las guías anteriores conservan sus pasos reproducibles; sus pendientes de cierre quedan resueltos por esta confirmación.

El acercamiento se limita al primer paso y reemplaza la traslación de raíz ofensiva para no duplicar avance. Shift sin dirección se aleja del target. Se conservan cámara/lock-on y referencia de WASD. La continuación válida tiene prioridad sobre el dash al cerrar un paso; recuperación y NA mantienen sus plazos.

**Cierre:** atacar a un objetivo lejano puede avanzar, quedar corto y ejecutar igualmente el golpe. No hay teletransporte ni acierto garantizado. Evasión cierra la cadena en su frontera, sin cancelación libre en cualquier frame; sprint y movimiento lateral conservan seguimiento. NA permite locomoción y reacción/stun aplican sus restricciones propias.

### 5. Presentación de combate y cámara de referencia

**Completada — usuario, 2026-10-27.** El trabajo restante y la comprobación de funcionamiento están confirmados en [053](reports/053-cierre-local-y-documentacion/reporte.md). La cámara de [042–046](reports/046-giro-libre-camara/reporte.md) incorpora seguimiento/combate, Eye/Interest, distancia/FOV, transiciones, oclusión y rumbo progresivo sin target. La calibración integral continúa en 8.

Los oclusores y el material de fade de 042 se coordinan con la colisión de cámara. El HUD de [048](reports/048-hud-combate-simple/reporte.md) presenta recursos, estados, selección/lista, proximidad y amenaza con alternancia F9. La compilación posterior de 051 y la confirmación del usuario resuelven sus pendientes anteriores; la advertencia previa al contacto sigue siendo distinta del bloqueo confirmado.

**Cierre:** acercamiento, retirada, laterales, cambios por F y guardia, y planos de acción mantienen un combate legible. Un obstáculo preparado se desvanece y recupera su presentación sin perder su colisión de movimiento; un límite invisible continúa restringiendo sin requerir fade. Cámara y HUD siguen al mismo rival, incluida ausencia de target.

**Hito A — Emulación local del combate: completado, 2026-10-27.** Etapas 0–5 aprobadas por el usuario; cierra intercambio y presentación local. La integración multijugador pertenece a 7.

### 6. Partida por equipos

**Completada — usuario, 2026-10-27.** 6.1 tiene implementación/evidencia MCP en [051](reports/051-partida-local/reporte.md) y cierre en [053](reports/053-cierre-local-y-documentacion/reporte.md). 6.2 tiene diseño en [054](reports/054-revision-seleccion-personaje/reporte.md), implementación y cierre en [055](reports/055-selector-executioner/reporte.md). El usuario confirmó que compiló y probó el selector y que funciona todo; no quedan pendientes locales para cerrar esta etapa.

La [partida funcional](game/functional.md) define dos entregas. **6.1:** ciclo local 2v2, jugador y un bot en A contra dos bots en B, todos Executioner. **6.2:** [selección propia y START](game/functional.md#selección-de-personaje-e-inicio), bots asignados del catálogo con repetidos permitidos y PLAY AGAIN de vuelta al selector conservando la preselección. El único personaje disponible sigue siendo Executioner; el usuario preparará después el segundo personaje y su DA. Las identidades/skills propias continúan en 9.

AVampsMatchGameMode administra equipos y elegibilidad enemiga en selección, HUD, IA y contactos. Las bajas conservan su contexto, recompensa por racha y respawn por equipo con protección; el resultado se decide después del lote de contactos. Objetivo, fórmula, recursos y plazos se definen en [partida](game/functional.md#puntuación). No existe portador.

El HUD presenta equipo, puntos, kills/muertes/K/D, preparación/respawn, tiempo restante y ganador/empate con «Otra partida». El DA referencia muerte normal/defensiva; la espera admite cámara libre con WASD/mouse. La protección termina al atacar. La revancha requiere aceptación local; esperar a todos queda para una ampliación posterior. R conserva su función de observación fuera del ciclo de partida.

**Cierre aceptado de 6.1:** aliados fuera de selección/contactos; dos aliados pueden atacar al mismo enemigo. La recompensa usa la racha anterior al lote y se registra una sola baja por vida. El mismo personaje reaparece con sus recursos y protección; cámara/HUD limpian al eliminado y admiten al reaparecido. Si ambos equipos alcanzan el objetivo simultáneamente, decide K/D; si coincide, hay empate. El resultado detiene la partida y la revancha inicia un ciclo limpio. La confirmación del usuario es general; no agrega trazas individuales de cada caso límite.

**Cierre aceptado de 6.2:** abrir en selección sin iniciar combate, elegir Executioner y pulsar START para preparar los cuatro participantes y comenzar los 5 s existentes, sin confirmación adicional. Respawn conserva el personaje. PLAY AGAIN regresa al selector con la preselección propia y START inicia otra ronda limpia. La compilación y comprobación general se atribuyen al usuario; la validación de configuración conserva su revisión de código, sin afirmar nuevas capturas individuales de errores. El segundo personaje, retratos y preview 3D no son requisitos de este cierre.

### 7. Varios jugadores y autoridad de red

Diseñar la entrega de solicitudes y el estado compartido para las reglas ya jugables. Implementar autoridad de acciones, contactos, daño y partida; sincronizar animación, resultados, bajas/respawn y HUD. La selección que intervenga en combate debe ser coherente con la autoridad, manteniendo presentación de cámara local por jugador. Elegir el tratamiento de latencia necesario al abordar esta integración; el video no determina predicción ni compensación.

**Cierre:** comprobar manualmente en PIE con servidor y clientes que ambos pueden solicitar ataque/defensa, encadenar y desplazarse, y que coinciden en resultados, puntuación, rachas, bajas/respawn y cierre. Repetir bajo las condiciones de latencia elegidas para la verificación. Ningún cliente necesita autoridad local para que se entregue su solicitud, ni ejecuta daño/puntos duplicados. Replicar atributos o usar ServerOnly por sí solos no aprueba esta etapa.

### 8. Calibración y cierre de la base

Recorrer las [escenas de referencia](reports/016-analisis-fgo-arcade/escenas.md) contra el comportamiento jugable, aplicando las exclusiones acordadas. Ajustar ventanas, recuperaciones, avance, evasión/sprint, cámara y señales como un conjunto; no copiar frames o distancias sin escala demostrada. Repetir los casos integrados localmente y en red cuando la calibración los afecte.

**Hito B — Base FGOA adaptada cerrada:** se cumplen los cierres de 0–7, las preguntas necesarias de combate/jugador/partida tienen respuesta incorporada en su especificación, y no queda una diferencia pendiente de las reglas incluidas escondida como «pulido». Registrar comparación, compilación, comprobaciones y límites en el reporte de cierre. Este es el requisito para empezar la capa propia.

### 9. Agregar Vamps sobre la base

Desarrollar [identidades y dos skills por personaje](extensions/characters/functional.md), parry y ultimate propia sin cinemática global, resolviendo sus preguntas cuando se aborde cada funcionalidad. El orden interno se decide después del hito B. No añadir ahora bindings, atributos o infraestructura para estas extensiones.

## Límites y seguimiento

La base excluye cartas/mano, Q/A/B, Extra Attack, reservas/sustitución, ranged inicial, SUPPORT, portador y NP del original. Estas exclusiones no son tareas pendientes de emulación. Se conservan las adaptaciones elegidas: cadena/NA directas, guardia 360°, Shift sin dirección hacia atrás, respawn del mismo personaje y recompensa creciente por racha. La selección de personaje pertenece a 6.2; parry, skills, perfiles y ultimate propias pertenecen a 9.

Cada implementación actualiza su estado, código/assets afectados y comprobaciones en un par de reportes. Revisar código, compilar mediante MCP directo cuando corresponda y comprobar manualmente en el editor; no crear ni ejecutar tests automatizados. Las operaciones sin MCP directo quedan como pasos exactos para el usuario, sin automatización de UI. No marcar una etapa cerrada solo porque compila ni avanzar decisiones abiertas mediante fallbacks.
