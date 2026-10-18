# 024 — Configurar el salto al cierre de la ventana

Si el montage no puede iniciar, aplicar [025 — Diagnosticar el montage](../025-validacion-montage/editor.md): las validaciones de configuración ahora cancelan el ataque e identifican el problema en Output Log.

**Pendiente para el usuario:** terminar las secciones y notifies del montage elegido, asignarlo al DA y observar los casos del paso 3. El código ya compiló y se aplicó en el editor abierto mediante Live Coding. No hace falta recargar el nivel para aplicar este cambio de funciones.

La configuración de armas sigue en [023](../023-armas-y-trazas/editor.md); root motion, movimiento, distancias y reacción siguen en [022](../022-cadena-y-na/editor.md). Esta guía reemplaza el momento de salto indicado allí.

## 1. Aplicar el código

**Hecho — Codex por MCP:** LiveCodingToolset.LiveCodingToolset.CompileLiveCoding devolvió Result: Success y aplicó un parche real. No se cambiaron campos reflejados ni fue necesario guardar Blueprints para este ajuste.

Recorrido reproducible en el editor: con Live Coding habilitado, compilar con Ctrl+Alt+F11 y esperar una compilación correcta. La compilación completa y reapertura habitual incorporarán también este código al ejecutable persistente.

## 2. Preparar el montage único

**Pendiente — usuario:** el DA /Game/Vamps/Data/DA_VampsCombatSettings tiene actualmente AttackMontage = /Game/Vamps/Animations/AM_BasicAttack. Abrir el montage que se usará y asignarlo a ese campo cuando esté listo.

1. Colocar los clips completos en orden, con ataque y recuperación. Crear una sección al inicio de cada clip; la primera comienza en cero. Por ejemplo: Attack1, Attack2 y Attack3. Se admiten de una a tres secciones. Los nombres no son obligatorios; el orden temporal define la cadena.
2. En Montage Sections, dejar cada sección sin enlace automático a otra, con Next Section = None. Esto no elimina las secciones siguientes: siguen disponibles para el salto explícito. El código además desconecta los enlaces de la instancia al comenzar el ataque.
3. En cada sección, colocar exactamente un ANS_AttackTrace sobre el tramo de contactos, después de la preparación y antes de la recuperación final.
4. En cada sección anterior a la última, colocar un ANS_ComboWindow. Su inicio debe ser posterior al inicio de Attack Trace y su final posterior al final de Attack Trace. El final de Combo Window marca la decisión de encadenar: ubicarlo al terminar el gesto que debe completarse antes del siguiente ataque.
5. Mantener el final de Combo Window dentro de su sección y antes del comienzo del blend-out, dejando recuperación posterior. La última sección puede omitir Combo Window. Conservar Auto Blend Out habilitado, velocidad positiva constante y sin Time Stretch Curve.
6. Guardar el montage y su asignación en el DA. Los tiempos dependen de los clips elegidos; no se copiaron duraciones manuales al DA.

ObjectTools no pudo leer CompositeSections. La configuración de secciones y notifies se deja al usuario porque no hay una operación MCP directa disponible para completarla; no se automatizó la interfaz.

## 3. Observar la cadena en PIE

**Pendiente — usuario:** con las armas y distancias calibradas según 023 y 022:

1. Conectar el primer golpe y pulsar dentro de Combo Window. El gesto debe continuar hasta el cierre; recién allí debe comenzar la sección siguiente, sin NA entre golpes. Repetir para el segundo paso.
2. Conectar sin pulsar para continuar. Sin otra interrupción ni salida visual por movimiento, debe verse toda la recuperación, detenerse en esa sección y entrar en NA, sin pasar automáticamente al siguiente clip.
3. Pulsar dentro de la ventana pero fallar el golpe. Al cierre se descarta la continuación; un contacto posterior no debe recuperar ese input.
4. Pulsar fuera de la ventana. Ese click no debe quedar guardado para un paso posterior.
5. Comprobar continuación tras un impacto bloqueado y que una restricción ofensiva propia impida encadenar. Al cerrar ventana, la resolución de contactos del frame tiene prioridad sobre el salto.
6. Confirmar que el último golpe termina sin reiniciar la cadena y que NA conserva la duración del DA. Moverse durante Recovery puede liberar la animación sin habilitar antes ataque o defensa.

Codex verificó compilación y flujo de código; estos casos todavía no se observaron con las animaciones definitivas.
