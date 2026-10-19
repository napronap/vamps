# Editor — Revisión del paso 5

**Para el usuario:** esta revisión no requiere cambiar ni compilar nada en Unreal. Las anotaciones y la salida al entrar en NA están incorporadas en el [prompt de cámara](prompt.md). El HUD/avisos se harán con otro prompt. La observación jugable corresponde a la implementación futura; no hay operaciones de implementación realizadas en esta iteración.

## 1. Lecturas existentes

**Hecho — Codex por MCP.** AssetTools.find_assets, ObjectTools.list_properties/get_properties, SceneTools.get_current_level/find_actors y ActorTools.get_components comprobaron referencias/defaults, componentes y mapa abierto. No se editaron ni guardaron assets. La [captura](estado-mcp.json) conserva los valores consultados; no es un backup completo del DA ni una lectura runtime de la cámara.

Para reproducir las lecturas relevantes manualmente:

1. Abrir `/Game/Vamps/Blueprints/BP_VampsPlayerCameraManager`, Class Defaults: FollowPitch=-15, TrackingSpeed=6, HorizontalTolerance=6, DefaultFOV=90, DefaultAspectRatio≈1,33333.
2. Abrir BP_VampsPlayerController y BP_VampsCombatTestPlayerController en la misma carpeta: ambos usan BP_VampsPlayerCameraManager; mappings IMC_Vamps/IMC_VampsCombatTest y referencias IA_Move/IA_Mobility.
3. Abrir BP_VampsCharacter_Player. CameraBoom: TargetArmLength=400, TargetOffset=(0,0,60), DoCollisionTest activo, ProbeSize=12, ProbeChannel=Camera, CameraLag desactivado, UsePawnControlRotation activo.
4. FollowCamera: FieldOfView=90, AspectRatio≈1,777778, ConstrainAspectRatio y OverrideAspectRatioAxisConstraint desactivados. El valor de eje del componente no determina la política runtime cuando no la sobrescribe.
5. Capsule: HalfHeight=88, Radius=34. Mesh: SKM_Manny_Simple, transformación relativa Z=-88/yaw=-90.
6. BP_VampsCombatTestGameMode: jugador BP_VampsCharacter_Player, controller BP_VampsCombatTestPlayerController, HUD VampsCombatTestHUD, rival BP_VampsCharacter y AI BP_VampsAIController. Mapa abierto: `/Game/Vamps/Maps/L_CharacterBasic`.

No se atribuyen operaciones nuevas de editor al usuario. La indicación de que el trabajo está en el paso 5 procede de su mensaje; esta revisión no ejecutó un cierre visual adicional del paso 4.

## 2. Preparación al implementar

**Pendiente — Codex por MCP, al implementar.** Releer configuración efectiva y descubrir las herramientas directas exactas para cada nuevo asset/propiedad/material/curva. Capturar defaults/componentes que vayan a cambiar. Si se modifica UVampsCombatSettings o un struct de sus propiedades, buscar antes todos los DA de clase/derivadas, capturar arrays/transforms/referencias completos y guardarlos. Tras compilar y el arranque limpio necesario, releer/restaurar/comparar y guardar únicamente los afectados. La revisión 041 no hizo ese cambio ni su preservación completa.

**Pendiente — Codex por MCP, al implementar.** Configurar los dos encuadres globales y sus transiciones en la ruta propia de cámara; usar ajuste de desarrollo común solo para iterar. Adaptar cámara/boom y conectar oclusores propios, conservando el HUD actual. Los valores iniciales se eligen y documentan para Vamps; no son datos por personaje. Mantener bOrientRotationToMovement y AM_Executioner_Dash_Fw/Bw con root motion de 039–040; no seguir las instrucciones de ocho montages o impacto final de 037.

**Pendiente — Codex por MCP, al implementar.** Preparar en L_CharacterBasic varios rivales y los obstáculos/materiales elegidos: uno visible con fade, otro sólido no preparado y un límite invisible. Comprobar que la consulta de fade detecta al preparado aunque ignore el canal usado para acortar cámara. Guardar y releer exclusivamente los assets/mapa afectados.

Si alguna operación de autoría no tiene herramienta MCP directa, documentar el recurso y el paso manual exacto cuando se elija. No se declara ahora una limitación de autoría que no se haya comprobado ni se intenta resolverla mediante UI automatizada.

**Pendiente — Codex por MCP, al implementar.** Compilar con LiveCodingToolset.LiveCodingToolset.CompileLiveCoding y distinguir NoChanges de parche real. Si hace falta build/reinicio sin herramienta directa, dejarlo al usuario con los pasos de preservación previos; no guardar referencias transitorias de recarga estructural.

## 3. Verificación cuando exista la implementación

**Pendiente — usuario, al implementar.** En L_CharacterBasic, observar manualmente:

1. Lateral sostenido, inicio/parada, A→D→A, acercamiento/retirada y diagonales con target: deriva y continuidad del jugador/target en pantalla, no solo yaw o una captura fija. Comparar con C01/C03/C08 de la investigación; sus perfiles runtime exactos no están medidos.
2. Shift con cardinales/diagonales y sin dirección, sprint y soltar Shift. La dirección de dash capturada en 040 se conserva; el giro instantáneo del cuerpo no resetea el historial de cámara. Repetir a distintas tasas de render para ajustar respuesta temporal.
3. F entre varios rivales, guardia que impone al agresor, daño de otro rival sin retarget ofensivo, desaparición/inelegibilidad de candidatos, ausencia y readquisición. HUD y cámara comparten CurrentTarget.
4. Ataque aceptado entra en combate; solicitud rechazada no. Bloqueo efectivo entra en combate; daño abierto no solicita cambio de encuadre. La continuación mantiene combate entre pasos. Al terminar la cadena, volver al seguimiento por NA propia para atacante y NA del agresor para receptor. Cancelación/invalidación no deja referencias colgadas; no alterar permisos ni duración de recuperación.
5. Conservar HUD y sus señales existentes. No comprobar avisos/proximidad nuevos en esta iteración: su implementación queda para otro prompt. El desfase aceptado de un frame no requiere mover el resolver.
6. Obstáculo preparado: fade gradual y restauración; personaje sigue colisionando. Sólido no preparado: reacción de cámara elegida. Límite invisible: restricción física sin fade. Probar varios oclusores y entrada/salida de planos de acción.
7. R durante transición/reacción/fade y fin de PIE. Limpiar historial y presentación del mismo pawn, sin necesitar respawn de partida para hacerlo.

Estos casos son comprobaciones futuras. No se inició PIE ni se compiló en esta revisión; no hay resultados visuales nuevos atribuidos a Codex.
