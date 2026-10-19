# 041 — Revisión del paso 5

Implementación posterior: [042 — Cámara de seguimiento, combate y oclusión](../042-camara-combate/reporte.md), con [guía de observación](../042-camara-combate/editor.md). El análisis y el prompt de esta carpeta se conservan como referencia; el HUD continúa separado.

## Alcance y resultado — 2026-10-20

Revisión solicitada de qué implementar, cómo integrarlo, archivos/sistemas afectados, decisiones abiertas y ajustes por cambios desde la documentación. El usuario sitúa el trabajo en el paso 5. Se toma como base el código actual y la movilidad de [039](../039-simplificacion-movilidad/reporte.md) y [040](../040-giro-direccional-dash/reporte.md), sin repetir las carencias históricas del prototipo ni implementar esta etapa todavía.

Alcance concretado para el siguiente prompt: cámara de seguimiento/combate y oclusión de jugador y target. HUD, proximidad y avisos quedan para otro prompt. Parámetros visuales globales, iguales para todos los personajes; edición temporal de desarrollo solo para iterar. La entrada al encuadre de combate ocurre al iniciar ataque propio o bloquear efectivamente. Recibir daño abierto conserva el encuadre vigente. La salida sigue el cierre de la cadena: NA propia para el atacante y NA del agresor para quien bloqueó, sin salir entre pasos. Se acepta consumir resultados un frame después. El [prompt de implementación](prompt.md) reúne estas reglas y las referencias necesarias.

No hubo cambios de código, configuración ni assets. Se agregan este reporte, la [guía](editor.md), la [captura MCP](estado-mcp.json) y sus enlaces desde el índice y 040. Las especificaciones y el roadmap permanecen sin editar; su estado y algunas reglas de movilidad quedaron anteriores a 039–040.

Referencias principales:

- [Paso 5 del roadmap](../../roadmap.md#5-presentación-de-combate-y-cámara-de-referencia), [funcional de jugador](../../player/functional.md) y [técnico de jugador](../../player/technical.md).
- [Investigación de cámara FGOA](<D:/fgo arcade/fgoac/research/camera-20261019-1544/INFORME_CAMARA.md>) y [especificación de traslado a Vamps](<D:/fgo arcade/fgoac/research/camera-20261019-1544/ESPECIFICACION_VAMPS.md>), realizadas el 19 de octubre. Se consultó también el decompilado conservado de 1403C8F10; no se volvió a ejecutar FGOA ni Ghidra.
- Evidencia visual histórica de [amenaza y fade](../016-analisis-fgo-arcade/escenas.md#n16n17n18-visibilidad-amenaza-y-extra-original). El análisis de cámara no define por sí solo el HUD completo.

## 1. Qué falta implementar

| Parte | Disponible actualmente | Trabajo del paso 5 |
|---|---|---|
| Seguimiento | Manager local con yaw/pitch suavizados; lock-on compartido con HUD | POV con posición de ojo y punto de interés persistentes, composición y perspectiva; deriva lateral y continuidad al frenar/invertir |
| Acciones | Fases, pasos, montages y reacciones reales; dash/sprint | Un encuadre de seguimiento y otro de combate; entrada por ataque propio/bloqueo efectivo y salida al cierre de la cadena de referencia |
| Obstáculos | Prueba de colisión nativa del SpringArm | Política única de obstrucción de la nueva vista; oclusores preparados con fade gradual/restauración y colisión de movimiento conservada |
| HUD | Panel Canvas de observación con vida, carga de guardia, energía, NA, reacción, stun, resultado/crítico y lista seleccionada | Se conserva; su presentación y los avisos quedan para otro prompt |
| Escenario | L_CharacterBasic y un rival creado por el GameMode | Varios rivales y obstáculos preparados para observar cambios de selección, amenazas distintas del target y oclusión |

El alcance termina en presentación local de combate. Puntuación/portador y respawn completo pertenecen a 6; sincronización multijugador a 7. No hacen falta clases nativas equivalentes a ManualCamera/FreeCamera/AuthCamera/FixCamera ni sistemas de cartas o NP del original.

## 2. Estado efectivo comprobado

Lecturas por MCP directo de defaults, referencias y componentes, guardadas en [estado-mcp.json](estado-mcp.json):

- BP_VampsPlayerController y BP_VampsCombatTestPlayerController usan `/Game/Vamps/Blueprints/BP_VampsPlayerCameraManager`. Sus mappings son IMC_Vamps e IMC_VampsCombatTest; comparten IA_Move e IA_Mobility.
- El manager tiene FollowPitch=-15°, TrackingSpeed=6 y HorizontalTolerance=6°. DefaultFOV=90 y DefaultAspectRatio≈4:3; esos defaults del manager no equivalen por sí solos a la proyección final del componente.
- En BP_VampsCharacter_Player, CameraBoom tiene longitud 400 cm, TargetOffset=(0,0,60), prueba ECC_Camera activa con radio 12 y lag de posición desactivado. FollowCamera tiene FOV=90, aspecto de referencia≈16:9, sin forzar barras ni sobrescribir la restricción de eje.
- El personaje usa SKM_Manny_Simple, cápsula de semialtura 88/radio 34 cm y mesh relativo Z=-88, yaw=-90°. Esto permite elegir el ancla en la escala real de Vamps; no demuestra equivalencia de unidades con FGOA.
- BP_VampsCombatTestGameMode referencia al jugador/controlador propios, BP_VampsCharacter como rival, BP_VampsAIController y la clase nativa VampsCombatTestHUD. El mapa abierto es L_CharacterBasic.

Se inspeccionaron las referencias de componentes y actores del mapa, sin auditar todos sus materiales ni hacer una lectura runtime del viewport. La búsqueda de assets propios no encontró recursos de cámara de acciones, HUD UMG o fade bajo `/Game/Vamps`. No se afirma que los materiales de otros packs ya tengan o carezcan de soporte compatible.

## 3. Cómo implementar la cámara

### Un dueño del POV

Ampliar AVampsPlayerCameraManager para calcular posición, rotación y FOV del pawn local en `UpdateViewTargetInternal(FTViewTarget&, float)`. La firma y su recorrido están comprobados en el motor local UE 5.8. Suprimir la competencia del seguimiento anterior en ProcessViewRotation y del boom al producir esa vista. Si se conservan componentes por referencias existentes, dejar explícito que no vuelven a resolver posición ni colisión del POV calculado.

El método base permite BlueprintUpdateCamera o CalcCamera; UpdateViewTarget aplica modificadores después. La implementación debe comprobar CameraStyle, overrides de Blueprint, blends y modificadores para que exista una sola ruta de seguimiento y una sola política de obstrucción. No basta con mover el boom y sumar otro lag encima.

### Geometría e historial

Mantener yaw orbital, Eye e Interest anteriores, perfil y progreso de transición como estado local. El pitch se obtiene de Interest−Eye; no se fuerza −15° para todas las situaciones. Usar un ancla estable relativa a cápsula y altura del personaje; la elección concreta entre esa referencia y un socket forma parte de las decisiones de composición.

Construir por perfil la distancia, altura del ojo, altura del interés, desplazamiento interior hacia el sujeto y offset angular. FGOA demuestra una mezcla interior acotada, no que siempre se apunte al centro del rival ni al punto medio exacto. Convertir ejes del motor original al plano XY/Z vertical de Unreal.

El filtro demostrado de 1403C8F10 corrige cada vector mediante `alpha=min(|desired-previous|/max(radius,0.1),1)^2`. Se aplica independientemente a Eye e Interest y conserva historial al soltar/invertir entrada. Radio cero deshabilita el filtro. No es una tolerancia angular ni un retraso fijo al pulsar A/D. Las condiciones nativas omiten ciertos filtros durante inicialización/transiciones y un caso de Step; no corresponde acumular siempre todos los suavizados.

Para la rama con dirección objetivo efectiva, FGOA aplica ganancia al error angular completo. Los 6° residuales actuales de Vamps y los 20° de otra rama nativa no son el mismo mecanismo. Conservar tolerancia visual y deriva no obliga a conservar la resta de 6° si el nuevo modelo ya produce el comportamiento acordado. No añadir histéresis o esperas sin necesidad observada.

### Perfiles, proyección y tiempo

Los parámetros de presentación son globales, iguales para todos los personajes. Implementarlos con valores comunes; permitir un ajuste de desarrollo acotado para iterar sin convertirlo en configuración por personaje o sistema de perfiles extensible. No ampliar UVampsCombatSettings ni crear Data Assets visuales por personaje. Las curvas o structs solo se justifican por una necesidad concreta de estos dos encuadres, no por anticipar planos de otras acciones.

La investigación demuestra FOV vertical, mientras FMinimalViewInfo.FOV es horizontal. Como referencia, 32,4° verticales equivalen a aproximadamente 54,63° horizontales a 16:9; no es un valor final elegido para Vamps. El motor convierte además según el aspecto de referencia y la restricción efectiva del eje. FollowCamera no sobrescribe esa restricción y BaseEngine.ini establece MaintainYFOV para LocalPlayer. No se comprobó el LocalPlayer runtime: fijar y verificar un contrato explícito de FOV/aspecto al generar el nuevo POV, sin heredar accidentalmente el 4:3 del manager ni hacer una doble conversión.

Las ganancias nativas son por actualización; el video de 30 fps no prueba su frecuencia de simulación. Elegir un paso temporal de referencia de Vamps y una aplicación dependiente de DeltaSeconds; para el filtro no lineal evaluar subpasos solo si la observación a distintas tasas de render lo requiere. No copiar ganancias por frame sin declarar su base temporal.

### Acciones y transiciones

Usar dos encuadres comunes, seguimiento y combate. Al comenzar un ataque aceptado se entra en combate. Un bloqueo efectivo también entra en combate; daño abierto no solicita cambio de encuadre y no fuerza mirada al agresor. No agregar planos independientes para Hit, Stun, crítico, NA, dash o sprint.

La referencia de cierre es la cadena que motivó la entrada: propia al atacar, del agresor al bloquear. Al entrar esa referencia en NA porque no continúa o agotó sus pasos, volver gradualmente al seguimiento. No salir al terminar un subimpacto o paso que aún continúa. La respuesta del usuario especificó el NA del rival; para el atacante se aplica el mismo cierre a su propia cadena. No deducir ese cierre de la duración del montage de bloqueo, ReactionEndsAt o de soltar RMB. Una cancelación definitiva o invalidación de esa secuencia, reset o cambio de pawn debe liberar referencias de presentación para evitar un encuadre retenido sin origen válido.

Al cambiar F o por guardia, capturar la vista y mezclar hacia la nueva composición. Al quedarse sin rival, transicionar hacia el seguimiento sin target ya previsto; al adquirirlo, volver a resolver anclas válidas. Una referencia temporal de presentación nunca sustituye CurrentTarget ni la lista del HUD. Para este alcance no hace falta un segundo sistema de adquisición nativo.

### Integración con movimiento y orden del frame

Move y MobilityPressed usan GetControlRotation().Yaw. El POV nuevo debe publicar un yaw coherente para ambos, conservando la dirección capturada al pulsar Shift de 040. El jugador ya usa bOrientRotationToMovement=true en AVampsCharacter; conservarlo como base, sin volverlo una decisión pendiente. El AIController tiene su política independiente y no debe cambiarse por esta cámara. El giro corporal instantáneo del dash no debe arrastrar bruscamente la cámara ni volver a orientar la dirección ya capturada. No forzar al personaje hacia el rival para corregir composición.

Hallazgo adicional respecto de la investigación: en `LevelTick.cpp`, UpdateCameraManager se ejecuta antes de OnWorldPostActorTick. Vamps resuelve contactos/guardia/continuación/dash en ese delegado posterior. Por tanto el manager normal puede observar una reacción o retarget defensivo recién resuelto en la actualización siguiente. Esto es orden de ejecución comprobado en fuentes, no una latencia medida en PIE.

Decisión confirmada por el usuario: consumir resultados ya confirmados en la siguiente actualización de cámara; un frame de desfase es aceptable. No cambiar el arbitraje del resolver ni calcular dos veces el POV para eliminarlo. La entrada consume el yaw publicado previamente, el movimiento sigue su ruta y la cámara actualiza con las posiciones disponibles.

R teletransporta/restaura los mismos personajes y no cambia necesariamente el pawn. ResetCombat debe notificar el reinicio de historial/pulsos/fade de presentación; detectar solo un pawn nuevo sería insuficiente. Inicialización y reset se distinguen de transiciones normales de target.

## 4. Cómo implementar oclusión y HUD

### Oclusores preparados

Propuesta para Vamps: un componente propio identifica meshes/slots preparados para fade; el manager local consulta los obstáculos entre la cámara y las anclas relevantes, conserva el conjunto afectado y ordena desvanecer/restaurar. Usar materiales propios con parámetro de visibilidad e instancias por componente, sin modificar un material compartido por todos los objetos. Restaurar presentación al salir de oclusión, hacer reset, cambiar de pawn o terminar la sesión.

La vista protegida comprende jugador y target seleccionado; sin target, comprende al jugador. Es la elección indicada por el usuario para este alcance.

Resolver la colisión de cámara una sola vez. Una política inicial razonable es permitir fade sin acortar por ese mismo oclusor preparado y usar colisión de cámara para sólidos no preparados; debe elegirse expresamente. Si todos siguen bloqueando primero el boom, puede que la cámara se retraiga y nunca muestre el efecto buscado. No desactivar Pawn/WorldStatic ni permitir atravesar el límite al personaje. La detección de fade debe seguir encontrando el obstáculo aunque ignore ECC_Camera; definir consulta/canal o marcado de detección coherente.

La investigación nativa demuestra capacidad de shader y una rama que limita radio, pero no cerró la selección/restauración de oclusores. El mecanismo Unreal anterior es una adaptación propuesta, no un algoritmo de FGOA reconstruido. Elegir un obstáculo visible y un límite invisible concretos para comprobar ambos casos.

### HUD y amenaza previa

Fuera del siguiente prompt por decisión del usuario. Lo siguiente conserva el análisis para la futura iteración de HUD, sin autorizar cambios ni infraestructura anticipada. La anotación sobre amenaza remite a las reglas de ataque/bloqueo para la cámara; no se la convierte en una definición adicional de flechas o avisos de HUD.

Reutilizar Health/MaxHealth, GuardLoad/MaxGuardLoad, MobilityEnergy/MaxMobilityEnergy, NARecoveryEndsAt, Reaction/ReactionEndsAt y resultados reales. Vida, carga y energía son tres indicadores distintos. NA no desaparece por presentar una reacción encima; crítico necesita una duración visual acotada en vez de mostrar indefinidamente el último booleano.

El panel actual dibuja una tarjeta de 425 px por rival y avanza 445 px: no sirve como lista compacta de varios enemigos. Mantener GetCandidates/CurrentTarget como fuente común y definir filas legibles que preserven el orden de F. La lista actual puede conservar candidatos muertos hasta su destrucción; el HUD de combate debe tratar elegibilidad/estados de fila coherentemente con el lock-on, sin crear otro orden de selección.

Para esta etapa basta ampliar el HUD Canvas con barras, lista compacta, selección y señales; UMG no es un requisito funcional. Si se elige UMG por diseño visual, crear widgets propios y conectarlos al HUD y GameMode existentes, manteniendo debug consultable. No hace falta introducir otro framework de UI.

El aviso previo requiere una regla propia y una fuente anterior al resolver de contactos. La selección de un enemigo por sí sola no demuestra que vaya a atacar al jugador. Tampoco basta con LastIncomingContact o ReactionSource, que llegan después. Una posibilidad acotada es registrar amenaza al aceptar un paso dirigido al jugador, retirándola al finalizar/cancelar, con distancia/sector y criterio explícitos; no garantiza que ese ataque vaya a acertar. Si debe detectar también un barrido de un enemigo orientado a otro rival, se necesita criterio geométrico adicional.

Proximidad muestra enemigos cercanos incluso sin aviso de ataque; amenaza es otra señal. Su dirección se proyecta respecto del yaw de la vista efectiva, incluida la parte posterior y fuentes fuera de pantalla. Guardia efectiva sigue siendo exclusivamente un resultado resuelto que puede cambiar selección.

## 5. Archivos y assets que afectaría la implementación

Rutas relativas a `Source/vamps/`, con headers en Public y cpp en Private; son impactos previstos, no cambios realizados:

| Archivos/sistema | Cambio previsto |
|---|---|
| Player/VampsPlayerCameraManager.h y .cpp | POV persistente, perfiles/curvas, transiciones, perspectiva, oclusión y yaw de presentación; núcleo del trabajo |
| Characters/VampsCharacter_Player.h y .cpp | Adaptar cámara/boom y ancla para evitar otro dueño de posición/colisión |
| Player/VampsPlayerController.h y .cpp | Usar la misma referencia de movimiento en Move y MobilityPressed; retirar dependencia del seguimiento anterior |
| Characters/VampsCharacter.h y .cpp | Señales mínimas de paso/reacción/contacto/reset y, según la regla elegida, amenaza; conservar permisos y resultados |
| Combat/Abilities/GA_Attack.cpp y GA_Dash.cpp | Solo si se requieren notificaciones de presentación no disponibles; mantener contratos de habilidad y root motion |
| Combat/VampsCombatResolver.h y .cpp | Solo si se necesita publicación de resultados por contacto/lote; no cambiar prioridades por añadir cámara |
| UI/VampsCombatTestHUD.h y .cpp | Fuera del próximo prompt; conservar la presentación existente |
| GameModes/VampsCombatTestGameMode.cpp | Reset de presentación y escenario de observación; no implementar aquí partida por equipos |
| Nuevo componente de oclusor en Public/Player y Private/Player | Marcar componentes/slots preparados y controlar recursos de fade; nombre a fijar al implementar |
| Characters/VampsLockOnComponent.h y .cpp | Principalmente dependencia existente; cambiar solo si hace falta compartir elegibilidad o notificar cambios, sin duplicar selección |

UVampsCombatMovementComponent, atributos GAS y UVampsAnimInstance aportan datos existentes; no se necesita reescribirlos para obtener el POV. Tampoco hace falta modificar CombatState o UVampsCombatSettings si los datos visuales quedan en presentación; hacerlo exige una necesidad concreta, no conveniencia anticipada.

Assets existentes involucrados: `/Game/Vamps/Blueprints/BP_VampsPlayerCameraManager`, BP_VampsCharacter_Player, ambos PlayerControllers y BP_VampsCombatTestGameMode; `/Game/Vamps/Maps/L_CharacterBasic`; los AM_Executioner_Combo/Block/Hit/Dash_Fw/Dash_Bw y ABP_Executioner como fuentes de tiempo/estado. No es obligatorio editar sus animaciones o graphs para todos los planos. Los perfiles/curvas, materiales/instancias y oclusores propios serían recursos nuevos; no hay rutas finales asignadas todavía.

Si se decide ampliar un DA o alguno de sus structs, antes se debe buscar por MCP clase/derivadas, capturar duraderamente todos sus valores y guardar los afectados; después releer, restaurar y comparar. La captura parcial de esta revisión no reemplaza esa preservación. Mantener cámara fuera del DA de combate evita una migración innecesaria.

## 6. Adaptaciones frente a documentación y referencia

1. **Movilidad vigente:** 039 usa dos montages con root motion, approach solo en el primer paso y elimina salida especial de Guard por impacto final; 040 captura WASD al pulsar Shift y gira una vez al iniciar dash aceptado. El funcional/técnico de jugador y partes del roadmap conservan ocho montages, desplazamiento programado, approach por paso o salida especial de Guard. La cámara debe integrarse con 039–040; esas descripciones requieren actualización cuando se autorice editar las especificaciones.
2. **Investigación anterior a 040:** su advertencia sobre preservar orientación de dash debe incorporar ahora el giro instantáneo hacia el vector capturado. No usar el frente corporal actualizado para volver a calcular la evasión o dar un salto de cámara.
3. **Manager 019:** conservar la continuidad visual y selección acordadas, pero sustituir el mecanismo de rotación/boom que no permite Eye/Interest independientes. Eliminar el pitch fijo y la tolerancia residual si no forman parte del modelo adoptado; no conservarlos por compatibilidad.
4. **Foco y perfil nativos:** FreeCamera es la clase que calcula el seguimiento, incluidas ramas de perfiles distintos; no significa solamente ausencia de lock o enemigo lejano. El perfil 3 tiene menor radio y FOV vertical que el 0 y sirve como referencia del encuadre cercano. Sus condiciones incluyen acciones/foco/temporización, por lo que is_target_mode no demuestra por sí solo «está atacando». Vamps conserva lock mientras haya enemigos elegibles y define explícitamente seguimiento/combate por ataque/bloqueo y cierre de cadena, sin portar toda la selección nativa ni añadir umbral por distancia sin requerimiento.
5. **Escala/FOV/tiempo:** valores nativos no son cm ni grados horizontales ni coeficientes por segundo. Incluso la proyección Unreal requiere resolver aspecto y restricción efectivos. Los números iniciales de Vamps deben identificarse como calibración propia.
6. **Resultados y recursos:** Guard/Hit/Stun/crítico/NA y energía separada ya existen; el HUD los presenta. Falta aviso previo, no otro resolver de daño o un recurso combinado.
7. **Muerte/transiciones:** no hay que esperar a implementar respawn completo para verificar pérdida de elegibilidad y ausencia de target, ni interpretar R como reaparición de partida. La cámara debe limpiar también un reset del mismo pawn.
8. **Estado documental:** roadmap/índice todavía anuncian paso 4 pendiente y referencias a 037. Esta revisión registra que el usuario ya sitúa el trabajo en 5, sin atribuirle a Codex comprobaciones nuevas de cierre jugable de 4.

## 7. Decisiones incorporadas y calibración

La cámara queda acotada a dos encuadres globales y sus transiciones; no hay decisiones de HUD necesarias para implementarla. Se conserva retorno gradual detrás del jugador sin target, sin foco alternativo de Hit. La oclusión protege ambos sujetos. El desfase de un frame está aceptado. La salida de combate sigue el cierre de la cadena de referencia indicado en la sección 3.

Elegir valores iniciales explícitos de Vamps para ancla, distancia, alturas, FOV, filtro y tiempos de mezcla; permitir ajuste de desarrollo común para acelerar la observación, sin datos por personaje. Preparar obstáculos/materiales propios mínimos. Estos números y recursos son decisiones técnicas de calibración, no nuevas preguntas de reglas de juego.

No son bloqueantes para una adaptación: IDs efectivos por frame del video, escala nativa exacta, opciones nativas en ejecución y reconstrucción completa de oclusores FGOA. Sí limitan afirmar equivalencia 1:1. Las mediciones pendientes están enumeradas en la investigación; no se simularon en esta revisión.

## 8. Orden de implementación y cierre

1. Aplicar las reglas concretadas en el [prompt](prompt.md); capturar configuración efectiva y preservar cualquier DA afectado antes de editar/compilar.
2. Implementar POV/anclas/FOV/historial y un yaw coherente con WASD/Shift. Primero observar lateral sostenido, parada, inversión, diagonales y ambos dashes sin planos ofensivos superpuestos.
3. Incorporar los dos encuadres, entrada por ataque/bloqueo, salida por NA de la cadena correspondiente, cambios por F/guardia y pérdida/adquisición; verificar cancelación y reset del mismo pawn.
4. Preparar material/oclusores y resolver detección/colisión/restauración como conjunto.
5. Conservar HUD y sus datos actuales; reservar rediseño, proximidad y amenazas para otro prompt.
6. Compilar mediante MCP soportado y observar manualmente los casos de la [guía](editor.md#3-verificación-cuando-exista-la-implementación). No cerrar por compilar o por una captura estática. Se conserva la referencia de movimiento, no se altera arbitraje ni colisión del personaje y cámara/HUD comparten selección incluso sin rival.

## Verificaciones y límites de esta revisión

Se revisaron fuentes propios, documentación vigente/histórica relevante, investigación de cámara, decompilado del filtro y código local UE 5.8 de cámara, proyección y orden de actualización. MCP confirmó assets/referencias y valores de defaults de cámara/cápsula/mesh. La primera consulta de assets usó un nombre de herramienta con prefijo y fue rechazada; se corrigió al nombre corto documentado y la lectura completó correctamente, sin mutación.

No se inició PIE, ejecutó gameplay, compiló ni modificó/guardó assets. La lectura no verifica aspecto runtime, encuadres nuevos, eficacia del fade ni timings de amenaza. No se ejecutaron ni agregaron tests automatizados. Se incorporaron las anotaciones del usuario y la respuesta sobre la salida al entrar en NA; se releyó bOrientRotationToMovement y se preparó el prompt. La implementación continúa pendiente.

Los enlaces a archivos de este reporte y su guía resuelven; la captura JSON se pudo releer. git diff --check no informó errores de espacios; Git advirtió normalización futura LF→CRLF en el reporte 040.
