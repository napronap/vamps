# Editor — Giro progresivo sin lock-on

**Para el usuario:** el parche está aplicado. En L_CharacterBasic, iniciar Play, pulsar F7 y sostener A/D o S varios segundos. La vista debe continuar girando suavemente hacia otras direcciones. Falta confirmar esa sensación; no hay assets, teclas ni parámetros que asignar. PIE quedó detenido.

## 1. Preparación

**Hecho — Codex por MCP.** EditorAppToolset.IsPIERunning confirmó false antes de compilar. Para reproducirlo, detener Play. Se conserva la configuración comprobada en [045](../045-estado-camara/editor.md#1-estado-recibido-y-configuración): ambos controllers usan `/Game/Vamps/Blueprints/BP_VampsPlayerCameraManager`, cuyo padre nativo contiene el ajuste. No se modifican ni guardan assets.

## 2. Compilación y arranque

**Hecho — Codex por MCP.** LiveCodingToolset.LiveCodingToolset.CompileLiveCoding devolvió Success y aplicó un parche real; UBT Succeeded. Para compilar el mismo cambio manualmente con el editor abierto y Play detenido, usar Live Coding (Ctrl+Alt+F11) y esperar éxito. No fue necesario cambiar clases reflejadas, compilar Blueprints ni reiniciar.

**Hecho — Codex por MCP, comprobación de arranque.** EditorAppToolset.StartPIE inició `/Game/Vamps/Maps/L_CharacterBasic`. SceneTools/ObjectTools confirmaron manager/controller propios y jugador vivo en Idle con cámara de seguimiento; LogsToolset no mostró nuevos errores/warnings de combate. StopPIE e IsPIERunning=false confirmaron el cierre. Esto no sustituye la observación del movimiento.

## 3. Observar el giro libre

**Pendiente — usuario.** En L_CharacterBasic:

1. Iniciar Play y dar foco al juego. Pulsar F7 sin modificadores: el HUD existente debe mostrar LOCK ON: None y el log indicar disabled.
2. Dar un toque corto de A o D y soltar: conservar acompañamiento suave. Después mantener cada lateral varios segundos: superar el pequeño desvío anterior y seguir orientando la vista. Repetir S sostenida: debe permitir mirar hacia el otro lado mediante giro gradual, sin salto de 180°.
3. Soltar y detenerse: la vista se estabiliza alrededor del nuevo rumbo, relajando la anticipación corta; no vuelve al rumbo inicial de la sesión. Invertir A/D y repetir diagonales, avance, sprint y dash direccional. WASD sigue siendo relativo a la cámara, de modo que la trayectoria acompaña su giro.
4. Si una pared impide desplazarse, la cámara no recibe intención de giro solo por mantener la tecla: usa velocidad real. Repetir el caso sostenido en un espacio con recorrido libre.
5. Pulsar F7 otra vez: readquirir rival con la transición existente. Confirmar seguimiento con target, encuadres de ataque/bloqueo y oclusión conservados. Pulsar R y volver a desactivar lock-on para comprobar que el giro sigue funcionando tras reset.
6. Detener Play. Confirmar si velocidad y retraso del giro requieren ajuste. Los valores nuevos son globales internos: respuesta del rumbo 0,4/s y máximo 60°/s; no hay una CVar nueva para ellos.

Estas pulsaciones y la valoración visual quedan manuales porque las herramientas MCP directas no las cubren; no se automatiza la interfaz, según AGENTS.md.
