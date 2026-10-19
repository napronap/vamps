# Editor — Cámara de seguimiento y combate

**Para el usuario:** el parche está compilado/aplicado y el arranque PIE fue comprobado. Resta la observación jugable de cámara, transiciones y colisiones del apartado 4. PIE quedó detenido. El HUD continúa pendiente para otro prompt.

## 1. Defaults y material

**Hecho — Codex por MCP.** ObjectTools.list_properties/get_properties capturaron cámara, boom y referencias de controllers en [configuracion-antes.json](configuracion-antes.json). No se cambió el DA de combate ni estructuras de sus propiedades.

**Hecho — Codex por MCP.** En `/Game/Vamps/Blueprints/BP_VampsCharacter_Player`, CameraBoom: Do Collision Test=false, Use Pawn Control Rotation=false, Auto Activate=false. FollowCamera: Auto Activate=false. ObjectTools.set_properties/get_properties y AssetTools.save_assets confirmaron esos defaults y su guardado antes de compilar C++.

**Hecho — Codex por MCP.** MaterialTools creó `/Game/Vamps/Materials/M_CameraOccluder` y sus expresiones; ObjectTools configuró:

1. Blend Mode=Masked, Two Sided=true, Dither Opacity Mask=true, Opacity Mask Clip Value=0,5.
2. Scalar Parameter `VampsCameraFade`: Default=0, Use Custom Primitive Data=true, Primitive Data Index=0.
3. Scalar→OneMinus→Opacity Mask; Constant3Vector=(0,18;0,32;0,44)→Base Color.
4. MaterialTools.recompile terminó sin error; conexiones y propiedades releídas, material guardado.

No hay parámetros editables por personaje. Para preparar otro mesh, todos sus slots que deban desaparecer deben usar un material propio compatible con ese escalar/índice.

## 2. Obstáculos del mapa

**Hecho — Codex por MCP.** SceneTools.add_to_scene_from_asset y ObjectTools configuraron y guardaron en `/Game/Vamps/Maps/L_CharacterBasic` tres cubos, escala (0,3;3;2,5):

| Label | Ubicación | Configuración |
|---|---|---|
| CameraFadeWall | (−500,0,125) | M_CameraOccluder, Component Tags=[VampsCameraFade], Custom Primitive Data=[0], Custom/WorldStatic/QueryAndPhysics, Camera=Ignore |
| CameraSolidWall | (−500,500,125) | Material de cubo existente, BlockAll, sin tag de fade |
| CameraInvisibleBoundary | (−500,−500,125) | Hidden In Game=true, Custom/WorldStatic/QueryAndPhysics, Camera=Ignore, sin tag de fade |

No activar Use Default Collision en esos componentes. Para los perfiles Custom, conservar Block en Pawn y los demás canales físicos; solo Camera ignora. El fade detecta por objeto/componente, por lo que no necesita que el obstáculo bloquee Camera. [Lecturas de configuración](oclusores-mcp.json).

## 3. Compilación y ajuste de desarrollo

**Hecho — Codex por MCP.** LiveCodingToolset.LiveCodingToolset.CompileLiveCoding aplicó un parche Success, no NoChanges. La primera compilación se detuvo por un nombre que ocultaba FadeAlpha heredado; se corrigió. [Resultado final](live-coding-final.json) y [log UBT](build-final.log). No se ejecutó build completo ni reinicio.

**Hecho — Codex por MCP, parcial.** StartPIE inició el mapa. ObjectTools y ActorTools confirmaron el manager nuevo, jugador vivo/Idle, Eye=(-820,0,160,15), Interest=(-220,0,120,15) y posición publicada coincidente: la pared preparada no retrae la cámara. LogVampsCombat no devolvió errores/warnings. StopPIE terminó la sesión e IsPIERunning confirmó false. Esto no verifica la presentación renderizada de fade, acciones o movimiento.

Live Coding advirtió cambios de tipos; no se guardaron assets después del parche. Antes de empaquetar, cerrar el editor, compilar `vampsEditor` con `Development Editor / Win64` desde la solución del proyecto y reabrir `C:/dev/vamps/vamps.uproject`. Ese build limpio no se atribuye a Codex ni es una observación de gameplay. No guardar referencias transitorias durante una recarga inestable.

Las variables `vamps.Camera.*` existen solo en builds no Shipping y son comunes a todas las cámaras; su registro y los valores iniciales fueron releídos con EditorAppToolset.SearchCVars. En la consola de Unreal, por ejemplo `vamps.Camera.FollowDistance 650` cambia la distancia de seguimiento. Son ajustes temporales de desarrollo; al reiniciar vuelven a los valores C++.

| Sufijo de vamps.Camera. | Inicial |
|---|---:|
| FollowDistance / CombatDistance | 600 / 425 cm |
| FollowEyeHeight / CombatEyeHeight | 70 / 30 cm |
| FollowInterestHeight / CombatInterestHeight | 30 / −20 cm |
| FollowVerticalFOV / CombatVerticalFOV | 38 / 32,4° |
| InteriorDistance / CombatYawOffset | 95 cm / 20° |
| PositionRadius | 180 cm; 0 deshabilita filtro |
| AngularSpeed | 6/s |
| TransitionDuration | 0,3 s |

Elegir distancias mayores a 12 cm, FOV en (0,170), velocidad positiva, radio/interior/duración no negativos y valores finitos. Los valores inválidos producen diagnóstico y cancelan esa actualización de cámara.

## 4. Observación jugable

**Pendiente — usuario.** En L_CharacterBasic, iniciar Play después de la compilación/arranque confirmado:

1. Con target, mantener A/D, frenar e invertir A→D→A; repetir diagonales, avance y retirada. Comparar deriva y continuidad con C01/C03/C08 de la investigación de cámara. El cuerpo puede moverse en pantalla y el rival no queda fijo al píxel central.
2. Repetir Shift direccional, retirada sin dirección y sprint. El giro instantáneo del dash no resetea historial ni recalcula su dirección capturada. bOrientRotationToMovement conserva la orientación del movimiento normal.
3. Iniciar ataque: entrar suavemente al encuadre cercano. Continuar cadena: mantenerlo entre pasos. No continuar/agotar pasos: volver al seguimiento al entrar el atacante en NA. Una solicitud rechazada no inicia el plano.
4. Bloquear una cadena enemiga: entrar en combate y mantenerlo hasta NA del agresor, aunque termine antes el gesto/reacción de bloqueo. Daño abierto no solicita otro plano ni retarget. Cancelaciones definitivas y muerte no dejan una referencia pendiente.
5. Cambiar por F y por guardia; cámara y HUD conservan la misma selección. Sin enemigos elegibles, retorno gradual detrás del jugador; readquisición desde la vista previa. Con varios rivales, repetir sobre otros ángulos.
6. CameraFadeWall está entre la cámara inicial y los personajes: debe desvanecerse y recuperar presentación al dejar de ocultarlos. Acercarse a ella: el personaje sigue bloqueado. Repetir con el target detrás de ella aunque el jugador esté visible.
7. Cerca de Y=500, CameraSolidWall retrae la cámara. Cerca de Y=−500, CameraInvisibleBoundary limita movimiento sin presentación que desvanecer. No hay cambios de colisión del personaje por fade.
8. Pulsar R durante encuadre/fade/transición: se limpian historial y referencias del mismo pawn. Terminar Play restaura los recursos afectados.
9. Comparar a distintas tasas de render y ajustar conjuntamente distancia/FOV/radio/ganancia. Los números iniciales son de Vamps; no se atribuyen al estado runtime exacto de FGOA.

Estas observaciones no se consideran realizadas por tener valores guardados o compilación correcta. No agregar ni ejecutar tests automatizados.

La captura por MCP no acreditó la imagen jugable: CaptureViewport produjo una captura del viewport de edición y CaptureEditorImage no pudo capturar ventanas. La comprobación visual queda al usuario; no se intentó automatizar la interfaz.
