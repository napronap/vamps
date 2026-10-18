# 026 — Leer los diagnósticos del paso 2

Para el tiempo adicional por golpe y la restauración de configuración al cambiar el DA, aplicar [027 — Delay por golpe](../027-delay-por-golpe/editor.md).

**Pendiente para el usuario:** continuar configurando el montage y observar los ataques y transiciones. Los diagnósticos de código revisados se filtran ahora con LogVampsCombat. Los pendientes jugables de [024](../024-cierre-ventana-combo/editor.md) y de armas de [023](../023-armas-y-trazas/editor.md) se mantienen; esta revisión no ajusta los assets.

## 1. Aplicar y comprobar el código

**Hecho — Codex por MCP:** LiveCodingToolset.LiveCodingToolset.CompileLiveCoding compiló y aplicó la revisión, la propagación del rechazo de trazas y el ajuste final del rechazo de NA inválida con Success y parches reales. Se agregó LogVampsCombat y se sustituyeron asserts/casteos fatales en las rutas propias de combate y sus conexiones. No fue NoChanges.

No hay nuevas propiedades reflejadas ni es necesario reasignar o guardar Blueprints por esta revisión. El parche ya está aplicado en el editor abierto; no requiere recargar el nivel ni reiniciar para quedar activo. En una compilación completa habitual con el editor cerrado, build.bat incorpora también estos cambios al módulo base.

**Hecho — Codex por MCP:** StartPIE/StopPIE en /Game/Vamps/Maps/L_CharacterBasic confirmó dos personajes en Idle y un arma por personaje. ObjectTools confirmó el componente nativo de movimiento en ambos y LogsToolset confirmó la categoría nueva sin errores durante esa sesión y su cierre. find_actors no encontró armas después de detener PIE. Recorrido reproducible: Play en ese mapa y, al terminar la observación, Stop; el nivel debe quedar sin las armas creadas para la sesión.

## 2. Leer un fallo

**Pendiente — usuario:** en el panel Output Log, filtrar LogVampsCombat y solicitar la acción que se está configurando. El mensaje incluye la función, el objeto o asset y la causa; los límites de sección y notifies se imprimen cuando corresponde. Codex puede leer las mismas entradas con LogsToolset.GetLogEntries.

Ejemplos de diagnóstico esperado:

- GA_Attack: montage y sección sin ANS_AttackTrace, ventana inválida o reproducción que no pudo comenzar.
- Personaje: socket ausente, clase de arma inválida, fase inesperada, duración de sección/NA inválida o daño negativo.
- Arma: mesh sin asignar, marcadores coincidentes, radio/escala inválidos o muestreo angular imposible.
- Movimiento: receptor/dirección/distancias inválidos o reacción asignada que no puede reproducirse.
- Input/IA/partida: assets, tipos, pawn, clases o participantes ausentes/incompatibles.

La operación afectada se rechaza o cancela. No debe cerrarse el editor por un assert del código propio ni ejecutarse una sustitución silenciosa. Las condiciones normales del combate no se registran como errores de configuración.

## 3. Assets y observación

**Pendiente — usuario:** corregir únicamente el asset o valor identificado. El DA verificado en 025 era /Game/Vamps/Data/DA_VampsCombatSettings y su montage /Game/Vamps/Animations/Executioner/AM_Executioner_Combo. La ausencia de ANS_AttackTrace en Default detectada allí debe corregirse si todavía existe; renombrar la sección no agrega ese notify.

Configurar secciones/notifies sigue siendo manual porque no hay una herramienta MCP directa disponible para esas operaciones. No se automatizó la interfaz ni se añadió un atajo. Después, observar inicio, continuación al cierre de ventana, recuperación/NA, armas y movimiento con la configuración elegida. Los mensajes rojos del HUD identifican los datos que impiden dibujar su panel.
