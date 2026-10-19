# Editor — Giro instantáneo solo para dash

**Para el usuario:** no hay assets ni campos nuevos que configurar. Queda observar WASD frente a WASD+Shift y comprobar evasiones laterales/diagonales.

## 1. Compilación

**Hecho — Codex por MCP.** Con PIE detenido, LiveCodingToolset.LiveCodingToolset.CompileLiveCoding aplicó un parche Success. [Salida](live-coding.json). No fue una compilación NoChanges. No se guardaron assets durante la recarga de tipos.

Los montages y clips de /Game/Vamps/Animations/Executioner y DA_VampsCombatSettings conservan la configuración de [039](../039-simplificacion-movilidad/editor.md). No repetir su migración ni agregar montages laterales.

## 2. Comprobar arranque

**Hecho — Codex por MCP, verificación limitada.** EditorAppToolset.StartPIE inició /Game/Vamps/Maps/L_CharacterBasic. SceneTools.find_actors y ObjectTools.get_properties verificaron jugador/rival vivos en Idle y con permiso de dash; LogsToolset.GetLogEntries no devolvió errores/warnings de LogVampsCombat. StopPIE e IsPIERunning confirmaron finalización. No se pulsaron controles ni se observó el nuevo giro.

## 3. Observar control direccional

**Pendiente — usuario.** Abrir /Game/Vamps/Maps/L_CharacterBasic e iniciar Play:

1. Pulsar A/D/W/S sin Shift: el cuerpo debe seguir girando gradualmente como antes.
2. Desde un facing distinto, pulsar A+Shift y D+Shift: al empezar el dash, el cuerpo gira instantáneamente hacia la izquierda/derecha respecto de cámara y sale en esa dirección con el montage frontal. Repetir W/S y diagonales; la cápsula acompaña al mesh.
3. Pulsar Shift sin movimiento: retirada alejándose del target. Sin target, rechazo con diagnóstico y sin giro/coste.
4. Pedir dash lateral durante la ventana del ataque: conserva la dirección de esa pulsación hasta la ejecución. Una continuación válida debe ganar sin producir giro ni dash residual. Los permisos y plazos de Recovery/NA/reacción siguen vigentes.
5. Con energía insuficiente o en Guard/Hit/Stun, Shift no debe causar el giro instantáneo. Mantener Shift tras un dash válido conserva el sprint; la locomoción posterior vuelve a usar el giro habitual.
6. Observar cámara/target, root motion y blending al esquivar entre ataques. El cambio elimina la espera de orientación; no amplía las ventanas ni garantiza evitar un contacto ya capturado.

Detener PIE al terminar y registrar resultado. Usar R para reiniciar y F6 para el modo de IA cuando sea necesario.
