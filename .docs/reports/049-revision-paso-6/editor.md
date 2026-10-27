# Editor — Revisión previa del paso 6

**Para el usuario:** esta revisión no requiere operaciones nuevas en Unreal. Primero hay que responder las [decisiones de partida](reporte.md#decisiones-necesarias-antes-de-implementar-el-ciclo-completo). No se crearon clases, Blueprints, mapas ni asignaciones de partida. La guía de [048](../048-hud-combate-simple/editor.md) conserva la compilación/observación del HUD anterior, sin atribuir una nueva ejecución a esta revisión.

## 1. Fijar las reglas de la entrega local

**Pendiente — usuario.** Elegir formato y participantes, valores iniciales de puntuación/objetivo/tiempo, empate, ciclo del portador, respawn, presentación durante la baja e inicio/fin/reinicio. Las preguntas concretas y su impacto están en el reporte. No hace falta resolver red, lobby, skills ni balance definitivo para esta etapa.

## 2. Preparación posterior a la implementación

**Pendiente — usuario, después de definir e implementar las reglas.** La configuración de una partida necesitará puntos de aparición por equipo y referencias propias a los participantes/controladores/HUD elegidos. Todavía no existen rutas nuevas ni posiciones aprobadas que asignar. El escenario actual `/Game/Vamps/Maps/L_CharacterBasic` y su GameMode de observación no deben contarse como una partida por equipos terminada ni R como respawn.

Al implementar, las operaciones del editor soportadas se harán mediante MCP directo y se documentarán con sus valores reales. Las no soportadas quedarán como pasos manuales exactos; no se automatizará UI. Si se modifica alguna clase/struct de un DA existente, primero habrá que capturar y guardar sus configuraciones por MCP según AGENTS.md. Esta revisión no solicita editar DA_VampsCombatSettings.

## 3. Comprobaciones de cierre que deberá cubrir la implementación

**Pendiente — usuario, cuando exista el ciclo jugable.** Observar en el escenario local preparado:

1. Asignación correcta de equipos; aliados fuera de selección, lista, amenazas y contactos ofensivos enemigos. Dos aliados pueden golpear al mismo enemigo.
2. Una baja registra puntos una sola vez, retira al personaje del combate y habilita el respawn previsto sin reutilizar contactos/acciones previos.
3. Reaparición del mismo personaje con ubicación, recursos y protección elegidos; IA/cámara/HUD reacquieren enemigos elegibles.
4. Asignación, visibilidad y transferencia del portador, incluyendo bajas simultáneas y su recompensa diferencial.
5. Resultado correcto por puntos/tiempo/empate; las bajas válidas del mismo lote se resuelven antes de decidir el fin. Después del resultado, acciones y respawns siguen la regla acordada.
6. HUD con puntuación, equipo, portador y estado de espera/resultado; controles de debug que se hayan mantenido funcionando sin depender erróneamente del GameMode de observación.

Los nombres de assets, cantidades, ubicaciones y resultados concretos se completarán en la guía de implementación cuando estén decididos. No se declara hecho ningún paso del editor ni se agregan comprobaciones de red a este cierre local.
