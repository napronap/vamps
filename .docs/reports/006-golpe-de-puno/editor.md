# 006 — Configurar el puño

**Pendiente para el usuario:** compilar con el editor cerrado, crear el socket FistHit y asignar un montage de un solo puñetazo. Codex solo modificó código/documentación; no realizó operaciones de editor ni verificó assets. Esta guía reemplaza los pasos de arma de 004 y 005.

[Reporte](reporte.md) · [Flujo del código](../../combat/technical.md#cómo-leer-el-código).

## 1. Compilar y revisar el personaje

**Pendiente — usuario.**

1. Cerrar PIE y Unreal Editor. En Rider/Visual Studio, compilar `vampsEditor / Development Editor / Win64` para `C:\dev\vamps\vamps.uproject`. Regenerar archivos del proyecto si faltan tipos nuevos.
2. Reabrir con UE 5.8. Cambian componentes nativos y una clase reflejada: usar compilación completa, sin depender de Live Coding.
3. Abrir `/Game/Duel/Blueprints/BP_DuelCharacter`. Debe aparecer FistHitbox bajo Mesh; Weapon y WeaponTrace ya no forman parte de la clase. Retirar referencias/nodos antiguos y cualquier mesh de arma que se hubiera añadido manualmente al Blueprint.
4. Mantener Auto Possess AI = Disabled, Attack Damage = 12 y los componentes de GAS/cámara existentes. Compilar y guardar el Blueprint.

No hay herramientas MCP directas de Unreal disponibles en esta sesión; esta compilación no fue ejecutada.

## 2. Crear el socket y ajustar la esfera

**Pendiente — usuario.**

1. Desde `BP_DuelCharacter > Mesh > Skeletal Mesh`, abrir la mesh asignada y su Skeleton. No se presupone otro skeleton distinto del que ya use ese personaje.
2. En Skeleton Tree, localizar el hueso de la mano que pega en la animación elegida; por ejemplo, hand_r si es el puño derecho. Menú contextual del hueso → Add Socket; llamarlo **FistHit**.
3. Mover ese socket al centro del puño cerrado, junto a los nudillos. Previsualizar la animación de puñetazo y verificar que acompaña la mano durante todo el golpe. Guardar el Skeleton. La posición exacta depende de la mesh y animación elegidas.
4. En `BP_DuelCharacter > FistHitbox`, dejar Socket Name = FistHit y Shape > Sphere Radius = **12 cm** como valor inicial. La escala afecta el radio del barrido. Ajustar el radio para cubrir el puño; ajustar el centro moviendo el socket, no el transform relativo del componente.
5. Mantener Collision Enabled = NoCollision y Generate Overlap Events deshabilitado. Las trazas usan el volumen de la esfera sin eventos de overlap ni bloqueo físico. Guardar.

Solo se usa ese puño: elegir una animación de un golpe con esa mano. No hacen falta TraceBase, TraceTip ni una Static Mesh adicional. Si falta FistHit, el Output Log informa `missing fist socket` y el ataque no se activa.

## 3. Configurar el puñetazo

**Pendiente — usuario.**

1. Crear o editar `/Game/Duel/Animations/AM_BasicAttack` con el skeleton del personaje. Asignar **una sola secuencia de puñetazo** compatible con ese skeleton y con la mano del socket. Si procede de otro skeleton, preparar primero su versión compatible en el editor.
2. Mantener un segmento, un golpe, Rate Scale = 1, Loop Count = 1 y sección Default sin loop. Slot = DefaultGroup.DefaultSlot. No usar una cadena de golpes.
3. En la pista de notifies **del montage**, colocar un único Notify State **Attack Trace** desde que el puño comienza el tramo capaz de impactar hasta que termina ese tramo, antes del blend-out. Los tiempos concretos dependen de la animación elegida; no hay duraciones de ataque que mapear en C++.
4. Confirmar Branching Point, Notify Trigger Chance = 1, sin filtro LOD y Trigger on Dedicated Server habilitado. Eliminar notifies antiguos que ejecuten otra lógica de combate.
5. En `/Game/Duel/Animations/ABP_Duel`, conservar DefaultGroup.DefaultSlot antes de la salida. En BP_DuelCharacter, asignar Attack Montage = AM_BasicAttack. Compilar y guardar.

## 4. Probar con los controles existentes

**Pendiente — usuario.**

Mantener las clases y entradas descritas en [005, paso 2](../005-ia-basica/editor.md#2-revisar-clases-e-input). Abrir `/Game/Duel/Maps/L_Duel` con BP_DuelGameMode y comenzar Play local con foco en el viewport.

1. **Passive:** acercarse y pulsar LMB. El puñetazo debe quitar 12 de vida una sola vez aunque la esfera permanezca dentro de la cápsula del rival. Fuera de alcance o del tramo Attack Trace no debe producir daño.
2. **F1 hasta Guard:** golpear al rival cubierto. No pierde vida y se cancela el montage del jugador. No debe aparecer daño residual.
3. **F1 hasta Attack:** mantener RMB para bloquear al rival; soltar y volver a recibir un contacto para observar daño. El rival ahora intenta atacar a 100 cm entre personajes. Si la animación no alcanza, ajustar AttackDistance en `Source/vamps/Public/AI/DuelAIController.h` y recompilar; no aumentar arbitrariamente la esfera para compensarlo.
4. **R:** restaura posiciones y vida, cancela el ataque y mantiene el modo IA. Las trazas no deben unir la posición anterior al reinicio con la siguiente.
5. Comprobar los restantes modos F1, movimiento WASD, cámara, Tab y HUD según [005, paso 4](../005-ia-basica/editor.md#4-comprobaciones-manuales). Sus controles se conservan; cualquier referencia al arma o a 150 cm de esa guía queda sustituida por esta configuración.

Si el montage no se reproduce, comprobar Attack Montage, slot y existencia de FistHit. Si se reproduce sin contacto, comprobar que Attack Trace esté en el montage y cubra el tramo correcto, la posición del socket y Sphere Radius. La esfera puede inspeccionarse seleccionando FistHitbox en el editor; no se agregó un visualizador de trazas en juego.
