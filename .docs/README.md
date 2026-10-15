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

[Roadmap de implementación](roadmap.md): intercambio/cadena → recuperación y recursos → movilidad/cámara → partida por equipos → extensiones propias.

1. Leer el [alcance y exclusiones](game/functional.md#alcance-de-la-base), seguido de combate y jugador.
2. Consultar el [flujo GAS existente](prototype/technical.md#cómo-leer-el-código) para ubicar el punto de partida.
3. Resolver las preguntas al final de la funcionalidad que se implemente; las preguntas de extensiones no bloquean la base.

## Análisis de referencia

[016 — Cuatro videos de FGO Arcade](reports/016-analisis-fgo-arcade/analisis.md): [escenas y evidencia](reports/016-analisis-fgo-arcade/escenas.md), [comparación histórica con el prototipo](reports/016-analisis-fgo-arcade/comparacion.md), [decisiones y ubicación de preguntas](reports/016-analisis-fgo-arcade/preguntas.md) y [reporte](reports/016-analisis-fgo-arcade/reporte.md). La revisión de videos está cerrada. [017](reports/017-base-fgoa-y-extensiones/reporte.md) integra su alcance en las especificaciones principales, sin mantener una copia paralela dentro del reporte de análisis.

## Edición

- [Reglas de trabajo del repositorio: MCP y entregables](../AGENTS.md).
- [Convenciones del proyecto](AGENTS.md).
- [Organización y estilo C++](cpp-style.md).
- [Skill para agregar y revisar funcionalidades](../.agents/skills/game-feature-docs/SKILL.md).

## Reportes de trabajo

Iteración vigente: [019 — Cámara automática sobre lock-on](reports/019-camara-lockon/reporte.md), implementada y configurada por MCP, con seguimiento, pérdida y readquisición comprobados en PIE. Quedan [compilación completa y comprobaciones visuales](reports/019-camara-lockon/editor.md). El componente y HUD están en [018](reports/018-lockon-component/reporte.md), y la reorganización documental en [017](reports/017-base-fgoa-y-extensiones/reporte.md). El setup Unreal anterior sigue en [015 — Retirada de Duel](reports/015-retirada-duel/reporte.md) y su [guía](reports/015-retirada-duel/editor.md), con las clases/conexiones de [014](reports/014-copias-nomenclatura-vamps/reporte.md) y la [configuración de personajes y animación](reports/013-recuperacion-blueprints/reporte.md). La IA mantiene [011](reports/011-ia-combate-prueba/reporte.md) y sus [controles F6/R](reports/011-ia-combate-prueba/editor.md).

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
