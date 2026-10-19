# Editor — Estado y cierre pendiente del paso 4

Guía vigente: [039 — Simplificación de movilidad](../039-simplificacion-movilidad/editor.md), que elimina la revisión de índices finales y las ocho direcciones de esta configuración anterior.

**Para el usuario:** quedan revisar el impacto final y comprobar movilidad en juego. La configuración recibida ya contiene avance, ocho dash y sprint. No repetir la creación de assets ni el recálculo de montages de 037.

## 1. Trabajo previo recibido

**Hecho — usuario, según 037.** Build limpio de vampsEditor Development Editor/Win64, reapertura y recálculo de los ocho montages a 1 s.

**Hecho — Codex por MCP, según 037.** Preservación/restauración del DA, configuración de entrada, controllers, dash y BlendSpace; compilaciones y arranque limitado de PIE. Sus herramientas, valores y operaciones reproducibles se conservan en la [guía 037](../037-movilidad-combate/editor.md). No son operaciones repetidas en esta revisión.

## 2. Consultar configuración recibida

**Hecho — Codex por MCP.** `ObjectTools.list_properties/get_properties` consultó `/Game/Vamps/Data/DA_VampsCombatSettings` y los ocho `/Game/Vamps/Animations/Executioner/AM_Executioner_Dash_Fw/FR/R/BR/Bw/BL/L/FL`. Valores completos en [estado-mcp.json](estado-mcp.json).

Para reproducir: abrir el DA en Content Browser y revisar ComboSteps, DashMontages y Mobility; abrir cada montage y revisar Asset Details y el segmento de DefaultSlot. Se esperan avance 250/0,25/130, energía 100, dash 350 cm/0,3 s con coste 25, sprint 700 cm/s con coste 20/s y recuperación 25/s tras 1 s. Los clips y montages duran 1 s; el dash los reproduce ajustados a 0,3 s. La lectura no acredita pose/blending ni guardado actual.

## 3. Revisar impacto final

**Pendiente — usuario.** Abrir `/Game/Vamps/Animations/Executioner/AM_Executioner_Combo` y `/Game/Vamps/Data/DA_VampsCombatSettings`. Seguir el [paso 4 de 037](../037-movilidad-combate/editor.md#4-verificar-la-autoría-del-impacto-final): contrastar cada sección y sus AN_ResetWeaponHits con FinalImpactWeaponIndex/FinalImpactIndex. Actualmente todos están en 0/0. Conservar los notifies existentes y guardar solo assets ajustados. No se vuelve a afirmar que los notifies o secciones hayan sido leídos mediante MCP.

## 4. Observar movilidad y cerrar

**Pendiente — usuario.** Abrir `/Game/Vamps/Maps/L_CharacterBasic`, iniciar Play y completar los siete casos del [paso 5 de 037](../037-movilidad-combate/editor.md#5-comprobar-el-comportamiento-integrado). Controles: RMB combate, Shift movilidad, WASD dirección, F target, F6 modo de IA y R reset. Revisar HUD y Output Log con LogVampsCombat.

Se esperan avance limitado sin acierto garantizado, dash direccional sin invulnerabilidad, sprint y energía independientes de guardia, salida de Guard tras el final correcto, restricciones NA/Hit/Stun, prioridad de continuación y limpieza por reset/contacto/muerte. Detener PIE y registrar fallos o confirmar los casos antes de cerrar 4.

## 5. Siguiente etapa

La propuesta posterior de simplificar animaciones y encapsular dash en GAS no cambia las operaciones vigentes: en esta revisión no se modificaron código ni assets. Si se observa cápsula inmóvil, la casilla Enable Root Motion por sí sola no corrige el desplazamiento programado; primero corresponde comprobar que el dash fue aceptado y consultar LogVampsCombat. No se marca ese síntoma como reproducido.

**Pendiente — usuario.** Tras completar la comprobación, confirmar el cierre del paso 4 para registrar su aprobación y continuar con cámara de acciones, oclusión y HUD del paso 5. No hay nuevas operaciones de creación/configuración del paso 5 ejecutadas en esta revisión.
