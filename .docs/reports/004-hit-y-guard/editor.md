# 004 — Preparar un ataque y una guardia

**Configuración del golpe reemplazada:** [006](../006-golpe-de-puno/editor.md) usa un socket del puño y una esfera. Ya no seguir el paso de arma de esta guía.

**Continuación:** la [guía 005](../005-ia-basica/editor.md) conserva esta base y agrega IA básica con F1. Reemplaza la indicación de eliminar el controlador y las pruebas con G/H del paso 5. Seguir 005 para configurar el estado actual.

**Pendiente para el usuario:** compilar en frío, revisar Blueprints, asignar arma y un montage de un solo ataque. No hubo operaciones de editor realizadas por Codex en esta iteración. Los assets anteriores no fueron editados.

[Reporte](reporte.md) · [Flujo del código](../../combat/technical.md#cómo-leer-el-código).

## 1. Compilar y limpiar referencias

**Pendiente — usuario.**

1. Cerrar PIE y Unreal Editor. En Rider/Visual Studio, compilar `vampsEditor / Development Editor / Win64` para `C:\dev\vamps\vamps.uproject`. Regenerar archivos del proyecto si los tipos nuevos no aparecen.
2. Reabrir con UE 5.8. Esta iteración elimina clases reflejadas y propiedades; no basar la migración en un parche de Live Coding.
3. Abrir `/Game/Duel/Blueprints/BP_DuelCharacter`. En Pawn, fijar Auto Possess AI = Disabled y limpiar AI Controller Class si conserva DuelAIController. Revisar Weapon y WeaponTrace heredados.
4. Compilar BP_DuelCharacter, BP_DuelPlayerController, BP_DuelGameMode y `/Game/Duel/Animations/ABP_Duel`. Retirar nodos que dependan de Phase, Result, StrikeIndex, NA, CombatConfig o funciones eliminadas. `RequestGuard` ahora recibe un bool: true para cubrirse, false para dejar de hacerlo.
5. BP_DuelGameMode conserva Default Pawn Class/Opponent Class = BP_DuelCharacter, Player Controller Class = BP_DuelPlayerController y HUD Class = DuelHUD. Ya no usa SharedConfig.
6. `/Game/Duel/Data/DA_DuelCombat` queda obsoleto: sus valores ya no configuran el combate y su clase fue eliminada. Retirar referencias restantes con Reference Viewer antes de borrarlo manualmente. No recrear un Data Asset para esta base.

## 2. Arma

**Pendiente — usuario.**

1. Elegir un Static Mesh de arma propio o una copia de un recurso visual; asignarlo a `BP_DuelCharacter > Weapon`, adjunto a Mesh/hand_r. Ajustar su transform para alinearlo con la mano.
2. En el Static Mesh Editor, Socket Manager, crear `TraceBase` y `TraceTip` sobre comienzo y punta de la hoja. Guardar.
3. Weapon mantiene Collision Enabled = NoCollision. WeaponTrace usa Base Socket = TraceBase, Tip Socket = TraceTip y Radius = 8 cm como valor inicial.
4. Los nombres y el radio ahora se ajustan en WeaponTrace, no en DA_DuelCombat. No se eligió ni creó una mesh de arma en esta iteración; sus posiciones exactas dependen del recurso visual.

## 3. Un solo montage

**Pendiente — usuario.**

1. Crear `/Game/Duel/Animations/AM_BasicAttack` con el skeleton del personaje y **solo** `/Game/Duel/Animations/AS_DuelAttack1` como segmento. No usar las tres animaciones seguidas.
2. Slot = DefaultGroup.DefaultSlot; Play Rate/Rate Scale = 1; Loop Count = 1. Conservar una única sección Default sin loop. No hacen falta Prepare/Active/Recovery ni enlaces entre secciones.
3. En la pista de Notifies **del montage**, colocar un único Notify State `Attack Trace` sobre el tramo donde el arma debe golpear. No usar Duel Combat Window ni Combo.
4. Confirmar Branching Point, Notify Trigger Chance = 1, sin filtro LOD y Trigger on Dedicated Server habilitado. Dejar el notify antes del blend-out. Las secuencias visuales no deben ejecutar lógica del ejemplo.
5. En BP_DuelCharacter, asignar Attack Montage = AM_BasicAttack y Attack Damage = 12. No hay más parámetros de fases o ventanas en el personaje.
6. En ABP_Duel conservar el slot DefaultGroup.DefaultSlot antes de la salida. Verificar que el montage efectivamente se reproduce y la hoja sigue la mano. Guardar.

Resultado: LMB reproduce un golpe completo. Otro click durante la habilidad no encadena ni se almacena. Al ser bloqueado, se cierra la detección inmediatamente y GAS cancela la animación con su blend-out.

## 4. Guardia, input y HUD

**Pendiente — usuario.**

1. En BP_DuelPlayerController conservar las cuatro referencias Move, Look, Lock y Reset junto con IMC_Duel.
2. Attack/Guard se crean por código: LMB y RMB. No necesitan assets nuevos. En IMC_Duel se pueden retirar IA_DuelAIMode y IA_DuelShared. **Conservar IA_DuelLock/Tab.**
3. Mantener RMB activa guardia; soltarlo la cancela. No se configura Guard Montage ni un notify defensivo. Si se pulsa guardia durante el ataque, la solicitud se rechaza: soltar y volver a presionar después.
4. El HUD muestra IDLE/ATTACK/GUARD y vida de los dos personajes. Es suficiente para observar la defensa aunque aún no haya una pose visual.
5. Opcionalmente, en ABP_Duel usar bIsGuarding para mezclar una pose de guardia antes del Slot. Esa pose no cambia la regla ni agrega una ventana de gameplay.

## 5. Observar las dos reglas manualmente

**Pendiente — usuario.**

Abrir `/Game/Duel/Maps/L_Duel` y jugar localmente. El rival ya no tiene IA.

Para solicitar sus acciones a mano, se pueden conectar temporalmente dos teclas en el Level Blueprint del mapa: `G Pressed/Released → Get Game Mode → Cast to DuelGameMode → Opponent → RequestGuard(true/false)`; `H Pressed → ... → Opponent → RequestAttack`. Son entradas manuales para el segundo personaje, no una secuencia autónoma. Esos nodos no fueron creados por Codex.

1. Sin G, acercarse y pulsar LMB. El HUD del rival debe perder 12 de vida una sola vez por ataque.
2. Mantener G y pulsar LMB: el rival sigue en GUARD, su vida no baja y la animación del jugador se cancela al contacto. Dejar pasar tiempo no debe producir daño residual.
3. Soltar G: el siguiente ataque vuelve a restar vida.
4. Mantener RMB y pulsar H: la misma regla debe proteger al jugador y cancelar la animación del rival.
5. Sin RMB, pulsar H: el jugador recibe daño, sin empuje, reacción ni cooldown.
6. Comprobar Tab y seguimiento de cámara; WASD sigue funcionando. R restaura vida/posiciones y deja a ambos en IDLE.
7. Pulsar LMB repetidamente no debe crear combos. Mantener guardia permite bloquear varios ataques; no hace falta renovar cobertura.
8. Tras R, soltar y volver a presionar cualquier botón que estuviera mantenido. No hay cola de inputs ni restauración automática de guardia.

No hay muerte ni victoria/derrota al llegar a cero; solo vida limitada a su rango. No se solicita crear ni ejecutar tests automatizados.
