# Documentación del juego

| Área | Comportamiento | Implementación |
|---|---|---|
| Juego y partida | [Funcional](game/functional.md) | [Técnico](game/technical.md) |
| Combate | [Funcional](combat/functional.md) | [Técnico](combat/technical.md) |
| Identidad de personajes — después de la demo | [Funcional](characters/functional.md) | [Técnico](characters/technical.md) |

Cada funcionalidad ocupa una carpeta con `functional.md` y `technical.md`. Las subfuncionalidades pueden anidarse cuando necesiten documentación propia.

## Orden de trabajo

[Roadmap de prototipos](roadmap.md).

1. Leer el [flujo básico de GAS](combat/technical.md#cómo-leer-el-código).
2. Completar la [configuración del puño y las trazas](reports/006-golpe-de-puno/editor.md), con los controles de IA existentes.
3. Observar daño y bloqueo antes de agregar otra regla.

## Análisis de referencia

[016 — Cuatro videos de FGO Arcade](reports/016-analisis-fgo-arcade/analisis.md): [escenas y evidencia](reports/016-analisis-fgo-arcade/escenas.md), [contraste con documentos/código](reports/016-analisis-fgo-arcade/comparacion.md), [decisiones y precisiones restantes](reports/016-analisis-fgo-arcade/preguntas.md) y [reporte](reports/016-analisis-fgo-arcade/reporte.md). Revisión de videos cerrada. El diálogo se integra en un [alcance funcional](reports/016-analisis-fgo-arcade/alcance/functional.md) y [base técnica](reports/016-analisis-fgo-arcade/alcance/technical.md) separados; las especificaciones del prototipo y el setup Unreal conservan su contenido.

## Edición

- [Reglas de trabajo del repositorio: MCP y entregables](../AGENTS.md).
- [Convenciones del proyecto](AGENTS.md).
- [Organización y estilo C++](cpp-style.md).
- [Skill para agregar y revisar funcionalidades](../.agents/skills/game-feature-docs/SKILL.md).

## Reportes de trabajo

Iteración vigente: [016 — Análisis de FGO Arcade](reports/016-analisis-fgo-arcade/reporte.md), con [continuación documental](reports/016-analisis-fgo-arcade/editor.md). El setup Unreal sigue en [015 — Retirada de Duel](reports/015-retirada-duel/reporte.md) y su [guía](reports/015-retirada-duel/editor.md), con las clases/conexiones de [014](reports/014-copias-nomenclatura-vamps/reporte.md) y la [configuración de personajes y animación](reports/013-recuperacion-blueprints/reporte.md). La IA mantiene [011](reports/011-ia-combate-prueba/reporte.md) y sus [controles F6/R](reports/011-ia-combate-prueba/editor.md).

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
