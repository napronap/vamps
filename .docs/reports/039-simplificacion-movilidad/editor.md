# Editor — Simplificación de movilidad

La regla de usar el frente corporal previo con input se reemplaza por el [giro al iniciar dash de 040](../040-giro-direccional-dash/editor.md). La configuración de assets de esta guía sigue vigente.

**Para el usuario:** la compilación, el reinicio, la restauración del DA y la limpieza de montages están completados. Queda observar el comportamiento del punto 5 antes de cerrar el paso 4. No hay que volver a configurar índices de impacto final ni ocho direcciones.

## 1. Preservar configuración

**Hecho — Codex por MCP.** ObjectTools.search_subclasses y AssetTools.find_assets buscaron UVampsCombatSettings y derivadas sin limitar raíces; encontraron únicamente /Game/Vamps/Data/DA_VampsCombatSettings. list_properties/get_properties capturaron todos sus datos y AssetTools.save_assets lo guardó antes de cambiar clase/struct. [Captura previa](data-assets-antes.json).

Para reproducir en el editor: abrir el DA desde Content Browser y conservar armas/transform, montages, tres pasos y escalares antes de modificar tipos. La captura previa permite recuperar los valores exactos; no reemplazarlos por defaults.

## 2. Preparar los dos montages

**Hecho — Codex por MCP.** AssetTools.duplicate creó /Game/Vamps/Animations/Executioner/A_Executioner_Dash_Fw y A_Executioner_Dash_Bw desde /Game/GreatSwordAnimV1/Animation/RM/A_GreatSwordCombatAnimationV1_Dash_Fw_RM y Dash_Bw_RM. ObjectTools.set_properties activó Enable Root Motion y Root Motion Root Lock=AnimFirstFrame en las copias.

En el editor, abrir cada AnimSequence propia, buscar Root Motion en Asset Details, verificar esos dos valores y guardar. No editar las secuencias fuente del pack.

**Hecho — Codex por MCP.** ObjectTools.set_properties sustituyó la referencia del segmento en AM_Executioner_Dash_Fw/Bw por la copia correspondiente. DefaultSlot, un segmento 0–1 s desde StartPos=0, Rate=1 y una repetición; duración total 1 s, RateScale=1, Auto Blend Out y blends 0,06/0,08 s. AssetTools.save_assets guardó únicamente esos dos montages y dos clips. Lectura posterior al arranque limpio confirmada en [animaciones-mcp.json](animaciones-mcp.json).

Para reproducir, abrir cada montage y reemplazar su segmento por el clip propio del mismo sufijo, conservando el rango y los valores anteriores. Los clips definen la duración y el recorrido reales; el código ya no fuerza 0,3 s/350 cm.

## 3. Compilar y restaurar el DA

**Hecho — Codex por MCP.** LiveCodingToolset.LiveCodingToolset.CompileLiveCoding aplicó un parche Success. La lectura posterior reveló defaults en ComboSteps; no se guardó esa recarga.

**Hecho — usuario.** Cerrar Unreal sin guardar DA/Blueprints recargados; compilar vampsEditor Development Editor/Win64 mediante build.bat o el IDE y reabrir vamps.uproject. Confirmación recibida en la conversación.

**Hecho — Codex por MCP.** Tras el arranque: ObjectTools.list_properties/get_properties verificó el esquema nuevo y valores conservados; set_properties aplicó [data-assets-objetivo.json](data-assets-objetivo.json). Comparación final con cero diferencias, sin referencias transitorias; save_assets guardó únicamente el DA e is_dirty devolvió false.

En /Game/Vamps/Data/DA_VampsCombatSettings deben verse:
- ForwardDashMontage=AM_Executioner_Dash_Fw y BackwardDashMontage=AM_Executioner_Dash_Bw.
- ApproachDistance=250, ApproachDuration=0,25 y ApproachStopDistance=130, fuera de ComboSteps. El código los usa solo al empezar el primer ataque.
- Tres ComboSteps con alineación/empuje/reacción 130/100/1, 130/100/1 y 150/250/1.
- Energía 100; dash 25; sprint 20/s y 700 cm/s; recuperación 25/s tras 1 s.
- Sin DashMontages, DashDistance, DashDuration ni índices FinalImpact.

**Hecho — Codex por MCP.** CompileLiveCoding final devolvió NoChanges: no hubo un segundo parche.

## 4. Retirar montages sobrantes y comprobar arranque

**Hecho — Codex por MCP.** AssetTools.get_referencers devolvió listas vacías para AM_Executioner_Dash_FR/R/BR/BL/L/FL; AssetTools.delete eliminó cada uno y find_assets confirmó que solo quedan Fw/Bw. Para reproducir, comprobar Reference Viewer y borrar únicamente esos seis assets propios desde Content Browser.

**Hecho — Codex por MCP, verificación parcial.** EditorAppToolset.StartPIE abrió /Game/Vamps/Maps/L_CharacterBasic. SceneTools.find_actors, ObjectTools.get_properties y AbilitySystemInspectorToolset.GetGrantedAbilities/GetAttributeValues confirmaron GA_Dash y GA_Attack concedidas a ambos personajes, Idle y energía 100/100. LogsToolset.GetLogEntries no encontró errores/warnings de LogVampsCombat. StopPIE/IsPIERunning confirmaron fin de sesión. No hubo inputs simulados ni observación visual.

## 5. Observar el comportamiento

**Pendiente — usuario.** Abrir /Game/Vamps/Maps/L_CharacterBasic, iniciar Play y observar la cápsula/mesh, HUD y Output Log con LogVampsCombat. Controles existentes: WASD, Shift, RMB, F, F6 y R.

1. Con input direccional, pulsar Shift: un único dash frontal según el frente corporal actual. La cápsula debe acompañar al mesh, sin desplazamiento visual que vuelva al origen. Repetir cambiando dirección justo antes de pulsar; no se impone un giro nuevo para el dash.
2. Sin input y con target, Shift debe alejarse del target mediante el clip de retirada; sin target debe rechazarse con diagnóstico y sin coste.
3. Mantener Shift y movimiento al terminar el dash: sprint a 700 cm/s, energía 20/s; soltar detiene intención. Dash cobra 25 una vez; recuperación 25/s tras 1 s fuera de dash/sprint. No debe cambiar GuardLoad ni producir stun por agotamiento.
4. Iniciar ataques desde lejos y encadenar tres: solo el primero hace approach, limitado a 250 cm y detención 130 cm. Puede quedar corto; los siguientes conservan giro/alineación del combate sin repetir acercamiento.
5. Pedir continuación y dash durante la ventana: la continuación válida gana. Sin continuación, dash respeta el cierre de ventana y conserva Recovery/NA; durante NA siguen prohibidos ataque/defensa. No debe quedar un dash pendiente para otro paso.
6. Bloquear/recibir contactos: Guard/Hit/Stun impiden dash durante su reacción y se liberan por su calendario existente. No hay salida anticipada por un contacto marcado como final. Repetir multihit conservando AN_ResetWeaponHits: los reinicios permiten nuevos contactos, sin índices finales.
7. Atacar durante dash, recibir un contacto, usar R y comprobar muerte/reset: no deben quedar raíz, velocidad, intención ni habilidades activas residuales. Repetir junto a paredes y comprobar colisión, transición de animación y cámara/target.

Detener PIE al terminar. Registrar los resultados antes de cerrar 4; esta guía sustituye las verificaciones de impacto final y ocho direcciones de 037.
