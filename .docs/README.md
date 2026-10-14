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

## Edición

- [Reglas de trabajo del repositorio: MCP y entregables](../AGENTS.md).
- [Convenciones del proyecto](AGENTS.md).
- [Organización y estilo C++](cpp-style.md).
- [Skill para agregar y revisar funcionalidades](../.agents/skills/game-feature-docs/SKILL.md).

## Reportes de trabajo

Iteración vigente de implementación: [009 — Base definitiva de personajes](reports/009-personajes-base/reporte.md). [Preparación del editor](reports/009-personajes-base/editor.md). El duelo anterior queda como referencia; las piezas compartidas de combate y animación usan la base nueva. Diagnóstico del entorno: [008 — Generación de proyectos Visual Studio](reports/008-generacion-visual-studio/reporte.md).

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
