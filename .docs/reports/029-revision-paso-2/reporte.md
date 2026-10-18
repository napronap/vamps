# 029 — Revisión del paso 2 frente al contraste FGOA

El cierre posterior de 2 y las reglas elegidas para 3 se registran en [031](../031-cierre-paso-2-y-reglas-paso-3/reporte.md). Esta revisión conserva sus resultados históricos; las decisiones vigentes están incorporadas en las especificaciones de combate.

La implementación elegida continúa en [030 — Reinicio opcional de impactos por arma](../030-reinicio-impactos/reporte.md): un solo AttackTrace y AN con índices explícitos del DA. Esta revisión conserva la comparación anterior; sus alternativas no son un estado pendiente de esa implementación.

18 de octubre de 2026. Revisión de [028 — Contraste FGOA y Vamps](../028-contraste-fgoa/analisis.md) contra los fuentes actuales y los límites de las etapas 2–4 del [roadmap](../../roadmap.md). Se aplicó la skill [game-feature-docs](../../../.agents/skills/game-feature-docs/SKILL.md) para distinguir reglas vigentes, implementación y propuestas de la investigación.

## Resultado

La cadena y NA sirven como base. Para el alcance temporal acordado de un impacto por animación, el núcleo está implementado; el contraste no obliga a rehacerlo. Sin embargo, el roadmap y el funcional ya distinguen varios impactos dentro de un paso: **conviene completar ese soporte en 2 antes de darlo por cerrado con ese alcance**. La reacción defensiva, sus permisos y tiempos pertenecen a 3; evasión y acercamiento siguen en 4.

Esta revisión no cierra la etapa ni certifica nuevamente su comportamiento en PIE. No hubo cambios de código, clases, Blueprints, Data Assets ni configuración. Solo se agregaron este reporte y su [guía](editor.md), enlaces desde el índice y una referencia desde 028. No se modificaron las especificaciones ni el roadmap.

## Ajuste recomendado en 2: paso y ventana de impacto separados

Actualmente [GA_Attack](C:/dev/vamps/Source/vamps/Private/Combat/Abilities/GA_Attack.cpp:95) rechaza un segundo ANS_AttackTrace en una sección. [SetCombatContacts](C:/dev/vamps/Source/vamps/Private/Characters/VampsCharacter.cpp:391) exige Preparation al abrir y pasa a Recovery al cerrar: quitar solamente aquella validación tampoco permitiría una segunda ventana. [BeginCombatStep](C:/dev/vamps/Source/vamps/Private/Characters/VampsCharacter.cpp:276) limpia las víctimas una vez por sección; [VampsWeapon](C:/dev/vamps/Source/vamps/Private/Combat/VampsWeapon.cpp:84) las conserva hasta el próximo paso.

La ampliación debe permitir varios impactos sucesivos en una misma sección sin convertirlos en varios pasos ni liberar la recuperación entre ellos. Cada oportunidad de impacto necesita su registro de víctimas; cualquier contacto válido sigue habilitando la continuación del paso. Si la primera falla y la segunda acierta, la segunda conserva su perfil autorado. Los valores de alineación y empuje deben poder distinguir impactos cuando la animación lo requiera. El cierre de Combo Window debe ocurrir después del último contacto habilitante.

**Conservar independencia por arma.** La propuesta 12.2 de 028 comparte víctimas entre armas de una ventana. Eso difiere del requisito explícito para dual swords. Para Vamps se recomienda deduplicar por arma, ventana y víctima: dos espadas pueden contactar al mismo rival, y una espada puede volver a hacerlo en otra ventana. No hace falta separar artificialmente dos ventanas simultáneas para expresar dos armas. No se cambió el texto de la investigación.

Puede conservarse el serial local del paso para su responsabilidad actual y añadir la identificación necesaria de ventana. Qué continuaciones comparten identidad defensiva se decide al diseñar Guard en 3; no hace falta introducir ahora un administrador global de IDs ni copiar el formato nativo.

### Alternativa propuesta: AN de reinicio de impactos

Con el código actual, conservar un ANS_AttackTrace y usar un AN que limpie víctimas es el cambio más pequeño. Mantiene Preparation→Contacts→Recovery y la validación de un ANS por sección. El reinicio afecta solo al registro del impacto, sin borrar el contacto habilitante ni avanzar ComboStep. ResetSectionHits no se puede llamar sin cambios: también desactiva tracing.

Para varias armas, el AN debería permitir elegir explícitamente todas las armas o entradas concretas del array Weapons. Un reinicio global antes del segundo golpe de una espada también habilitaría otro impacto de la espada que sigue tocando al rival. La selección evita ese reinicio accidental, pero no cambia qué armas tienen tracing activo: actualmente el ANS las activa a todas.

La diferencia funcional es que el AN renueva la posibilidad de impactar mientras las trazas siguen activas, incluso entre los dos gestos. Si el arma sigue dentro del receptor, puede volver a golpear inmediatamente tras el reinicio. Varios ANS permiten intervalos separados sin daño y, añadiendo selección, activaciones distintas por arma; requieren separar la fase del paso de cada apertura/cierre y gestionar ventanas simultáneas. La elección queda pendiente: AN de reinicio para contactos continuos; ANS separados si las animaciones necesitan huecos sin daño o activación precisa de cada arma. No se implementó ninguna alternativa.

## Conservar del paso 2

- Un montage, secciones ordenadas y salto al cerrar Combo Window. [ResolveCombatContinuation](C:/dev/vamps/Source/vamps/Private/Characters/VampsCharacter.cpp:444) exige input, contacto y disponibilidad; [GA_Attack](C:/dev/vamps/Source/vamps/Private/Combat/Abilities/GA_Attack.cpp:227) desactiva los enlaces automáticos. La investigación no aporta una razón para cambiar a montages individuales.
- Contacto abierto o bloqueado contra cualquier rival habilita al atacante, sin imponer retarget ofensivo. El [resolver](C:/dev/vamps/Source/vamps/Private/Combat/VampsCombatResolver.cpp:142) resuelve contactos antes de continuación y locomoción.
- NA distinta de recuperación visual y restricción ofensiva. [FinishCombatAction](C:/dev/vamps/Source/vamps/Private/Characters/VampsCharacter.cpp:313) conserva el plazo lógico cuando locomoción libera el montage; NA impide ataque/defensa y permite movimiento.
- BP por arma con trazas, DA por personaje y [componente de movimiento](C:/dev/vamps/Source/vamps/Private/Combat/VampsCombatMovementComponent.cpp:10) para alineación, empuje y root motion con colisión. No se encontró necesidad de reemplazar esas responsabilidades.

## Cambios que corresponden a 3

| Tema | Código actual y cambio recomendado al incorporar la reacción |
|---|---|
| Guardia efectiva | [ReceiveCombatHit](C:/dev/vamps/Source/vamps/Private/Characters/VampsCharacter.cpp:735) registra bloqueo y retorna. La restricción impide inicios y continuaciones, pero no apaga los contactos del ataque ya activo. Guard debe tener contexto propio y sustituir la ofensiva incompatible si se adopta ese comportamiento de FGOA. Es una revisión de la regla anterior de conservar ambas ofensivas, no un error contra aquella regla. |
| Tiempo defensivo | [QueueCombatContact](C:/dev/vamps/Source/vamps/Private/Combat/VampsCombatResolver.cpp:62) calcula sección completa a velocidad efectiva más adicional. [ApplyOffensiveRestriction](C:/dev/vamps/Source/vamps/Private/Characters/VampsCharacter.cpp:570) conserva el mayor vencimiento. La referencia reconstruida usa duración por impacto y mínimo de reacción, y reemplaza el calendario con cada contacto aceptado. Para buscar fidelidad, sustituir este modelo al implementar Guard; seguir calibrando solo el adicional no reproduce esa regla. No copiar los 42 frames como segundos de balance. |
| Permisos y presentación | La restricción actual no bloquea caminar durante todo el plazo. El componente solo controla el breve desplazamiento; [AnimInstance](C:/dev/vamps/Source/vamps/Private/Animation/VampsAnimInstance.cpp:35) expone bCanDefend como bIsGuarding. Guard necesita permisos de reacción y una señal de bloqueo efectivo distinta de elegibilidad; la salida especial por evasión se integra en 4. |
| Interrupción y NA | [OnInterrupted](C:/dev/vamps/Source/vamps/Private/Combat/Abilities/GA_Attack.cpp:208) termina por la misma ruta que el fin normal. Si el montage de daño reproduce e interrumpe el ataque, esa ruta puede iniciar NA. Separar interrupción por reacción y cierre normal al integrar Guard/Hit: recibir daño o bloquear no debe crear NA por el mero reemplazo visual. Es una consecuencia estática condicional; no se comprobó la asignación actual de HitReactionMontage. |
| Contactos ya encolados | [ActionNumber](C:/dev/vamps/Source/vamps/Private/Combat/VampsCombatResolver.cpp:211) protege el registro de contacto habilitante, pero ReceiveCombatHit queda fuera de esa condición. Al introducir interrupciones, definir expresamente qué contactos capturados sobreviven y cómo se invalidan generaciones antiguas. No basta añadir una comprobación de fase que descarte también contactos legítimos del último frame. |
| Fuente y varios atacantes | La selección actual favorece al target participante o al primer candidato; el movimiento elige un contacto por receptor. Guard debe distinguir selección, fuente de reacción y dirección de empuje. La investigación no cierra una regla universal de empate que justifique cambiar por suposición la resolución conjunta actual. |

La defensa por pulsación, la cobertura 360° y las armas independientes son decisiones de Vamps. Mantener botón o recordar GuardID/HitID para admisión defensiva exige decidir qué comportamiento se adopta en 3. Residual, recurso, ruptura y stun también pertenecen a esa etapa: 028 no respalda atribuir automáticamente la ruptura nativa a agotar un recurso. Estas decisiones no obligan a modificar ahora cadena, cartas excluidas, cámara o red.

## Verificación y límites

Se leyeron la guía, evidencia y reporte de 028; el roadmap, combate funcional/técnico y los recorridos actuales de Character, habilidad, resolver, armas, movimiento, AnimInstance, controller e IA. Los 42 archivos del manifiesto de 028 conservan exactamente sus hashes SHA256: sus observaciones sobre esos fuentes siguen vigentes.

Se revisaron los enlaces de los entregables nuevos y el diff documental. No se consultaron assets vivos ni se ejecutaron MCP de editor, compilación, PIE o tests automatizados. Las conclusiones de FGOA proceden de la investigación conservada en 028; no se repitió su análisis nativo. Las referencias históricas a AM_Executioner_Combo y DA_VampsCombatSettings no certifican su configuración actual.

El siguiente cambio acotado recomendado para 2 es soporte de varios impactos por paso conservando independencia por arma, mediante reinicio o ventanas separadas según el control temporal necesario; la siguiente unidad de 3 es Guard renovable con fuente, tiempo, permisos y finalización propia. Esta revisión no autoriza ni realiza esas implementaciones.
