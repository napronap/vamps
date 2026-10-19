# Editor — Movilidad integrada al combate

**Para el usuario:** el build/arranque limpio y el recálculo de los ocho montages ya están completados. Quedan la comprobación del impacto final y la observación de gameplay. El DA, las entradas y los montages están configurados y guardados. El paso 4 sigue pendiente de cierre.

## 1. Preservar configuración y compilar

**Hecho — Codex por MCP.** ObjectTools.search_subclasses y AssetTools.find_assets buscaron UVampsCombatSettings y derivadas en todas las raíces: un solo /Game/Vamps/Data/DA_VampsCombatSettings. ObjectTools.list_properties/get_properties capturaron todas sus propiedades en data-assets-antes.json; AssetTools.save_assets guardó el DA antes de editar clase/struct.

**Hecho — Codex por MCP.** LiveCodingToolset.LiveCodingToolset.CompileLiveCoding aplicó un parche Success. La lectura posterior mostró defaults en las entradas ComboSteps; no se guardaron. La captura original conserva arma, transform, montages y escalares.

**Hecho — usuario.** Cerrar sin guardar DA/Blueprints afectados, compilar vampsEditor Development Editor/Win64 y reabrir vamps.uproject. Confirmación recibida en la conversación. No fue un cierre/build ejecutado por Codex.

**Hecho — Codex por MCP.** Tras el arranque: releer el DA, aplicar data-assets-objetivo.json con ObjectTools.set_properties, comparar los campos previos y guardar. Coincidencia completa, sin referencias transitorias. is_dirty=false para el DA y ambos controllers. Una comprobación de Live Coding devolvió NoChanges; ajustes C++ posteriores de validación tuvieron Success.

## 2. Completar la autoría de los montages

**Hecho — Codex por MCP, verificación parcial.** AssetTools.duplicate creó en /Game/Vamps/Animations/Executioner los ocho AM_Executioner_Dash_Fw, FR, R, BR, Bw, BL, L y FL a partir del montage propio AM_Executioner_Block. ObjectTools.set_properties configuró DefaultSlot con un segmento del clip A_GreatSwordCombatAnimationV1_Dash_<dirección>_RM de /Game/GreatSwordAnimV1/Animation/RM, rango 0–1 s, StartPos=0, Rate=1 y LoopingCount=1. Blend In=0,06 s, Blend Out=0,08 s, RateScale=1 y Auto Blend Out activo. Se guardaron los ocho assets. No se cambió el pack fuente ni se copió lógica Third Person.

**Hecho — usuario, verificado por Codex mediante MCP.** Los ocho montages ahora tienen duración 1 s y is_dirty=false; sus segmentos conservan los clips direccionales correspondientes, rango 0–1 s y velocidad 1. [Captura](dash-montages-mcp.json). Pasos reproducibles:

1. Abrir el asset en el Montage Editor. Verificar un único segmento en DefaultSlot y una sección Default iniciada en cero.
2. Eliminar el segmento y arrastrar desde Asset Browser el clip Dash con el mismo sufijo direccional al inicio del slot. Dejar StartPos=0, rango de clip=0–1 s, Rate=1 y una repetición.
3. Confirmar que el editor recalcula la duración total a 1 s, sin cola vacía hasta 1,7667 s. Verificar que no haya notifies de ataque en estos montages. Conservar los blends anteriores y guardar.
4. Repetir en Fw/FR/R/BR/Bw/BL/L/FL. La comprobación visual con skeleton/mesh de Executioner para observar pose, dirección y blending sigue pendiente; la captura acredita configuración y guardado.

sequenceLength fue rechazado por MCP como propiedad de solo lectura; el recálculo se dejó al usuario porque no existe una herramienta directa de montaje/recálculo en el catálogo descubierto. No se aplicó un atajo por código. El dash diagnostica y rechaza longitudes inconsistentes antes de cobrar energía o abandonar Guard.

## 3. Entrada, datos y locomoción

**Hecho — Codex por MCP.** Se duplicó /Game/Vamps/Input/IA_Combat como IA_Mobility: Boolean, sin triggers/modifiers. Se agregó LeftShift → IA_Mobility en IMC_Vamps y en IMC_VampsCombatTest, conservando los demás mappings. En Class Defaults de BP_VampsPlayerController y BP_VampsCombatTestPlayerController, MobilityAction=IA_Mobility. Ambos Blueprints compilados mediante BlueprintTools.compile_blueprint y guardados.

**Hecho — Codex por MCP.** En DA_VampsCombatSettings, DashMontages tiene ocho entradas Fw/FR/R/BR/Bw/BL/L/FL. Energía 100, coste dash 25, sprint 20/s, recuperación 25/s después de 1 s, dash 350 cm/0,3 s, sprint 700 cm/s. Cada paso agrega avance 250 cm/0,25 s y detención 130 cm. Conserva alineación/empuje previos 130/100, 130/100 y 150/250, reacción 1 s y NA 0,5 s.

**Hecho — Codex por MCP.** En BS_Executioner, eje Speed hasta 700 y cuarta muestra Run_Fw_IP en 700 con RateScale=1,6667; las muestras anteriores en 0/180/420 permanecen. ABP_Executioner sigue usando el slot existente y RootMotionFromMontagesOnly. No se modifican sus graphs.

## 4. Verificar la autoría del impacto final

**Pendiente — usuario.** Abrir /Game/Vamps/Animations/Executioner/AM_Executioner_Combo y revisar cada sección frente a ComboSteps del DA. FinalImpactWeaponIndex es el índice en Weapons; con la greatsword actual es 0. FinalImpactIndex es cero para el primer subimpacto y aumenta por cada AN_ResetWeaponHits que reinicia esa arma dentro de la sección.

La configuración inicial es 0/0 por paso. Si hay reinicios para esa arma, configurar el último índice y comprobar visualmente que corresponde al golpe de cierre con knockback. UGA_Attack informa en LogVampsCombat el número de reinicios autorados si el dato no coincide. No usar el número de paso de combo como índice de impacto. Un arma distinta puede ser la que expresa el cierre en una configuración con varias armas; elegirla expresamente.

Los notifies de las animaciones fuente no sustituyen los del montage. Conservar ANS_AttackTrace, ANS_ComboWindow y los reinicios existentes. No reconstruir su configuración desde supuestos. Guardar únicamente el montage/DA si se ajustan.

## 5. Comprobar el comportamiento integrado

**Hecho — Codex por MCP, comprobación limitada de arranque.** EditorAppToolset.StartPIE, SceneTools.find_actors, ObjectTools.get_properties y LogsToolset.GetLogEntries verificaron jugador/rival vivos, Idle, energía 100/100, guardia 0, permiso general de dash y ausencia de errores de combate en la consulta. No se introdujeron pulsaciones ni contactos. StopPIE e IsPIERunning confirmaron el final de la sesión. La lista siguiente sigue pendiente de observación real.

**Pendiente — usuario.** Abrir /Game/Vamps/Maps/L_CharacterBasic, iniciar Play y observar HUD/Output Log con LogVampsCombat. RMB solicita combate, F cambia target, F6 cambia modo de IA y R reinicia.

1. Atacar desde cerca y lejos: avance máximo 250 cm por paso, golpe corto posible, sin suma adicional de root motion. Repetir frente a pared y rival alejándose. Cambiar/perder target durante avance no debe producir teletransporte ni persecución renovada.
2. Shift con WASD cardinal/diagonal y sin dirección: dirección solicitada y retroceso alejándose del target. Sin target, usar dirección explícita; el retroceso sin target se rechaza con diagnóstico.
3. Mantener Shift/dirección tras dash para sprint; soltar para terminar intención. Energía baja una vez 25 por dash y 20/s en sprint, se recupera 25/s tras 1 s y no cambia GuardLoad. Insuficiencia rechaza dash; agotamiento termina sprint sin stun/ruptura.
4. En multihit, Shift antes del golpe final bloqueado debe rechazarse. Después del final, puede salir sin esperar el plazo de Guard o fin del empuje. Si el final falla, Guard termina por su plazo normal. Repetir con otro agresor y con ruptura/daño simultáneo: una nueva reacción invalida el permiso anterior.
5. Durante NA, dash/sprint mantienen su plazo y ataque/defensa continúan prohibidos. Durante Hit/Stun/muerte no se permite dash. Fuera de esas restricciones, iniciar ataque durante dash/sprint debe sustituir movimiento y mezclar animación correctamente.
6. Solicitar dash dentro de la ventana y pedir continuación válida: gana el siguiente ataque y no se ejecuta un dash residual. Sin continuación, salir en la frontera conservando Recovery y NA. Pulsaciones tempranas fuera de ventana no se guardan para después.
7. Recibir contacto durante dash: no hay invulnerabilidad; un contacto válido causa el resultado correspondiente. R/muerte/cambio de posesión limpian desplazamiento, intención y permiso. Cámara/selección conservan seguimiento.

Estas observaciones no se consideran aprobadas por compilar o leer propiedades. Registrar resultados y valores ajustados antes de cerrar 4. Detener PIE al terminar.
