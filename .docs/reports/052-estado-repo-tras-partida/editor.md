# Editor — Continuación desde el estado revisado

Guía histórica: sus pendientes quedaron cerrados por la confirmación del usuario registrada en [053](../053-cierre-local-y-documentacion/editor.md). No es necesario repetirlos para cerrar 6.1.

No falta crear la partida ni configurar assets nuevos según 051. Queda comprobar el ciclo local en esta PC, especialmente PLAY AGAIN y la retirada de la pose de muerte después del último parche. Esta revisión no realizó operaciones de Unreal: no hay herramientas MCP directas disponibles.

## 1. Sincronización y trabajo comunicado

**Hecho — Codex, consulta de Git.** `master` local y la referencia remota consultada coinciden en `0c0d79d`. El árbol estaba limpio al comenzar el análisis. Solo se agregan reportes y sus enlaces.

**Hecho — usuario, alcance informado.** El usuario comunica que hizo lo restante del paso 5. No se atribuyen a esa comunicación comprobaciones específicas nuevas del paso 6.1.

## 2. Cargar el módulo actual en esta PC

**Pendiente — usuario, si todavía no se reconstruyó este checkout aquí.** Detener Play y cerrar Unreal. Desde `C:/dev/vamps`, ejecutar `build.bat`; debe terminar con `Result: Succeeded`. Abrir después con `editor.bat`. Ambos usan Unreal 5.8 desde `C:/Program Files/Epic Games/UE_5.8`, presente en esta máquina. No se ejecutaron esos scripts durante esta revisión.

Los binarios y parches locales están excluidos de Git. La compilación exitosa guardada en 051 acredita aquella sesión; no acredita el módulo cargado ahora en esta PC. El último cambio de pose se aplicó allí por Live Coding después del build completo.

## 3. Conservar la configuración recibida

**Hecho — Codex por MCP en 051; sin relectura actual.** La creación, configuración, compilación y guardado de los Blueprints, montages, DA y mapa constan en la [guía de implementación](../051-partida-local/editor.md#2-preservar-da-y-configurar-muerte). No volver a crearlos para realizar esta revisión.

**Pendiente — usuario, comprobación local.** El proyecto debe abrir `/Game/Vamps/Maps/L_LocalMatch`. World Settings debe conservar GameMode Override `BP_VampsMatchGameMode`; sus clases son `BP_VampsCharacter_Player`, `BP_VampsCharacter`, `BP_VampsMatchPlayerController` y HUD `VampsCombatTestHUD`. Los valores y ubicaciones exactos de los dos spawns están en [051](../051-partida-local/editor.md#4-mapa-y-configuración). Si algo difiere, registrar el dato antes de modificarlo; las capturas anteriores no son una autorización para sobrescribir configuraciones nuevas.

## 4. Completar observación de 6.1

**Pendiente — usuario.** Iniciar Play en L_LocalMatch:

1. Confirmar cuatro participantes, equipos A/B, preparación de 5 s y reloj de 5 min. F9 alterna combate/debug conservando marcador y reloj.
2. Al morir, observar cuerpo sin bloqueo y cámara libre con WASD/mouse. Tras 5 s vuelve el personaje con su cámara y sin retener la pose de muerte. Observar las variantes normal y defensiva cuando ocurran esos contactos letales.
3. Confirmar vida máxima, guardia cero y energía máxima al reaparecer. La protección dura hasta 2 s; un ataque aceptado la termina antes y un click rechazado la conserva.
4. Al terminar, pulsar PLAY AGAIN: puntos, kills/muertes y rachas vuelven a cero; se restauran posiciones y recursos y comienza otra preparación. No hay una observación de este click registrada en 051.
5. Si ocurren cierre por tiempo u objetivo simultáneo, comprobar sus reglas de [resultado](../../game/functional.md#duración-y-cierre). Son casos revisados en fuentes, todavía sin observación jugable registrada.

Usar `/Game/Vamps/Maps/L_CharacterBasic` para los controles de observación R/F6/F7. El controller de partida no los enlaza. No se requieren cambios de mappings para F9.

Registrar cualquier fallo con el mensaje de LogVampsCombat, el mapa y la acción que lo produjo. La selección de personaje de 6.2 y la red de 7 siguen siendo trabajo posterior.
