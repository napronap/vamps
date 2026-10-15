# 003 — Configurar combate animado en el editor

La base vigente es [004 — Un ataque y una guardia](../004-hit-y-guard/reporte.md), con su [guía del editor](../004-hit-y-guard/editor.md). Este documento conserva el estado de una iteración anterior.

Queda para el usuario compilar en frío, equipar arma con sockets, colocar ventanas en montages y comprobar el duelo. **No hubo operaciones de editor realizadas por Codex en esta iteración.** Se implementó C++ y documentación; los assets anteriores permanecen intactos.

[Reporte](reporte.md) · [Reglas de combate](../../prototype/functional.md) · [Implementación](../../prototype/technical.md).

## 1. Compilar y reabrir

**Pendiente — usuario.**

1. Detener PIE y cerrar Unreal Editor.
2. En Rider/Visual Studio, regenerar archivos del proyecto si los tipos nuevos no aparecen. Compilar el target `vampsEditor`, configuración `Development Editor`, plataforma `Win64`, para `C:\dev\vamps\vamps.uproject`.
3. Abrir el proyecto con UE 5.8 y revisar Output Log.
4. Abrir y compilar `/Game/Duel/Blueprints/BP_DuelCharacter`, `BP_DuelPlayerController`, `BP_DuelGameMode` y `/Game/Duel/Animations/ABP_Duel`. Reparar referencias a funciones retiradas si existen: PressShared, SetMoveIntent o Parry. Las nuevas solicitudes son RequestAttack, RequestGuard y RequestMove.
5. Confirmar que BP_DuelCharacter muestra Weapon y WeaponTrace heredados. Guardar los Blueprints una vez corregidos.

Esta iteración cambia tipos reflejados, propiedades y subobjetos nativos. La verificación base debe ser una compilación completa con el editor cerrado. No se ejecutó Live Coding por falta de herramienta MCP directa expuesta; no hay parche confirmado.

## 2. Equipar el arma y crear sus sockets

**Pendiente — usuario.**

1. Elegir un Static Mesh de arma. No hay uno nuevo creado ni una referencia asignada por esta iteración. Si se reutiliza un recurso visual del ejemplo, duplicar solo ese recurso en `/Game/Duel/Weapons/`; nombre sugerido de la copia: `SM_DuelWeapon`.
2. Abrir el Static Mesh Editor, habilitar Socket Manager y crear `TraceBase` al comienzo de la parte que golpea y `TraceTip` en su punta. Usar posiciones locales sobre la hoja; no sobre el mango. Guardar.
3. En `BP_DuelCharacter > Components > Weapon`, asignar esa mesh. El componente se adjunta por defecto a `Mesh / hand_r`; ajustar transform relativo y, si hace falta, el socket de agarre para que acompañe la mano durante el ataque.
4. Conservar Weapon con Collision Enabled = NoCollision. La detección la realiza WeaponTrace.
5. En `/Game/Duel/Data/DA_DuelCombat`, comprobar Weapon Base Socket = TraceBase, Weapon Tip Socket = TraceTip, Weapon Trace Radius = 8 cm como punto de partida y Draw Combat = true.
6. Comprobar que los extremos se mueven con la hoja en una previsualización. No sustituirlos por coordenadas delante de la cápsula.

El radio es tolerancia geométrica; se ajusta a la hoja equipada. Las muestras se distribuyen entre ambos sockets. Falta decidir el recurso visual concreto y sus transformaciones; no hay valores universales de posición o rotación que puedan darse antes de elegirlo.

## 3. Preparar ventanas del ataque

**Pendiente — usuario.**

1. Abrir `/Game/Duel/Animations/AM_DuelAttack`. Conservar el skeleton compatible con `ABP_Duel` y el slot `DefaultGroup.DefaultSlot`. Mantener Play Rate de los segmentos = 1, Rate Scale del montage = 1 y Loop Count = 1.
2. Conservar las nueve secciones que el código enlaza para cada golpe: `Hit1_Prepare`, `Hit1_Active`, `Hit1_Recovery`, y el mismo patrón para 2 y 3. Deben ordenar preparación, contacto y recuperación dentro del clip correspondiente. No escalar duraciones para imitar los tiempos del Data Asset antiguo.
3. En las pistas de Notifies **del montage**, agregar un Notify State `Duel Combat Window` por golpe. En Details, Window = AttackActive. Su comienzo y final delimitan el tramo real en que el arma puede golpear. Colocarlo después del comienzo de Prepare y antes de la salida de Recovery.
4. En los golpes 1 y 2 agregar otro `Duel Combat Window`, Window = Combo, después de cerrar AttackActive y dentro de la recuperación. Colocar sus bordes en instantes distintos del cierre de AttackActive. No superponer estados del mismo tipo.
5. El tercer golpe no necesita Combo: termina e inicia NA al cerrar AttackActive.
6. Confirmar Montage Tick Type = Branching Point (el tipo nativo lo fuerza), Notify Trigger Chance = 1, sin filtrado por LOD para estas ventanas y Trigger on Dedicated Server habilitado.
7. Mantener todas las ventanas antes del comienzo del blend-out automático; ajustar Blend Out Trigger Time/Blend Out para no cortar la cola de gameplay. Revisar especialmente la duración restante de Recovery del primer y segundo golpe.
8. Revisar `AS_DuelAttack1/2/3`: retirar notifies antiguos de lógica del ejemplo si aún existen. Los notifies nuevos van en el montage, no en las secuencias. Conservar Root Motion desactivado en esta base; no hay acercamiento artificial en C++.
9. Guardar montage y confirmar `DA_DuelCombat > Attack Montage = AM_DuelAttack` y los nueve nombres en Strikes.

Resultado esperado: preparación sin daño, trazas solo en AttackActive, siguiente pulsación aceptada únicamente en Combo y después de cerrar el contacto activo. Al continuar se reproduce inmediatamente la sección Prepare siguiente a velocidad normal.

## 4. Crear guardia y reacción

**Pendiente — usuario.**

Estos dos montages necesitan clips visuales compatibles. No están creados por esta iteración. Preparar recursos propios usando solo animaciones/modelos del ejemplo si corresponde, sin su lógica.

1. Crear `/Game/Duel/Animations/AM_DuelGuard` con un clip de guardia en DefaultGroup.DefaultSlot, sin loop. Agregar `Duel Combat Window > GuardActive` al tramo que debe cubrir el golpe. Dejar una cola antes del blend-out para aceptar otra pulsación después de bloquear.
2. Usar una única sección sin enlace de repetición automática. Play Rate/Rate Scale = 1. Mantener la ventana fuera del blend-out.
3. Crear `/Game/Duel/Animations/AM_DuelHit` con un clip de reacción en el mismo slot, sin loop. Agregar `Duel Combat Window > HitResponse` después del tramo que debe impedir movimiento/guardia, hasta cerca del final de la reacción y antes del blend-out.
4. Aplicar las mismas opciones de notify del paso 3: branching point, chance 1, sin filtro LOD, habilitado en dedicado.
5. Asignar ambos en `DA_DuelCombat > Guard Montage / Hit Montage`. Guardar.

Sin Guard Montage no se acepta guardia. Sin Hit Montage se aplica daño pero la reacción termina inmediatamente y se informa el problema; eso no valida el comportamiento final. Las longitudes de las ventanas se eligen mirando los clips, no introduciendo tiempos duplicados en C++.

## 5. Revisar input y valores

**Pendiente — usuario.**

1. En `BP_DuelPlayerController`, conservar Duel Mapping = IMC_Duel y las cinco referencias Move, Look, Lock, Reset y AI Mode.
2. Attack y Guard se crean en memoria por código: LMB y RMB, prioridad 1, evento Started. No crear ni asignar nuevos Input Actions para estos botones.
3. En `/Game/Duel/Input/IMC_Duel`, retirar el mapping de IA_DuelShared para limpiar la configuración anterior. El asset IA_DuelShared puede conservarse sin usar; no es necesario borrarlo.
4. Revisar cualquier gráfico que dependiera de SharedAction, PressShared o Parry. La función y las propiedades antiguas ya no existen.
5. En DA_DuelCombat revisar daño de los tres golpes, vida, ángulo, giro inicial, movimiento, pushback y NA. Los campos antiguos de buffer/parry, duración de fases, alcance frontal, guardia/reacción y acercamiento ya no gobiernan el juego.
6. Valores iniciales nativos: daño 12; vida 100; NA ofensivo 0,45 s; defensivo 0,18 s; espera tras daño 0,20 s; cobertura 180°; giro inicial 90°; velocidad 420 cm/s; pushbacks 24/12 cm. Los valores guardados de campos conservados en el asset prevalecen.
7. Guardar. La clase de IA nativa empieza en Passive; si algún Blueprint conserva un override anterior, revisarlo o usar F1 para llegar a Passive.

## 6. Revisión manual progresiva

**Pendiente — usuario.**

Abrir `/Game/Duel/Maps/L_Duel`, iniciar PIE local y avanzar en este orden:

1. **Movimiento:** WASD, cámara libre, Tab y reinicio R. Mantener dirección durante preparación debe permitir retomar movimiento al entrar en recuperación.
2. **Ataque aislado:** en Passive, aproximarse con WASD y pulsar LMB una vez. La hoja debe producir trazas rojas únicamente durante AttackActive; alcanzar al rival fuera de esa ventana no daña.
3. **Geometría:** probar distintas distancias, orientación y un obstáculo con bloqueo Visibility. Un mismo golpe debe descontar daño una sola vez, aunque cruce varias muestras. Contrastar trazas con la hoja visible.
4. **Combo:** pulsar temprano debe mostrar rechazo y no producir continuación posterior. Pulsar dentro de Combo inicia el golpe siguiente. Probar hasta tres y salida mediante movimiento.
5. **Guardia:** con el rival en Combo, pulsar RMB desde reposo. Comprobar cobertura frontal, fallo desde atrás, consumo por contacto y necesidad de nueva pulsación. Mantener RMB no renueva defensa.
6. **Reacción:** recibir daño y pedir movimiento o guardia antes/durante HitResponse. Durante NA la guardia sigue prohibida. Verificar pausa reemplazada por golpes sucesivos.
7. **Limpieza:** R durante preparación, contacto, combo, guardia, reacción y NA. Comprobar vida y posiciones restauradas, sin trazas ni ventanas/timers residuales. Comprobar también derrota durante ataque.
8. **Interrupciones y velocidad:** recibir un golpe durante ataque, y repetir bajo distintas cadencias de frames. Revisar pérdidas de contactos en ventanas cortas/hitches antes de ajustar balance.
9. Volver a Passive entre revisiones para aislar cada acción. Guardar los ajustes finales.

El HUD muestra rechazos. Para más detalle usar Output Log y activar manualmente `Log LogDuelCombat Verbose`. Esta revisión local no valida multijugador. La guía no incluye tests automatizados.
