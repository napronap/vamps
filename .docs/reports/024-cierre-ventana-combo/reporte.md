# 024 — Continuación al cerrar la ventana de combo

Diagnóstico de configuración vigente: [025 — Validación del montage](../025-validacion-montage/reporte.md), con su [guía](../025-validacion-montage/editor.md). Sustituye los cierres por validación de UGA_Attack por rechazo explícito y mensajes detallados.

## Resultado — 2026-10-17

Se conserva un único montage con una sección completa por golpe. El click dentro de ANS_ComboWindow queda pendiente: el salto a la siguiente sección se decide al cerrar la ventana, después de resolver los contactos del mismo frame. Ya no se salta al recibir el click ni al registrar un contacto.

Live Coding compiló y aplicó el cambio con Result: Success. Falta observar las transiciones con las animaciones y ventanas definitivas que está configurando el usuario; esta iteración no cierra la etapa 2.

Continúa [023 — Armas y trazas](../023-armas-y-trazas/reporte.md) y reemplaza el momento de continuación descrito en [022 — Cadena y NA](../022-cadena-y-na/reporte.md). No se modificaron assets, estructuras reflejadas, especificaciones ni roadmap.

## Código y comportamiento

Único archivo de código modificado en esta iteración: Source/vamps/Private/Characters/VampsCharacter.cpp.

- RequestCombatAction conserva la solicitud durante la ventana sin enviarla todavía al resolver. RegisterCombatContact registra el contacto sin solicitar un salto.
- SetCombatComboWindow envía la continuación al cerrar el notify, únicamente si hay una solicitud pendiente. ResolveCombatContinuation rechaza una resolución mientras la ventana sigue abierta y consume la solicitud al cerrarse, aunque el golpe haya fallado.
- Se mantienen los requisitos de contacto abierto o bloqueado y ausencia de restricción ofensiva. Sin continuación válida, sigue la recuperación de la sección actual y luego NA. Se conserva la salida visual a locomoción durante Recovery; esa salida no adelanta la disponibilidad de ataque o defensa.
- UVampsCombatResolver conserva su orden: contactos y restricciones del frame, continuaciones y locomoción. Esto permite contar un contacto del mismo frame de cierre y evita que un salto adelante la resolución defensiva. No fue necesario modificarlo.
- UGA_Attack sigue desactivando los enlaces automáticos de todas las secciones en la instancia de reproducción. La sección siguiente se obtiene por índice temporal y luego se salta por nombre: no depende del enlace Next Section. Se mantienen de una a tres secciones y los nombres son libres.
- Cambiar de sección mantiene la limpieza de solicitud, contacto y registros independientes de las armas. La IA sigue usando la misma solicitud pendiente; no cambian sus reglas ni el DataAsset.

## Conexiones con assets

DA consultado por MCP: /Game/Vamps/Data/DA_VampsCombatSettings. Su AttackMontage continúa apuntando a /Game/Vamps/Animations/AM_BasicAttack; NARecoveryDuration sigue en 0,5 s. No se alteraron sus referencias ni datos.

ANS_ComboWindow existente notifica el cierre a AVampsCharacter. El usuario configura el final de esa ventana en el punto donde desea encadenar, antes de terminar la recuperación y antes del blend-out. Cada sección conserva ANS_AttackTrace para los contactos. Manny, el AnimBP compartido y los Blueprints de armas no requieren cambios para este ajuste de código.

## Verificaciones y límites

- Revisión de las llamadas: la única solicitud de QueueCombatContinuation desde el personaje está ahora en el cierre de SetCombatComboWindow. Se comprobó el orden de resolución y la limpieza al terminar o reiniciar la acción.
- clang-format aplicado al archivo modificado; git diff --check sin errores de whitespace.
- Compilación directa por MCP con LiveCodingToolset.LiveCodingToolset.CompileLiveCoding: Success y parche real aplicado; no fue NoChanges.
- El editor respondió por MCP y no había una sesión PIE activa. No se inició PIE ni se interrumpió la configuración de animaciones del usuario.
- ObjectTools no permite leer CompositeSections del montage actual. No se verificaron por esa vía los nombres, enlaces ni tiempos de sus secciones, ni se modificaron mediante una alternativa de interfaz.
- Pendiente: configuración y observación jugable de los casos de la [guía del editor](editor.md). No se crearon ni ejecutaron tests automatizados.
