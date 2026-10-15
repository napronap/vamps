# 013 — Configuración recuperada

Continuación: [014 — HUD, efecto y tags Vamps](../014-copias-nomenclatura-vamps/editor.md). En el modo de prueba, usar VampsCombatTestHUD; AM_BasicAttack ahora referencia AS_BasicAttack1.

**Queda para el usuario:** observar idle/movimiento/ataque y comprobar inputs en PIE. No hace falta reconstruir ni reasignar estos assets: las operaciones MCP terminaron, están guardados y PIE quedó detenido. El daño por contacto sigue sin confirmar; revisar el notify del montage si los golpes no quitan vida.

[Reporte](reporte.md) · [Controles y parámetros de combate](../011-ia-combate-prueba/editor.md).

## 1. Preparación y rutas

**Hecho — usuario.** Regenerar BP_VampsCharacter_Player sobre VampsCharacter_Player y trasladar los recursos a `/Game/Vamps/Animations`. Los nombres actuales son ABP_VampsCharacter, BS_VampsCharacter y AM_BasicAttack. El editor volvió a estar disponible para MCP.

**Hecho — Codex por MCP.** Consultar registro y padres únicamente en Vamps. No abrir, reparentar, mover, borrar ni guardar assets Duel. No hay cambios de código que requieran otra compilación nativa en esta iteración.

## 2. Reconstruir la animación

**Hecho — Codex por MCP.** ObjectTools.set_properties/get_properties y AssetTools.save_assets para BS_VampsCharacter:

- Skeleton = SK_Mannequin, conservado y consultado.
- Eje X Speed: Min 0, Max 420, Grid Divisions 4, sin Wrap ni Snap; ejes restantes sin uso.
- Samples: MM_Idle (0,0), MF_Unarmed_Walk_Fwd (180,0), MF_Unarmed_Jog_Fwd (420,0). Recursos en Characters/Mannequins/Anims/Unarmed. Rate Scale 1, Loop habilitado.

Para reproducirlo, abrir BS_VampsCharacter y revisar Axis Settings y samples; guardar. El esqueleto no se reasignó: la herramienta rechazó escribir esa propiedad, que ya tenía el valor correcto.

**Hecho — Codex por MCP.** BlueprintTools.create_node/connect_pins/delete_node/compile_blueprint y AssetTools.save_assets para ABP_VampsCharacter. Class Settings → Parent Class = VampsAnimInstance, Target Skeleton = SK_Mannequin. AnimGraph:

1. Get Speed de VampsAnimInstance → X del BlendSpace Player BS_VampsCharacter.
2. Pose del BlendSpace → Source de Slot DefaultSlot.
3. Pose del Slot → Result de Output Pose.

BlendSpace Player: Play Rate 1 y Loop habilitado. EventGraph vacío. Compilar y guardar. No añadir lógica del ejemplo Third Person.

## 3. Personajes

**Hecho — Codex por MCP.** ObjectTools, BlueprintTools y AssetTools. En BP_VampsCharacter y BP_VampsCharacter_Player:

- Mesh: SKM_Manny_Simple; Use Animation Blueprint; Anim Class ABP_VampsCharacter_C.
- Mesh transform: Location (0,0,-88), Rotation (0,-90,0); colisión NoCollision; AlwaysTickPoseAndRefreshBones, Update Rate Optimizations deshabilitado.
- Combat: Attack Montage = `/Game/Vamps/Animations/AM_BasicAttack`, Attack Damage = 12.
- Auto Possess Player/AI = Disabled. El GameMode y controllers realizan la posesión.

Verificados: cápsula Radius 34/Half Height 88, Max Walk Speed 420, Orient Rotation to Movement habilitado, Controller Rotation Yaw deshabilitado. En el jugador: CameraBoom 400, Use Pawn Control Rotation habilitado; FollowCamera lo tiene deshabilitado. Compilar y guardar ambos Blueprints.

## 4. GameModes, controllers y mapa

**Hecho — Codex por MCP.** Restaurar Default Pawn Class = BP_VampsCharacter_Player en ambos GameModes; confirmar las demás conexiones y compilar/guardar los siete Blueprints de Vamps.

| Configuración | Modo normal | Modo de prueba |
|---|---|---|
| GameMode | BP_VampsGameMode | BP_VampsCombatTestGameMode |
| Controller | BP_VampsPlayerController | BP_VampsCombatTestPlayerController |
| HUD | HUD nativo | DuelHUD nativo |
| Rival / IA | Sin rival | BP_VampsCharacter / BP_VampsAIController |
| Contexto de inputs | IMC_Vamps | IMC_VampsCombatTest |

Test GameMode: Initial AI Mode = Passive, Player Spawn (-220,0,100)/Yaw 0, Opponent Spawn (220,0,100)/Yaw 180. IA: Attack Distance 100, Attack Interval 1, Mixed Mode Duration 4. Actions y mappings siguen según 011: WASD, Mouse2D, LMB/RMB y solo en prueba F6/R.

World Settings de L_CharacterBasic → GameMode Override = BP_VampsCombatTestGameMode. Guardar el mapa. Project Settings → Maps & Modes ya conservaba startup/default map L_CharacterBasic y modo global BP_VampsGameMode; no se modificó.

## 5. Comprobaciones

**Actualización de estado:** contacto/daño del montage y observación básica del escenario quedaron confirmados por el usuario al cerrar la etapa 0 el 15 de octubre de 2026; [registro en 020](../020-roadmap-emulacion-fgoa/reporte.md#cierre-de-la-etapa-0). Las indicaciones siguientes conservan el setup histórico de 013; cámara/input actuales se consultan en [019](../019-camara-lockon/editor.md).

**Hecho — Codex por MCP, inspección de estado.** EditorAppToolset inició y detuvo PIE para Passive, Attack temporal y modo normal temporal. SceneTools, ActorTools, ObjectTools y AbilitySystemInspectorToolset confirmaron personajes, componentes, habilidades y referencias. LogsToolset no encontró errores/warnings de LogPlayLevel. AssetTools confirmó todo Vamps guardado y sin dependencias a assets Duel. Passive y override de prueba restaurados.

**Pendiente — usuario, comprobación visual e inputs.** Play en L_CharacterBasic: verificar idle, WASD y cámara; LMB reproduce el golpe y RMB mantiene guardia. F6 cambia los modos sin wireframe y R reinicia el combate. El graph conserva la locomoción anterior; todavía no contiene una pose de guardia.

Si el ataque no quita vida, abrir únicamente `/Game/Vamps/Animations/AM_BasicAttack`. Revisar Notify State Attack Trace durante el contacto del puño, antes del blend-out, Branching Point y Trigger Chance 1. El MCP disponible no expone esa operación. Ajustar el intervalo observando el golpe; no se inventan tiempos de contacto. No abrir assets Duel para esta revisión.

## 6. Consulta sobre retirar Duel

**Hecho — Codex por MCP y revisión de archivos.** Dependencias de los 22 assets Vamps y referencias de código/configuración revisadas, sin cargar assets Duel. No se realizó ni se autorizó un borrado en esta consulta. Los recursos aún activos con ese nombre figuran en el [reporte](reporte.md#revisión-posterior-qué-sigue-usando-el-nombre-duel); no borrar todos los archivos por coincidencia con Duel.
