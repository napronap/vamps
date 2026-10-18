# 022 — Cadena por secciones y recuperación NA

El salto inmediato descrito en esta iteración se reemplaza por la decisión al cierre de la ventana en [024 — Cierre de Combo Window](../024-cierre-ventana-combo/reporte.md), con su [guía](../024-cierre-ventana-combo/editor.md).

Configuración vigente de armas y reemplazo del puño: [023 — Armas y trazas](../023-armas-y-trazas/reporte.md), con su [guía del editor](../023-armas-y-trazas/editor.md). Esta iteración conserva la evidencia previa de cadena, NA y movimiento.

## Resultado — 2026-10-17

Implementación nativa de la etapa 2 del [roadmap](../../roadmap.md#2-cadena-y-recuperación-na): cadena por secciones, continuación, NA y restricción ofensiva. Se agregaron movimiento de combate con colisión, alineación/knockback por paso, soporte de reacción y salida visual de Recovery a locomoción sin adelantar la disponibilidad ofensiva. El usuario realizó la compilación completa y reapertura; Codex verificó el componente nuevo en ambos Blueprints y sus instancias de PIE, compiló los ocho Blueprints y guardó personajes y DataAsset sin errores. Posteriormente se centralizaron los montages de ataque y reacción en ese DA, con otro parche real de Live Coding, guardado y comprobación de arranque en PIE.

**La etapa no está cerrada.** El usuario está eligiendo las animaciones. Faltan configurar el montage de tres pasos y la reacción, calibrar distancias y observar cadenas, movimiento y recuperación en PIE. El arranque limpio y la verificación de assets ya se completaron. La [guía](editor.md) distingue lo pendiente de las operaciones realizadas.

La iteración continúa la [021](../021-solicitud-compartida/reporte.md). Se conservaron los cambios de documentación que ya estaban presentes al comenzar. No se modificaron las especificaciones funcionales/técnicas ni el roadmap durante esta implementación.

## Comportamiento implementado

- Cada sección representa un paso; se admiten de uno a tres, en orden temporal. El avance siempre utiliza Montage_JumpToSection hacia la sección siguiente, sin nombres de sección hardcodeados. Las conexiones automáticas entre secciones se desconectan en la instancia de reproducción: sin continuación, termina el paso actual.
- El nuevo ANS_ComboWindow acepta clicks desde su inicio hasta su final. Debe comenzar después de AttackStart y terminar después de AttackEnd, dentro de la misma sección y antes del blend-out. El desplazamiento concreto de ambos extremos se ajusta sobre el clip elegido.
- Un click anterior a la ventana puede rearmar defensa si la fase lo permite, pero no se conserva para continuar. Un click dentro de ventana queda asociado únicamente al paso actual; si todavía no hubo contacto, espera su resolución. Un contacto abierto, bloqueado o contra otro rival habilita el salto. Fallar elimina la solicitud al terminar el paso, sin atacar automáticamente después.
- El resolver procesa las continuaciones después de todos los contactos del frame. Un bloqueo del mismo lote puede impedir una continuación solicitada antes de resolverlo. Cambiar de paso limpia la solicitud, el contacto habilitante y la deduplicación; los notifies pendientes de la sección anterior no modifican el nuevo paso.
- Preparation y Contacts conservan defensa hasta AttackEnd, salvo consumo. Recovery admite continuación solamente con ventana abierta y no rearma defensa. El salto aceptado inicia la preparación del siguiente paso y su defensa. El código contempla un impacto por sección, conforme al alcance elegido; no se agregaron marcadores de impactos adicionales.
- Al terminar la reproducción de la sección sin saltar, se entra en NARecovery durante 0,5 s iniciales. La duración pertenece al DataAsset del personaje; no depende del clip. NA rechaza ataque y defensa y no modifica locomoción. Bloquear o recibir daño no inicia NA por sí solo; el cierre de una ofensiva propia conserva su recuperación.
- El contacto transporta una copia de la duración completa de la sección ofensiva dividida por la velocidad efectiva del montage. Al bloquear, el defensor queda sin nuevos inicios ni continuaciones ofensivas durante ese tiempo desde la resolución del bloqueo. Margen adicional inicial: cero. Bloqueos sucesivos conservan el vencimiento más lejano; nunca lo acortan.
- Esa restricción no cancela el ataque ya en ejecución ni crea NA. Sigue siendo posible rearmar defensa en las fases válidas; en Idle restringido, el click arma únicamente defensa hasta su consumo o el vencimiento de la restricción. Al vencer, hace falta otra pulsación para atacar: no se guarda una ofensiva durante la restricción.
- El retarget sigue perteneciendo al bloqueo efectivo. Un acierto ofensivo contra un rival diferente no cambia la selección. Se conservan contactos simultáneos y daño abierto por GAS.

### Movimiento y presentación incorporados el 17 de octubre

- UVampsCombatMovementComponent extiende UCharacterMovementComponent y reemplaza el componente nativo de AVampsCharacter mediante ObjectInitializer. Es el único componente que ejecuta el movimiento; no se agregó una librería ni un segundo sistema que traslade la cápsula por fuera de CharacterMovement.
- Cada entrada de ComboSteps contiene TargetAlignmentDistance y KnockbackDistance en centímetros. Valores iniciales: pasos 1/2 = 100/60; paso 3 = 100/180. Son calibración inicial, no distancias extraídas de videos. El tercer paso exige el knockback mayor. NA conserva 0,5 s. No hay copia manual de tiempos del montage en el DataAsset.
- El resolver captura origen, frente y datos del paso al detectar el contacto. Después de resolver daño/bloqueo, selecciona un movimiento por receptor/frame: daño abierto antes que bloqueo; dentro del mismo resultado, target actual y orden de candidatos. Los demás contactos conservan daño y resultados; no se suman varios reposicionamientos contradictorios en el mismo frame.
- Un impacto abierto alinea al receptor delante del atacante, a la distancia fija del DA, y orienta su cuerpo hacia él. La interpolación inicial dura 0,08 s. Después calcula el knockback desde la posición real alcanzada y lo ejecuta durante 0,12 s. Los bloqueos aplican directamente knockback, sin alineación ni giro forzados. Con un único impacto por sección, ese contacto es también su remate.
- CharacterMovement hace los barridos de colisión. Una obstrucción puede acortar alineación y empuje; el componente no rodea la pared mediante deslizamiento lateral durante esos movimientos. Se conserva la prioridad colisión → posicionamiento → knockback. No se impone una distancia mínima/máxima de ajuste ni se teletransporta al destino.
- HitReactionMontage pertenece a la configuración del receptor. Puede permanecer sin asignar mientras se elige la animación; el movimiento físico funciona sin esa presentación. Si se asigna, debe ser un montage distinto del ataque, compatible con su esqueleto y con Auto Blend Out. Un bloqueo no reproduce ese montage ni cancela por sí solo la ofensiva propia.
- Se suprime el root motion extraído mientras el componente ejecuta alineación/knockback y para el montage de reacción, evitando sumar desplazamiento de la animación al definido en el DA. El ataque conserva su root motion normal. Reinicio, salida del personaje y pérdida de vida hasta cero limpian el movimiento controlado, detienen montages y velocidad y descartan root motion pendiente.
- El input de movimiento del jugador y la IA usa el override nativo AddMovementInput. Durante Recovery solicita liberar el montage; el resolver procesa la solicitud después de las continuaciones y espera a que cierre Combo Window. Una continuación válida gana antes de la salida visual.
- La liberación cancela únicamente la habilidad ofensiva actual, con el blend-out normal de su montage. La fase lógica sigue en Recovery hasta el final calculado de la sección y después comienza NA. El root motion del montage saliente deja de gobernar locomoción; esa supresión se conserva hasta el siguiente ataque o reinicio, incluso si un NA corto vence antes del blend-out. Moverse no adelanta el siguiente ataque ni evita NA. Los notifies de la reproducción cancelada se ignoran por su ID.
- HUD agrega MOTION, bloqueo del desplazamiento y RELEASED. No se añadieron Niagara, Motion Warping, giro compartido con el atacante ni varios impactos por sección. El giro compartido quedó como alternativa opcional; el frente del atacante dirige el posicionamiento implementado.

Las distancias fijas buscan conservar el espaciado del combo; falta elegir/calibrar los clips para verificar continuidad. No se fabrican contactos ni se otorga daño automático para ocultar un fallo de las trazas.

### Montages por personaje

AttackMontage se trasladó de AVampsCharacter a UVampsCombatSettings. HitReactionMontage ya pertenecía al mismo DA. La habilidad GAS, los saltos de sección, la identificación de notifies, el cálculo de duración, la IA y la supresión de root motion consultan esa única configuración. Se eliminó la propiedad del personaje, sin copia, getter ni ruta alternativa al valor anterior.

DA_VampsCombatSettings conserva AttackMontage=AM_BasicAttack, la referencia previamente compartida por ambos Blueprints. HitReactionMontage sigue vacío mientras se elige el clip; NA y distancias permanecen iguales. Manny y ABP_VampsCharacter siguen compartidos y no se trasladan al DA. AttackDamage permanece en el personaje. La reacción animada de bloqueo queda para la etapa 3; no se agregó su referencia ni reproducción en este cambio.

## Código y conexiones

Rutas relativas a Source/vamps/.

| Archivos/clases | Cambio y relación |
|---|---|
| Public/Combat/Data/VampsCombatSettings.h | UVampsCombatSettings con AttackMontage, NARecoveryDuration, ComboSteps de FVampsCombatStepSettings y HitReactionMontage. Los datos del atacante definen montage y distancias; el receptor aporta su animación de reacción |
| Public/Combat/VampsCombatMovementComponent.h y Private/Combat/VampsCombatMovementComponent.cpp | Nuevo movimiento nativo: interpolación, orientación del receptor, knockback, arbitraje con root motion y colisión. Se instala como CharacterMovement del personaje y expone su estado al HUD |
| Public/Animation/ANS_ComboWindow.h y Private/Animation/ANS_ComboWindow.cpp | Nuevo notify state nativo, Branching Point; comunica apertura/cierre a AVampsCharacter |
| Public/Combat/VampsCombatState.h | Agrega NARecovery, paso de cadena, ventana, solicitud de continuación, restricción ofensiva y vencimientos visibles en debug |
| Public/Characters/VampsCharacter.h y Private/Characters/VampsCharacter.cpp | Instala el componente nativo, coordina solicitud, salto, fases, defensa, NA y restricción. Separa Recovery visual/lógica y entrega movimiento al resolver; calcula tiempos desde la sección y limpia timers/contactos/movimiento al reiniciar o salir |
| Private/Combat/Abilities/GA_Attack.cpp | Conserva una habilidad GAS para toda la cadena. Valida configuración, exige Attack Trace por sección y Combo Window antes del último paso, y desconecta enlaces automáticos de la reproducción. Admite velocidad constante positiva y rechaza Time Stretch Curve activa |
| Public/Combat/VampsCombatResolver.h y Private/Combat/VampsCombatResolver.cpp | FVampsPendingContact incorpora duración, origen, dirección y distancias del paso. El subsystem resuelve contactos, desplazamiento, continuaciones y solicitudes de locomoción, conservando el arbitraje defensivo existente |
| Private/AI/VampsAIController.cpp | Attack solicita continuación con contacto y ventana, sin saltarse NA ni restricción. Guard conserva la solicitud compartida |
| Private/UI/VampsCombatTestHUD.cpp | Muestra paso, ventana, contacto habilitante, continuación pendiente, restricción/NA y estado del movimiento controlado/liberación visual |

UFistHitboxComponent conserva la deduplicación espacial; BeginCombatStep la reinicia para que cada sección pueda alcanzar otra vez al mismo rival. UANS_AttackTrace conserva AttackStart/AttackEnd. Controllers, input, cámara, AnimBP y GameMode no incorporan resolución paralela.

## Assets

- **Creado y actualizado por MCP:** /Game/Vamps/Data/DA_VampsCombatSettings, clase nativa VampsCombatSettings; AttackMontage=AM_BasicAttack, NARecoveryDuration=0,5, ComboSteps=100/60, 100/60 y 100/180 cm. HitReactionMontage permanece vacío mientras se elige el clip. Valores releídos y paquete guardado con save_assets=true e is_dirty=false.
- **Asignados, compilados, guardados y releídos por MCP:** CombatSettings en /Game/Vamps/Blueprints/BP_VampsCharacter y BP_VampsCharacter_Player. Ambos apuntan a DA_VampsCombatSettings y su CharacterMovement es /Script/vamps.VampsCombatMovementComponent, verificado también en ejecución. AssetTools.save_assets devolvió true e is_dirty=false para los dos. Git registra ambos .uasset modificados.
- **Mapa sin edición por Codex en esta ampliación:** /Game/Vamps/Maps/L_CharacterBasic. El actor colocado que antes conservaba CombatSettings=None ya no figuraba en la consulta del editor previa a esta implementación. Ese pendiente anterior no exige otra recarga para una instancia ausente. Las modificaciones del mapa y la importación/eliminación de packs corresponden al usuario.
- **Configurado y guardado por MCP:** /Game/Vamps/Animations/AS_BasicAttack1, Enable Root Motion=true, valor releído. Se conservó Root Motion Root Lock=RefPose. Esto habilita la extracción del clip actual; no acredita el desplazamiento ni la calidad de las animaciones todavía por elegir.
- **Sin edición:** /Game/Vamps/Animations/AM_BasicAttack. Sigue siendo el asset de ataque asignado. Las secciones y notifies no son accesibles mediante las herramientas MCP directas disponibles; la selección del clip continuo y su preparación quedan en la guía. No se copiaron clases ni lógica de Third Person.

## Verificación del núcleo y recuperación — 2026-10-16

- Aplicado clang-format a todos los archivos C++ propios modificados/agregados. Git diff --check del código no informó problemas. Se revisaron nuevos nombres contra las clases base e interfaces nativas relevantes.
- **Hecho — usuario:** compilación completa con el editor cerrado y reapertura, confirmadas por su respuesta «listo». La conexión MCP y las clases reflejadas se verificaron en la sesión nueva.
- LiveCodingToolset.LiveCodingToolset.CompileLiveCoding: Success, con parche real aplicado tras la reapertura; no fue NoChanges. Incluye la corrección de velocidad que usa la instancia por ID también durante el blend-out. Consultar solamente el montage activo podía devolver velocidad cero durante contactos en ese tramo. La primera recarga reflejada había producido warnings de tipo transitorio CombatState; no se repitió una modificación de tipos durante la recuperación.
- BlueprintTools.compile_blueprint con warnings_as_errors=true completó en la sesión recuperada los ocho Blueprints: ABP_VampsCharacter, ambos personajes, ambos controllers de jugador, BP_VampsAIController y ambos GameModes. Ambos personajes se guardaron sin repetir el assert.
- Al guardar BP_VampsCharacter, Unreal falló en generación de thumbnails con Assertion failed: Ret->IsA(T::StaticClass()), Class.h:4627. El log registra el guardado previo del DataAsset y el cierre del editor; MCP quedó desconectado. La relación causal con la recarga de tipos es una hipótesis, no un diagnóstico confirmado. Evidencia local: Saved/Logs/vamps.log y Saved/Crashes/UECC-Windows-CDE138644A10BCE631B9F6846043C05B_0000.
- Se inició y detuvo PIE mediante EditorAppToolset en L_CharacterBasic, con IA Passive. ObjectTools confirmó Idle, ComboStep=0 y ausencia de solicitudes/restricciones en los tres personajes presentes en esa revisión. Jugador y rival creado por el GameMode recibieron el DataAsset; el actor colocado conservaba un override vacío y posteriormente fue retirado por el usuario. No hubo solicitudes de combate ni comprobación de cadena/NA en ejecución. El log consultado no devolvió errores de Blueprint, asserts ni errores de PIE; sí una entrada anterior de sesión MCP vencida al reconectar. PIE quedó detenido.
- No se crearon ni ejecutaron tests automatizados.

### Verificación de la ampliación — 2026-10-17

- Aplicados clang-format y revisión estática a los archivos propios modificados y al componente nuevo. Nuevos nombres revisados contra la jerarquía nativa de CharacterMovement, ActorComponent y UObject; AddMovementInput y los métodos de movimiento implementan contratos virtuales deliberadamente.
- LiveCodingToolset.LiveCodingToolset.CompileLiveCoding devolvió Success y aplicó un parche real para la ampliación completa. UHT registró el componente, enum y estructura de datos nuevos. La recarga emitió warnings transitorios de CombatState y advertencia de packaging por cambios de tipos.
- **Hecho — usuario:** guardó su trabajo, recompiló con Unreal cerrado y reabrió el editor; confirmó «listo, recompilado y vuelto a iniciar el editor». Los assets afectados no se guardaron desde la recarga estructural. No hay MCP directo para esa compilación completa/reapertura.
- Después de la reapertura, Live Coding aplicó otro parche real con Success para dos ajustes de cuerpos de métodos: limpieza de montages/root motion al reiniciar y conservación de la liberación visual después de NA. No hubo nuevos cambios de tipos reflejados en ese parche.
- ObjectTools.get_class confirmó /Script/vamps.VampsCombatMovementComponent en CharMoveComp de ambos personajes. get_properties confirmó sus referencias al DA, AttackMontage=AM_BasicAttack y estado inicial limpio. Se releyeron los nuevos datos del DA; personajes y DA se guardaron con save_assets=true e is_dirty=false.
- BlueprintTools.compile_blueprint, con warnings_as_errors=true, volvió a compilar los ocho Blueprints enumerados arriba, ahora con el componente nuevo. No hubo errores ni advertencias y el guardado no reprodujo el assert anterior.
- StartPIE/StopPIE en L_CharacterBasic, IA Passive: los dos personajes presentes —jugador y rival del GameMode— usaron el componente nuevo y DA_VampsCombatSettings, en Idle, ComboStep=0, sin solicitudes, restricción, ventana ni liberación visual. El log consultado no devolvió asserts ni errores de Blueprint/PIE; solo una sesión MCP vencida durante la reconexión. PIE quedó detenido. No se solicitaron ataques ni se acreditan alineación, knockback, colisión o root motion observados en combate.

### Verificación del traslado de montages al DA — 2026-10-17

- Aplicados clang-format y revisión de referencias a los seis archivos C++ afectados. AttackMontage solo se declara en VampsCombatSettings; todos los consumidores propios consultan el DA. Git diff --check no informó problemas.
- LiveCodingToolset.LiveCodingToolset.CompileLiveCoding devolvió Success con un parche real. Se recargaron las clases de configuración y personaje; el log informó 20 scriptstructs sin cambios y la advertencia de packaging por tipos recargados. El traslado todavía no pasó por una nueva compilación completa con el editor cerrado; el paso 1 de la guía explica cómo incorporar el cambio a la próxima apertura sin el parche.
- ObjectTools confirmó que ambos Blueprints usaban AM_BasicAttack antes del traslado. Se asignó esa referencia al nuevo campo del DA y se releyó. list_properties confirmó que AttackMontage ya no aparece en ninguno de los dos personajes.
- Ambos personajes y ABP_VampsCharacter compilaron con warnings_as_errors=true. El DA y ambos personajes se guardaron con save_assets=true e is_dirty=false; no se reprodujo el assert de la revisión anterior.
- StartPIE/StopPIE confirmó ambos personajes con DA_VampsCombatSettings y estado inicial limpio en Idle. La consulta del log no encontró nuevos errores de Blueprint/PIE ni asserts. PIE quedó detenido. Esta comprobación acredita configuración y arranque, no una cadena ejecutada con las animaciones todavía en preparación.

Restan la preparación manual de las animaciones y la observación/calibración del combate según [editor.md](editor.md). El cierre de etapa 2 requiere comprobar los casos de esa guía.
