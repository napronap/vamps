# 005 — Preparar y probar la IA básica

**Configuración del golpe reemplazada:** [006](../006-golpe-de-puno/editor.md) usa una esfera sobre el puño y distancia IA inicial de 100 cm. Los controles y modos se conservan; seguir 006 para preparar el contacto y la animación.

**Pendiente para el usuario:** compilar con el editor cerrado, completar los assets del combate mínimo si todavía faltan y comprobar los modos con F1. Codex modificó código y documentación; no ejecutó operaciones de editor ni modificó assets. No hay pasos manuales confirmados como hechos por el usuario.

[Reporte](reporte.md) · [Controles y modos actuales del prototipo](../../prototype/functional.md).

## 1. Compilar

**Pendiente — usuario.**

1. Cerrar PIE y Unreal Editor.
2. En Rider/Visual Studio, compilar `vampsEditor / Development Editor / Win64` para `C:\dev\vamps\vamps.uproject`. Regenerar los archivos del proyecto si no aparecen los tipos nuevos.
3. Reabrir con UE 5.8. Esta actualización reintroduce una clase y un enum reflejados, además de las eliminaciones previas: hacer compilación completa, sin depender de Live Coding.

No hay herramientas MCP directas del editor expuestas en esta sesión; la compilación y su resultado no están verificados.

## 2. Revisar clases e input

**Pendiente — usuario.**

1. En `/Game/Duel/Blueprints/BP_DuelCharacter`, confirmar padre VampsDuelCharacter y Auto Possess AI = Disabled. No hace falta asignar un Blueprint de AIController: el GameMode establece DuelAIController y llama SpawnDefaultController únicamente para el rival.
2. En `/Game/Duel/Blueprints/BP_DuelGameMode`, confirmar Default Pawn Class y Opponent Class = BP_DuelCharacter, Player Controller Class = BP_DuelPlayerController y HUD Class = DuelHUD.
3. En `/Game/Duel/Blueprints/BP_DuelPlayerController`, mantener las referencias Move, Look, Lock y Reset, y Duel Mapping = `/Game/Duel/Input/IMC_Duel`. En ese context conservar WASD, mouse, Tab y R asociados a sus acciones originales.
4. LMB, RMB y F1 se crean y mapean por código. No necesitan assets nuevos ni una referencia AIModeAction en el Blueprint. IA_DuelShared e IA_DuelAIMode antiguos pueden retirarse de IMC_Duel; conservar IA_DuelLock.
5. Si se agregaron las teclas G/H en el Level Blueprint de `/Game/Duel/Maps/L_Duel` siguiendo 004, desconectar esos eventos. Ahora F1 controla las acciones del rival.
6. Compilar y guardar los Blueprints. Retirar cualquier uso de las antiguas opciones Combo/Reactive o de propiedades retiradas como CombatConfig, Phase y Result.

## 3. Completar el combate mínimo

**Pendiente — usuario, si no se realizó ya.**

Seguir los pasos de [arma](../004-hit-y-guard/editor.md#2-arma), [montage único](../004-hit-y-guard/editor.md#3-un-solo-montage) y [guardia/input/HUD](../004-hit-y-guard/editor.md#4-guardia-input-y-hud) de 004. La IA no reemplaza esa preparación:

- Weapon debe tener una mesh con sockets TraceBase y TraceTip, adjunta a hand_r.
- Attack Montage debe apuntar a `/Game/Duel/Animations/AM_BasicAttack`: un clip, un golpe, un Notify State Attack Trace y DefaultGroup.DefaultSlot disponible en ABP_Duel.
- Attack Damage = 12. Guard funciona con el tag GAS; una pose visual de guardia es opcional.
- Retirar referencias al Data Asset obsoleto `/Game/Duel/Data/DA_DuelCombat` como indica el paso 1 de 004. No eliminar el nuevo controlador de IA ni seguir las pruebas G/H del paso 5 de esa guía.

No se requiere NavMesh, Behavior Tree, State Tree ni otro asset de IA. El controlador usado es la clase nativa; AttackDistance (150 cm) y AttackInterval (1 s) se ajustan por ahora en `Source/vamps/Public/AI/DuelAIController.h` y requieren compilar.

## 4. Comprobaciones manuales

**Pendiente — usuario.**

Abrir `/Game/Duel/Maps/L_Duel`, verificar GameMode Override = BP_DuelGameMode en World Settings y comenzar Play con un jugador local. Dar foco al viewport.

1. **Passive:** el HUD debe mostrar AI: Passive, ambas vidas y estados. El rival permanece quieto; acercarse y atacar debe restarle 12 una sola vez por golpe.
2. **F1 → Attack:** tras la espera inicial de 0,75 s, el rival se acerca y repite ataques individuales. Mantener RMB: el jugador conserva vida y la animación del rival se cancela al contacto. Soltar RMB: el siguiente contacto resta vida.
3. **F1 → Guard:** el rival cancela su ataque y mantiene guardia. Acercarse y atacar: se cancela la animación del jugador y el rival conserva vida. No hay parry ni límite de duración de guardia.
4. **F1 → Sidestep:** el rival deja de cubrirse y se mueve lateralmente alrededor del jugador. No ataca; puede recibir daño.
5. **F1 → Mixed:** alterna Attack, Guard y Sidestep cada cuatro segundos. Sus resultados deben ser los mismos que en los modos individuales.
6. **F1 → Passive:** cesan movimiento, ataque y guardia. No debe quedar una traza de ataque activa ni daño residual.
7. **R en cualquier modo:** restaura posiciones y vida y cancela las acciones actuales. El modo del HUD se conserva; el rival retoma su comportamiento tras 0,75 s. Mixed comienza nuevamente por Attack. Soltar y volver a presionar RMB si estaba mantenido al reiniciar.
8. **WASD, mouse y Tab:** comprobar movimiento, cámara libre sin lock-on y seguimiento con lock-on. La IA debe continuar independientemente de que el jugador libere su objetivo.

La distancia de ataque es una aproximación inicial. Si el rival reproduce el golpe sin alcanzar al jugador, revisar arma, sockets y notify; si el recorrido es correcto pero corto, reducir AttackDistance. La IA camina en línea recta y puede quedar contra obstáculos. Vida cero no finaliza el duelo; usar R para seguir observando.

Si el HUD muestra AI: Unavailable, revisar que el mapa use BP_DuelGameMode y que Auto Possess AI esté Disabled antes de iniciar PIE. Si F1 no responde, verificar foco del viewport y que se use BP_DuelPlayerController. No agregar lógica alternativa al Level Blueprint para compensarlo.
