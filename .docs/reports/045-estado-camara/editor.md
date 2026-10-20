# Editor — Estado de cámara

Seguimiento libre actualizado en [046 — Giro progresivo sin lock-on](../046-giro-libre-camara/editor.md). Usar su guía para observar el desplazamiento sostenido sin target.

**Para el usuario:** no hace falta recrear los assets ni repetir el arranque por el fallo histórico de 044: el editor actual inicia Play con la cámara propia. Queda confirmar la observación visual integrada del apartado 3 y calibrar si corresponde. El HUD del paso 5 continúa para otro trabajo. PIE quedó detenido.

## 1. Estado recibido y configuración

**Hecho — usuario, alcance informado.** El usuario indica que realizó la cámara en otra PC. No se marca como aprobada toda la observación jugable por esa indicación general.

**Hecho — Codex por MCP.** En Content Browser, revisar `/Game/Vamps/Blueprints/BP_VampsCombatTestGameMode`: Default Pawn Class=BP_VampsCharacter_Player, Player Controller Class=BP_VampsCombatTestPlayerController, HUD Class=VampsCombatTestHUD. Ambos controllers de jugador tienen Player Camera Manager Class=BP_VampsPlayerCameraManager. ObjectTools y BlueprintTools confirmaron referencias y padres propios.

**Hecho — Codex por MCP.** En BP_VampsCharacter_Player, CameraBoom tiene Do Collision Test, Use Pawn Control Rotation y Auto Activate deshabilitados; FollowCamera tiene Auto Activate deshabilitado. ObjectTools confirmó los valores.

**Hecho — Codex por MCP.** En `/Game/Vamps/Materials/M_CameraOccluder`, MaterialTools/ObjectTools confirmaron Scalar VampsCameraFade=0 con Custom Primitive Data habilitado/índice 0, conectado a OneMinus y Opacity Mask. Material Masked, Two Sided y Dither Opacity Mask activos, corte 0,5.

**Hecho — Codex por MCP.** En `/Game/Vamps/Maps/L_CharacterBasic`, revisar las tres paredes descritas en el [reporte](reporte.md#configuración-y-arranque-comprobados). SceneTools/ObjectTools confirmaron ubicación, material, tags y colisión. AssetTools.is_dirty confirmó mapa/material/BP jugador/BP manager sin cambios pendientes; no se guardaron assets.

## 2. Arranque actual y F7

**Hecho — Codex por MCP.** EditorAppToolset.IsPIERunning confirmó inicialmente false. StartPIE inició L_CharacterBasic. SceneTools/ObjectTools confirmaron el controller de pruebas, manager propio y jugador vivo en Idle con encuadre de seguimiento. LogsToolset no mostró nuevos errores/warnings de combate en esa sesión. StopPIE y posterior IsPIERunning=false confirmaron el cierre. Para reproducirlo, abrir ese mapa e iniciar/detener Play.

**Hecho — Codex por MCP, verificación parcial.** LogsToolset encontró alternancias F7 disabled/enabled anteriores a esta revisión en el log actual. Confirman ejecución del binding de 044. No se pulsaron teclas por herramientas ni se atribuye a Codex esa interacción. La compilación completa original no fue ejecutada ni acreditada mediante un resultado de build en esta revisión; sí se confirmó la carga de la DLL principal y el arranque actual.

## 3. Validación visual integrada

**Pendiente — usuario.** En L_CharacterBasic, iniciar Play y dar foco al juego:

1. Con selección normal, comprobar lateral, parada/inversión, diagonales, sprint y dash. La cámara mantiene continuidad y el dash conserva su dirección capturada.
2. Pulsar F7 sin modificadores: LOCK ON: None y log disabled. Observar acompañamiento suave sin órbita continua al mantener A/D y retorno suave al detenerse. F y guardia no recuperan selección durante ese modo. Pulsar F7 nuevamente: log enabled y readquisición. La IA continúa activa.
3. Iniciar un ataque aceptado y continuar la cadena: plano cercano hasta NA propia. Bloquear efectivamente: mantenerlo hasta NA del agresor. Daño abierto y solicitudes rechazadas no deben solicitar otro plano. Verificar cancelación/muerte y cambios de target por F/guardia.
4. CameraFadeWall debe desvanecerse si oculta al jugador o al rival real y restaurarse al salir; sigue bloqueando movimiento. Cerca de Y=500, CameraSolidWall debe retraer la cámara. Cerca de Y=−500, CameraInvisibleBoundary limita el movimiento sin tapar la vista.
5. Pulsar R durante encuadre/fade/transición y con F7 activado/desactivado: limpia la cámara y conserva el modo de depuración. Detener Play y comenzar otra sesión restablece el lock-on normal y restaura los recursos de oclusión.
6. Comparar distintas tasas de render y ajustar los valores globales `vamps.Camera.*` si hace falta, según [042](../042-camara-combate/editor.md#3-compilación-y-ajuste-de-desarrollo). Los cambios de consola son temporales. Confirmar los resultados para cerrar la parte visual de cámara.

La guía de [044](../044-seguimiento-libre-f7/editor.md#3-observar-la-cámara) detalla el seguimiento libre; la observación completa de combate/oclusión sigue en [042](../042-camara-combate/editor.md#4-observación-jugable), aplicando el seguimiento libre vigente de 044. Las herramientas directas no cubren estas pulsaciones/valoración visual; el paso es manual según AGENTS.md.
