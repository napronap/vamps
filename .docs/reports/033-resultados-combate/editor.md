# Editor — Resultados de daño y defensa

**Al volver:** abrir PIE y observar los intercambios indicados en 5; después calibrar el DA y, si se desean poses de reacción, conectar las señales en el AnimBP asignado al mesh. No hace falta recompilar, reiniciar ni restaurar configuración para empezar: el editor queda abierto, PIE detenido y DA guardado. El paso 3 sigue pendiente de ese cierre manual.

## 1. Captura y preparación

**Hecho — Codex por MCP.** AssetTools.find_assets y ObjectTools.search_subclasses encontraron un único DA VampsCombatSettings, sin derivadas: `/Game/Vamps/Data/DA_VampsCombatSettings`. ObjectTools.list_properties/get_properties leyeron todas las propiedades, arrays y transforms; AssetTools.save_assets lo guardó antes del cambio. [Captura](data-assets-antes.json).

La comprobación de cambios pendientes del conjunto Vamps/abiertos cubrió 35 assets y no encontró dirty assets. La consulta global previa falló por una referencia externa inexistente; no se asumió una auditoría global completa. No se guardaron indiscriminadamente otros assets.

## 2. Cierre, build y reapertura

**Hecho — Codex por shell, con la excepción autorizada en este pedido.** Se identificó el editor de vamps por PID y línea de comandos. CloseMainWindow pidió cierre ordenado; salió en 3 s, sin terminar el proceso a la fuerza. PIE no estaba activo.

Para reproducir manualmente: guardar lo necesario, cerrar Unreal y ejecutar `C:\dev\vamps\build.bat`. El build completo del cambio estructural terminó con código 0 y `Result: Succeeded` en 46,14 s. Solo entonces se ejecutó `C:\dev\vamps\editor.bat`; cargó `/Game/Vamps/Maps/L_CharacterBasic` y MCP volvió a responder. Los plazos fueron 60 s para cierre, 15 min para build y 5 min para carga/reconexión; no se agotaron.

Los ajustes finales de IA/movimiento y del montage opcional de daño no cambiaron tipos reflejados. **Hecho — Codex por MCP:** LiveCodingToolset.LiveCodingToolset.CompileLiveCoding compiló y aplicó dos parches reales con Success (patch_0 y patch_1). No fueron NoChanges. Logs y estados en el [reporte](reporte.md).

## 3. Restauración y configuración del DA

**Hecho — Codex por MCP.** Después de comprobar la clase nativa estable, ObjectTools.get_properties/set_properties releyó y reaplicó los valores preservados y los campos nuevos. AssetTools.save_assets guardó únicamente el DA; is_dirty=false y comparación exacta de las 14 propiedades completas. La relectura posterior a PIE/Live Coding siguió coincidiendo.

Para reproducir o calibrar, abrir Content Browser → `/Game/Vamps/Data/DA_VampsCombatSettings`:

| Campo | Valor de entrega |
|---|---|
| Weapons | Una BP_VampsGreatSword, socket HandGrip_R, transform identidad |
| Attack Montage | `/Game/Vamps/Animations/Executioner/AM_Executioner_Combo` |
| Combo Steps | Tres entradas: alineación/empuje 130/100, 130/100 y 150/250 cm |
| Contact Reaction Duration de cada paso | 0,5 s totales; el campo adicional anterior fue retirado |
| NA Recovery Duration | 0,5 s |
| Hit Reaction Montage | None, preservado |
| Critical Damage / Critical Chance Percent | 24 / 10 |
| Block Damage Reduction | 10, reducción plana |
| Guard Capacity / Guard Load Per Hit | 100 / 25 |
| Guard Recovery Delay / Guard Recovery Per Second | 1 s / 20 |
| Stun Duration / Defense Request Duration | 2 s / 0,5 s |

Mantener orden de Weapons, referencias y transforms salvo que se quiera cambiar el equipamiento. No copiar el antiguo adicional como si fuera un total calculado. Guardar solo el DA editado. [Valores finales completos](data-assets-despues.json).

## 4. Arranque de observación

**Hecho — Codex por MCP, alcance parcial.** EditorAppToolset.StartPIE inició el mapa; SceneTools.find_actors y ObjectTools.list_properties/get_properties confirmaron dos personajes, una espada cada uno, HP 100/100, carga 0/100, Idle, Reaction=None y movimiento/inicio disponibles. No se produjeron errores nuevos de combate, Blueprint, PIE o fatales. StopPIE e IsPIERunning confirmaron salida y false.

Esto comprueba inicialización y lectura de estado; no certifica contactos ni reacciones. No se simularon clicks, se manipularon atributos ni se ejecutó un arnés.

## 5. Intercambios y calibración

**Pendiente — usuario.** En `/Game/Vamps/Maps/L_CharacterBasic`, iniciar PIE. RMB sigue siendo pulsación individual; F cambia target, F6 recorre los modos de IA y R reinicia el combate de observación. Filtrar Output Log por LogVampsCombat. El HUD muestra Reaction, Reaction Time, NA, MOVE, Guard Load, Damage, Critical y la interacción defendida.

1. Con ataque de IA, bloquear mientras se prepara un ataque propio: debe cancelarse la ofensiva propia y pasar a Guard, sin trazas nuevas ni NA causada por la interrupción. El agresor puede continuar. Guard/Hit deben impedir WASD y ataque durante la reacción; el empuje sigue respetando colisión.
2. Verificar daño abierto normal 12 y defendido normal 2 con los valores actuales. El crítico de 24 debe resultar en 14 al bloquear. Para aislar visualmente el crítico, se puede poner Critical Chance Percent=100 en el DA antes de PIE y devolverlo a 10 al terminar; esto es una comprobación manual, no un cambio realizado por Codex.
3. Observar la barra: cada contacto bloqueado suma 25. El cuarto bloqueo consecutivo desde cero debe llenar 100 y entrar en Stun durante 2 s; ese contacto aplica residual. Después no admite defensa, y al salir la carga queda en cero. Fuera de reacción, tras 1 s desde el bloqueo, la carga baja a 20/s.
4. En las secciones que ya tengan AN_ResetWeaponHits, comprobar que los subimpactos del mismo paso mantienen la interacción defensiva sin otro click y renuevan 0,5 s desde cada contacto. Un fallo no renueva. El paso siguiente tiene identidad nueva y exige otra pulsación válida. Con poca vida, el residual puede llevar HP a cero.
5. Comprobar continuidad de cadena al golpear abierto o bloqueado y NA tras su cierre normal. Recibir daño no crea otra NA ni elimina una NA que ya estaba activa. R debe limpiar reacción, carga, memoria, montages, movimiento y contactos previos.
6. Cuando se observe una situación con varios agresores, comprobar A→B→A: bloquear B requiere pulsación, cambia target y elimina la memoria de A; otro impacto de A exige pulsar otra vez. La escena de esta entrega tiene un solo rival; Codex no agregó una configuración de varios agresores ni certifica ese intercambio en ejecución.

Calibrar Contact Reaction Duration y el espaciado de los contactos de las animaciones como conjunto; todos los subimpactos del paso comparten duración. No se garantiza que el reposicionamiento convierta un fallo en acierto. Revisar visibilidad de HUD y tiempos antes de cerrar el paso 3.

## 6. Presentación de animación

**Pendiente — usuario, si se requieren poses y clips de reacción.** No se asignaron animaciones nuevas. Abrir el BP del personaje, seleccionar Mesh → Animation → Anim Class y abrir el AnimBP realmente asignado. En `/Game/Vamps/Animations/` existen ABP_VampsCharacter y Executioner/ABP_Executioner; comprobar la asignación en vez de asumir cuál está en uso.

Las señales nativas disponibles son bIsGuarding (Guard efectivo), bIsHitReacting, bIsStunned y CombatReactionRemaining. Usarlas para las poses y transiciones correspondientes, conservando la lógica propia del proyecto. La disponibilidad bCanDefend no debe elegir una pose de bloqueo efectivo.

Para daño abierto puede asignarse un HitReactionMontage compatible con el skeleton/slot del mesh, distinto de AttackMontage, con Auto Blend Out y RateScale constante positivo, sin Time Stretch Curve. El código ajusta su velocidad al plazo de reacción compensando RateScale, detiene la reproducción anterior al sustituir reacción y elimina su root motion para no duplicar desplazamiento. El valor de entrega sigue siendo None; no se inventó una ruta de animación ni se usó otro montage como sustituto. Guardar únicamente los assets editados y observar que la presentación no prolongue los permisos lógicos.
