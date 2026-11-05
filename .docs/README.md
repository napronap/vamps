# Documentación del juego

Primero se reproduce la **base FGOA adaptada**; después se agregan las **extensiones propias de Vamps**. El estado del prototipo existente se documenta aparte. Las especificaciones describen el comportamiento objetivo; no certifican que ya esté implementado.

| Base FGOA adaptada | Comportamiento | Implementación |
|---|---|---|
| Juego y partida | [Funcional](game/functional.md) | [Técnico](game/technical.md) |
| Combate | [Funcional](combat/functional.md) | [Técnico](combat/technical.md) |
| Controles, movilidad, target, cámara y HUD | [Funcional](player/functional.md) | [Técnico](player/technical.md) |

| Extensiones Vamps, después de la base | Comportamiento | Implementación |
|---|---|---|
| Alcance propio: parry y ultimate | [Funcional](extensions/functional.md) | [Técnico](extensions/technical.md) |
| Identidades, perfiles y dos skills por personaje | [Funcional](extensions/characters/functional.md) | [Técnico](extensions/characters/technical.md) |

| Punto de partida existente | Comportamiento | Implementación |
|---|---|---|
| Prototipo local de observación | [Funcional](prototype/functional.md) | [Técnico, clases y assets](prototype/technical.md) |

Cada funcionalidad ocupa una carpeta con `functional.md` y `technical.md`. Las subfuncionalidades pueden anidarse cuando necesiten documentación propia.

## Orden de trabajo

Combos según el montage: [065 — Cualquier cantidad de secciones desde una](reports/065-combos-sin-limite-fijo/reporte.md), con [configuración y comprobaciones](reports/065-combos-sin-limite-fijo/editor.md). Se retira el máximo de tres y la comparación fija del knockback del tercer golpe; el final y las ventanas se validan usando N secciones. Compilado por MCP con los cuatro DAs preservados; configuración de secciones/notifies y observación de N>3 y una sección quedan al usuario.

Selector desde datos: [064 — Nombres, catálogo y reparto sin repetidos](reports/064-selector-desde-data-assets/reporte.md), con [configuración y comprobaciones](reports/064-selector-desde-data-assets/editor.md). El HUD genera opciones desde los cuatro DAs del GameMode; el jugador usa el elegido y los bots reciben otros tres distintos. El ajuste de muerte permite compartir montage normal y bloqueado, compiló por MCP y conservó los valores actuales de los cuatro DAs. El usuario avanzó los recursos; queda comprobar START y ambas muertes.

Locomoción configurable: [063 — BlendSpace por DA y ABP compartido](reports/063-locomocion-configurable/reporte.md), con [asignaciones y comprobación visual](reports/063-locomocion-configurable/editor.md). `ABP_VampsCharacter` usa `LocomotionBlendSpace` del DA; Executioner conserva `BS_Executioner` y los otros tres campos quedan vacíos. Código y Blueprints compilados por MCP; arranque verificado y locomoción visual pendiente del usuario.

Data Assets por personaje: [062 — Executioner, Dual Swords, Sword and Shield y Lancer](reports/062-data-assets-personajes/reporte.md), con [configuración y comprobación de GameModes](reports/062-data-assets-personajes/editor.md). El DA original se llama ahora `DA_ExecutionerCombatSettings`; los tres nuevos tienen parámetros iniciales y tres steps, con armas/montages pendientes. Los modos actuales conservan Executioner y se verificó el arranque de observación y 2v2 por MCP.

Fin de Recovery: [061 — Cierre por AN_EndRecovery](reports/061-fin-recovery-notify/reporte.md), con [compilación y configuración manual del montage](reports/061-fin-recovery-notify/editor.md). Un marcador por golpe libera movimiento, inicia NA de 0,5 s y solicita el regreso de cámara; sustituye la predicción temporal de [060](reports/060-salida-framing-antes-na/reporte.md). Código compilado; usuario recompiló/reabrió tras la recarga del struct y Codex verificó el arranque limpio. Pendientes los tres notifies y la comprobación de combate.

Indicadores sobre personajes: [059 — Lock-on y barra de NA](reports/059-indicadores-personaje/reporte.md), con [configuración y observación del editor](reports/059-indicadores-personaje/editor.md). Componente de pantalla propio de cada personaje; arranque/configuración verificados por MCP y flecha del target local y NA descendente confirmados visualmente por el usuario el 2026-10-29.

Presentación con widgets: [058 — UI con UMG](reports/058-ui-widgets/reporte.md), con [operaciones y comprobación del editor](reports/058-ui-widgets/editor.md). Conserva el flujo local y el aspecto de 048/051/055; migración compilada y assets conectados, con revisión interactiva final pendiente.

Diseño del siguiente paso: [056 — Servidor dedicado y alojamiento](reports/056-analisis-servidor-dedicado/reporte.md), con [guía de preparación](reports/056-analisis-servidor-dedicado/editor.md). Demo privada en Japón, equipos/bots, ready e inicio por creador; reconexión con IA temporal, traspaso de organizador y 30 s de revancha acordados. Prioriza una VM sencilla de proveedor global para hasta tres partidas; revisa regiones, costes y propuestas de compensación aún por acordar. No implementa red ni despliega servicios.

Cierre vigente: [055 — Selector Executioner y START](reports/055-selector-executioner/reporte.md), con [compilación y comprobación final](reports/055-selector-executioner/editor.md). **6.2 completado:** el usuario confirmó compilación y funcionamiento el 2026-10-27. Etapas 0–6 cerradas; **sigue 7, multijugador y autoridad de red**, después calibración en 8 y extensiones en 9. El segundo personaje y su DA no bloquean ese avance.

Diseño de la entrega: [054 — Selección de personaje, paso 6.2](reports/054-revision-seleccion-personaje/reporte.md), con [preparación del editor](reports/054-revision-seleccion-personaje/editor.md). Acordado: solo Executioner, selección propia + START sin confirmación adicional, bots del catálogo con repetidos y revancha al selector conservando preselección. Implementación en 055; el segundo personaje y su DA quedan para después.

Cierre local anterior: [053 — Movilidad, presentación y ciclo 6.1](reports/053-cierre-local-y-documentacion/reporte.md), con [guía y comprobación final](reports/053-cierre-local-y-documentacion/editor.md). El selector posterior y su cierre están en 055; la próxima entrega es 7.

Revisión previa del checkout: [052 — Estado tras la partida local](reports/052-estado-repo-tras-partida/reporte.md), con [guía histórica](reports/052-estado-repo-tras-partida/editor.md). Sus pendientes y desfases documentales quedan resueltos en 053.

Implementación local actual: [051 — Partida 2v2](reports/051-partida-local/reporte.md), con [configuración y comprobaciones del editor](reports/051-partida-local/editor.md). Incluye assets guardados, compilación y ronda observada hasta el resultado; comprobación final del usuario registrada en 053.

Decisiones de partida: [050 — Ciclo local 2v2 y recompensa por racha](reports/050-reglas-partida-local/reporte.md), con [guía de planificación](reports/050-reglas-partida-local/editor.md). Retira portador y separa 6.1 con Executioner de 6.2 con selección. Especificación vigente: [partida](game/functional.md).

Planificación anterior: [049 — Partida por equipos del paso 6](reports/049-revision-paso-6/reporte.md), con [guía histórica](reports/049-revision-paso-6/editor.md). La implementación posterior está en 051 y su cierre en 053.

HUD vigente: [048 — HUD de combate simple y alternancia F9](reports/048-hud-combate-simple/reporte.md), con [guía reproducible](reports/048-hud-combate-simple/editor.md). Compilado en 051 y funcionamiento confirmado por el usuario en 053; F9 ahora se enlaza desde el controller común.

Revisión histórica: [047 — Verificación del avance del plan](reports/047-verificacion-avance-plan/reporte.md), con [guía del editor](reports/047-verificacion-avance-plan/editor.md). Conserva el estado anterior a HUD y partida; el cierre vigente está en 053.

[Roadmap hacia la emulación FGOA](roadmap.md): consolidar contacto/cámara → botón compartido y fases → cadena/NA → daño/guardia → movilidad → presentación → equipos → red → calibración → extensiones propias. Incluye estado actual, decisiones previas y criterios de cierre; distingue combate local y base completa.

**Cierres registrados:** 0 y 1 en [020](reports/020-roadmap-emulacion-fgoa/reporte.md#cierre-de-la-etapa-0) y [021](reports/021-solicitud-compartida/reporte.md#cierre-de-la-etapa-1); 2 en [031](reports/031-cierre-paso-2-y-reglas-paso-3/reporte.md), 3 en [035](reports/035-revision-paso-3/reporte.md#cierre-del-paso-3), 4/5/6.1 en [053](reports/053-cierre-local-y-documentacion/reporte.md) y 6.2 en [055](reports/055-selector-executioner/reporte.md). El estado y orden de entregas se controlan en el roadmap.

La movilidad vigente está en [039](reports/039-simplificacion-movilidad/reporte.md) y [040](reports/040-giro-direccional-dash/reporte.md); 037 conserva su configuración anterior. Cámara y HUD están en 042–048. Sus comprobaciones de implementación se complementan con la confirmación final del usuario en 053.

1. Leer el [alcance y exclusiones](game/functional.md#alcance-de-la-base), seguido de combate y jugador.
2. Consultar la [implementación del paso 3](reports/033-resultados-combate/reporte.md) y su [revisión y cierre 035](reports/035-revision-paso-3/reporte.md); la [revisión 028](reports/028-contraste-fgoa/analisis.md#10-evaluación-concreta-del-código-actual-de-vamps) y el prototipo conservan el punto de partida histórico.
3. Resolver las preguntas al final de la funcionalidad que se implemente; las preguntas de extensiones no bloquean la base.

## Análisis de referencia

[028 — Contraste FGOA y Vamps](reports/028-contraste-fgoa/analisis.md): referencia nativa y empírica, [13 recortes de video y trazas](reports/028-contraste-fgoa/evidencia.md) y correcciones de supuestos sobre defensa, IDs, tiempos y energía frente a carga de guardia. Su evaluación de C++ corresponde a la captura histórica; las implementaciones posteriores están en 033–035 y 037. [Reporte](reports/028-contraste-fgoa/reporte.md) y [guía del editor](reports/028-contraste-fgoa/editor.md). No cierra etapas.

[016 — Cuatro videos de FGO Arcade](reports/016-analisis-fgo-arcade/analisis.md): [escenas y evidencia](reports/016-analisis-fgo-arcade/escenas.md), [comparación histórica con el prototipo](reports/016-analisis-fgo-arcade/comparacion.md), [decisiones y ubicación de preguntas](reports/016-analisis-fgo-arcade/preguntas.md) y [reporte](reports/016-analisis-fgo-arcade/reporte.md). La revisión de videos está cerrada. [017](reports/017-base-fgoa-y-extensiones/reporte.md) integra su alcance en las especificaciones principales, sin mantener una copia paralela dentro del reporte de análisis.

## Edición

- [Reglas de trabajo del repositorio: MCP y entregables](../AGENTS.md).
- [Convenciones del proyecto](AGENTS.md).
- [Organización y estilo C++](cpp-style.md).
- [Skill para agregar y revisar funcionalidades](../.agents/skills/game-feature-docs/SKILL.md).

## Reportes de trabajo

Mantenimiento de Git: [057 — Caché local fuera del seguimiento](reports/057-cache-fuera-de-git/reporte.md), con [operaciones y revisión del commit](reports/057-cache-fuera-de-git/editor.md). Ignora `/.cache/` y retira sus índices del repositorio conservando los archivos locales.

Giro libre vigente: [046 — Orientación progresiva sin lock-on](reports/046-giro-libre-camara/reporte.md). Mantener desplazamiento permite orientar la vista conservando el acompañamiento suave. [Guía reproducible](reports/046-giro-libre-camara/editor.md); cierre de presentación en 053.

Revisión de cámara: [045 — Código, assets y arranque](reports/045-estado-camara/reporte.md). Incluye 042–044 y registros de F7. [Guía reproducible](reports/045-estado-camara/editor.md); observación integrada cerrada en 053 y calibración integral reservada a 8.

Seguimiento libre vigente: [044 — Acompañamiento suave y F7](reports/044-seguimiento-libre-f7/reporte.md). Anticipación amortiguada de desplazamiento sin seguir el yaw corporal; F7 alterna el lock-on de prueba. [Guía y observación](reports/044-seguimiento-libre-f7/editor.md).

Corrección anterior de cámara sin target: [043 — Referencia virtual y modo de observación](reports/043-camara-sin-target/reporte.md). El rumbo fijo y el comando de consola quedan sustituidos por 044. [Guía histórica](reports/043-camara-sin-target/editor.md).

Implementación de cámara: [042 — Seguimiento, combate y oclusión](reports/042-camara-combate/reporte.md). Dos encuadres, Eye/Interest, entrada por ataque/bloqueo, salida por cierre de cadena y obstáculos preparados. [Guía reproducible](reports/042-camara-combate/editor.md); evolución en 043–046, HUD en 048 y cierre en 053.

Revisión del paso 5: [041 — Cámara, oclusión y alcance posterior de HUD](reports/041-revision-paso-5/reporte.md). Incorpora las anotaciones: dos encuadres globales, ataque/bloqueo y salida por cierre de cadena, oclusión de ambos sujetos y HUD diferido. [Prompt de implementación de cámara](reports/041-revision-paso-5/prompt.md) y [lecturas/futura verificación](reports/041-revision-paso-5/editor.md). Solo análisis y preparación; sin implementación nueva.

Control direccional vigente: [040 — Giro instantáneo solo al iniciar dash](reports/040-giro-direccional-dash/reporte.md). Captura WASD al pulsar Shift, conserva el giro gradual de locomoción y usa dos montages. [Guía reproducible](reports/040-giro-direccional-dash/editor.md); cierre de movilidad en 053.

Implementación vigente de movilidad: [039 — Simplificación con GA_Dash y root motion](reports/039-simplificacion-movilidad/reporte.md). Dos montages, approach inicial y restricciones de reacción; retira índices finales y desplazamiento programado del dash de 037. DA preservado y compilación/arranque comprobados. [Guía reproducible](reports/039-simplificacion-movilidad/editor.md); cierre en 053.

Revisión histórica: [038 — Estado del paso 4](reports/038-estado-paso-4/reporte.md), con lectura MCP del DA y los ocho montages de aquella configuración. La simplificación 039 retiró impacto final y seis montages; el cierre vigente está en 053. [Guía histórica](reports/038-estado-paso-4/editor.md).

Implementación anterior de movilidad: [037 — Acercamiento, dash y sprint](reports/037-movilidad-combate/reporte.md). Su configuración de ocho direcciones e impacto final queda sustituida por 039; conserva las capturas de preservación del DA y su [guía histórica](reports/037-movilidad-combate/editor.md).

Planificación histórica: [036 — Paso 4: acercamiento, evasión y sprint](reports/036-revision-paso-4/reporte.md), con [lecturas y guía](reports/036-revision-paso-4/editor.md). Su propuesta precede a la simplificación de 039–040; el estado actual es el cierre de movilidad en 053.

Revisión y cierre vigente: [035 — Revisión del paso 3](reports/035-revision-paso-3/reporte.md). Montages Guard/Hit asignados y guardados, reacción de 1 s y 154 contactos previos con daños esperados verificados por MCP. Cierre aprobado por el usuario el 2026-10-19; [guía reproducible y conservación en Git](reports/035-revision-paso-3/editor.md).

Corrección vigente de presentación: [034 — Montages independientes de Guard y Hit](reports/034-montage-guardia/reporte.md). Agrega GuardReactionMontage separado de HitReactionMontage y rechaza referencias iguales; conserva el resto del paso 3. [Configuración y comprobación](reports/034-montage-guardia/editor.md).

Implementación base de 3: [033 — Resultados de daño y defensa](reports/033-resultados-combate/reporte.md). Agrega Guard/Hit, memoria defensiva, residual, carga/ruptura/stun y crítico; conserva cadena, NA y armas independientes. Build completo y parche final aplicados, DA preservado/configurado y arranque PIE verificados. Sus límites de observación son históricos; el cierre posterior está en [035](reports/035-revision-paso-3/reporte.md#cierre-del-paso-3). El cierre de 2 y las decisiones previas siguen en [031](reports/031-cierre-paso-2-y-reglas-paso-3/reporte.md).

Comprobación previa de scripts: [032 — Build y arranque del editor](reports/032-comprobacion-build-editor/reporte.md). Conserva la primera validación del ciclo autorizado; el cierre/reinicio y la verificación de los cambios actuales están en 033. [Guía 032](reports/032-comprobacion-build-editor/editor.md).

Base de impactos conservada: [030 — Reinicio opcional de impactos por arma](reports/030-reinicio-impactos/reporte.md). Agrega AN_ResetWeaponHits con índices del array Weapons del DA, mantiene un AttackTrace por sección y registros independientes. Colocación opcional en la [guía](reports/030-reinicio-impactos/editor.md). Las capturas de configuración previas permanecen en 027/030; la captura vigente antes y después del cambio estructural está en 033.

La [026 — Diagnósticos y cancelación controlada del paso 2](reports/026-diagnosticos-paso-2/reporte.md) reemplaza asserts/casteos fatales del código propio por validaciones explícitas y diagnósticos en LogVampsCombat. Incluye armas, movimiento, resolver y conexiones de inicio/input/IA/HUD/reset. Compilado y aplicado por Live Coding; arranque y limpieza de armas en PIE verificados sin errores nuevos. Pasos de observación en la [guía](reports/026-diagnosticos-paso-2/editor.md).

La [025 — Validación del montage sin cerrar el editor](reports/025-validacion-montage/reporte.md), del 2026-10-17, conserva la evidencia del primer rechazo de configuración sin cierre: faltaba ANS_AttackTrace en Default del montage Executioner. La revisión y categoría de log vigentes están en 026.

La [024 — Continuación al cerrar la ventana de combo](reports/024-cierre-ventana-combo/reporte.md) conserva un montage por cadena y decide el salto al cerrar Combo Window; compilado y aplicado por Live Coding. Restan configurar las animaciones y observar las transiciones según la [guía](reports/024-cierre-ventana-combo/editor.md). Continúa [023 — Armas por Blueprint y trazas independientes](reports/023-armas-y-trazas/reporte.md), con creación/enganche/limpieza de una greatsword por personaje verificados en PIE y calibración/impactos/duales pendientes según su [guía](reports/023-armas-y-trazas/editor.md), y [022 — Cadena, NA y movimiento](reports/022-cadena-y-na/reporte.md). Manny y el AnimBP siguen compartidos. La etapa 1 conserva su [cierre 021](reports/021-solicitud-compartida/reporte.md). El plan y el cierre de etapa 0 permanecen en [020](reports/020-roadmap-emulacion-fgoa/reporte.md). Se conservan cámara [019](reports/019-camara-lockon/reporte.md), selección [018](reports/018-lockon-component/reporte.md) y el setup de assets de [015](reports/015-retirada-duel/reporte.md), [014](reports/014-copias-nomenclatura-vamps/reporte.md) y [013](reports/013-recuperacion-blueprints/reporte.md).

| Tema | Cambios y verificaciones | Pasos del editor |
|---|---|---|
| 001 — Preparación del MVP y flujo MCP | [Reporte](reports/001-preparacion-mvp/reporte.md) | [Guía](reports/001-preparacion-mvp/editor.md) |
| 002 — MVP del duelo local | [Reporte](reports/002-mvp-duelo/reporte.md) | [Guía](reports/002-mvp-duelo/editor.md) |
| 003 — Combate animado y acciones explícitas | [Reporte](reports/003-combate-animado/reporte.md) | [Guía](reports/003-combate-animado/editor.md) |
| 004 — Un ataque y una guardia | [Reporte](reports/004-hit-y-guard/reporte.md) | [Guía](reports/004-hit-y-guard/editor.md) |
| 005 — IA básica y controles de prueba | [Reporte](reports/005-ia-basica/reporte.md) | [Guía](reports/005-ia-basica/editor.md) |
| 006 — Golpe de puño con hitbox y trazas | [Reporte](reports/006-golpe-de-puno/reporte.md) | [Guía](reports/006-golpe-de-puno/editor.md) |
| 007 — Revisión de scripts de build y editor | [Reporte](reports/007-revision-scripts/reporte.md) | [Guía](reports/007-revision-scripts/editor.md) |
| 008 — Bloqueo de generación de proyectos Visual Studio | [Reporte](reports/008-generacion-visual-studio/reporte.md) | [Guía](reports/008-generacion-visual-studio/editor.md) |
| 009 — Base definitiva de personajes | [Reporte](reports/009-personajes-base/reporte.md) | [Guía](reports/009-personajes-base/editor.md) |
| 010 — HUD de prueba para los personajes nuevos | [Reporte](reports/010-hud-prueba/reporte.md) | [Guía](reports/010-hud-prueba/editor.md) |
| 011 — IA nueva y GameMode de prueba de combate | [Reporte](reports/011-ia-combate-prueba/reporte.md) | [Guía](reports/011-ia-combate-prueba/editor.md) |
| 012 — AnimInstance y AnimBP Vamps | [Reporte](reports/012-animacion-vamps/reporte.md) | [Guía](reports/012-animacion-vamps/editor.md) |
| 013 — Recuperación de Blueprints y animación Vamps | [Reporte](reports/013-recuperacion-blueprints/reporte.md) | [Guía](reports/013-recuperacion-blueprints/editor.md) |
| 014 — Copias con nomenclatura Vamps | [Reporte](reports/014-copias-nomenclatura-vamps/reporte.md) | [Guía](reports/014-copias-nomenclatura-vamps/editor.md) |
| 015 — Retirada de Duel | [Reporte](reports/015-retirada-duel/reporte.md) | [Guía](reports/015-retirada-duel/editor.md) |
| 016 — Análisis de cuatro videos de FGO Arcade | [Reporte](reports/016-analisis-fgo-arcade/reporte.md) | [Guía](reports/016-analisis-fgo-arcade/editor.md) |
| 017 — Base FGOA y extensiones Vamps | [Reporte](reports/017-base-fgoa-y-extensiones/reporte.md) | [Guía](reports/017-base-fgoa-y-extensiones/editor.md) |
| 018 — Componente de lock-on y HUD de debug | [Reporte](reports/018-lockon-component/reporte.md) | [Guía](reports/018-lockon-component/editor.md) |
| 019 — Cámara automática sobre lock-on | [Reporte](reports/019-camara-lockon/reporte.md) | [Guía](reports/019-camara-lockon/editor.md) |
| 020 — Plan de emulación FGOA | [Reporte](reports/020-roadmap-emulacion-fgoa/reporte.md) | [Guía](reports/020-roadmap-emulacion-fgoa/editor.md) |
| 021 — Solicitud compartida y fases | [Reporte](reports/021-solicitud-compartida/reporte.md) | [Guía](reports/021-solicitud-compartida/editor.md) |
| 022 — Cadena por secciones y NA | [Reporte](reports/022-cadena-y-na/reporte.md) | [Guía](reports/022-cadena-y-na/editor.md) |
| 023 — Armas por Blueprint y trazas independientes | [Reporte](reports/023-armas-y-trazas/reporte.md) | [Guía](reports/023-armas-y-trazas/editor.md) |
| 024 — Continuación al cerrar la ventana de combo | [Reporte](reports/024-cierre-ventana-combo/reporte.md) | [Guía](reports/024-cierre-ventana-combo/editor.md) |
| 025 — Validación del montage sin cerrar el editor | [Reporte](reports/025-validacion-montage/reporte.md) | [Guía](reports/025-validacion-montage/editor.md) |
| 026 — Diagnósticos y cancelación controlada del paso 2 | [Reporte](reports/026-diagnosticos-paso-2/reporte.md) | [Guía](reports/026-diagnosticos-paso-2/editor.md) |
| 027 — Tiempo adicional de restricción por golpe | [Reporte](reports/027-delay-por-golpe/reporte.md) | [Guía](reports/027-delay-por-golpe/editor.md) |
| 028 — Contraste FGOA y Vamps | [Reporte](reports/028-contraste-fgoa/reporte.md) | [Guía](reports/028-contraste-fgoa/editor.md) |
| 029 — Revisión del paso 2 | [Reporte](reports/029-revision-paso-2/reporte.md) | [Guía](reports/029-revision-paso-2/editor.md) |
| 030 — Reinicio opcional de impactos por arma | [Reporte](reports/030-reinicio-impactos/reporte.md) | [Guía](reports/030-reinicio-impactos/editor.md) |
| 031 — Cierre del paso 2 y reglas del paso 3 | [Reporte](reports/031-cierre-paso-2-y-reglas-paso-3/reporte.md) | [Guía](reports/031-cierre-paso-2-y-reglas-paso-3/editor.md) |
| 032 — Comprobación de build y arranque | [Reporte](reports/032-comprobacion-build-editor/reporte.md) | [Guía](reports/032-comprobacion-build-editor/editor.md) |
| 033 — Resultados de daño y defensa | [Reporte](reports/033-resultados-combate/reporte.md) | [Guía](reports/033-resultados-combate/editor.md) |
| 034 — Montages independientes de Guard y Hit | [Reporte](reports/034-montage-guardia/reporte.md) | [Guía](reports/034-montage-guardia/editor.md) |
| 035 — Revisión y cierre del paso 3 | [Reporte](reports/035-revision-paso-3/reporte.md) | [Guía](reports/035-revision-paso-3/editor.md) |
