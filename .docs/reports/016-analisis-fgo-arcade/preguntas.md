# Decisiones del diálogo y precisiones restantes

R01–R07 se integran en [el análisis original](analisis.md) y en [el alcance de Vamps](alcance/functional.md), con [base técnica](alcance/technical.md). No se requieren nuevas grabaciones. Las particularidades de cartas se documentan como referencia y no se trasladan a la implementación.

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

Definir cómo se consume/recupera guardia y qué restricciones produce el guard break, y si crítico modifica también el daño residual. Probabilidad, reducción, bonus, tiempos y cantidades de recurso se calibrarán después; no se requieren números exactos para cerrar el análisis.

### P03. Partida y portador de recompensa

Fijar tamaño de equipos y condiciones de final/empate; momento de aparición del portador, criterio de asignación y transferencia tras morir. La hipótesis kill leader del equipo ganador no está comprobada como regla original. La curación observada al adquirir el Grial se conserva como evidencia, sin decidir todavía que el portador de Vamps cure o añada otros efectos.

### P04. Pérdida del target y arbitraje técnico

Definir selección al morir/perderse el target o no quedar enemigos elegibles. Elegir después el sistema de fases/contactos y su criterio de desempate, como indicó R03; no imponer ahora stats, reloj o IDs. Los parámetros de encuadre, fade y movilidad se calibrarán con el prototipo conservando los comportamientos elegidos.

Estas precisiones completan el documento de alcance; no reabren la revisión de videos. SUPPORT no requiere diseño porque quedó excluido, y la ultimate se define cuando llegue su etapa.
