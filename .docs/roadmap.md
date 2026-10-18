# Roadmap hacia la emulación de FGOA

El objetivo es reproducir la **[base FGOA adaptada](game/functional.md#alcance-de-la-base)**: intercambio, movilidad, selección, cámara y partida por equipos con un único melee jugable. Después se incorporan las [extensiones propias de Vamps](extensions/functional.md). Emular significa conseguir los comportamientos acordados, no reconstruir algoritmos internos del original ni copiar sus números de balance.

Las reglas viven en [combate](combat/functional.md), [jugador](player/functional.md) y [partida](game/functional.md), con sus documentos técnicos. El [análisis 016](reports/016-analisis-fgo-arcade/analisis.md) y sus [escenas](reports/016-analisis-fgo-arcade/escenas.md) sirven para comparar resultados. No hace falta reabrir la investigación ni pedir nuevas grabaciones para avanzar.

## Punto de partida

| Área | Disponible | Brecha hacia la referencia |
|---|---|---|
| Personaje y GAS | Clases propias, atributos de vida, montage y trazas del puño; fases de una acción delimitadas por un único ANS | No hay cadena ni NA |
| Ataque y defensa | RMB compartido, defensa consumible hasta AttackEnd, resolución por frame y retarget al bloquear; cierre de 1 confirmado por el usuario | Incorporar cadena en 2 y resultados de guardia/daño en 3; el bloqueo actual evita todo daño |
| Lock-on y HUD | Componente compartido, F según lista, selección inicial, reemplazo al perder elegibilidad y retarget por bloqueo; debug de fases y resultados | Incorporar equipos y respawn cuando existan |
| Cámara | Manager local, seguimiento automático suave, tolerancia lateral y retorno detrás del jugador sin target ([019](reports/019-camara-lockon/reporte.md)); observación visual aprobada al cerrar 0 | Pitch fijo, sin variación de distancia por acción ni fade |
| Escenario e IA | Mapa local, rival con modos de observación y solicitud compartida, F6/R y HUD de debug | R no representa muerte ni respawn |
| Partida y red | Habilidades ServerOnly y atributos replicados | No hay partida por equipos ni entrega completa de solicitudes de clientes |

018–019 y las fuentes actuales prevalecen para lock-on/cámara sobre la descripción anterior del [prototipo](prototype/technical.md). Sus comprobaciones MCP se complementan con el cierre manual de 0 registrado en [020](reports/020-roadmap-emulacion-fgoa/reporte.md#cierre-de-la-etapa-0).

## Orden de ejecución

Este archivo lleva el estado del plan; los reportes conservan la evidencia y quién confirmó cada cierre. Al completar una etapa se actualizan aquí su estado, fecha y referencia al reporte. No se mantiene otro archivo con una copia del avance.

**Estado al 16 de octubre de 2026:** etapas 0 y 1 completadas por confirmación del usuario. La etapa 1 usa un único ANS: defensa desde el inicio hasta AttackEnd, contactos entre AttackStart y AttackEnd y recuperación hasta MontageEnd. RMB espera al final del montage; Recovery permite locomoción y no es NA. Próxima etapa: 2, cadena y NA. [Cierre 021](reports/021-solicitud-compartida/reporte.md#cierre-de-la-etapa-1).

| Etapa | Estado | Entrega | Dependencia y salida |
|---|---|---|---|
| 0 | Completada | Consolidar el escenario actual | Cierre manual confirmado el 2026-10-15; [registro](reports/020-roadmap-emulacion-fgoa/reporte.md#cierre-de-la-etapa-0) |
| 1 | Completada | Botón compartido y fases de una acción | Cierre confirmado por el usuario el 2026-10-16; [registro 021](reports/021-solicitud-compartida/reporte.md#cierre-de-la-etapa-1) |
| 2 | Pendiente | Cadena de hasta tres pasos y NA | Fronteras y contactos de 1; agregar ventanas y distinguir continuidad, fallo y NA |
| 3 | Pendiente | Guardia, residual, reacción, stun y crítico | Decidir fórmulas y restricciones; resultados de combate completos y visibles |
| 4 | Pendiente | Acercamiento, evasión y sprint | Fronteras de 2 y restricciones de 3; movilidad integrada sin acierto garantizado |
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

Sobre las fases de 1, incorporar hasta tres pasos con ventana y contacto habilitante. Distinguir paso de cadena y evento de impacto: la deduplicación actual por ataque debe permitir los impactos definidos para una misma acción. Elegir los clips y sus tiempos sin introducir cartas.

Implementar continuación por pulsación dentro de ventana; cierre por fallo, por no continuar y después del tercer paso; entrada a NA y salida de recuperación. La frontera de fin de paso será también la que use evasión en 4. La IA debe poder ejercer presión con estas mismas reglas, sin saltarse ventanas.

**Cierre:** un click temprano se descarta; un contacto abierto, bloqueado o contra otro rival permite encadenar sin forzar retarget ofensivo. Un paso fallido corta aunque haya pulsaciones. NA impide atacar/defender y permite locomoción; bloquear por sí solo no inicia NA ni borra el contexto de cadena del defensor. La demora de salida ofensiva tras bloqueo se distingue de entrada deshabilitada.

### 3. Resultados de daño y defensa

Resolver antes las [reglas de guardia, residual, reacción y crítico](combat/functional.md#preguntas): consumo/regeneración, restricciones entre impactos, break/stun, fórmula residual e interacción del crítico con guardia. Después calibrar cantidades y tiempos, sin tratarlos como valores deducidos del video.

Agregar los atributos y estados necesarios a la resolución existente de GAS. Incorporar residual letal, guard break, reacción/desplazamiento y crítico. Animación y HUD presentan el resultado resuelto; no generan otro estado de disponibilidad. Las solicitudes de IA respetan las mismas restricciones.

**Cierre:** distinguir daño abierto, daño defendido, guard break, reacción, stun y NA. Comprobar guardia 360°, residual que puede llevar HP a cero, gasto/recuperación de guardia y señal de crítico. Recibir daño no crea NA automáticamente; no se introduce un cooldown universal por bloquear. La baja y reaparición completas se integran en 6.

### 4. Movimiento integrado al combate

Resolver las restricciones/costos de evasión y el umbral entre pulsación y sprint de [jugador](player/functional.md#preguntas). Elegir un mecanismo de avance apropiado para las animaciones disponibles, con máximo por acción y respeto de colisión; no mantener varias soluciones alternativas por anticipación.

Incorporar acercamiento ofensivo limitado, evasión direccional, Shift sin dirección hacia atrás y sprint mantenido. Coordinar referencia de movimiento, orientación corporal y target sin convertirlos en una misma dirección obligatoria. Enlazar cierre por evasión con la frontera de fin de paso de 2.

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
