# Documentación del juego

| Área | Comportamiento | Implementación |
|---|---|---|
| Juego y partida | [Funcional](game/functional.md) | [Técnico](game/technical.md) |
| Combate | [Funcional](combat/functional.md) | [Técnico](combat/technical.md) |
| Identidad de personajes — después de la demo | [Funcional](characters/functional.md) | [Técnico](characters/technical.md) |

Cada funcionalidad ocupa una carpeta con `functional.md` y `technical.md`. Las subfuncionalidades pueden anidarse cuando necesiten documentación propia.

## Orden de trabajo

[Roadmap de prototipos](roadmap.md).

1. Resolver las [preguntas de la primera versión](combat/functional.md#reglas-del-núcleo) y especificar sus casos de combate.
2. Ejecutar los [accionables técnicos de combate](combat/technical.md) y los [ensayos de rendimiento](game/technical.md#accionables-rendimiento).
3. Avanzar por el roadmap e incorporar las funcionalidades adicionales en sus propias carpetas cuando corresponda.

## Edición

- [Reglas de trabajo del repositorio: MCP y entregables](../AGENTS.md).
- [Convenciones del proyecto](AGENTS.md).
- [Organización y estilo C++](cpp-style.md).
- [Skill para agregar y revisar funcionalidades](../.agents/skills/game-feature-docs/SKILL.md).

## Reportes de trabajo

| Tema | Cambios y verificaciones | Pasos del editor |
|---|---|---|
| Preparación del MVP y flujo MCP | [Reporte](reports/preparacion-mvp.md) | [Guía](reports/preparacion-mvp-editor.md) |
| MVP del duelo local | [Reporte](reports/mvp-duelo.md) | [Guía](reports/mvp-duelo-editor.md) |
