# 021 — Configuración y comprobación de la etapa 1

La [guía 022](../022-cadena-y-na/editor.md) agrega cadena/NA y sustituye la preparación del montage cuando se completen sus pasos. Esta guía conserva las comprobaciones históricas de etapa 1.

**Sin pendientes de cierre para el usuario. Hecho — usuario, aceptación de la etapa, 2026-10-16.** La etapa 1 se cerró por su confirmación explícita. Los pasos siguientes quedan como guía reproducible; no implican nuevas ejecuciones por Codex ni un detalle de operaciones manuales comunicado por el usuario. El único Attack Trace de `/Game/Vamps/Animations/AM_BasicAttack` delimita contactos: AttackEnd inicia Recovery y termina defensa; RMB espera hasta MontageEnd. Ver el [cierre](reporte.md#cierre-de-la-etapa-1).

## 1. Código y compilación comprobados

La revisión de código confirma las fronteras acordadas: Preparation → Contacts al comenzar Attack Trace; Contacts → Recovery al terminarlo; Recovery → Idle al finalizar el montage. Solo Preparation/Contacts conservan defensa y admiten rearme. Recovery rechaza RMB y no bloquea locomoción; no es NA. No se requiere un cambio nativo para agregar otra fase o notify.

**Hecho — Codex por MCP, comprobación parcial de compilación actual.** LiveCodingToolset.LiveCodingToolset.CompileLiveCoding devolvió NoChanges el 2026-10-16. No representa un parche ni acredita la versión descrita por el reporte anterior.

No se confirmó una compilación completa desde cero. Las compilaciones actuales de Blueprint figuran en el paso 4; no hace falta repetirlas sin cambios o errores nuevos.

## 2. Entrada y asignaciones ya verificadas

**Hecho — Codex por MCP, lectura actual; configuración atribuida a la iteración previa.** ObjectTools.list_properties/get_properties confirmó:

1. `/Game/Vamps/Input/IA_Combat`: Value Type=Digital (bool), sin triggers ni modifiers. El controller enlaza Started.
2. `/Game/Vamps/Input/IMC_Vamps`: RMB=IA_Combat, D/A/W/S=IA_Move con sus modificadores, F=IA_CycleTarget; seis entradas.
3. `/Game/Vamps/Input/IMC_VampsCombatTest`: lo anterior más F6=IA_CycleAIMode y R=IA_ResetCombat; ocho entradas.
4. En Class Defaults → Input de `/Game/Vamps/Blueprints/BP_VampsPlayerController`: Default Input Mapping Context=IMC_Vamps, Combat Action=IA_Combat, Move Action=IA_Move, Cycle Target Action=IA_CycleTarget.
5. En `BP_VampsCombatTestPlayerController`: Default Input Mapping Context=IMC_VampsCombatTest y las mismas acciones; además Cycle AI Mode Action=IA_CycleAIMode y Reset Combat Action=IA_ResetCombat.
6. Ambos controllers conservan Player Camera Manager Class=BP_VampsPlayerCameraManager.

No hace falta repetir esta configuración. En MCP las entradas actuales están en defaultKeyMappings.mappings; mappings vacío es la propiedad anterior. No hay overrides de perfil. LMB, IA_Guard e IA_Look no están mapeados en esos contextos.

## 3. Delimitar las fases en el montage

**Hecho — usuario, aceptación de la etapa; tiempos no inspeccionados por Codex.** El catálogo MCP no ofrece edición directa de notifies de montage, y ObjectTools no expone Notifies/CompositeSections. Estos son los pasos manuales reproducibles; no se usó automatización de UI ni un script como sustituto.

1. Abrir `/Game/Vamps/Animations/AM_BasicAttack` desde Content Browser.
2. Conservar el segmento AS_BasicAttack1 en DefaultGroup.DefaultSlot, velocidad 1 y sin bucle de secciones. **Hecho — Codex por MCP, lectura parcial:** duración 1 s, Rate Scale 1, un segmento en DefaultSlot y una repetición. El enlace entre secciones sigue sin comprobar.
3. En Notifies del montage, conservar exactamente un Notify State Attack Trace (ANS_AttackTrace) sobre el contacto del puño confirmado en etapa 0. No duplicarlo en AS_BasicAttack1. Su inicio debe ser posterior a 0 y su final posterior al inicio.
4. El inicio de ese ANS es AttackStart y su final es AttackEnd. El mismo AttackEnd apaga trazas/defensa y comienza Recovery. No agregar un notify de recuperación ni una ventana separada para esta etapa.
5. Respetar `0 < AttackStart < AttackEnd < MontageEnd`. El código actual exige preparación y recuperación de duración positiva. El comienzo del blend-out no debe modificar esas fases.
6. Conservar Enable Auto Blend Out activo, Rate Scale=1, Blend Out Time=0,25 s y Blend Out Trigger Time=-1, valores releídos por MCP. Comprobar Montage Tick Type=Branching Point, Trigger Chance=1 y ausencia de filtros/umbrales de peso que impidan ejecutar los marcadores, incluido el tramo de blend-out.
7. Guardar solo si hubo cambios. La secuencia esperada es Preparation → Contacts → Recovery → Idle. La defensa abarca inicio–AttackEnd, salvo consumo. Recovery no admite ataque ni defensa, permite locomoción y conserva el resto visual del montage hasta su fin o interrupción por otra acción permitida. RMB no lo interrumpe.

La aceptación del usuario no se presenta como una confirmación individual de revisión o guardado de cada marcador. No se crean Blueprints de habilidad, subsystem o notify.

## 4. Referencias, Blueprints y mapa

**Hecho — Codex por MCP, revisión actual.** BlueprintTools.compile_blueprint con warnings_as_errors=true completó sin errores los ocho Blueprints: `/Game/Vamps/Animations/ABP_VampsCharacter` y, en `/Game/Vamps/Blueprints`, BP_VampsCharacter, BP_VampsCharacter_Player, BP_VampsPlayerController, BP_VampsCombatTestPlayerController, BP_VampsAIController, BP_VampsGameMode y BP_VampsCombatTestGameMode. AssetTools.is_dirty=false para esos ocho y AM_BasicAttack; no se guardaron cambios de assets.

ObjectTools confirmó Attack Montage=AM_BasicAttack y Attack Damage=12 en BP_VampsCharacter. SceneTools.get_current_level confirmó L_CharacterBasic. No se reinspeccionaron socket, grafo del AnimBP ni override del mapa en esta revisión.

**Hecho — usuario, aceptación de la etapa; referencia visual sin nueva inspección de Codex.** En ambos personajes conservar mesh/esqueleto, socket FistHit y Anim Class=ABP_VampsCharacter. En el AnimBP conservar BlendSpace → DefaultSlot → Output Pose. En L_CharacterBasic → World Settings comprobar GameMode Override=BP_VampsCombatTestGameMode. En ese GameMode conservar las clases Vamps y HUD Class=VampsCombatTestHUD, Initial AI Mode=Passive. En BP_VampsAIController conservar Attack Distance=100, Attack Interval=1 y Mixed Mode Duration=4; estos valores proceden de la configuración previa, no de una lectura nueva.

## 5. Comprobar el intercambio en PIE

**Hecho — usuario, aceptación global de la etapa el 2026-10-16.** EditorAppToolset.IsPIERunning confirmó PIE detenido durante la revisión de Codex. Codex no inició PIE ni observó estos casos, y el usuario no comunicó un resultado individual por caso. Se conserva este recorrido para reproducir el comportamiento aceptado:

1. En Passive, mantener RMB hasta después del montage: una solicitud/acción, Preparation → Contacts → Recovery → Idle. Defensa UNARMED desde AttackEnd. Soltar y pulsar después de MontageEnd inicia otra acción; LMB no combate.
2. Repetir clicks durante Preparation y Contacts: sube RequestCount y aparece DefenseRearmed, sin cambiar ActionNumber ni reiniciar el montage. Durante Recovery, el click muestra Rejected y no rearma defensa ni guarda un ataque para el final; después de MontageEnd hace falta otro click. Verificar los marcadores dentro del blend-out si los hay.
3. Contra Passive, acertar resta 12 HP una sola vez por rival/acción. Fuera de alcance o con pared no resta vida; al finalizar aparece Miss. Acertar a otro rival no cambia el target ofensivo.
4. Con F6 en Attack, sincronizar clicks con los golpes entrantes: debe poder bloquearse durante Preparation y Contacts. El primer contacto consume defensa, sin cancelar la ofensiva del agresor ni la propia. Otro click antes de AttackEnd rearma. Desde AttackEnd, incluso con defensa antes armada, el golpe entrante resuelve abierto.
5. Un nuevo contacto en un frame posterior, antes de AttackEnd y sin nuevo click tras el consumo, resuelve abierto. Al reproducir el caso, comprobar que la separación de ataques produzca ese intervalo para poder observarlo.
6. Seleccionar otro rival con F y bloquear a la IA: franja dorada, LOCK ON y cámara pasan al agresor. Daño abierto no fuerza ese cambio.
7. Comprobar contactos recíprocos con el mismo FRAME de resolución: ambos armados bloquean y consumen defensa; ambos desarmados reciben daño aunque uno llegue a cero HP. Con varios agresores, los contactos del mismo frame usan la defensa previa y la consumen juntos. Se conserva al target si participa o se selecciona el primero del grupo según la lista. Frames distintos no demuestran simultaneidad; no fabricar contactos mediante un arnés.
8. Guard solicita cuando queda desarmado y puede atacar; Attack conserva su intervalo y Mixed alterna modos. R durante las fases limpia combate, trazas y contactos pendientes del jugador/rival creado por GameMode, y restaura vida/posición. No debe llegar daño del intercambio anterior tras R.
9. Conservar WASD, F, cámara automática y F6/R; comprobar coherencia de selección entre HUD y cámara. Durante Recovery comprobar movimiento libre y la mezcla visual con locomoción. No dar por verificada la integración con otras acciones aún inexistentes.

El cierre está atribuido a la aceptación explícita del usuario y conserva los límites de observación de Codex. Cadena/NA y guardia residual corresponden a etapas posteriores.
