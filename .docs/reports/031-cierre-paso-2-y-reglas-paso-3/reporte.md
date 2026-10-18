# 031 — Cierre del paso 2 y reglas del paso 3

La implementación de estas reglas continúa en [033 — Resultados de daño y defensa](../033-resultados-combate/reporte.md), con captura nueva del DA, compilación y guía de comprobaciones pendientes.

La comprobación real posterior de scripts, build y arranque se registra en [032](../032-comprobacion-build-editor/reporte.md), con autorización específica del usuario. Los resultados documentales de esta iteración se conservan.

## Resultado — 2026-10-18

El usuario confirmó «Marca paso 2 como completado porque lo esta». Se registra el cierre de cadena y NA en el [roadmap](../../roadmap.md#2-cadena-y-recuperación-na), con 3 como siguiente etapa. El cierre se atribuye al usuario; no representa una nueva compilación ni una nueva observación de PIE por Codex. Las verificaciones y configuraciones anteriores permanecen en 022–030.

No hubo cambios de código, clases, Blueprints, Data Assets ni configuración del editor. La implementación del paso 3 sigue pendiente.

## Decisiones incorporadas

Las reglas vigentes se integran en [combate funcional](../../combat/functional.md) y su arquitectura en [combate técnico](../../combat/technical.md). Sustituyen las reglas anteriores que conservaban la ofensiva del defensor al bloquear, consumían un saldo de guardia o calculaban restricción con la longitud de la sección.

- Guard cancela la ofensiva propia y su continuación, cierra sus contactos futuros y sustituye el ataque por reacción. No cancela por ello al agresor ni inicia NA por la interrupción.
- Bloqueo y daño abierto restringen ataque y movimiento voluntario por la misma duración total configurada en el atacante. Los subimpactos del paso comparten configuración; cada contacto aceptado reemplaza el plazo desde ese contacto. No se agrega un mínimo dependiente del clip. Un fallo no renueva ni garantiza el siguiente acierto.
- Solo se recuerda la interacción defensiva actual. Bloquear a B mientras se defiende A exige pulsación válida, cambia el lock-on y reemplaza el recuerdo de A. Un subimpacto posterior de A exige otra pulsación. No se descartan los ataques del primer agresor al cambiar de interacción.
- Los contactos válidos ya capturados se resuelven una vez aunque se interrumpa la ofensiva. Mantienen contexto original y no habilitan una nueva generación ni mantienen trazas activas después de cancelar.
- La barra acumula carga al bloquear; al llenarse produce ruptura con stun. El usuario confirma esta regla y la cobertura 360° por experiencia de juego. Se conserva la evidencia técnica de 028 sin atribuirle una demostración que no contiene; no hace falta reabrir la investigación para adoptar estas reglas.
- Se conserva daño plano y se extiende a una cantidad configurable para daño crítico. La probabilidad es un porcentaje en el DA del atacante. El bloqueo resta el mismo coeficiente plano al daño normal o crítico, con mínimo cero; el crítico no agrega otras consecuencias. No se incorpora escalado por atributos. Quedan como decisiones menores la granularidad de la tirada y la ubicación/valor del coeficiente.

## Archivos, responsabilidades y conexiones

| Documento | Cambio |
|---|---|
| `.docs/roadmap.md` | Cierre de 2, punto de partida actualizado y alcance de 3 conforme a las decisiones. La descripción de 1 conserva su cierre histórico. |
| `.docs/combat/functional.md` | Reglas de reacción, interacción defensiva, restricción compartida, carga de guardia, contactos pendientes y daño plano; preguntas reducidas a decisiones abiertas. |
| `.docs/combat/technical.md` | Reemplaza referencias retiradas a GA_Guard/FistHitbox por las clases actuales y distingue base existente de trabajo de 3. |
| `.docs/README.md` | Avance, última iteración documental y enlaces de entregables. |
| Reportes 029–030 y guía 030 | Referencias al cierre y decisiones posteriores, conservando sus verificaciones originales. |
| Este reporte y `editor.md` | Atribución del cierre, alcance documental, comprobaciones y ausencia de operaciones nuevas de editor. |

`AVampsCharacter` y `FVampsCombatState` siguen alojando combate; `UGA_Attack` reproduce la cadena; `UVampsCombatResolver` resuelve contactos; `UVampsCombatMovementComponent` aplica movimiento; `UVampsAttributeSet` y `UGE_VampsDamage` conservan vida/daño. El técnico identifica los cambios necesarios en estas responsabilidades y en AnimInstance/HUD, sin presentarlos como implementados. Se conservan `AVampsWeapon`, ANS_AttackTrace y AN_ResetWeaponHits con independencia por arma.

No se reasignaron padres, referencias, notifies ni valores de assets. La referencia histórica `/Game/Vamps/Data/DA_VampsCombatSettings` y el montage `/Game/Vamps/Animations/Executioner/AM_Executioner_Combo` conservan la configuración documentada en 030; no fueron releídos en vivo en esta iteración. Si la implementación cambia el DA o structs, se requiere inventario y captura MCP nuevos, no reutilizar los valores históricos por suposición.

## Verificación y pendientes

Se revisaron fuentes de estado, settings, resolver, daño, animación y rutas de interrupción/movimiento, contrastándolos con las especificaciones y reportes 028–030. Se revisaron el diff, whitespace y destinos de enlaces locales de los documentos afectados. Se verificó que el roadmap marca 2 completada y 3 pendiente, y que las reglas nuevas se presentan como trabajo futuro.

No se ejecutaron compilación, operaciones sobre el editor/assets, PIE ni tests automatizados. El cambio es documental; no acredita comportamiento nuevo en ejecución. No quedan operaciones de editor necesarias para este cierre; [guía](editor.md).

Para preparar la ejecución sin presencia del usuario se releen `build.bat`, `editor.bat` y `vars.bat`. Actualmente vars solo define variables, build invoca una compilación y editor solo lanza el proceso; las observaciones de 007 sobre builds duplicados son históricas. `start` retorna sin certificar que Unreal haya cargado. Se descubrieron los toolsets de MCP y los esquemas de EditorAppToolset y LiveCodingToolset: hay compilación Live Coding y control de PIE, sin cierre/reinicio del proceso en los esquemas inspeccionados. No se ejecutaron los scripts, cierre, build o arranque; se prepara un prompt con autorización específica para su futura ejecución y esperas acotadas. Los permisos de sandbox siguen aplicando.

Las preguntas de vigencia/identidad entre pasos, carga y recuperación de guardia, duración/salida de stun, configuración de coeficiente, granularidad de crítico y simultaneidad permanecen al final de combate funcional/técnico. Los valores y assets nuevos se configuran al implementar 3; no se inventaron cantidades ni animaciones.
