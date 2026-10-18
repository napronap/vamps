# Decisiones del diálogo y precisiones restantes

R01–R07 se integran en [el análisis original](analisis.md). [017](../017-base-fgoa-y-extensiones/reporte.md) traslada el alcance a las especificaciones principales y separa las extensiones propias. No se requieren nuevas grabaciones. Las particularidades de cartas se documentan como referencia y no se trasladan a la implementación.

## Respuestas integradas

| Grupo | Resultado |
|---|---|
| R01 — Cartas y daño | Distinguir mano lista de acciones usadas en el original. Se conserva el caso de preparación que termina en guardia sin draw y la posibilidad de consumo por ofensiva ya lanzada. Vamps excluye los casos finos causados por cartas; usa cadena/recuperación directamente |
| R02 — Solicitudes | Pulsaciones individuales, ventana de combo y rechazo del click temprano. No se retiene input fuera de ventana. Tras bloquear, el botón sigue habilitado para ataque/defensa aunque la salida ofensiva pueda tardar más por recuperación/timings; esa demora no es NA |
| R03 / P01 — Contacto y cierre | Acertar a otro enemigo o un golpe bloqueado habilitan continuación para el atacante, sin retarget ofensivo. Guardia efectiva contra otro agresor cambia el target del defensor, que puede retomar ofensiva. Defensa durante toda la acción; cierre voluntario al final del paso por evasión/no continuar y empate al diseñar resolución |
| R04 — Guardia | Vamps adopta 360°, daño residual según stats que puede matar y recurso de guardia con guard break/stun. La flecha HUD avisa amenaza y no prueba bloqueo trasero. Parry/mecánicas originales quedan para después |
| R05 — Movimiento | Evasión con dirección y sprint mantenido. Shift sin dirección será evasión hacia atrás; parámetros de movimiento y recuperación se calibran |
| R06 — Target/cámara | F sigue orden vertical del HUD, más retarget por guardia efectiva. Se conservan corrección, distancia visual y planos de ataque/defensa, incluido el movimiento de cámara al atacar confirmado por el usuario. Obstáculos visibles pueden desvanecerse sin perder colisión; límites invisibles no tienen presentación que ocultar |
| R07 — Alcance | Sin cartas, Q/A/B, Extra Attack, ranged inicial ni reservas. Un personaje con respawn, equipos/puntos y portador con recompensa de baja mayor. Crítico incluido; SUPPORT excluido tras aclarar que es distinto de atacar al mismo target que un aliado. Ultimate posterior propia sin cinemática global |

Los tiempos aproximados aportados se localizaron en los archivos: Da Vinci preparada/defendiendo desde 00:54,33 en newdemo2; guardia con retarget cerca de 01:08,30 en el mismo video; Holmes defendiendo dentro de ofensiva en 00:11,70 y retarget cerca de 00:14,30 en newdemo3. [escenas.md](escenas.md) contiene las secuencias y límites de atribución.

## Reglas que requieren respuesta

### P02. Recurso de guardia y crítico

Las reglas vigentes de solicitud defensiva, acumulación/recuperación, break, reacción, residual y crítico están en [combate](../../combat/functional.md#guardia-daño-y-reacción). Las decisiones posteriores y la implementación se registran en [033](../033-resultados-combate/reporte.md); los valores se calibran por observación.

### P03. Partida y portador de recompensa

Las preguntas vigentes sobre equipos, final/empate, portador y respawn están en [partida](../../game/functional.md#preguntas). La hipótesis de kill leader y la curación observada del Grial permanecen en el análisis, sin convertirlas en decisiones para Vamps.

### P04. Pérdida del target y arbitraje técnico

La selección inicial/pérdida/ausencia de target se resuelve en [jugador](../../player/functional.md#preguntas). Las decisiones posteriores sobre fases/contactos y desempate están en la [resolución de combate](../../combat/technical.md#contexto-y-resolución-del-contacto); esta sección conserva el contexto de R03. Los parámetros de encuadre, fade y movilidad se calibran con el prototipo.

Estas precisiones se resuelven por funcionalidad según el [roadmap](../../roadmap.md); no reabren la revisión de videos. SUPPORT quedó excluido. Las preguntas propias de parry, ultimate y personajes están en [extensiones](../../extensions/functional.md#preguntas-para-esta-etapa), sin bloquear la base.
