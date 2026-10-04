# Convenciones de documentación

Estas reglas se aplican a la documentación de este proyecto.

- Guardar la documentación Markdown dentro de `.docs/`. Las skills del proyecto se alojan en `.agents/skills/` para que Codex las detecte.
- Organizar cada funcionalidad en una carpeta con `functional.md` y `technical.md`. Usar subcarpetas cuando representen subfuncionalidades con entidad propia. Los reportes, guías de trabajo y convenciones generales no requieren ese par.
- Escribir las reglas del juego directamente, como especificaciones vigentes y editables. Evitar etiquetas como «intención del usuario», «propuesto», «no confirmado» y comentarios como «falta definir».
- Convertir las indefiniciones en preguntas accionables al final del documento correspondiente. No inventar respuestas para completar secciones.
- Mantener una única explicación principal de cada regla y enlazarla desde otros documentos. No explicar la misma funcionalidad más de dos veces; preferir una sola.
- El funcional describe comportamiento esperado con precisión: condiciones, acciones, resultados, límites y casos relevantes. No contiene decisiones de código.
- El técnico describe cómo se implementa ese comportamiento, qué partes afecta, sus dependencias y por qué se elige esa solución. Al describir código existente, verificarlo; al diseñar código futuro, escribir la arquitectura sin fingir que ya existe.
- Excluir experiencia personal, hardware del desarrollador, historia de la conversación y advertencias sobre el estado de la documentación. En reportes y guías, incluir el estado operativo y los resultados necesarios para continuar el trabajo.
- Mantener los documentos breves sin perder reglas necesarias. No crear secciones vacías, registros de cada edición ni copias históricas redundantes. Los reportes consolidan cambios y resultados de una iteración; no transcriben la conversación ni las llamadas a herramientas.
- Al resolver una pregunta, integrar la respuesta en la especificación y retirar la pregunta.

Para agregar, modificar o revisar una funcionalidad, leer y aplicar [game-feature-docs](../.agents/skills/game-feature-docs/SKILL.md).

## Reportes y guías del editor

Aplicar las [reglas de trabajo del repositorio](../AGENTS.md). Guardar los entregables en `.docs/reports/`, con nombres por tema, por ejemplo `mvp-duelo.md` y `mvp-duelo-editor.md`. Reutilizar el mismo par al continuar ese trabajo y enlazar ambos desde [el índice](README.md).

### Reporte de cambios: `<tema>.md`

- Indicar alcance y resultado de la iteración.
- Enumerar archivos, clases y assets agregados, modificados o eliminados. Para cada clase afectada, explicar brevemente su responsabilidad, el motivo del cambio y cómo interactúa con otras clases.
- Describir las conexiones con Blueprints y otros `.uasset`: clase padre, referencias, configuración, eventos o datos relevantes. Usar rutas reales; no presentar piezas planificadas como existentes.
- Incluir cambios de configuración o documentación, verificaciones realizadas con su resultado y límites de lo probado.
- Registrar pendientes y enlazar los pasos manuales de la guía del editor. Atribuir correctamente los cambios del usuario; no contarlos como cambios hechos por Codex.
- Si no hubo cambios de código o assets, decirlo en una línea. No llenar tablas con piezas sin cambios.

### Guía reproducible: `<tema>-editor.md`

- Cubrir, en orden de dependencia, todas las operaciones necesarias fuera de la edición de código: preparación, plugins, assets, configuración, asignaciones, compilación, guardado y comprobaciones manuales que correspondan al trabajo. No incluir creación ni ejecución de tests automatizados.
- Incluir también los pasos ya realizados por MCP, con una explicación que el usuario pueda reproducir en el editor. No limitar la guía a los pendientes.
- Marcar cada paso como **Hecho — Codex por MCP**, **Hecho — usuario**, **Pendiente — usuario** o **Pendiente — Codex por MCP**. Marcarlo hecho solo cuando se haya confirmado su resultado, e identificar cualquier verificación parcial.
- Para cada paso, dar ubicación o menú, nombres y rutas de assets, valores concretos, orden de acciones y resultado esperado, según corresponda. Si se ejecutó por MCP, indicar la herramienta usada.
- Abrir la guía con lo que le queda por hacer al usuario. Si un dato impide completar un paso, explicitarlo; no inventar valores.
- Si falta soporte MCP, dejar instrucciones manuales. No probar una alternativa mediante automatización de UI.
