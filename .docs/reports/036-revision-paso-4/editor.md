# Editor — Revisión del paso 4

**Para el usuario:** esta revisión no requiere cambiar ni compilar nada en Unreal. Se incorporaron tus decisiones y se revisó su encaje con los fuentes; el paso 4 sigue pendiente de implementación. Los [detalles restantes](reporte.md#decisiones-incorporadas-y-detalles-pendientes) se explicitan en el prompt de ejecución. Los pasos futuros se distinguen de las lecturas MCP previas; no hubo una nueva inspección del editor al revisar los comentarios.

## 1. Consultar entrada y presentación existentes

**Hecho — Codex por MCP.** AssetTools.load_asset/find_assets/get_dependencies y ObjectTools.list_properties/get_properties permitieron comprobar referencias y datos, sin modificar assets.

Para reproducir en el editor:

1. Abrir /Game/Vamps/Blueprints/BP_VampsPlayerController y revisar Class Defaults: IMC_Vamps, IA_Move, IA_Combat e IA_CycleTarget.
2. Abrir /Game/Vamps/Input/IMC_Vamps: WASD, RMB y F; no hay asignación de Shift.
3. Abrir /Game/Vamps/Animations/Executioner/ABP_Executioner: Root Motion Mode=RootMotionFromMontagesOnly.
4. Abrir /Game/Vamps/Animations/Executioner/BS_Executioner: Idle=0, Walk_Fw=180, Run_Fw=420; eje Speed de 0 a 420.
5. Abrir AM_Executioner_Combo en esa carpeta: sus cinco segmentos de AttackCombo7 no equivalen a cinco pasos. Stage1 tiene Enable Root Motion activo; no se midió aquí cuánto desplaza.

La lectura DSL de AnimGraph devolvió vacío y no valida sus nodos/transiciones. La lectura de compositeSections fue rechazada por no estar expuesta en ObjectTools; no se verificaron secciones por esa vía.

## 2. Confirmar parámetros del paso 3

**Hecho — Codex por MCP.** ObjectTools.get_properties releyó /Game/Vamps/Data/DA_VampsCombatSettings: pasos con alineación/empuje 130/100, 130/100, 150/250 cm y reacción de 1 s; NA=0,5 s; capacidad/carga=100/25; recuperación=20/s después de 1 s; stun=2 s. Se conservó su configuración.

## 3. Seleccionar y comprobar clips al implementar

**Pendiente — usuario, al implementar.** En /Game/GreatSwordAnimV1/Animation/RM, previsualizar Dash_Fw/R/L/FR/FL/Bw/BR/BL y Dodge disponibles para elegir el gesto. Dash_Fw_RM dura 1 s y tiene Enable Root Motion desactivado, confirmado por MCP; no cambiarlo aún por su nombre. Contrastar skeleton y pose con SKM_Manny_Simple/ABP_Executioner y elegir las variantes que preservan el facing si esa es la regla acordada.

**Pendiente — Codex por MCP, al implementar.** Descubrir las herramientas directas necesarias para crear/asignar entrada y montages propios, configurar propiedades y consultar resultados. Si autoría de secciones, notifies o transiciones no tiene cobertura directa, dejar esa operación concreta al usuario con intervalos y assets elegidos. No automatizar UI ni usar scripts del editor como atajo.

La selección visual no se realizó y no es requisito para terminar esta revisión. No hay animaciones nuevas creadas ni referencias nuevas asignadas.

## 4. Preservar los Data Assets antes del cambio estructural

**Pendiente — Codex por MCP, al implementar.** Buscar todos los assets de UVampsCombatSettings y sus derivadas antes de editar su clase o structs. Capturar en archivo todos sus valores completos, referencias, arrays y transforms; guardar esos assets por MCP. Después de la compilación/arranque limpio necesario, releer, restaurar valores previos e incorporar los nuevos acordados. Comparar y guardar únicamente los afectados. La lectura parcial de esta revisión no sustituye esa captura.

Si una propiedad no puede leerse/restaurarse, documentar y resolver su conservación manual antes de continuar el cambio que pueda perderla. Live Coding debe usar LiveCodingToolset.LiveCodingToolset.CompileLiveCoding y distinguir NoChanges de parche aplicado. Si el reinicio/build necesario no puede hacerse por MCP, dar el paso manual exacto al usuario.

## 5. Observar el resultado cuando exista la implementación

**Pendiente — usuario, al implementar.** En /Game/Vamps/Maps/L_CharacterBasic, con los valores elegidos y el HUD de observación:

1. Atacar desde cerca y fuera del máximo de avance; comprobar golpe al aire y corte de cadena por fallo. Repetir frente a pared y con rival que se aleja.
2. Evadir en direcciones cardinales/diagonales y sin dirección; mantener/soltar Shift y cambiar dirección según la política acordada. No atravesar sólidos ni sumar empuje/avance incompatibles.
3. Solicitar evasión antes/durante/después de la frontera del paso y junto con RMB. La continuación válida gana; no queda un dash residual tardío. Conservar contactos ya capturados y recuperación/NA.
4. Durante Guard comprobar rechazo entre subimpactos y habilitación al registrar el impacto final autorado con knockback, sin esperar además ReactionEndsAt. Durante NA permitir dash sin recuperar ataque/defensa. Mantener restricciones de Hit/Stun y verificar el caso de otro agresor.
5. Consumir/recuperar energía de movilidad y comprobar que GuardLoad permanece independiente. No hay invulnerabilidad: un contacto geométricamente válido puede golpear durante el dash. Fuera de restricciones previas, ataque/defensa durante dash/sprint deben blendar correctamente. Shift sin dirección debe alejar del target.
6. Comprobar seguimiento lateral y sprint, F y pérdida de target, presentación direccional, daño/interrupción durante movilidad y limpieza por R/muerte/cambio de posesión.

Son comprobaciones futuras, no resultados obtenidos. No se inició PIE ni se compiló en esta revisión.
