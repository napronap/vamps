# Preparación del MVP y flujo MCP

La base vigente es [004 — Un ataque y una guardia](../004-hit-y-guard/reporte.md), con su [guía del editor](../004-hit-y-guard/editor.md). Este documento conserva el estado de una iteración anterior.

Informe de la iteración anterior. Para el código vigente y las operaciones nuevas, consultar [003 — Combate animado](../003-combate-animado/reporte.md) y su [guía de editor](../003-combate-animado/editor.md). No repetir los pasos históricos que contradigan esa guía.

**Fecha:** 2026-10-01. **Resultado:** reglas de trabajo y entregables documentados. La creación del MVP no se inició.

## Cambios de esta iteración

| Archivo | Cambio y propósito |
|---|---|
| [AGENTS.md](../../../AGENTS.md) | Nuevo reglamento general: operaciones del editor por MCP directo, pasos sin soporte a cargo del usuario y prohibición de automatizar la UI. Exige reporte y guía por tema; conserva la restricción sobre código y Blueprints de Third Person. |
| [Convenciones de documentación](../../AGENTS.md) | Define contenido, estados y nivel de detalle de los reportes y guías. Los exceptúa del formato funcional/técnico de las funcionalidades. |
| [Índice](../../README.md) | Enlaza el reglamento y los entregables de esta preparación. |
| [Este reporte](reporte.md) | Registra cambios, resultados comprobados y límites de la preparación. |
| [Guía del editor](editor.md) | Reúne los pasos reproducibles de preparación y compilación, con responsables y estado. |

No se agregaron ni modificaron clases C++, Blueprints, Animation Blueprints u otros `.uasset` en esta iteración. No hay nuevas interacciones entre código y assets que documentar; la arquitectura del MVP sigue siendo una especificación en [juego](../../game/technical.md) y [combate](../../combat/technical.md).

## Preparación técnica comprobada antes de este reglamento

| Comprobación | Resultado y alcance |
|---|---|
| Proyecto y editor | Proyecto asociado a UE 5.8; instalación comprobada 5.8.3. Editor abierto desde Rider. |
| Plugins | `ModelContextProtocol`, `AllToolsets` y `LiveCodingToolset` habilitados en `vamps.uproject`. El usuario agregó `LiveCodingToolset`; Codex verificó su disponibilidad. |
| Conexión MCP | El usuario inició el servidor. Codex pudo descubrir herramientas y consultar el editor. |
| Compilación convencional | `vampsEditor Win64 Development`: `Succeeded`, target actualizado, sin nueva compilación de código. |
| Live Coding por MCP directo | `LiveCodingToolset.LiveCodingToolset.CompileLiveCoding` devolvió `NoChanges`: correcto, sin cambios pendientes. Todavía no prueba la aplicación de un parche nuevo. |

La prueba inicial de Live Coding usó la consola del editor mediante UI por MCP. La prueba posterior con la herramienta directa la reemplaza. El reglamento nuevo excluye repetir aquella automatización de UI.

## Validación y pendientes

- Se revisaron la coherencia de las reglas y los enlaces locales de los documentos modificados.
- No se ejecutaron pruebas de gameplay: esta iteración solo modifica documentación e instrucciones.
- No hay una acción manual pendiente para cerrar esta preparación. La [guía](editor.md) indica cómo reproducirla.
- Queda para la implementación autorizada del MVP comprobar un parche real y verificar las operaciones concretas de creación y configuración de sus assets. Tener MCP conectado no demuestra que todas esas operaciones estén cubiertas.
