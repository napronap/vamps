# 021 — Configurar la solicitud compartida y las fases

**Pendiente para el usuario:** agregar y guardar `Combat Recovery` en `AM_BasicAttack`, después del final de `Attack Trace` y antes del final del montage; ver [paso 4](#4-delimitar-las-fases-en-el-montage). Su posición define el comienzo de recuperación, sin límite impuesto por el inicio del blend-out. El usuario confirmó que todavía no lo agregó. Después queda comprobar el intercambio en PIE. Input, asignaciones y compilación/guardado de Blueprints ya están hechos por MCP; no hay que repetirlos.

El MCP está disponible. Su catálogo y `ObjectTools` no admiten leer/editar los notifies de montage, por lo que ese paso queda manual según `AGENTS.md`. El [reporte](reporte.md) consolida las operaciones hechas y sus límites.

## 1. Compilación nativa

**Hecho — Codex por MCP.** `LiveCodingToolset.LiveCodingToolset.CompileLiveCoding` compiló y aplicó la corrección de independencia respecto del blend-out con **Success**. La consulta anterior había devuelto **NoChanges**; no se cuenta como otro parche. Mediante reflexión se confirmó que el editor tiene cargados `VampsCombatResolver`, `AN_CombatRecovery`, los campos de fase/contadores y `CombatAction`. Los ocho Blueprints volvieron a compilarse/guardarse sin errores ni warnings.

**Pendiente — usuario, consolidación antes de empaquetar.** Live Coding advirtió que los cambios de tipos pueden afectar al empaquetado. Guardar, cerrar Unreal, compilar **vampsEditor / Development Editor / Win64** en Rider o Visual Studio para `C:\fran\vamps\vamps.uproject`, y reabrir tras una compilación correcta. No se atribuye esa compilación completa al usuario sin confirmación.

No iniciar PIE hasta agregar el notify del paso 4: el código lo exige y reporta un error si falta.

## 2. Crear la entrada compartida y cambiar los mappings

**Hecho — Codex por MCP.** `DataAssetTools.create`, `ObjectTools.list_properties/get_properties/set_properties` y `AssetTools.save_assets/is_dirty`. Se creó `IA_Combat` y se verificaron Boolean, triggers/modifiers vacíos y los mappings finales. Pasos reproducibles en Content Browser:

1. En `/Game/Vamps/Input`, crear **Input → Input Action** llamada `IA_Combat`.
2. Abrirla y establecer **Value Type = Digital (bool)**. Dejar **Triggers** y **Modifiers** vacíos. Guardar. El controller escucha `Started`, por lo que mantener el botón no repite la solicitud.
3. Abrir `/Game/Vamps/Input/IMC_Vamps`. Quitar las entradas de `IA_Attack` y `IA_Guard`. Agregar una sola entrada de `IA_Combat` con tecla **Right Mouse Button**, sin triggers ni modifiers.
4. Repetir el cambio en `/Game/Vamps/Input/IMC_VampsCombatTest`.
5. Conservar los mappings y modificadores existentes de `IA_Move` para D/A/W/S y el mapping F de `IA_CycleTarget`. En el contexto de prueba conservar F6 de `IA_CycleAIMode` y R de `IA_ResetCombat`.
6. Guardar los dos contextos. Con la configuración previa de 019, deben quedar seis mappings en el normal y ocho en el de prueba. LMB ya no inicia combate y `IA_Look` continúa fuera de ambos contextos. No hace falta borrar los assets antiguos de input.

## 3. Asignar los dos controllers

**Hecho — Codex por MCP.** Se asignó y releyó `combatAction` en ambos controllers mediante `ObjectTools`, y se compilaron/guardaron. Para reproducir, abrir **Class Defaults → Input** de:

| Blueprint en `/Game/Vamps/Blueprints` | Default Input Mapping Context | Combat Action |
|---|---|---|
| `BP_VampsPlayerController` | `/Game/Vamps/Input/IMC_Vamps` | `/Game/Vamps/Input/IA_Combat` |
| `BP_VampsCombatTestPlayerController` | `/Game/Vamps/Input/IMC_VampsCombatTest` | `/Game/Vamps/Input/IA_Combat` |

En ambos, conservar **Move Action = IA_Move**, **Cycle Target Action = IA_CycleTarget** y **Player Camera Manager Class = BP_VampsPlayerCameraManager**. En el de prueba conservar **Cycle AI Mode Action = IA_CycleAIMode** y **Reset Combat Action = IA_ResetCombat**. Compilar y guardar ambos Blueprints. Las propiedades Attack Action y Guard Action se retiraron del código y se sustituyen por Combat Action.

## 4. Delimitar las fases en el montage

**Pendiente — usuario.** Abrir `/Game/Vamps/Animations/AM_BasicAttack`:

1. Conservar el segmento único de `AS_BasicAttack1`, slot `DefaultGroup.DefaultSlot`, reproducción hacia delante a velocidad 1 y sin enlace de sección que produzca un bucle. **Hecho — Codex por MCP, lectura parcial:** se verificaron duración **1 s**, Rate Scale **1**, Auto Blend Out activo, Blend Out Time **0,25 s** y Blend Out Trigger Time **−1**. Las secciones/notifies no están expuestos; su revisión sigue pendiente para el usuario.
2. En la pista **Notifies del montage**, conservar exactamente un Notify State **Attack Trace** (`ANS_AttackTrace`) sobre el contacto del puño confirmado en etapa 0. Su inicio debe ser posterior a 0 y su final posterior al inicio. No duplicarlo en la secuencia de animación.
3. Después del final de Attack Trace, agregar **Add Notify → Combat Recovery** (`AN_CombatRecovery`) donde deba terminar la ventana de continuación y comenzar la recuperación del clip. El tramo anterior al notify es ventana; el posterior es recuperación. No se fija un segundo universal para todas las animaciones.
4. Mantener el orden `0 < inicio de trazas < fin de trazas < Combat Recovery < final del montage`. Los marcadores pueden atravesar el inicio del blend-out natural; este no cambia la fase ni apaga las trazas. La reproducción debe alcanzar todos los notifies antes de terminar. No superponer sus fronteras ni colocar Recovery justo en el último instante.
5. Mantener **Enable Auto Blend Out = activo** y **Rate Scale = 1**. Ambos tipos de notify son Branching Points nativos; comprobar **Montage Tick Type = Branching Point**, **Trigger Chance = 1**, sin filtros que impidan su ejecución, y un umbral de peso que permita dispararlos en ese tramo.
6. Guardar el montage. El código valida que haya exactamente un Attack Trace y un Combat Recovery en ese orden antes del final; no usa un timer como sustituto si faltan ni compara sus posiciones con el comienzo del blend-out.

Secuencia esperada: aceptar acción → `Preparation`; comienzo de Attack Trace → `Contacts`; final de Attack Trace → `ContinuationWindow`; Combat Recovery → `Recovery`; final del montage → `Idle`.

La defensa se arma con el click y vence al finalizar la acción, no al empezar Recovery. No requiere otro notify ni un plazo de 0,25 s. Consumirla o rearmarla no reinicia el montage. Para otro personaje se configura su propio Attack Montage con estos mismos tipos de notify y tiempos ajustados a su clip.

## 5. Verificar referencias, compilar y guardar

**Hecho — Codex por MCP, referencias, compilación y guardado.** Se usaron `ObjectTools.list_properties/get_properties`, `BlueprintTools.get_parent/compile_blueprint`, `SceneTools.get_current_level/find_actors` y `AssetTools.save_assets/is_dirty`. Ocho Blueprints compilados con warnings tratados como errores, sin errores ni warnings; once assets guardados y con `is_dirty = false`. No se reinspeccionó el grafo del AnimBP ni el socket; se conserva su setup previo. Comprobaciones reproducibles:

1. En `/Game/Vamps/Blueprints/BP_VampsCharacter` y `BP_VampsCharacter_Player`, comprobar **Attack Montage = /Game/Vamps/Animations/AM_BasicAttack** y **Attack Damage = 12**, conservando mesh, socket `FistHit` y **Anim Class = ABP_VampsCharacter**.
2. Conservar el grafo propio de `/Game/Vamps/Animations/ABP_VampsCharacter`: BlendSpace → DefaultSlot → Output Pose. Su consulta nativa de guardia presenta defensa armada; el AnimBP no decide si hubo bloqueo.
3. Compilar y guardar ambos personajes, los dos controllers, `BP_VampsAIController`, `BP_VampsGameMode`, `BP_VampsCombatTestGameMode` y el AnimBP. Revisar que no haya errores ni referencias a la habilidad `GA_Guard` retirada.
4. En `BP_VampsCombatTestGameMode`, conservar las clases Vamps de jugador/controller/rival/IA, **HUD Class = VampsCombatTestHUD** e **Initial AI Mode = Passive**. Conservar los valores de observación de la IA: **Attack Distance = 100**, **Attack Interval = 1**, **Mixed Mode Duration = 4**.
5. Abrir `/Game/Vamps/Maps/L_CharacterBasic`, comprobar **World Settings → GameMode Override = BP_VampsCombatTestGameMode**. Se verificaron ese mapa, el override y `is_dirty = false`; no hubo cambios de actores ni guardado del mapa por Codex.

El resolver es un subsystem nativo del mundo: no se coloca un actor ni se crea un Blueprint para activarlo. Tampoco se crean Blueprints para `GA_Attack` o los notifies.

## 6. Comprobar en PIE

**Pendiente — usuario, después del paso 4.** Las consultas de estado por MCP pueden retomarse cuando se haya guardado el notify; la observación de inputs/presentación sigue siendo manual. Se confirmó PIE detenido y no se lo inició sin el notify requerido. Los resultados siguientes son criterios, no comprobaciones ya realizadas. Los campos `IN` y `OUT` muestran el último resultado y su frame; pueden pertenecer a momentos distintos. El contador de solicitudes y el número de acción permiten distinguir otro click de otra acción.

1. **Pulsación y vencimiento.** En Passive, mantener RMB desde idle hasta después del fin del montage: una sola solicitud y una sola acción. Deben recorrerse las fases y volver a `Idle`, con defensa `UNARMED`, sin arrancar otro ataque mientras se mantenga el botón. Soltar y volver a pulsar inicia otra acción.
2. **Clicks durante la acción.** Pulsar de nuevo durante Preparation, Contacts, ContinuationWindow y Recovery en intentos separados. Cada click incrementa solicitudes y muestra `DefenseRearmed`; conserva el número de acción y no reinicia el montage. En esta etapa todavía no hay continuación ofensiva. Un click temprano no queda guardado para el final. Si un marcador está dentro del blend-out, comprobar que siga cambiando la fase en ese punto y que el cierre de trazas lo siga determinando Attack Trace.
3. **Contacto abierto y fallo.** Contra el rival Passive, acercarse y ejecutar un golpe: resta 12 HP una sola vez por rival y acción, aunque el puño permanezca dentro. Fuera de alcance o con pared entre personajes no debe restar vida; al terminar se muestra `Miss`. Un acierto a otro rival no cambia el target ofensivo.
4. **Defensa durante la acción.** Pasar a Attack con F6, acercarse y sincronizar clicks con los ataques del rival. Observar defensa válida tanto en Preparation como en Contacts. El primer contacto entrante muestra `Blocked`, consume defensa y conserva el montage/fase propios; no cancela al agresor. Otro click rearma hasta el final de esa misma acción.
5. **Consumo y nuevo contacto.** Si llega otro golpe en un frame posterior antes de terminar la acción y sin otro click, debe resolver abierto. No considerar demostrado este caso si la separación de ataques hace que la acción ya haya terminado. Registrar el caso pendiente si el escenario no produce ese intervalo.
6. **Retarget por bloqueo.** Con otro rival seleccionado mediante F, bloquear al rival controlado por la IA: la franja dorada, `LOCK ON` y cámara pasan al agresor. El daño abierto no fuerza ese cambio. El bloqueo no corta la ofensiva de ninguno por sí mismo.
7. **Simultaneidad.** En Attack, buscar un intercambio cuyos contactos de ambos participantes muestren el mismo `FRAME` en `OUT`. Pausar PIE después del contacto y comparar los dos paneles. Si ambos estaban armados, ambos reciben `Blocked` y quedan desarmados; si ambos estaban sin defensa, ambos deben recibir el daño aunque uno quede con HP cero. Frames distintos no comprueban este criterio. Con varios agresores, todos los contactos entrantes del mismo frame usan la defensa previa y la consumen juntos; el target conserva al seleccionado si participa, o pasa al primero del grupo según la lista del HUD. El escenario debe producir esos contactos reales para cerrar ese caso; no se agregó un control que los fabrique.
8. **IA y reinicio.** Guard solicita combate cuando queda desarmado y por ello puede atacar; no sostiene una guardia pasiva. Attack conserva su intervalo y Mixed alterna los modos. R durante preparación/contactos/recuperación debe limpiar fase, defensa, resultados y contadores del jugador y del rival creado por el GameMode, cerrar trazas y restaurar vida/posición. No deben aparecer daños pendientes del intercambio anterior después del reinicio.
9. **Selección y locomoción.** Conservar WASD, F, cámara automática y F6/R del escenario. El HUD debe seguir mostrando el mismo objetivo que la cámara.

No cerrar la etapa 1 únicamente por compilar. Falta observar y confirmar estos comportamientos. La cadena de tres pasos, NA y el daño residual/recurso de guardia se implementan en sus etapas posteriores.
