# Editor — Revisión del paso 3

**Para el usuario:** si estas comprobaciones ya quedaron aprobadas en la otra sesión, confirmar cuáles para registrar el cierre sin repetirlas. Falta acreditar ruptura/stun, residual letal, memoria con varios agresores y presentación/permisos de reacción. Además, conservar en Git el DA modificado y los dos montages nuevos juntos. No hay un cambio de código que requiera compilar para esta auditoría.

## 1. Revisar el estado guardado

**Hecho — Codex por MCP.** AssetTools.find_assets, ObjectTools.list_properties/get_properties, BlueprintTools.get_parent y AssetTools.is_dirty verificaron rutas, conexiones y guardado. EditorAppToolset.IsPIERunning confirmó false; SceneTools.get_current_level identificó /Game/Vamps/Maps/L_CharacterBasic.

Para reproducir: en Content Browser abrir /Game/Vamps/Data/DA_VampsCombatSettings. Debe mostrar Hit Reaction Montage=AM_Executioner_Hit, Guard Reaction Montage=AM_Executioner_Block, Contact Reaction Duration=1 s en los tres pasos, Stun Duration=2 s y los demás valores de la [captura](estado-mcp.json). No hace falta reconfigurarlos.

**Hecho — usuario, verificado parcialmente por MCP.** Los dos montages ya existen, están asignados y no tienen cambios sin guardar. Su reproducción visual todavía no está acreditada por esta revisión.

## 2. Confirmar los intercambios conocidos

**Hecho — usuario, con resultados numéricos verificados por Codex mediante MCP.** LogsToolset.GetLogEntries recuperó 154 contactos previos: abierto 12/24 y bloqueado 2/14 para normal/crítico. No se ejecutó un nuevo PIE ni input desde Codex. El log no permite marcar como hechas las comprobaciones siguientes.

## 3. Cerrar las comprobaciones restantes

**Pendiente — usuario; omitir la repetición de los casos ya observados y confirmarlos.** Abrir /Game/Vamps/Maps/L_CharacterBasic, iniciar Play y filtrar Output Log por LogVampsCombat. RMB solicita ataque/defensa; F cambia selección; F6 cambia modo de IA; R reinicia. Consultar Reaction, Reaction Time, Guard Load, Damage, Critical y NA en el HUD.

1. Acumular carga hasta 100. Cada contacto bloqueado agrega 25, pero hay recuperación a 20/s fuera de reacción después de 1 s: cuatro bloqueos solo bastan si no hubo recuperación entre ellos. El contacto que llena la barra sigue aplicando residual; debe producir GuardBreak/Stun durante 2 s, impedir defensa y dejar carga cero al salir.
2. Verificar que un bloqueo con poca HP puede llevarla a cero. Comprobar bloqueo desde atrás para acreditar 360°.
3. Para A→B→A se necesitan dos agresores. El GameMode actual crea un solo rival; su duelo no acredita este caso. En una escena preparada con dos rivales, defender A, admitir B con otra pulsación y volver a recibir A: la memoria anterior de A debe haberse perdido y requerir una nueva pulsación. No se agregó otro rival en esta revisión; la preparación y observación siguen pendientes si no se hicieron previamente.
4. Verificar multihit del mismo paso y necesidad de nueva admisión en el siguiente; bloqueo que cancela ataque propio; reacción que impide movimiento/ataque sin crear NA; NA previa que se conserva; R que limpia reacción, carga, memoria, montages y movimiento.
5. Observar los dos montages en el mesh real del jugador y rival. Guard y Hit deben reproducir clips distintos y terminar según el plazo desde el último contacto: 1 s actual, salvo stun de 2 s. Comprobar sustitución entre reacciones y distinguir stun de NA en el HUD. Si una animación no se ve, revisar skeleton y DefaultSlot del mesh/ABP_Executioner antes de considerar aprobada la presentación.

Detener PIE al terminar. No alterar parámetros para aparentar el cierre. Si se calibra algún valor, registrar el valor final y guardar solo los assets afectados.

## 4. Conservar el trabajo entre PCs

**Pendiente — usuario.** En el cliente Git, incluir juntos el DA modificado y los montages Block/Hit sin seguimiento enumerados en el [reporte](reporte.md), revisar y hacer commit/push según el flujo habitual. En esta auditoría Codex no realizó operaciones de escritura en Git.

El estado de la etapa sigue en el roadmap. Esta guía acredita inspección y resultados numéricos existentes; no declara aprobado el cierre manual.
