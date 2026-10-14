# 009 — Preparar los personajes nuevos

El receptor colocado y el GameMode del paso 5 quedan reemplazados por [011 — IA y GameMode de prueba](../011-ia-combate-prueba/editor.md). El rival lo crea el modo de prueba; los assets de personaje, mesh y combate de esta guía se conservan.

La asignación de HUD nativo del paso 5 queda reemplazada por [010 — HUD de prueba](../010-hud-prueba/editor.md): BP_VampsGameMode ya usa DuelHUD, que permite observar vida y estados durante las comprobaciones del paso 6.

**Pendiente para el usuario:** revisar el notify Attack Trace de AM_BasicAttack y comprobar movimiento, mirada, ataque, daño y guardia en PIE. Los diez assets nuevos están creados, configurados y guardados; los cuatro Blueprints compilaron. El editor quedó en /Game/Vamps/Maps/L_CharacterBasic, con PIE detenido. El duelo anterior queda como referencia.

[Reporte](reporte.md).

## 1. Compilación y conexión

**Hecho — usuario:** confirmó la compilación del proyecto tras corregir el shadowing y después de renombrar el controller a VampsPlayerController.

**Hecho — Codex por MCP:** conexión al servidor de .codex/config.toml, en http://127.0.0.1:8000/mcp. Se descubrieron list_toolsets, describe_toolset y call_tool y los esquemas de las operaciones utilizadas. El editor resuelve las clases /Script/vamps.VampsCharacter, VampsCharacter_Player y VampsPlayerController.

No hubo cambios nuevos de C++ ni necesidad de Live Coding en esta etapa. La creación/configuración de assets y el inicio/cierre de PIE se realizaron con herramientas directas, sin automatizar la interfaz.

## 2. Inputs y controller

**Hecho — Codex por MCP.** Herramientas: DataAssetTools.create, ObjectTools.list_properties/get_properties/set_properties, BlueprintTools.create/compile_blueprint y AssetTools.save_assets/is_dirty.

En /Game/Vamps/Input están IA_Move e IA_Look con Value Type = Axis2D, IA_Attack e IA_Guard con Boolean, y el contexto IMC_Vamps. Todas las acciones y mappings tienen Triggers vacíos.

Para reproducir: Content Browser → Input → crear las cuatro Input Actions y un Input Mapping Context. En IMC_Vamps, configurar:

| Acción | Tecla | Modifiers del mapping |
|---|---|---|
| IA_Move | D | Ninguno |
| IA_Move | A | Negate: X habilitado, Y/Z deshabilitados |
| IA_Move | W | Swizzle Input Axis Values, Order = YXZ |
| IA_Move | S | Negate solo X, después Swizzle YXZ |
| IA_Look | Mouse XY 2D-Axis | Negate solo Y |
| IA_Attack | Left Mouse Button | Ninguno |
| IA_Guard | Right Mouse Button | Ninguno |

Los modificadores son instancias propias del contexto. Se verificaron sus referencias, ejes y orden después de escribirlos. No se utilizan Tab, R, F1 ni una acción compartida entre ataque y guardia.

En /Game/Vamps/Blueprints/BP_VampsPlayerController, Parent Class = VampsPlayerController. Class Defaults → Input:

- Default Input Mapping Context = IMC_Vamps.
- Move Action = IA_Move; Look Action = IA_Look.
- Attack Action = IA_Attack; Guard Action = IA_Guard.

Compilar y guardar. No agregar bindings en los personajes ni en los Event Graphs. El controller nativo gestiona Started de ataque/guardia y Completed/Canceled de guardia.

## 3. Personajes y recursos visuales

**Hecho — Codex por MCP.** Herramientas: BlueprintTools.create/get_parent/get_default_object/compile_blueprint, ActorTools.get_components, ObjectTools y AssetTools.save_assets.

Assets y padres:

| Blueprint | Parent Class |
|---|---|
| /Game/Vamps/Blueprints/BP_VampsCharacter | VampsCharacter |
| /Game/Vamps/Blueprints/BP_VampsCharacter_Player | VampsCharacter_Player |

Ambos usan Mesh → Skeletal Mesh = /Game/Characters/Mannequins/Meshes/SKM_Manny_Simple, Animation Mode = Use Animation Blueprint y Anim Class = /Game/Duel/Animations/ABP_Duel.ABP_Duel_C. Transform del mesh: Z = -88 cm, Yaw = -90°. Class Defaults: Attack Montage = /Game/Duel/Animations/AM_BasicAttack y Attack Damage = 12; Auto Possess AI = Disabled.

ABP_Duel es propio y su padre comprobado es DuelAnimInstance. Su AnimGraph conecta BS_Duel, alimentado por Speed, a DefaultSlot y Output Pose. El EventGraph contiene Blueprint Update Animation y Try Get Pawn Owner; no se encontraron casts al personaje viejo. No se agrega una pose visual de guardia en esta etapa.

La base tiene cápsula, mesh, CharacterMovement, ASC y FistHitbox. Solo el jugador añade CameraBoom y FollowCamera: brazo = 400 cm, Use Pawn Control Rotation habilitado en el brazo y deshabilitado en FollowCamera. Movimiento: Max Walk Speed = 420 y Orient Rotation to Movement habilitado. Las asignaciones se verificaron también sobre los actores de PIE.

## 4. Socket y montage

**Hecho — Codex por MCP (verificación):** SkeletalMeshTools.get_skeleton/get_socket_names/get_socket_bone/get_socket_transform confirma que Manny Simple y AM_BasicAttack comparten /Game/Characters/Mannequins/Meshes/SK_Mannequin. FistHit ya existía sobre middle_metacarpal_r, con transform local identidad; no se creó ni modificó. FistHitbox usa Socket Name = FistHit, Sphere Radius = 12 cm, NoCollision y Generate Overlap Events deshabilitado.

**Hecho — Codex por MCP (verificación parcial):** ObjectTools confirma que AM_BasicAttack contiene un único segmento AS_DuelAttack1, de 0 a 1 segundo, Rate = 1, Loop Count = 1, Slot = DefaultSlot y Auto Blend Out habilitado.

**Pendiente — usuario:** revisar el tramo de contacto y el notify. No hay una operación directa de montage/notifies en los toolsets descubiertos. ObjectTools.get_properties rechazó la lectura de Notifies; no se asume que el notify esté configurado.

1. Abrir /Game/Duel/Animations/AM_BasicAttack y previsualizar AS_DuelAttack1. Confirmar que es el golpe deseado con la mano derecha.
2. En la pista de notifies del montage, verificar o colocar un único Notify State **Attack Trace** desde que el puño puede impactar hasta que termina ese tramo, antes del blend-out. Los tiempos dependen de la animación y no fueron inferidos.
3. Confirmar Montage Tick Type = Branching Point, Notify Trigger Chance = 1, sin filtro LOD y Trigger on Dedicated Server habilitado. Retirar cualquier notify antiguo que ejecute otra lógica de combate.
4. Revisar visualmente el centro de FistHit durante el golpe. Ajustarlo en Skeleton Tree solo si no cubre el puño. Guardar Skeleton y montage si se modifican.

## 5. Escena separada

**Hecho — Codex por MCP.** Herramientas: BlueprintTools.create/compile_blueprint, AssetTools.duplicate/save_assets, SceneTools.load_level/add_to_scene_from_asset/find_actors, ObjectTools y ActorTools.get_actor_transform.

Para reproducir:

1. Crear /Game/Vamps/Blueprints/BP_VampsGameMode desde GameModeBase. Class Defaults: Default Pawn Class = BP_VampsCharacter_Player, Player Controller Class = BP_VampsPlayerController y HUD Class = HUD nativo. Compilar y guardar.
2. Duplicar el mapa propio /Game/Duel/Maps/L_Duel a /Game/Vamps/Maps/L_CharacterBasic y abrir la copia. Solo se reutilizan suelo, iluminación y actores nativos; no se copian personajes ni lógica del ejemplo.
3. World Settings → GameMode Override = BP_VampsGameMode. PlayerStart conserva Location = (-220, 0, 100) y Rotation = (0, 0, 0).
4. Colocar BP_VampsCharacter con nombre VampsTarget, Location inicial = (220, 0, 100), Yaw = 180° y ajustar al suelo. Resultado verificado: Z ≈ 88,964 cm. Dejar Auto Possess Player deshabilitado y Auto Possess AI = Disabled. El suelo conserva colisión QueryAndPhysics y perfil BlockAllDynamic.
5. Guardar el mapa. No se cambian el mapa ni el GameMode predeterminados del proyecto, y L_Duel permanece intacto.

## 6. Verificación en PIE

**Hecho — Codex por MCP (alcance limitado):** EditorAppToolset.StartPIE inició la escena. SceneTools encontró el jugador BP_VampsCharacter_Player, el receptor BP_VampsCharacter y BP_VampsPlayerController. AbilitySystemInspectorToolset confirmó en ambos GA_Attack/GA_Guard a nivel 1, inactivas, sin tags activos y Health/MaxHealth = 100/100. Se comprobaron mesh, cámara, movimiento y hitbox. LogsToolset no devolvió errores ni warnings de LogPlayLevel. StopPIE terminó la sesión e IsPIERunning confirmó false. Los diez assets tienen is_dirty = false.

**Pendiente — usuario:** interacción y combate. No hay una herramienta directa para entregar estos inputs de juego; no se simulan mediante UI ni se añade un arnés.

1. Ejecutar Play en L_CharacterBasic con un jugador local y foco en el viewport. WASD debe mover respecto al yaw de cámara; el mouse debe girar la vista. La cápsula gira hacia el desplazamiento, sin pitch/roll de cámara.
2. Acercarse al receptor y pulsar LMB: un único ataque debe quitar 12 de vida una sola vez. Mantener LMB no encadena golpes. Fuera de alcance, detrás de una pared que bloquee Visibility o fuera de Attack Trace, no debe haber daño. Inspeccionar Attributes → Health del receptor durante PIE.
3. Mantener RMB debe activar guardia; soltar debe terminarla. LMB durante guardia no activa ataque. RMB durante ataque no lo interrumpe ni se encola; volver a pulsarlo cuando termine.
4. Para observar un receptor cubierto, detener PIE y configurar temporalmente en BP_VampsCharacter: Event BeginPlay → RequestGuard con Pressed = true. Compilar y ejecutar PIE. Golpearlo no debe quitar vida y debe cancelar el montage del atacante. Detener PIE y retirar esa llamada para devolver el receptor a reposo.
5. Comprobar pérdida de foco durante guardia y detener/reiniciar PIE durante ataque/guardia. Las acciones anteriores no deben persistir. Si se cambia la posesión mediante lógica propia de Blueprint, el pawn anterior debe cancelar sus acciones y el nuevo recibir las entradas del controller.

No se ha confirmado todavía el recorrido input → montage → notify → contacto → daño/bloqueo. La captura de viewport durante PIE falló; no se entrega una captura ni se cuenta como verificación visual.
