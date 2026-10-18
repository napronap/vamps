# 022 — Configurar y comprobar cadena y NA

El momento de continuación se actualiza en [024 — Cierre de Combo Window](../024-cierre-ventana-combo/editor.md): el click queda pendiente y el salto se decide al cerrar la ventana.

La detección del puño se reemplaza en [023 — Configurar armas y trazas](../023-armas-y-trazas/editor.md). Aplicar allí la configuración de armas; esta guía conserva la preparación de montages, ventanas y comprobación de cadena/NA.

**Pendiente para el usuario:** elegir las animaciones de ataque/reacción, preparar las secciones/notifies del paso 4 y comprobar/calibrar el combate del paso 5. Antes de la próxima apertura, ejecutar build.bat con Unreal cerrado para incorporar el traslado posterior de AttackMontage al DA. Ese traslado ya compiló por Live Coding, se guardaron los assets y se verificó el arranque en PIE. El componente de movimiento también tiene su arranque limpio verificado. La etapa 2 todavía no está cerrada.

La configuración anterior se conserva en [021](../021-solicitud-compartida/editor.md). Esta guía reemplaza su montaje de un paso cuando se complete la preparación siguiente; input, cámara y mapa se mantienen.

## 1. Recuperar el editor y la compilación persistente

**Hecho — usuario.** Realizó la compilación completa y reapertura del 17 de octubre, confirmadas con «listo, recompilado y vuelto a iniciar el editor». La recuperación anterior fue realizada el día 16. Esta ampliación cambia la clase del componente de movimiento y tipos reflejados. No hay MCP directo para compilación completa/reapertura; Codex no las ejecutó por terminal ni automatizó la interfaz. Recorrido reproducible:

1. Guardar las animaciones propias que se estén editando. Cerrar Unreal sin volver a guardar los Blueprints Vamps afectados por la recarga estructural de Live Coding. Con Unreal cerrado, ejecutar C:\dev\vamps\build.bat; compila vampsEditor Win64 Development con Unreal 5.8.
2. Esperar una compilación correcta. Abrir el proyecto con C:\dev\vamps\editor.bat.
3. Confirmar que el editor inicia y MCP vuelve a estar disponible. Si el Crash Reporter sigue abierto, cerrarlo manualmente.

**Hecho — Codex por MCP:** Live Coding compiló y aplicó el código ampliado con Result: Success y parche real. UHT registró componente, enum y estructura nuevos, con advertencias de recarga de tipos. Después del arranque limpio se aplicó otro parche real, sin cambios de tipos reflejados, para completar la limpieza de montages/root motion al reiniciar y conservar la liberación visual tras NA. Los assets se verificaron y guardaron después del arranque limpio, sin reproducir el assert anterior.

**Pendiente — usuario, para la próxima apertura:** el traslado posterior de AttackMontage al DA se compiló y aplicó por Live Coding, con Success. Los assets ya están guardados y verificados en la sesión actual. Para cargar ese código sin depender del parche en el siguiente arranque, guardar el trabajo, cerrar Unreal, ejecutar C:\dev\vamps\build.bat y abrir con C:\dev\vamps\editor.bat.

## 2. DataAsset de combate y montages

**Hecho — Codex por MCP.** DataAssetTools.create creó /Game/Vamps/Data/DA_VampsCombatSettings usando /Script/vamps.VampsCombatSettings. El paquete se guardó y ObjectTools confirmó NARecoveryDuration=0,5 después de la reapertura.

Para reproducirlo: Content Browser → Vamps/Data → Miscellaneous → Data Asset → VampsCombatSettings, nombre DA_VampsCombatSettings. En Details → Combat, Attack Montage=AM_BasicAttack y NA Recovery Duration=0,5 s. Guardar. El asset ya existe; no crear otro con un nombre diferente ni duplicar aquí tiempos del montage.

**Hecho — Codex por MCP:** ObjectTools.set_properties trasladó al DA la referencia /Game/Vamps/Animations/AM_BasicAttack.AM_BasicAttack que usaban ambos personajes; se guardó y releyó con save_assets=true e is_dirty=false. Attack Montage y Hit Reaction Montage se configuran aquí por personaje. Manny y ABP_VampsCharacter siguen compartidos, fuera del DA. La reacción de bloqueo se incorpora en la etapa 3.

**Hecho — Codex por MCP:** se releyeron los nuevos campos de DA_VampsCombatSettings después del arranque limpio y se guardó el paquete; save_assets=true e is_dirty=false. En Details → Combat, las tres entradas de Combo Steps son:

| Índice / paso | Target Alignment Distance | Knockback Distance |
|---|---|---|
| 0 / primero | 100 cm | 60 cm |
| 1 / segundo | 100 cm | 60 cm |
| 2 / tercero | 100 cm | 180 cm |

Son valores iniciales para calibrar con los clips elegidos. Mantener NA Recovery Duration=0,5 s. Hit Reaction Montage puede quedar vacío mientras se elige; posteriormente asignar el montage de reacción del receptor, compatible con su esqueleto, diferente de Attack Montage y con Auto Blend Out. Las distancias salen del DA del atacante y la reacción visual del DA del receptor. No copiar aquí tiempos de secciones.

## 3. Asignar configuración a ambos personajes

**Hecho — Codex por MCP:** ambas referencias se asignaron, compilaron, guardaron y releyeron. Después del arranque limpio se verificó la clase nueva del componente, se recompilaron ambos Blueprints y se guardaron; save_assets=true e is_dirty=false. Recorrido reproducible:

1. Abrir /Game/Vamps/Blueprints/BP_VampsCharacter → Class Defaults → Combat. Conservar Attack Damage=12 y asignar Combat Settings=DA_VampsCombatSettings. Attack Montage ya no aparece en el personaje: se configura en el DA del paso 2. Compile y Save.
2. Repetir la asignación explícita en /Game/Vamps/Blueprints/BP_VampsCharacter_Player. Su valor no se actualizó al configurar el otro Blueprint.
3. En Components → Character Movement, verificar que la clase real es VampsCombatMovementComponent, instalada por el constructor nativo. No agregar un segundo componente de movimiento. Releer ambas referencias y verificar guardado. CombatState debe comenzar en Idle, ComboStep=0, sin ventana, solicitudes, restricción ni liberación visual de Recovery.

Las herramientas son ObjectTools.list_properties/get_properties/get_class/set_properties, BlueprintTools.compile_blueprint y AssetTools.save_assets/is_dirty. get_class confirmó /Script/vamps.VampsCombatMovementComponent en CharMoveComp de ambos Blueprints y de sus instancias de PIE. El cambio previo del componente se guardó después del arranque limpio; el traslado posterior de montages se verificó y guardó tras su parche de Live Coding.

**Hecho — Codex por MCP tras el traslado de montages:** list_properties confirmó la ausencia de AttackMontage en ambos personajes. Ambos conservaron su referencia al DA, compilaron con warnings_as_errors=true y se guardaron con is_dirty=false. El DA es la única fuente del montage de ataque.

El pendiente anterior de recarga de BP_VampsCharacter_C_1 no se mantiene para una instancia ausente: la consulta posterior del mapa no encontró personajes colocados. Si el usuario coloca otros personajes, verificar su Combat Settings y componente nativo antes de usarlos.

## 4. Preparar el único montage de la cadena

**Pendiente — usuario.** El catálogo MCP no incluye edición directa de secciones/notifies de montage, y ObjectTools no expone CompositeSections ni Notifies. La animación continua y sus tiempos todavía no están elegidos; por eso no se fijan aquí segundos de contacto que aparenten corresponder a un clip existente.

1. Abrir /Game/Vamps/Animations/AM_BasicAttack. Preparar el clip continuo elegido, compatible con el esqueleto actual, en DefaultGroup.DefaultSlot. Conservar el grafo propio de ABP_VampsCharacter. No reutilizar lógica ni AnimBPs de Third Person.
2. Dividir la línea de tiempo en tres secciones consecutivas: Attack1 desde 0, Attack2 al comienzo del segundo golpe y Attack3 al comienzo del tercero. Los nombres son descriptivos; el código usa su orden, no esos nombres concretos. Cada sección debe incluir preparación, contacto y recuperación propia.
3. En cada Animation Sequence usada, activar Enable Root Motion y revisar que el desplazamiento esté en el hueso raíz. **Hecho — Codex por MCP:** se activó, guardó y releyó Enable Root Motion=true en /Game/Vamps/Animations/AS_BasicAttack1, conservando Root Lock=RefPose. Las animaciones nuevas requieren su propia revisión. ABP_VampsCharacter conserva Root Motion from Montages Only.
4. Mantener Rate Scale=1 inicialmente, Enable Auto Blend Out activo, Blend Out Time=0,25 s y Blend Out Trigger Time=-1. No usar Time Stretch Curve ni reproducción inversa. En ejecución se desconectan los enlaces automáticos: solo un salto aceptado reproduce el siguiente paso.
5. En cada sección agregar exactamente un Notify State Attack Trace (ANS_AttackTrace), sobre el contacto real del puño. Debe cumplirse SectionStart < AttackStart < AttackEnd < SectionEnd. Colocarlos en el montage, sin duplicarlos en la secuencia fuente.
6. En Attack1 y Attack2 agregar un Notify State Combo Window (ANS_ComboWindow). Desplazar sus extremos aproximadamente AttackStart+n y AttackEnd+n, con n positivo elegido sobre el clip. Debe empezar después de AttackStart, terminar después de AttackEnd y cerrar antes del comienzo del blend-out de esa sección. Con los valores iniciales: ComboEnd < SectionEnd−0,25 s. Si no hay espacio, ajustar el clip/fronteras/blend de forma coherente; no cruzar a la sección siguiente.
7. Attack3 puede omitir Combo Window porque no tiene continuación. El tercer paso siempre termina la cadena.
8. Para los notifies, mantener Branching Point, Trigger Chance=1 y condiciones que permitan ejecutarlos durante toda su ventana. No añadir un notify de NA: el cierre lógico de la acción inicia la recuperación desde el DataAsset.
9. Preparar el montage de reacción en DefaultSlot, sin Attack Trace ni Combo Window, con Auto Blend Out activo. Asignarlo en el DA del receptor cuando esté elegido. Su traslación raíz no se suma al knockback programado.
10. Guardar los montages. Un click de continuación aceptado salta directamente al inicio de la siguiente sección; no espera a terminar la recuperación de la anterior. Elegir fronteras que mantengan continuidad visual al saltar desde la ventana.

La validación nativa rechaza configuración incompleta o ventanas fuera de sus fronteras. Actualmente admite un impacto por sección; no agregar varios Attack Trace al mismo paso.

## 5. Blueprints y observación

**Hecho — Codex por MCP:** después del arranque limpio, BlueprintTools.compile_blueprint con warnings_as_errors=true compiló ABP_VampsCharacter; BP_VampsCharacter y BP_VampsCharacter_Player; BP_VampsPlayerController y BP_VampsCombatTestPlayerController; BP_VampsAIController; BP_VampsGameMode y BP_VampsCombatTestGameMode. Los ocho completaron sin errores ni advertencias.

**Hecho — Codex por MCP, comprobación parcial de arranque con la ampliación:** StartPIE/StopPIE y ObjectTools verificaron los dos personajes presentes —jugador y rival del GameMode— en Idle, ComboStep=0, sin solicitudes, restricciones, ventana ni liberación visual. Ambos recibieron DA_VampsCombatSettings y usaron VampsCombatMovementComponent. No se solicitaron ataques ni se observó el combo. El log consultado no devolvió asserts ni errores de Blueprint/PIE; solo una sesión MCP vencida durante la reconexión. PIE quedó detenido y la IA conserva Passive.

**Hecho — Codex por MCP tras el traslado de montages:** ambos personajes y ABP_VampsCharacter volvieron a compilar sin advertencias. Se repitió StartPIE/StopPIE y se releyó el DA de ambos personajes, en Idle y sin solicitudes. No hubo nuevos errores de Blueprint/PIE ni asserts; PIE quedó detenido. Manny, el AnimBP y las animaciones no se editaron en este traslado.

**Pendiente — usuario, comprobación jugable.** Abrir /Game/Vamps/Maps/L_CharacterBasic con BP_VampsCombatTestGameMode, jugar en PIE y conservar RMB por click, WASD, F, F6 y R.

1. Con IA Passive y un rival al alcance, ejecutar tres clicks dentro de las ventanas de los pasos correspondientes: STEP 1 → 2 → 3, sin reproducción automática de otro golpe. Cada paso debe poder aplicar un nuevo contacto al mismo rival.
2. Mantener RMB o hacer todos los clicks antes de abrir la ventana: no encadena. Un click temprano puede rearmar defensa antes de AttackEnd, pero no genera un salto posterior.
3. Pulsar dentro de ventana antes del contacto: CONTINUATION queda PENDING únicamente para ese paso. Al confirmar contacto se acepta el salto; si el paso falla, termina sin continuar. Repetir el fallo del primer y segundo paso, incluso pulsando varias veces.
4. Dejar de pulsar tras el primer o segundo acierto: reproducir su recuperación y entrar en NARecovery. Después del tercero siempre debe ocurrir ese cierre. Durante NA, RMB no ataca ni defiende; WASD sigue funcionando. Al vencer 0,5 s, STEP vuelve a 0 y hace falta otra pulsación para atacar.
5. Comprobar defensa hasta AttackEnd y ausencia de rearme durante Recovery, incluso si Combo Window sigue abierta. Un contacto defendido debe habilitar continuación para el atacante. Golpear a otro rival también habilita sin cambiar el target ofensivo; bloquear sí retargetea al agresor.
6. En IA Attack, comprobar que utiliza las mismas ventanas y contactos para encadenar. No debe saltarse una sección fallida, NA ni OFFENSE LOCK. El intervalo de decisión de IA es 0,1 s: usar inicialmente ventanas suficientemente amplias para observar su solicitud.
7. Bloquear y observar OFFENSE LOCK: duración completa de la sección que golpeó, ajustada por su velocidad, desde cada bloqueo y con margen cero. Mientras dure, no iniciar ni continuar ofensiva; poder rearmar defensa en fases válidas. Un bloqueo posterior renueva el vencimiento sin acortarlo. El golpe propio ya iniciado no se cancela por bloquear.
8. Separar restricción de NA: bloquear estando en Idle no debe iniciar NA. Si se bloquea durante una ofensiva propia, su cierre normal conserva NA. Expirar la restricción no dispara un ataque guardado.
9. Con contactos del mismo frame, comprobar que el bloqueo se resuelve antes de autorizar la continuación. Mantener la resolución conjunta de defensa y daño; frames diferentes no acreditan simultaneidad.
10. Pulsar R durante preparación, contactos, ventana, restricción y NA. Deben limpiarse pasos, trazas, solicitudes, contactos y timers, sin daño ni cambios de fase tardíos del intercambio anterior. Conservar F, cámara y locomoción.
11. Con un impacto abierto, observar MOTION=Alignment → Knockback → None. El receptor se orienta y se acomoda delante del atacante a 100 cm iniciales; después retrocede 60 cm en pasos 1/2 y 180 cm en el tercero. Con bloqueo, pasar directamente a Knockback y conservar la orientación/acción del defensor. Las paredes pueden reducir ambas distancias.
12. Situar el intercambio cerca de una pared: la alineación termina donde permite la colisión y el empuje parte desde esa posición real. El knockback se detiene al bloquear, sin deslizar lateralmente para rodear la pared. Aceptar la distancia menor y el solapamiento visual de las animaciones; las cápsulas siguen respetando la colisión.
13. Comprobar que el root motion del atacante mueve su cápsula y que no vuelve al origen al terminar el montage. Los saltos de sección deben aplicar solamente el desplazamiento reproducido. La reacción no debe agregar un segundo desplazamiento sobre el del DA.
14. Mantener WASD al atacar: la ventana de combo sigue disponible durante Recovery. Si se continúa válidamente, prevalece el siguiente paso. Al cerrar la ventana sin continuar, RELEASED pasa a YES y la animación sale con blend-out hacia locomoción. La fase lógica conserva el resto de Recovery y luego NA; el próximo ataque no debe habilitarse antes por moverse.
15. Repetir R durante Alignment, Knockback y Recovery visualmente liberado: limpiar desplazamiento, reacción y timers. Cambiar el modo de IA debe conservar esa limpieza.

La alineación inicial dura 0,08 s y el knockback 0,12 s en código, como punto de partida. Ajustar las distancias del DA junto con el avance real de las nuevas animaciones para comprobar que la cadena conserve alcance. No se añadieron contactos automáticos ni varios impactos por sección.

Codex comprobó únicamente el arranque en PIE y no observó estos casos de combate. El cierre de la etapa requiere registrar lo observado; las compilaciones por sí solas no lo sustituyen.
