# Editor — Verificación del avance del plan

**Para el usuario:** esta revisión no exige asignar ni guardar assets. Queda confirmar la integración de movilidad/cámara en `/Game/Vamps/Maps/L_CharacterBasic`; el HUD de amenazas/proximidad requiere implementación posterior. No hay herramientas MCP directas de Unreal disponibles en esta sesión, por lo que no se hizo una nueva comprobación del editor.

## 1. Revisar configuración actual

**Pendiente — usuario.** Abrir `/Game/Vamps/Data/DA_VampsCombatSettings` y comprobar sus referencias actuales de ataque, reacción y dash frontal/retirada. El commit más reciente modifica este asset; los números de 039 no se consideran una lectura vigente. No restaurar valores anteriores por suposición.

En `/Game/Vamps/Maps/L_CharacterBasic`, revisar que el modo de prueba usa los Blueprints propios y que su controller referencia `/Game/Vamps/Blueprints/BP_VampsPlayerCameraManager`, según la [configuración documentada en 045](../045-estado-camara/editor.md#1-estado-recibido-y-configuración). No se requieren cambios nuevos derivados de esta revisión.

## 2. Confirmar movilidad integrada

**Hecho — usuario, confirmación parcial histórica.** 040 registra que el usuario confirmó el funcionamiento del dash con root motion de 039. No equivale a aprobar el giro nuevo ni todos los casos integrados.

**Pendiente — usuario.** Iniciar Play en L_CharacterBasic y seguir la [observación direccional de 040](../040-giro-direccional-dash/editor.md#3-observar-control-direccional): WASD gradual, WASD+Shift con giro al iniciar dash, diagonales, retirada sin dirección y prioridades de continuación/reacciones. Completar [039, paso 5](../039-simplificacion-movilidad/editor.md#5-observar-el-comportamiento) para sprint/energía, acercamiento solo inicial, NA, colisión, interrupciones y reset. Para el dash direccional prevalece 040 sobre el punto 1 de esa guía anterior; contrastar recursos con el DA vigente, sin imponer sus números históricos.

Resultado esperado: cápsula y mesh se desplazan juntos, los permisos y prioridades de combate se conservan y no quedan acciones o velocidades residuales. No hace falta configurar un impacto final ni ocho montages: esa lógica se retiró en 039.

## 3. Confirmar cámara y oclusión

**Pendiente — usuario.** Completar [045, validación visual integrada](../045-estado-camara/editor.md#3-validación-visual-integrada): seguimiento, planos de ataque/bloqueo, salida por NA de la cadena correspondiente, cambio/pérdida de target, fade/restauración, pared sólida, límite invisible y reset.

Para el seguimiento sin target prevalece [046, giro libre](../046-giro-libre-camara/editor.md#3-observar-el-giro-libre) sobre el punto 2 de 045: F7 para quitar lock-on, sostener A/D o S con espacio de recorrido, detenerse y readquirir rival. Resultado esperado: giro progresivo sin saltos y conservación del rumbo al parar. Detener Play al terminar y registrar cualquier ajuste necesario.

## 4. Continuación del paso 5

Los avisos de amenaza previa al contacto, dirección y proximidad siguen sin implementar; no se resuelven con asignaciones del editor. Después de su desarrollo y de las comprobaciones anteriores, registrar el resultado y el cierre que corresponda. La revisión actual no marca nuevas etapas completadas.
