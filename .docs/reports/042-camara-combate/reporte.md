# 042 — Cámara de seguimiento, combate y oclusión

La salida de combat framing queda actualizada en [060 — Regreso antes de NA sin cortar combos](../060-salida-framing-antes-na/reporte.md). El encuadre general debe estar recuperado al comenzar NA, conservando su duración.

Corrección posterior del seguimiento sin target: [043 — Referencia virtual y modo de observación](../043-camara-sin-target/reporte.md), con [guía de prueba sin lock-on](../043-camara-sin-target/editor.md). Sustituye el retorno hacia el frente corporal por la nueva referencia de vista independiente.

## Alcance — 2026-10-20

Implementa el [prompt 041](../041-revision-paso-5/prompt.md): dos encuadres globales, Eye/Interest persistentes, transiciones y oclusión de jugador/target. Mantiene el HUD actual; no agrega amenazas, proximidad ni sistemas de partida/red. Parche Live Coding aplicado y arranque PIE comprobado. La observación jugable continúa pendiente; no se declara cerrado el paso 5 completo.

## Código y relaciones

Rutas relativas a Source/vamps, con header en Public y cpp en Private:

- Player/VampsPlayerCameraManager.h/.cpp: calcula el POV local en UpdateViewTargetInternal. Conserva yaw orbital, Eye e Interest; compone dos encuadres comunes y convierte FOV vertical a horizontal para aspecto de referencia 16:9 con MaintainYFOV explícito. UpdateCamera publica la rotación final en ControlRotation para Move y MobilityPressed. ProcessViewRotation no vuelve a calcular seguimiento. No hay datos por personaje ni DA visual.
- Characters/VampsCharacter.cpp: solicita encuadre solo después de BeginCombatAction aceptado y desde RememberDefendedContact. El helper local requiere el manager propio y no hace nada en personajes sin player controller local, condición normal de IA. No modifica arbitraje, permisos, daño, ventanas, root motion ni HUD.
- Characters/VampsCharacter_Player.cpp: desactiva seguimiento/colisión y tick del boom y autoactivación de FollowCamera. Se conservan los componentes existentes; el manager produce directamente el POV sin llamar CalcCamera para el pawn local.

La cámara conserva el agresor que originó el encuadre, su generación de combate y su instancia de montage; los pasos de continuación comparten esa instancia. Sigue Recovery aunque locomoción haya liberado la reproducción visual. NA, cancelación definitiva, muerte o invalidación de esa cadena liberan la referencia. No usa el final del montage de bloqueo ni ReactionEndsAt para decidir su salida. Reset R se detecta por CombatGeneration del mismo pawn; cambio de pawn y EndPlay restauran los oclusores y limpian historial.

Un ataque propio entra en combate; bloquear efectivamente entra por la cadena del agresor. Daño abierto no solicita otro plano ni target. Se conserva UVampsLockOnComponent como única selección y el orden de F; sin target se sigue gradualmente el frente corporal. bOrientRotationToMovement y la captura/giro del dash de 040 permanecen. El desfase aceptado de un frame entre resolver y cámara se conserva, sin mover callbacks ni duplicar resolución.

## Valores iniciales y adaptación

| Parámetro común | Seguimiento | Combate |
|---|---:|---:|
| Distancia horizontal | 600 cm | 425 cm |
| Altura del ojo desde centro de cápsula | 70 cm | 30 cm |
| Altura de interés desde centro de cápsula | 30 cm | −20 cm |
| Desplazamiento interior hacia target | 0 cm | 95 cm, limitado a 0,95 de la separación |
| Offset yaw con target | 0° | 20° |
| FOV vertical | 38° | 32,4° |

Transición cosenoidal de 0,3 s desde la vista publicada anterior, sin apilar el filtro espacial durante esa mezcla. Ganancia angular exponencial de 6/s; radio espacial común 180 cm, cero deshabilita filtro. El filtro parte del error espacial cuadrático de la investigación, con conversión temporal `1-(1-alphaRef)^(dt*60)` y subpasos máximos de 1/120 s. 60 es la referencia de adaptación elegida para Vamps, no una frecuencia nativa demostrada. Los objetivos espaciales del frame se mantienen durante los subpasos; falta comparar el movimiento renderizado a distintas tasas.

Los números son una calibración inicial propia basada en la escala de cápsula/mesh existente y las relaciones de la investigación; no equivalen a mediciones runtime de FGOA. Ajustes globales de desarrollo mediante vamps.Camera.*, registrados solo fuera de Shipping con ECVF_Cheat; no hay configuración por personaje. La guía enumera nombres y uso.

## Oclusión y assets

El manager hace una única prueba de esfera de radio 12 cm por ECC_Camera desde el centro de cápsula hacia el ojo deseado para sólidos no preparados. El estado Eye sin colisión permanece independiente de esa posición publicada. La consulta de oclusores usa overlap de objetos y trazas sobre cada componente marcado; puede encontrar varias paredes preparadas aunque ignoren ECC_Camera y protege segmentos ojo→jugador y ojo→target.

El contrato del componente preparado es Component Tags=[VampsCameraFade], respuesta Camera=Ignore y material con escalar VampsCameraFade usando Custom Primitive Data índice 0. La cámara modifica ese dato por componente, no un material compartido; conserva el valor inicial y lo restaura gradualmente al salir de oclusión e inmediatamente al limpiar la cámara. No sustituye materiales ni cambia colisiones durante gameplay.

- Nuevo `/Game/Vamps/Materials/M_CameraOccluder`: Masked, Two Sided, Dither Opacity Mask, clip=0,5. Opacity Mask=1−VampsCameraFade; escalar por Custom Primitive Data índice 0, default=0. Base Color=(0,18;0,32;0,44). Creado, conectado, recompilado y releído por MCP; el dither nativo del material aplica la máscara gradual sin una función de shader adicional.
- Modificado `/Game/Vamps/Blueprints/BP_VampsCharacter_Player`: defaults CameraBoom DoCollisionTest=false, UsePawnControlRotation=false y AutoActivate=false; FollowCamera AutoActivate=false. Guardado antes de Live Coding y relectura confirmada. Los demás valores capturados se conservan.
- Modificado `/Game/Vamps/Maps/L_CharacterBasic`: tres actores nativos StaticMeshActor con `/Engine/BasicShapes/Cube`, escala (0,3;3;2,5), Z=125 y X=−500. CameraFadeWall tiene Y=0 y material/tag anteriores; CameraSolidWall tiene Y=500 y BlockAll; CameraInvisibleBoundary tiene Y=−500, HiddenInGame y Camera=Ignore. Los preparados/invisibles usan Custom, WorldStatic, QueryAndPhysics; la respuesta modificada es Camera=Ignore y se conserva bloqueo físico. La configuración se releyó y se guardó el mapa.

`BP_VampsPlayerCameraManager` conserva su clase nativa propia. Ambos controllers siguen referenciándolo y conservan sus mappings/acciones. No se modifican montages, clips, AnimBP, UVampsCombatSettings, structs de DA ni atributos. La [captura previa](configuracion-antes.json) conserva las propiedades de cámara/controladores consultadas; no se presenta como una captura completa del DA. Los [oclusores](oclusores-mcp.json) conservan las lecturas de configuración.

## Verificaciones y pendientes

- clang-format aplicado a los cuatro fuentes. Se retiraron únicamente los comentarios automáticos de cierre de namespace para respetar la regla de no agregar comentarios. Revisión de jerarquía para overrides y nombres; git diff --check sin errores de espacios.
- Material recompilado por MaterialTools.recompile sin error; conexiones y propiedades releídas. AssetTools.save_assets confirmó guardado del material, Blueprint afectado y mapa antes de compilar C++.
- La primera llamada MCP devolvió [CompileNotStarted](live-coding-1.json), pero su [log UBT](build-1.log) muestra compilación efectiva y un error C4458: FadeAlpha ocultaba un miembro heredado. Se corrigió a OccluderBlendAlpha. La segunda llamada aplicó un parche [Success](live-coding-final.json), no NoChanges, con [UBT Succeeded](build-final.log). No se ejecutó un build completo/reinicio.
- Live Coding advirtió cambios de tipos y posible impacto en empaquetado. No se guardaron assets después de esa recarga. Las referencias CDO/runtime leídas conservan rutas reales propias, sin REINST. Antes de empaquetar corresponde un build limpio del editor cerrado, fuera de esta comprobación de PIE.
- Relectura posterior del manager confirmó Eye/Interest y encuadre nuevos, sin los parámetros editables anteriores. SearchCVars confirmó los 13 valores globales de desarrollo. Material, Blueprint afectado y mapa devolvieron is_dirty=false. [Estado final del material](material-mcp.json).
- PIE iniciado en L_CharacterBasic mediante EditorAppToolset.StartPIE. Jugador vivo/Idle, sin acción solicitada y manager propio de runtime. Eye=(-820,0,160,15), Interest=(-220,0,120,15); transformación publicada coincidente con Eye, yaw=0 y pitch≈−3,814°. Esto confirma que el manager publica la distancia de seguimiento de 600 cm y la pared preparada situada en X=−500 no acorta el POV. [Lectura](pie-estado.json).
- Consulta LogVampsCombat Error/Warning vacía en la sesión. No se simularon inputs ni se invocaron acciones de combate para crear un arnés. No se afirma haber observado entrada/salida de combate, dirección del dash, suavidad lateral ni fade/restauración renderizados.
- CaptureViewport inicialmente rechazó el parámetro opcional sin default; con parámetros explícitos produjo una imagen del viewport de edición a pose de captura, no una prueba del POV de PIE. CaptureEditorImage no pudo capturar ventanas. No se usó automatización de UI ni se interpretaron esas capturas como aprobación visual. Queda la observación manual de la guía.
- PIE detenido por MCP y confirmado IsPIERunning=false. No se ejecutaron ni agregaron tests automatizados. Pendiente: [observación jugable](editor.md#4-observación-jugable) y calibración conjunta por el usuario.
