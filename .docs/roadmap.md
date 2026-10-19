# Roadmap hacia la emulación de FGOA

El objetivo es reproducir la **[base FGOA adaptada](game/functional.md#alcance-de-la-base)**: intercambio, movilidad, selección, cámara y partida por equipos con un único melee jugable. Después se incorporan las [extensiones propias de Vamps](extensions/functional.md). Emular significa conseguir los comportamientos acordados, no reconstruir algoritmos internos del original ni copiar sus números de balance.

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

**Estado al 19 de octubre de 2026:** etapas 0, 1, 2 y 3 completadas por confirmación del usuario. El [cierre de 3 en 035](reports/035-revision-paso-3/reporte.md#cierre-del-paso-3) conserva las verificaciones MCP previas y registra la aprobación del usuario. Próxima etapa: 4, acercamiento, evasión y sprint.

**Cierre de 3:** implementación en [033](reports/033-resultados-combate/reporte.md), montages independientes en [034](reports/034-montage-guardia/reporte.md) y configuración/intercambios revisados en [035](reports/035-revision-paso-3/reporte.md). El usuario aprobó el cierre el 2026-10-19; no se atribuye a una nueva ejecución de PIE o compilación por Codex.

| Etapa | Estado | Entrega | Dependencia y salida |
|---|---|---|---|
| 0 | Completada | Consolidar el escenario actual | Cierre manual confirmado el 2026-10-15; [registro](reports/020-roadmap-emulacion-fgoa/reporte.md#cierre-de-la-etapa-0) |
| 1 | Completada | Botón compartido y fases de una acción | Cierre confirmado por el usuario el 2026-10-16; [registro 021](reports/021-solicitud-compartida/reporte.md#cierre-de-la-etapa-1) |
| 2 | Completada | Cadena de hasta tres pasos y NA | Cierre confirmado por el usuario el 2026-10-18; [registro 031](reports/031-cierre-paso-2-y-reglas-paso-3/reporte.md) |
| 3 | Completada | Guardia, residual, reacción, stun y crítico | Cierre confirmado por el usuario el 2026-10-19; [registro 035](reports/035-revision-paso-3/reporte.md#cierre-del-paso-3) |
| 4 | Pendiente | Acercamiento, evasión y sprint | Código y assets en 037; falta comprobar impacto final y movilidad en juego antes del cierre |
| 5 | Pendiente | Cámara de acciones, oclusión y HUD de combate | Estados y movimiento de 1–4; presentación coherente con la referencia |
| 6 | Pendiente | Equipos, baja, respawn, puntuación y portador | Decidir reglas de partida; ciclo local de partida completo |
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

Implementación y configuración en [037](reports/037-movilidad-combate/reporte.md), con [operaciones y pendientes del editor](reports/037-movilidad-combate/editor.md). La etapa permanece pendiente de cierre hasta completar su configuración y observación. Las reglas elegidas están en [jugador](player/functional.md#movimiento-y-evasión): energía de movilidad independiente, dash inmediato y sprint sostenido, salida de Guard tras impacto final y dash durante NA, sin invulnerabilidad.

El acercamiento se limita por paso y reemplaza la traslación de raíz ofensiva para no duplicar avance. Shift sin dirección se aleja del target. Se conservan cámara/lock-on y referencia de WASD. La continuación válida tiene prioridad sobre el dash al cerrar un paso; recuperación y NA mantienen sus plazos.

**Cierre:** atacar a un objetivo lejano puede avanzar, quedar corto y ejecutar igualmente el golpe. No hay teletransporte ni acierto garantizado. Evasión cierra la cadena en su frontera, sin cancelación libre en cualquier frame; sprint y movimiento lateral conservan seguimiento. NA permite locomoción y reacción/stun aplican sus restricciones propias.

### 5. Presentación de combate y cámara de referencia

Completar el manager de 019 con la relación visual jugador/target, variación de distancia aparente y planos de ataque/defensa. Calibrar distancia/FOV y transiciones con las acciones y el movimiento ya disponibles. Mantener seguimiento automático, tolerancia lateral y selección compartida; no imponer target clavado al centro ni histéresis no demostrada.

Preparar los oclusores visibles y materiales que se desvanecen/restauran, coordinados con la colisión de cámara. Completar HUD de combate: vida, guardia, NA, reacción, stun, crítico, selección/lista, enemigos próximos y dirección de amenaza. La advertencia de amenaza debe existir antes del resultado de contacto y distinguirse de un bloqueo confirmado.

**Cierre:** acercamiento, retirada, laterales, cambios por F y guardia, y planos de acción mantienen un combate legible. Un obstáculo preparado se desvanece y recupera su presentación sin perder su colisión de movimiento; un límite invisible continúa restringiendo sin requerir fade. Cámara y HUD siguen al mismo rival, incluida ausencia de target.

**Hito A — Emulación local del combate:** etapas 0–5 comprobadas en el escenario de observación, con un melee y varios rivales. Cierra el intercambio y su presentación; todavía no representa una partida multijugador completa.

### 6. Partida por equipos

Resolver tamaño de equipos, victoria/tiempo/empate, respawn y reglas del portador en [partida](game/functional.md#preguntas). Definir asignación, aparición, transferencia y efectos del portador sin asumir kill leader ni curación por analogía con el Grial.

Implementar equipos y elegibilidad de enemigos tanto en selección como en contactos. Agregar baja, salida efectiva del combate, puntos, respawn del mismo personaje, portador y fin de partida. Conectar HUD de puntuación/portador y limpieza de acciones, trazas y selección al morir/reaparecer. Mantener R como control de debug, separado del ciclo de partida.

**Cierre:** aliados no figuran como objetivos enemigos; golpear a un enemigo ocupado con otro aliado sigue permitido. Una baja normal y la del portador conceden el valor definido; el personaje reaparece según la regla sin consumir reservas. Cámara/HUD dejan al eliminado y admiten al reaparecido. El resultado de equipo cierra la partida sin confundirlo con una derrota individual.

### 7. Varios jugadores y autoridad de red

Diseñar la entrega de solicitudes y el estado compartido para las reglas ya jugables. Implementar autoridad de acciones, contactos, daño y partida; sincronizar animación, resultados, bajas/respawn y HUD. La selección que intervenga en combate debe ser coherente con la autoridad, manteniendo presentación de cámara local por jugador. Elegir el tratamiento de latencia necesario al abordar esta integración; el video no determina predicción ni compensación.

**Cierre:** comprobar manualmente en PIE con servidor y clientes que ambos pueden solicitar ataque/defensa, encadenar y desplazarse, y que coinciden en resultados, puntuación, portador y cierre. Repetir bajo las condiciones de latencia elegidas para la verificación. Ningún cliente necesita autoridad local para que se entregue su solicitud, ni ejecuta daño/puntos duplicados. Replicar atributos o usar ServerOnly por sí solos no aprueba esta etapa.

### 8. Calibración y cierre de la base

Recorrer las [escenas de referencia](reports/016-analisis-fgo-arcade/escenas.md) contra el comportamiento jugable, aplicando las exclusiones acordadas. Ajustar ventanas, recuperaciones, avance, evasión/sprint, cámara y señales como un conjunto; no copiar frames o distancias sin escala demostrada. Repetir los casos integrados localmente y en red cuando la calibración los afecte.

**Hito B — Base FGOA adaptada cerrada:** se cumplen los cierres de 0–7, las preguntas necesarias de combate/jugador/partida tienen respuesta incorporada en su especificación, y no queda una diferencia pendiente de las reglas incluidas escondida como «pulido». Registrar comparación, compilación, comprobaciones y límites en el reporte de cierre. Este es el requisito para empezar la capa propia.

### 9. Agregar Vamps sobre la base

Desarrollar [identidades y dos skills por personaje](extensions/characters/functional.md), parry y ultimate propia sin cinemática global, resolviendo sus preguntas cuando se aborde cada funcionalidad. El orden interno se decide después del hito B. No añadir ahora bindings, atributos o infraestructura para estas extensiones.

## Límites y seguimiento

La base excluye cartas/mano, Q/A/B, Extra Attack, reservas/sustitución, ranged inicial, SUPPORT y NP del original. Estas exclusiones no son tareas pendientes de emulación. Se conservan las adaptaciones elegidas: cadena/NA directas, guardia 360°, Shift sin dirección hacia atrás, respawn del mismo personaje y portador sin temática Grial. Parry, skills, perfiles y ultimate propias pertenecen a 9.

Cada implementación actualiza su estado, código/assets afectados y comprobaciones en un par de reportes. Revisar código, compilar mediante MCP directo cuando corresponda y comprobar manualmente en el editor; no crear ni ejecutar tests automatizados. Las operaciones sin MCP directo quedan como pasos exactos para el usuario, sin automatización de UI. No marcar una etapa cerrada solo porque compila ni avanzar decisiones abiertas mediante fallbacks.
