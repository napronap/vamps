# 010 — Usar el HUD de prueba

La asignación y las comprobaciones de esta guía se continúan en [011 — IA y GameMode de prueba](../011-ia-combate-prueba/editor.md). DuelHUD se asigna al nuevo modo de prueba, no al GameMode normal; el rival ahora se crea con IA y se controla con F6/R.

**Pendiente para el usuario:** iniciar Play en L_CharacterBasic y comprobar visualmente los paneles, la guardia y el ataque. Código, compilación en vivo y asignación del HUD están hechos. La captura de ventanas por MCP falló; las interacciones de juego no tienen una herramienta directa disponible.

[Reporte](reporte.md) · [Configuración previa de personajes, inputs y notify](../009-personajes-base/editor.md).

## 1. Aplicar el código

**Hecho — Codex por MCP.**

Se descubrió el esquema de LiveCodingToolset.LiveCodingToolset y se ejecutó CompileLiveCoding con PIE detenido. Resultado: Success y generación de un parche real patch_2; no fue una compilación NoChanges. No hace falta una nueva compilación para observar este cambio en la sesión actual.

Para reproducir la compilación en vivo desde el editor: habilitar Live Coding en Editor Preferences, usar su acción Compile y revisar Output Log. Codex debe seguir utilizando la herramienta MCP directa.

## 2. Asignar el HUD

**Hecho — Codex por MCP.** Herramientas: ObjectTools.get_properties/set_properties, BlueprintTools.compile_blueprint y AssetTools.save_assets/is_dirty.

1. Abrir `/Game/Vamps/Blueprints/BP_VampsGameMode`.
2. En Class Defaults → Classes, establecer **HUD Class = DuelHUD**. Es la clase nativa `/Script/vamps.DuelHUD`; no requiere un Blueprint adicional.
3. Mantener Default Pawn Class = BP_VampsCharacter_Player y Player Controller Class = BP_VampsPlayerController.
4. Compilar y guardar. Los tres valores se consultaron después del cambio; el guardado devolvió true y el asset quedó sin cambios pendientes.

`/Game/Vamps/Maps/L_CharacterBasic` ya usa BP_VampsGameMode como GameMode Override, según 009. No se cambió el mapa ni el GameMode predeterminado del proyecto.

## 3. Verificar el inicio

**Hecho — Codex por MCP (verificación parcial).** Herramientas: EditorAppToolset.StartPIE/StopPIE/IsPIERunning, SceneTools.find_actors, ObjectTools, AbilitySystemInspectorToolset y LogsToolset.

PIE creó DuelHUD_0 para el controller nuevo con bShowHUD = true. Jugador y receptor comenzaron con vida 100/100 y sin ataque/guardia activos. No aparecieron errores ni warnings de LogPlayLevel. La sesión se detuvo y su cierre quedó confirmado.

CaptureEditorImage devolvió `Failed to capture any editor windows.`; el inicio del HUD está comprobado, su apariencia en pantalla no.

## 4. Observar vida y estados

**Pendiente — usuario.**

1. Abrir L_CharacterBasic y pulsar Play local con foco en el viewport. Arriba a la izquierda debe aparecer PLAYER, 100/100, barra completa e IDLE. A la derecha debe aparecer el receptor, identificado por su nombre de actor, con los mismos valores iniciales. Los personajes adicionales aparecen debajo del receptor.
2. Mantener RMB: PLAYER debe pasar a GUARD; soltar debe devolverlo a IDLE. La pose visual de guardia depende del AnimBP y no se agrega en esta iteración.
3. Pulsar LMB: PLAYER debe mostrar ATTACK mientras la habilidad esté activa y volver a IDLE al terminar. En reposo, un nuevo clic debe volver a activar el ataque.
4. Acercarse y golpear al receptor: tras un contacto válido su vida debe pasar de 100 a 88 y su barra reducirse. Si no hay daño, revisar primero Attack Trace y su ventana de contacto en AM_BasicAttack, según la guía 009; ese notify sigue pendiente de verificación.
5. Para observar bloqueo, usar la configuración temporal del receptor descrita en 009: Event BeginPlay → RequestGuard(true). Su panel debe mostrar GUARD; bloquear el golpe debe conservar su vida y terminar el ataque del jugador. Al finalizar, retirar la llamada temporal y guardar el Blueprint.

La ayuda inferior solo debe mostrar WASD, mouse, LMB y RMB. El HUD no muestra modo IA, lock-on ni reinicio del duelo.
