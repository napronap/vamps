# Combate

## Dinámica

El combate combina acción 3D con intercambios de iniciativa: «turnos disfrazados de acción». Las restricciones de acciones permiten leer los compromisos del rival y anticipar sus respuestas.

Iniciar una ofensiva tiene valor frente a un rival que espera. Atacar y defender requieren decisiones con compromisos: ni repetir ataques ni mantener la guardia debe resolver sistemáticamente el intercambio. El defensor obtiene oportunidades de castigo al responder a una exposición concreta; esperar el final de cualquier cadena no garantiza un contraataque.

## Primera versión

Los dos combatientes utilizan un único tipo de ataque y un único tipo de defensa, con las mismas reglas. El shared button ejecuta una u otra acción según el estado del combate.

El alcance comprende cadenas de hasta tres golpes, continuación de bloqueos, parry y salida de los intercambios mediante movimiento. El duelo utiliza lock-on y un acercamiento inicial limitado para probar alcance y respuesta.

Dash, stances y su RPS, carrera especial, burst, riposte y skills se incorporan después del MVP. El dash y sus i-frames se describen en las expansiones y no forman parte de los criterios de salida del núcleo local ni del primer duelo online.

### Pulsaciones y buffer

Cada ataque o continuación defensiva requiere una nueva pulsación del shared button. Mantenerlo pulsado no solicita acciones adicionales ni conserva una guardia permanente. Se admiten pulsaciones anticipadas mediante un buffer de duración configurable; una pulsación almacenada solo produce una acción cuando esta se vuelve válida y el input sigue vigente. Esperar en el buffer no abre una ventana defensiva.

### Cadena de ataques y modo NA

Al acertar, el atacante puede continuar la cadena hasta un máximo de tres golpes o cortarla para salir mediante movimiento. La continuación requiere aceptar una pulsación vigente dentro de su ventana, directamente o desde el buffer. Si esa ventana vence, el personaje entra en modo no atacar (`NA`): un cooldown breve durante el cual no puede atacar ni bloquear.

Si el rival bloquea el golpe, el atacante puede continuar con la misma ventana y el mismo máximo de tres golpes que al acertar. El bloqueo no reinicia el conteo de la cadena.

Completar el tercer golpe o cortar voluntariamente la cadena mediante movimiento también inicia el NA ofensivo. La salida por movilidad no elimina ese cooldown.

La salida voluntaria se permite entre golpes, durante recovery. Recovery es el retorno visual del arma a una posición que permite continuar el combo o volver a idle; no impone inmovilidad ni exige esperar a que termine la animación para moverse. Esta recuperación visual y NA son conceptos distintos: las restricciones de ataque y bloqueo pertenecen a NA. La animación de recovery puede coexistir con la ventana de continuación o con NA, según cómo termine el intercambio.

Los golpes posteriores al primero no corrigen su trayectoria, aunque exista lock-on. Si el rival sale de esa trayectoria, el atacante queda expuesto; la fijación de objetivo no redirige la cadena para perseguirlo.

Fallar significa que el golpe no alcanza a un oponente. Un fallo permite seguir encadenando dentro de la ventana, con el mismo máximo de tres golpes y sin corregir la trayectoria. Un golpe posterior puede acertar si un oponente entra en ella. Si no se continúa a tiempo, se aplica el mismo NA que al dejar de encadenar después de acertar.

### Continuación defensiva y parry

Después de bloquear, el defensor puede seguir bloqueando ataques posteriores o salir mediante movimiento. El NA defensivo comienza al salir de la secuencia de bloqueos y es más corto que el de ataque. Continuar la secuencia defensiva no inicia NA entre bloqueos. La salida de la secuencia inicia NA inmediatamente, sin agregar un intervalo de espera previo. Su menor NA da prioridad de recuperación a quien bloqueó satisfactoriamente.

En el MVP, bloqueo y parry anulan todo el daño del golpe defendido. Su cobertura angular se configura por personaje; ambos combatientes utilizan inicialmente 180 grados totales centrados en su frente, 90 grados a cada lado. Sus resultados se amplían con las reglas de RPS después de esta etapa.

La ventana de parry aparece únicamente al iniciar una cadena; el segundo y tercer golpe no abren otra. Un parry exitoso devuelve al defensor al estado de combate inicial (`default`), con su acción o cadena reiniciada y disponibilidad inmediata, sin NA. Puede iniciar una nueva cadena desde ese estado, incluida su ventana inicial de parry. No interrumpe la cadena del atacante ni le impone NA por sí mismo: el atacante puede continuar con las reglas habituales de ventana y máximo de tres golpes. Ambos deben reaccionar a la decisión del otro; el parry no garantiza un contraataque exitoso.

### Respuesta después de recibir un golpe

Recibir un golpe sin bloquear deja una oportunidad muy breve para salir mediante movimiento o bloquear el siguiente golpe de la cadena. El primer impacto no garantiza que los restantes conecten. El atacante conserva la ventaja del intercambio; la respuesta permite al defensor mejorar su situación sin garantizarle recuperar la iniciativa.

La oportunidad de respuesta depende de los parámetros del combate. La demo utiliza los mismos valores para ambos combatientes. Al incorporar stances y estadísticas diferenciadas, estas pueden modificar esa oportunidad. Si NA ya estaba activo, se aplica la [recuperación bajo presión](#recuperación-bajo-presión): la oportunidad de respuesta no permite bloquear mientras quede NA pendiente.

### Ventanas de ataque y defensa

Las fases de la acción y la restricción de NA se distinguen de la recuperación visual:

| Fase | Disponibilidad defensiva |
|---|---|
| Ventana inicial de la cadena | Puede defender un golpe recibido con el timing correcto (parry); no se repite al encadenar el segundo o tercer golpe. |
| Buildup | No puede defender. |
| Ataque | Puede pasar a defensa al perder un clash. |
| Recovery visual | Permite movimiento y transición a continuación o idle; no impone un cooldown propio. |
| NA | No puede atacar ni defender. |

La ventana inicial es breve: mantener el botón no conserva una guardia permanente. Las pulsaciones que no habilitan una nueva acción tampoco renuevan esa ventana; un input almacenado durante NA no defiende mientras espera su ejecución. La continuación de bloqueos utiliza las reglas de la secuencia defensiva.

En este documento, «clash» designa el cruce de dos ataques, independientemente del RPS entre stances. La regla del cruce establece que gana el primer contacto válido. El ataque perdedor se interrumpe y su personaje pasa a defensa, incluida su animación. Esta conversión se aplica durante la fase de ataque; un contacto recibido durante buildup o NA encuentra al personaje sin defensa. El orden depende del contacto efectivo, no solamente de la duración total de la animación.

El MVP no agrega un rango temporal de simultaneidad ni una reacción especial de empate: se resuelve a favor del primer contacto válido. Pulsar al mismo tiempo no implica que los contactos ocurran al mismo tiempo. La ponderación por RPS y estadísticas pertenece a las expansiones.

### Recuperación bajo presión

Al recibir un golpe durante NA, su contador se congela conservando el tiempo restante. Tras un intervalo de espera definido después del golpe, el contador vuelve a descontar desde ese valor. Esto se aplica tanto a NA ofensivo como defensivo. NA sigue impidiendo atacar y bloquear durante la espera y hasta consumir el tiempo pendiente.

Cada nuevo golpe durante la pausa reinicia la espera completa desde ese último golpe. El tiempo restante de NA permanece congelado; la espera pendiente se reemplaza por la espera completa, sin sumarlas.

Ejemplo ilustrativo, sin valores de balance finales: NA empieza en `t = 0` con 3 segundos. Un golpe en `t = 2` congela el segundo restante. Con una espera de 1 segundo y sin nuevos golpes, el contador se reanuda en `t = 3` y NA termina en `t = 4`. El golpe agrega el tiempo de pausa, sin reiniciar los 3 segundos de NA.

Con los mismos valores, si llega otro golpe en `t = 2,5`, la espera vuelve a ser de 1 segundo desde ese instante. NA se reanuda en `t = 3,5` con el segundo pendiente y termina en `t = 4,5`.

### Alcance y seguimiento

El inicio de la cadena cuerpo a cuerpo realiza un acercamiento hacia el objetivo fijado, con desplazamiento limitado. Alcance, acercamiento y pushback son propiedades configurables por personaje; el primer duelo utiliza los mismos valores para ambos combatientes. La cámara sigue al objetivo; el seguimiento ofensivo respeta la trayectoria de continuación descrita en la cadena.

### Parámetros iniciales del duelo local

Los dos combatientes comparten configuración. Estos valores son un punto de partida editable para validar el intercambio, no balance final.

| Parámetro | Valor inicial |
|---|---|
| Vida / daño por golpe | 100 / 12 |
| Parry inicial / buildup / contacto activo | 0,08 / 0,17 / 0,10 s |
| Continuación ofensiva | 0,22 s; el tercer golpe termina al cerrar el contacto activo |
| NA ofensivo / defensivo / espera para reanudar | 0,45 / 0,18 / 0,20 s |
| Buffer | 0,14 s; una entrada, la última pulsación reemplaza la anterior |
| Bloqueo por pulsación / espera de continuación tras bloquear | 0,22 / 0,42 s |
| Reacción al daño / oportunidad posterior de respuesta | 0,14 / 0,18 s |
| Cobertura defensiva | 180° totales, incluido su borde |
| Alcance / radio de contacto | 155 / 28 cm |
| Acercamiento máximo / giro inicial máximo | 100 cm / 90°; trayectoria fijada al iniciar la cadena |
| Pushback al recibir daño / bloquear / parrear | 24 / 12 / 18 cm sobre el defensor |
| Velocidad de movimiento | 420 cm/s |

Las ventanas temporales incluyen su inicio y excluyen su final. Una pulsación de continuación ofensiva solo continúa esa cadena; una pulsación defensiva solo solicita defensa. Si su contexto desaparece, no se convierte automáticamente en un ataque. Guardar un input no amplía una ventana ya abierta.

Un bloqueo consume su cobertura. Cada nueva pulsación aceptada habilita un bloqueo siguiente; no pulsar a tiempo o moverse finaliza la secuencia e inicia NA defensivo. Durante la reacción al daño, el shared button solicita guardia: el buffer puede anticiparla, pero solo se ejecuta al abrir la respuesta y si no hay NA.

Los desplazamientos respetan obstáculos. La cámara sigue al rival mientras hay lock-on y admite control manual cuando se libera. El HUD muestra fase, golpe de la cadena, vida, NA restante, pausa y último resultado; volver a `Default` señala disponibilidad completa.

### Casos de validación

| Situación | Resultado que debe describir la regla |
|---|---|
| Ataque a un objetivo libre y dentro del alcance | Daño, reacción y disponibilidad posterior de ambos. |
| Golpe recibido sin bloqueo y continuación rival | Una oportunidad breve permite salir o bloquear el siguiente golpe, respetando las restricciones de NA si estaba activo; probar inputs dentro y fuera de esa oportunidad, conservando la ventaja ofensiva. |
| Ataque bloqueado | El atacante conserva la misma ventana de continuación y el límite de tres golpes; el defensor sigue las reglas de continuación defensiva. |
| Continuación dentro de la ventana | Encadenar hasta tres golpes, sin corregir la trayectoria de los posteriores al primero. |
| Vencimiento de la ventana tras acierto, bloqueo o fallo | Entrar en NA y no permitir ataque ni bloqueo hasta que termine. |
| Ataque fuera de alcance | Poder continuar al aire; un golpe posterior solo acierta si encuentra un oponente en su trayectoria. |
| Fin del tercer golpe | Terminar la cadena y entrar en NA ofensivo. |
| Salida de una cadena mediante movimiento | Permitirla entre golpes, cortar la cadena y entrar en NA ofensivo sin imponer inmovilidad por la animación de recovery. |
| Bloqueos consecutivos y salida defensiva | Permitir continuar bloqueando o salir, respetando el NA defensivo más corto. |
| Parry válido | El defensor vuelve a default sin NA y puede comenzar una nueva cadena; el atacante conserva la posibilidad de continuar la suya. |
| Segundo o tercer golpe de una cadena | No abrir una nueva ventana inicial de parry. |
| Contacto dentro o fuera de la cobertura defensiva | Aplicar la apertura configurada por personaje, con 180 grados totales en el MVP. |
| Pulsación anticipada durante una restricción | Almacenar el input según la duración del buffer y ejecutarlo solo si sigue vigente al habilitarse la acción; no defender mientras espera. |
| Defensa cerca del instante de contacto | Límite exacto que separa bloqueo de impacto. |
| Ataques con contactos en momentos distintos | El primer contacto válido gana el clash; el rival pasa a defensa si está en fase de ataque. |
| Contacto durante buildup o NA | No se habilita defensa por perder el intercambio. |
| Contactos muy próximos | Resolver el primero válido sin agruparlos en una ventana de simultaneidad. |
| Botón mantenido o pulsado repetidamente | Mantener no repite acciones; cada pulsación aceptada o almacenada respeta las ventanas y no concede defensa mientras espera. |
| Golpe recibido durante NA | Conservar el tiempo restante, esperar el intervalo definido y reanudar el contador; no habilitar ataque ni bloqueo antes de que NA termine. |
| Nuevo golpe durante la pausa de NA | Reiniciar la espera completa desde el último golpe, conservando el NA pendiente sin acumular las esperas. |

## Expansiones del combate

### Stances

Un botón cicla entre tres stances. Cada una determina propiedades de ataque y bloqueo con ventajas y desventajas tipo piedra, papel o tijera (RPS).

RPS y estadísticas ponderan la resolución de los cruces: ser el primero puede dejar de ser suficiente frente a un golpe con propiedades superiores. Estas reglas también determinan el daño que atraviesa las defensas.

Solo se cambia desde `true idle`. No se permite cambiar entre golpes de una cadena ni inmediatamente después de un dodge: debe recuperarse `true idle`.

### Movilidad ampliada

Al incorporar stances, la cadena conserva la stance durante su ejecución.

Un botón adicional ejecuta un dodge direccional con una pulsación breve y permite correr al mantenerlo. El dodge consume un recurso limitado cuya capacidad varía por personaje. Sin recurso no se puede esquivar; recibir un golpe fuera de una defensa válida aplica el impacto.

El dash/dodge concede un intervalo de invulnerabilidad (i-frames). Permite salir de los intercambios en los puntos de cancelación habilitados, conservando el NA que corresponda. Se implementa después del MVP.

### Burst y selección de objetivo

Bloquear carga una barra de burst que permite romper la presión y separar a los enemigos. La recompensa defensiva debe coexistir con una ventaja por sostener una ofensiva efectiva.

En encuentros con varios enemigos, un control permite ciclar el objetivo del lock-on.

### Habilidades

Cada personaje tiene dos skills que alteran reglas del combate. El sistema prescinde de las cartas de Fate.

## Preguntas

### Reglas del núcleo

1. ¿Qué ajustes necesitan los parámetros iniciales después de jugar los casos de validación?
2. Para la etapa online, ¿qué feedback puede mostrarse antes de confirmar un golpe o bloqueo y qué correcciones visuales resultan aceptables?

### Expansiones

1. ¿Qué condiciones definen `true idle` y cuándo se recupera después de cada acción? ¿Cómo se señala la disponibilidad del cambio de stance?
2. ¿Las stances serán Rápida (L), Media (M) y Pesada (H)? ¿En qué orden ciclan y qué ocurre con un cambio solicitado fuera de `true idle`?
3. ¿Cuándo L interrumpe el startup de H y cuándo H vence un clash con L? ¿Qué hace H contra bloqueo L y qué obtiene M al bloquear H y responder?
4. ¿Qué acciones generan recurso para skills en cada stance y cuánto cuesta gastarlo?
5. ¿La riposte es universal o pertenece a un personaje? ¿Qué defensa la habilita?
6. ¿Qué distancia, duración, intervalo de i-frames, costo y regeneración tiene el dash por personaje? ¿Cómo se introduce su dirección y correr consume el mismo recurso?
7. ¿Cómo se activan las skills y cómo se comunican sus excepciones al rival?
8. ¿Cómo se resuelven bloqueos y clashes con varios atacantes y qué evita una inmovilización indefinida por la pausa de NA?
9. ¿Cuánto burst genera cada defensa, cuándo se puede activar y qué costo y efecto tiene? ¿Qué oportunidades de respuesta conserva el atacante frente al burst?
