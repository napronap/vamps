# Roadmap hacia la emulación de FGOA

El objetivo es reproducir la **[base FGOA adaptada](game/functional.md#alcance-de-la-base)**: intercambio, movilidad, selección, cámara y partida por equipos, usando Executioner para todos en la primera entrega local. La selección de personaje se incorpora en la segunda parte de 6; después de cerrar la base se agregan las [extensiones propias de Vamps](extensions/functional.md). Emular significa conseguir los comportamientos acordados, no reconstruir algoritmos internos del original ni copiar sus números de balance.

Las reglas viven en [combate](combat/functional.md), [jugador](player/functional.md) y [partida](game/functional.md), con sus documentos técnicos. El [análisis 016](reports/016-analisis-fgo-arcade/analisis.md) y sus [escenas](reports/016-analisis-fgo-arcade/escenas.md) sirven para comparar resultados. No hace falta reabrir la investigación ni pedir nuevas grabaciones para avanzar.

## Punto de partida

| Área | Disponible | Brecha hacia la referencia |
|---|---|---|
| Personaje y GAS | Clases propias, vida/guardia, cadena de hasta tres secciones, ventanas, NA, armas independientes y reacciones; paso 3 cerrado por el usuario en 035 | Integrar movilidad de 4 y presentación de 5 |
| Ataque y defensa | RMB compartido, memoria defensiva por paso, Guard que cancela ofensiva propia, residual, acumulación/ruptura/stun y crítico; cierre de 3 en 035 | Calibración integral en 8 |
| Lock-on y HUD | Componente compartido, F según lista, selección inicial, reemplazo al perder elegibilidad y retarget por bloqueo; debug de fases y resultados | Incorporar equipos y respawn cuando existan |
| Cámara | Manager local, seguimiento automático suave, tolerancia lateral y retorno detrás del jugador sin target ([019](reports/019-camara-lockon/reporte.md)); observación visual aprobada al cerrar 0 | Pitch fijo, sin variación de distancia por acción ni fade |
| Escenario e IA | Mapa local, rival con modos de observación y solicitud compartida, F6/R y HUD de debug | R no representa muerte ni respawn |
| Partida y red | Habilidades ServerOnly y atributos replicados | No hay partida por equipos ni entrega completa de solicitudes de clientes |

018–019 y las fuentes actuales prevalecen para lock-on/cámara sobre la descripción anterior del [prototipo](prototype/technical.md). Sus comprobaciones MCP se complementan con el cierre manual de 0 registrado en [020](reports/020-roadmap-emulacion-fgoa/reporte.md#cierre-de-la-etapa-0).

## Orden de ejecución

Este archivo lleva el estado del plan; los reportes conservan la evidencia y quién confirmó cada cierre. Al completar una etapa se actualizan aquí su estado, fecha y referencia al reporte. No se mantiene otro archivo con una copia del avance.

**Estado al 27 de octubre de 2026:** etapas 0–3 cerradas por el usuario. Movilidad y presentación tienen implementación posterior en 039–048, con los límites de verificación registrados allí. El trabajo actual define 6 en [050](reports/050-reglas-partida-local/reporte.md): ciclo local 2v2 primero y selección de personaje en una segunda parte. No se atribuyen nuevas comprobaciones del editor ni cierres de 4–5 a esta actualización.

**Cierre de 3:** implementación en [033](reports/033-resultados-combate/reporte.md), montages independientes en [034](reports/034-montage-guardia/reporte.md) y configuración/intercambios revisados en [035](reports/035-revision-paso-3/reporte.md). El usuario aprobó el cierre el 2026-10-19; no se atribuye a una nueva ejecución de PIE o compilación por Codex.

| Etapa | Estado | Entrega | Dependencia y salida |
|---|---|---|---|
| 0 | Completada | Consolidar el escenario actual | Cierre manual confirmado el 2026-10-15; [registro](reports/020-roadmap-emulacion-fgoa/reporte.md#cierre-de-la-etapa-0) |
| 1 | Completada | Botón compartido y fases de una acción | Cierre confirmado por el usuario el 2026-10-16; [registro 021](reports/021-solicitud-compartida/reporte.md#cierre-de-la-etapa-1) |
| 2 | Completada | Cadena de hasta tres pasos y NA | Cierre confirmado por el usuario el 2026-10-18; [registro 031](reports/031-cierre-paso-2-y-reglas-paso-3/reporte.md) |
| 3 | Completada | Guardia, residual, reacción, stun y crítico | Cierre confirmado por el usuario el 2026-10-19; [registro 035](reports/035-revision-paso-3/reporte.md#cierre-del-paso-3) |
| 4 | Implementada; cierre pendiente | Acercamiento, evasión y sprint | Configuración vigente en 039–040; la revisión 049 conserva diferencias respecto de 037 |
| 5 | Implementada; verificación pendiente | Cámara de acciones, oclusión y HUD de combate | Cámara 042–046 y HUD 048; conservar sus pendientes de compilación/observación |
| 6 | En definición | Equipos, bajas, recompensa por racha, respawn, resultado y revancha local | Primera parte 2v2 con Executioner; segunda parte con selección de personaje; decisiones en 050 |
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

La implementación vigente está en [039](reports/039-simplificacion-movilidad/reporte.md) y [040](reports/040-giro-direccional-dash/reporte.md), con sus guías de observación. Usa dos montages con root motion, giro hacia la dirección capturada al aceptar el dash y energía independiente para dash/sprint. Guard conserva su duración ordinaria; se retiró el permiso por impacto final. La etapa mantiene los pendientes de observación registrados allí.

El acercamiento se limita al primer paso y reemplaza la traslación de raíz ofensiva para no duplicar avance. Shift sin dirección se aleja del target. Se conservan cámara/lock-on y referencia de WASD. La continuación válida tiene prioridad sobre el dash al cerrar un paso; recuperación y NA mantienen sus plazos.

**Cierre:** atacar a un objetivo lejano puede avanzar, quedar corto y ejecutar igualmente el golpe. No hay teletransporte ni acierto garantizado. Evasión cierra la cadena en su frontera, sin cancelación libre en cualquier frame; sprint y movimiento lateral conservan seguimiento. NA permite locomoción y reacción/stun aplican sus restricciones propias.

### 5. Presentación de combate y cámara de referencia

La cámara de [042–046](reports/046-giro-libre-camara/reporte.md) incorpora seguimiento/combate, Eye/Interest, variación de distancia/FOV, transiciones, oclusión y rumbo progresivo sin target. Calibrar su presentación con las acciones y el movimiento disponibles, conservando selección compartida y los pendientes visuales de sus guías.

Los oclusores y el material de fade preparados en 042 se coordinan con la colisión de cámara. El HUD simple de [048](reports/048-hud-combate-simple/reporte.md) presenta recursos, estados, selección/lista, proximidad y dirección de amenaza, con alternancia F9. Conservar su pendiente de compilación/observación: la advertencia previa al contacto debe distinguirse del bloqueo confirmado.

**Cierre:** acercamiento, retirada, laterales, cambios por F y guardia, y planos de acción mantienen un combate legible. Un obstáculo preparado se desvanece y recupera su presentación sin perder su colisión de movimiento; un límite invisible continúa restringiendo sin requerir fade. Cámara y HUD siguen al mismo rival, incluida ausencia de target.

**Hito A — Emulación local del combate:** etapas 0–5 comprobadas en el escenario de observación, con un melee y varios rivales. Cierra el intercambio y su presentación; todavía no representa una partida multijugador completa.

### 6. Partida por equipos

La [partida funcional](game/functional.md) define dos entregas. **6.1:** ciclo local 2v2, jugador y un bot en A contra dos bots en B, todos Executioner. **6.2:** selección de personaje y preparación de participantes. Las identidades/skills propias continúan en 9; no se anticipa ese sistema en 6.1.

Implementar autoridad de partida, equipos y elegibilidad enemiga en selección, HUD, IA y contactos. Conectar bajas únicas con su contexto, recompensa creciente por racha de la víctima, respawn por equipo, protección breve y resultado después del lote de contactos. Objetivo, fórmula inicial, recursos y plazos se definen en [partida](game/functional.md#puntuación). El portador se elimina del alcance.

El HUD presenta equipo, puntos, kills/muertes/K/D, countdown de preparación/respawn, tiempo restante y panel de ganador/empate con «Otra partida». Incorporar muerte normal/defensiva desde referencias del DA y cámara libre durante la espera. La protección de respawn termina al atacar. La revancha local requiere solo la aceptación local; esperar la aceptación de todos queda para una ampliación posterior. Mantener R como control de observación separado del ciclo de partida.

**Cierre de 6.1:** aliados fuera de selección/contactos; dos aliados pueden atacar al mismo enemigo. La recompensa usa la racha anterior al lote y se registra una sola baja por vida. El mismo personaje reaparece en su equipo con sus recursos y protección; cámara/HUD limpian al eliminado y admiten al reaparecido. Si ambos equipos alcanzan el objetivo simultáneamente, decide K/D; si coincide, hay empate. El resultado detiene la partida y la revancha inicia un ciclo limpio. Resolver antes de implementar las [preguntas restantes](game/functional.md#preguntas). El cierre de 6.2 se especificará al abordar selección de personaje.

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
