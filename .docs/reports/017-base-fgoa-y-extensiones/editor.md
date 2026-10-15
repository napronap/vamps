# 017 — Guía de continuidad documental

**No quedan operaciones de Unreal para el usuario por esta iteración.** No requiere abrir el editor, compilar, cambiar inputs o asignar assets. Los pendientes previos de [015](../015-retirada-duel/editor.md) y de contacto/notify de [013](../013-recuperacion-blueprints/editor.md) conservan su estado; esta revisión no confirma su ejecución.

## 1. Referencia y decisiones

**Hecho — usuario.** Aportó las grabaciones y aclaraciones consolidadas en 016 y pidió priorizar la reproducción de FGOA, conservando las extensiones propias en archivos separados.

**Hecho — Codex, mediante herramientas de archivos.** Revisó análisis, comparación, respuestas y par de alcance de 016, junto con fuentes y reportes de la base Vamps. La revisión de videos ya estaba cerrada; no se solicitaron ni revisaron grabaciones nuevas.

## 2. Consultar la especificación y el punto de partida

**Hecho — Codex, mediante herramientas de archivos.** Reorganizó el [índice](../../README.md) y el [roadmap](../../roadmap.md). Orden de lectura reproducible:

1. [Alcance de la base](../../game/functional.md), con exclusiones y adaptaciones elegidas.
2. [Combate](../../combat/functional.md) y [jugador](../../player/functional.md), cada uno con su documento técnico.
3. [Prototipo existente](../../prototype/functional.md) y [conexiones de clases/assets](../../prototype/technical.md) para reconocer lo que funciona hoy.
4. [Extensiones Vamps](../../extensions/functional.md) y [personajes](../../extensions/characters/functional.md) para la etapa posterior.

No hubo pasos realizados por MCP del editor ni cambios de asignaciones, compilación o guardado de assets.

## 3. Decisiones antes de cada implementación

**Pendiente — usuario, decisiones de diseño por etapa.** Resolver las preguntas del área que se aborde: [solicitud defensiva, guardia/crítico y resolución](../../combat/functional.md#preguntas), [target y movilidad](../../player/functional.md#preguntas) y [formato de partida, portador y respawn](../../game/functional.md#preguntas). No es necesario responderlas todas para conservar esta integración documental. Las preguntas de extensiones se atienden después de reproducir la base.

Las futuras operaciones de Unreal se documentarán con assets, valores y verificaciones concretas en la iteración que implemente cada regla, usando MCP directo soportado. Esta guía no prescribe una migración de assets sin código que la sustente.
