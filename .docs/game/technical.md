# Base técnica del juego

Se usa Unreal Engine 5.8 y GAS. Comportamiento: [juego y partida](functional.md). Núcleo: [combate](../combat/technical.md).

## Código y recursos

Las clases y Blueprints propios heredan de clases nativas de Unreal. No se copia ni hereda lógica del ejemplo Third Person ni de sus variantes; solo se pueden reutilizar recursos visuales.

`AVampsDuelCharacter` contiene AbilitySystemComponent, AttributeSet, hitbox del puño y cámara. `ADuelPlayerController` conserva input, lock-on y reinicio. `ADuelGameMode` crea un segundo personaje, le asigna `ADuelAIController` mediante SpawnDefaultController y restaura ambas posiciones/vidas. AutoPossessAI permanece Disabled en el personaje: solo el rival recibe el controlador explícitamente. No hay Data Asset de combate.

`ADuelHUD` muestra vida, estado calculado desde GAS y modo del controlador de IA. `UDuelAnimInstance` expone Speed, Direction, bInAir y bIsGuarding al AnimBP.

El controller mantiene las cuatro Input Actions existentes para Move, Look, Lock y Reset. Crea Attack, Guard y AIMode y su Mapping Context en memoria: Started solicita la acción; Completed/Canceled de Guard la termina. F1 llama CycleAI y cambia el modo del rival. No necesita un asset IA_DuelAIMode ni input contextual.

Los assets anteriores requieren migración después de compilar. El [reporte 004](../reports/004-hit-y-guard/reporte.md) identifica la simplificación del combate; la [guía 006](../reports/006-golpe-de-puno/editor.md) prepara el golpe de puño y enlaza la configuración de IA.

## IA para observar el combate

`ADuelAIController` solo solicita acciones a su personaje: RequestAttack, RequestGuard y RequestMove. Nunca aplica daño ni decide si un contacto fue bloqueado. `EDuelAIMode` selecciona un comportamiento de prueba; no representa fases del combate.

Un timer llama Decide cada 0,1 segundos. Decide selecciona entre ApproachAndAttack, HoldGuard y Sidestep; cada método tiene sus condiciones. Mixed selecciona esos tres comportamientos en orden cada cuatro segundos. AttackDistance = 100 cm y AttackInterval = 1 segundo son los valores iniciales del controlador nativo.

Tick únicamente entrega el vector de movimiento a CharacterMovement mientras hay desplazamiento. AddMovementInput se consume cada frame; por eso esa entrega continua no usa el timer de decisiones. No hay lógica de ataque ni guardia en Tick. La orientación sigue usando el lock-on del personaje.

ResetBehavior cancela las acciones anteriores, limpia movimiento y vuelve a programar el timer con una espera de 0,75 segundos. Lo llaman la posesión, F1 y el reinicio del duelo. OnUnPossess y EndPlay limpian el timer y las acciones. No se agregan Behavior Trees, State Trees, NavMesh ni un subsystem propio.

## Alcance posterior

Multijugador, IA más elaborada y reglas adicionales se diseñarán después de entender y observar esta base. No hay coordinación temporal de combate, historial, compensación de latencia ni simulación de combate propia en esta iteración.
