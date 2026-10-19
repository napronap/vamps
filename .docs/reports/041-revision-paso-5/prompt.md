# Prompt — Cámara del paso 5

Implementá la cámara del paso 5 de Vamps: seguimiento, encuadre de combate, transiciones y oclusión. El HUD se hará en otro prompt; conservá el actual y no agregues avisos de amenaza/proximidad ni infraestructura para ellos.

## Referencias y punto de partida

- Respetá AGENTS.md y .docs/AGENTS.md.
- Leé .docs/reports/041-revision-paso-5/reporte.md y editor.md. La captura estado-mcp.json es una referencia de defaults previos; no reemplaza una lectura nueva ni el backup completo de un DA afectado.
- Leé D:/fgo arcade/fgoac/research/camera-20261019-1544/INFORME_CAMARA.md y ESPECIFICACION_VAMPS.md. Usá el análisis nativo y los clips C01/C03/C08 como referencia del comportamiento y del historial Eye/Interest, adaptados a Unreal. No reconstruyas FEngine ni copies ciegamente sus unidades, frecuencia, perfiles o adquisición de targets.
- Revisá el código actual. Movilidad vigente: 039–040, dos montages con root motion, approach solo inicial y dirección de dash capturada al pulsar Shift. Las descripciones de ocho montages/dash programado/salida de Guard por impacto final están desactualizadas.

## Reglas de cámara

1. Hay dos encuadres globales: seguimiento y combate. Sus valores son comunes a todos los personajes. No crear configuración por personaje, DAs visuales, extensiones de UVampsCombatSettings ni una jerarquía de cámaras/perfiles por acción. Se admite un mecanismo pequeño de ajuste de desarrollo para iterar valores comunes; fuera de eso no se requiere editabilidad.
2. Al comenzar realmente un ataque aceptado, el atacante entra al encuadre de combate. No activarlo por una solicitud rechazada.
3. El receptor entra al encuadre de combate cuando bloquea efectivamente. Un golpe abierto no solicita cambio de encuadre ni foco sobre el agresor. No crear planos especiales de Hit, Stun, crítico, NA, dash o sprint.
4. Mantener combate durante la cadena correspondiente, incluidos sus pasos y sus intervalos. Volver suavemente al seguimiento al terminar la cadena porque no continúa o agotó sus pasos: para el atacante, al entrar él en NA; para el receptor que bloqueó, cuando entra en NA el agresor. No salir por terminar un subimpacto o el montage de bloqueo. La cancelación definitiva o invalidación de la cadena debe liberar su referencia de presentación de forma controlada; no dejar un modo esperando un NA que ya no puede llegar. No cambiar reglas de combate para producir esa notificación.
5. Mantener la selección de UVampsLockOnComponent mientras haya enemigos elegibles, incluso lejos. La cámara no crea otra selección ni modifica el recorrido de F. Guardia efectiva sigue imponiendo al agresor mediante el sistema existente. Sin enemigos elegibles, volver gradualmente detrás del jugador; al readquirir, transicionar desde la vista previa. No agregar un umbral de distancia que libere lock-on.
6. Un cambio de encuadre no coloca al target en un píxel fijo ni reinicia el historial en cada inversión de dirección. Preservar continuidad al moverse lateralmente, frenar, invertir, evadir y cambiar target.

## Implementación e integración

- Ampliar AVampsPlayerCameraManager como único dueño del POV local final. La integración analizada es UpdateViewTargetInternal(FTViewTarget&, float); verificar el recorrido en el motor local y evitar que ProcessViewRotation, CalcCamera, BlueprintUpdateCamera, SpringArm u otra ruta compitan con la posición/seguimiento nuevos. Resolver la obstrucción una sola vez.
- Mantener yaw orbital, Eye e Interest persistentes e independientes y transición desde la vista previa. Elegir un ancla estable coherente con la cápsula/mesh actuales. Obtener rotación desde Interest−Eye, sin imponer el pitch fijo anterior. Implementar la composición y el filtro espacial recuperados cuando correspondan; no sustituirlos por modificar únicamente TrackingSpeed ni apilar suavizados indiscriminados.
- Diferenciar el mecanismo del filtro de la política elegida para los dos encuadres de Vamps. FreeCamera es la clase de seguimiento nativa; is_target_mode no equivale a CurrentTarget válido ni prueba una regla exclusiva de ataque. El perfil cercano nativo sirve como referencia visual, con las condiciones y límites de la investigación.
- Elegir y documentar valores iniciales propios para anclas, distancia, alturas, offsets, FOV, radios/ganancias y transiciones. Los números todavía requieren calibración; no detener la implementación para pedir cada valor. Evitar curvas/assets adicionales si una transición simple cubre estos dos encuadres.
- Resolver explícitamente FOV vertical de diseño, FOV horizontal del POV, aspecto de referencia y política de eje efectiva en UE. No copiar las unidades de FGOA como centímetros ni tratar su ganancia por actualización como grados/segundo. Usar DeltaSeconds con una base temporal declarada; no deducir el tick nativo de los 30 fps del video.
- El jugador ya usa bOrientRotationToMovement=true. Conservarlo, WASD relativo al yaw de cámara y la orientación existente de IA. No forzar el cuerpo hacia el target para reparar composición. Move y MobilityPressed deben consumir una referencia de yaw coherente con la vista; conservar la dirección capturada de Shift y su giro instantáneo al iniciar el dash aceptado.
- Se acepta que cámara consuma los resultados de contacto/retarget un frame después: UpdateCameraManager ocurre antes del callback OnWorldPostActorTick usado por el resolver. No cambiar el arbitraje ni duplicar actualizaciones para eliminar ese desfase.
- Usar estados/señales reales de inicio y cierre de cadena/bloqueo. Agregar solamente la comunicación mínima que falte. Limpiar historial/referencias en reset R, cambio de pawn y fin de sesión; R reutiliza y teletransporta los mismos personajes.

## Oclusión

- Proteger la vista tanto del jugador como del target seleccionado; sin target, proteger al jugador.
- Preparar oclusores y materiales propios mínimos con fade gradual y restauración. No modificar materiales compartidos de todos los objetos ni cambiar la colisión de movimiento del personaje/límites.
- Como solución inicial de Vamps, permitir fade en los objetos preparados sin retraer la cámara por ese mismo objeto y usar colisión de cámara ante sólidos no preparados. La consulta de fade debe seguir detectando al oclusor preparado aunque no bloquee ECC_Camera. Esta es una adaptación de Unreal, no una política nativa completamente demostrada.
- Restaurar los recursos afectados al salir de oclusión, cambiar/resetear pawn o terminar la sesión. Preparar en L_CharacterBasic un obstáculo visible con fade y comprobar también sólidos no preparados y límites invisibles. No crear una API extensible de oclusión si no hace falta.

## Ejecución y entrega

- Implementar código y configurar/verificar assets mediante MCP directo soportado, descubriendo antes herramientas y parámetros. No automatizar la UI ni usar scripts del editor como atajo. Una operación sin cobertura directa queda documentada con el paso manual exacto; continuar lo independiente.
- Mantener cambios acotados. No tocar combate, movimiento, animaciones o DA salvo una dependencia concreta de esta cámara. Si un cambio de clase/struct afecta Data Assets, aplicar la preservación completa de AGENTS antes de editar/compilar y comparar/restaurar después del arranque limpio necesario.
- Formatear C++ y revisar nombres contra miembros heredados. Validaciones con diagnóstico no fatal; sin fallbacks no pedidos, comentarios nuevos, tests automatizados ni código del ejemplo Third Person.
- Compilar mediante LiveCodingToolset.LiveCodingToolset.CompileLiveCoding; distinguir parche aplicado de NoChanges. Si hace falta build/reinicio sin soporte MCP, indicar el paso manual y preservar assets antes. No atribuir éxito a una compilación/operación que no se verificó.
- Verificar por MCP configuración y arranque cuando sea posible. Dejar observación manual de lateral/parada/inversión, dash/sprint, ataque/cadena, bloqueo/daño abierto, entrada en NA propia/del agresor, F, pérdida/readquisición, fade/restauración y R. La cámara no se da por terminada solo porque compila.
- Crear la nueva iteración numerada de reporte.md/editor.md, enlazarla desde el índice y esta revisión, y registrar cambios, valores elegidos, operaciones realizadas y límites/pendientes. No editar especificaciones ni roadmap ni cerrar el paso 5 completo: el HUD continúa para otro prompt.
