# Implementación del combate

Comportamiento de referencia: [combate](functional.md). Arquitectura compartida: [base técnica del juego](../game/technical.md).

## Arquitectura de la primera versión

GAS organiza la ejecución de acciones, atributos y efectos. La lógica propia define las reglas del combate usando sus puntos de extensión. V1 utiliza directamente estas piezas, sin `CombatComponent`, `HealthComponent` ni una máquina de estados paralela a las habilidades.

| Pieza | Responsabilidad |
|---|---|
| `AVampsDuelCharacter` | Personaje propio sobre `ACharacter`, común a jugador e IA. Alojar GAS y atributos, integrar movimiento, objetivo fijado y cámara, e interpretar el shared button para solicitar la habilidad correspondiente. |
| `AbilitySystemComponent` | Administrar habilidades, efectos y tags mediante los mecanismos de GAS, incluida su integración de red. |
| `GA_Attack` | Gobernar fases, ventana de continuación y cadena de hasta tres golpes, incluida la elegibilidad defensiva y la interrupción del ataque. |
| `GA_Guard` | Ejecutar la secuencia de bloqueos y su salida a NA, diferenciándola del parry sin NA. |
| `GA_NA` | Mantener una única fuente del tiempo restante de NA y su pausa, con independencia de la terminación del ataque, bloqueo o recuperación visual. |
| `GA_HitReaction` | Gobernar la reacción al impacto y la oportunidad posterior de respuesta, respetando NA y el daño ya resuelto. |
| `UVampsAttributeSet` | Declarar `Health` y `MaxHealth`, configurar su replicación y mantener sus invariantes numéricas. |
| `UCombatConfig` | Definir parámetros por personaje: tres golpes, ventanas, buffer, cobertura defensiva, alcance, daño, desplazamiento y NA. Ambos combatientes comparten configuración en el MVP. |
| `CombatTags` | Declarar identificadores de habilidades, restricciones y eventos compartidos. |
| `ADuelAIController` | Solicitar acciones al mismo personaje mediante patrones reproducibles, sujeto a sus restricciones y al reinicio del escenario. |
| Gameplay Effects | Aplicar modificaciones a atributos, como el daño autorizado. |
| Animación y UI | Representar acciones, valores y resultados. |

El código del duelo local se organiza en `Source/vamps/Public/` para headers y `Source/vamps/Private/` para implementaciones, con subcarpetas por responsabilidad según las [convenciones C++](../cpp-style.md); [reporte de implementación y validación](../reports/mvp-duelo.md). `UDuelGameplayAbility` concentra el ciclo común de las habilidades. `UGE_DuelDamage` modifica `Health` mediante la magnitud `Duel.Data.Damage`. Los tags `Duel.State.Attack`, `Guard`, `NA` y `HitReaction` acompañan la ejecución GAS; no sustituyen los datos temporales de cada habilidad.

Las habilidades se implementan en pares `GA_Attack.h/.cpp`, `GA_Guard.h/.cpp`, `GA_NA.h/.cpp` y `GA_HitReaction.h/.cpp`. Los otros archivos del núcleo son `VampsAttributeSet.h/.cpp`, `CombatConfig.h/.cpp`, `CombatTags.h/.cpp` y `DuelAIController.h/.cpp`. Las clases y Blueprints del escenario siguen la [base técnica del MVP](../game/technical.md#código-y-recursos-del-mvp). `GA_Dash`, su input y sus assets pertenecen a la expansión de movilidad.

### Estado y reglas

Cada habilidad gobierna su ejecución temporal. Las consultas de disponibilidad utilizan las habilidades activas, sus tags y los datos específicos de la acción. No se mantienen copias independientes de ese estado en el personaje o la animación.

La entrada del shared button genera solicitudes por pulsación, no por mantenimiento. El buffer conserva entradas anticipadas y su vigencia; la habilidad solo las consume cuando la acción o continuación es válida. Una entrada pendiente no otorga elegibilidad defensiva.

La detección de contactos y la evaluación de defensa comienzan como funciones pequeñas utilizadas por la habilidad. Las reglas compartidas se extraen cuando más de una acción las necesita; un componente adicional requiere una responsabilidad propia, no solo reenviar llamadas a GAS.

La elegibilidad para bloquear se consulta sobre la fase vigente de la acción antes de activar su respuesta defensiva. Abrir una ventana no requiere mantener una segunda habilidad ni una guardia independiente. La conversión por clash cierra los contactos pendientes del ataque perdedor para impedir que siga causando daño después de pasar a defensa.

En el MVP, los resultados de bloqueo y parry no aplican daño a `Health`. La comprobación direccional usa la cobertura configurada en el personaje defensor. Los cruces se ordenan por contacto válido sin una ventana de simultaneidad ni una reacción especial de empate. La ponderación por estadísticas y RPS se añade con esas expansiones.

`GA_NA` conserva tiempo restante, origen ofensivo o defensivo y tiempo de espera hasta su reanudación. Ataque y defensa solicitan su activación al salir; su terminación no cancela `GA_NA`. Al recibir un golpe, se suspende el descuento de NA y se temporiza la espera por separado; al terminar esa espera, se consume el NA pendiente. La pausa no restituye la duración inicial ni depende de detener la animación. La temporización pertenece a esta habilidad, fuera del `AttributeSet`, y bloquea las solicitudes de ataque y defensa tanto durante la pausa como durante el descuento restante. La espera es configurable; los valores del ejemplo funcional no fijan valores por defecto.

Un nuevo golpe durante la pausa sustituye el vencimiento de la espera por el instante del último golpe más la duración configurada. Se conserva el NA restante y se invalida la reanudación anterior para impedir que libere la pausa antes de tiempo. Las esperas no se acumulan.

La acción ofensiva mantiene el índice del golpe y la ventana de continuación. Resolver un fallo o un golpe bloqueado no termina automáticamente la cadena: la aceptación del siguiente golpe depende de la misma ventana y del límite de tres que al acertar. El resultado de bloqueo conserva el índice de la cadena. Al vencer la ventana o completar el tercer golpe se cierra la continuación y comienza NA ofensivo. La secuencia defensiva permite bloqueos sucesivos y aplica su NA inmediatamente al salir, sin espera adicional ni NA entre bloqueos. La salida ofensiva por movimiento se acepta entre golpes, cierra la cadena y sus contactos pendientes y activa NA ofensivo sin que la cancelación lo elimine. Recovery visual no deshabilita el movimiento ni extiende las restricciones hasta el fin del montage.

El jugador y la IA solicitan acciones sujetas a las mismas restricciones. En la prueba online, el servidor valida las solicitudes y autoriza los resultados; el cliente puede anticiparlos según la política de predicción definida para cada acción. GAS aporta mecanismos de predicción y reconciliación, pero la resolución temporal del contacto y sus consecuencias sigue siendo lógica del juego.

La ventana inicial de parry se abre solo al iniciar una cadena, no en el segundo o tercer golpe. Resolver un parry devuelve al defensor a su estado de combate inicial, cerrando su ejecución anterior y sin iniciar NA. Una nueva acción comienza una cadena nueva con su ventana inicial. El reinicio no cancela la habilidad ofensiva rival, altera su ventana de continuación ni le aplica NA por ese resultado. Las acciones posteriores de ambos se evalúan con las reglas normales de contacto y disponibilidad.

La resolución de daño y la reacción al impacto permiten evaluar la disponibilidad de movimiento y bloqueo antes del siguiente contacto, según la oportunidad breve definida en el funcional. Conectar un golpe no reserva impactos posteriores ni impone una inmovilización durante toda la cadena. La temporización de la reacción y la aceptación de esas respuestas pertenecen a la lógica de combate; la animación representa su resultado. La consulta de bloqueo sigue rechazando su ejecución si existe NA pendiente, aunque la reacción al impacto ya permita otras respuestas.

### Timings y animación

`GA_Attack` gobierna una línea temporal por golpe con tiempos configurables para la ventana inicial, preparación, contacto activo y continuación. Consulta los intervalos respecto del inicio de la acción al resolver inputs y contactos. La evaluación procesa los intervalos atravesados entre pasos, sin depender únicamente del orden de callbacks. `GA_NA` conserva su temporización separada.

`UDuelCombatSubsystem` coordina el reloj local de ambos personajes. Avanza como máximo 1/120 s por paso y subdivide también en los límites de fases, guardia, reacción y NA. Las habilidades conservan sus datos; el subsistema solo ordena su avance y la resolución. Los límites temporales son semiabiertos. En cada paso se consulta un barrido de esfera sobre el segmento de ataque, se rechazan contactos tapados por geometría y se admite un contacto por objetivo y golpe. Una conversión a guardia invalida el ataque perdedor antes de resolver otro contacto suyo.

El muestreo geométrico utiliza las posiciones disponibles del frame, sin historial de movimiento. Los contactos del mismo instante se desempatan por serial de acción y distancia; no se agrega una ventana de simultaneidad. Esta implementación local no resuelve la política temporal del futuro juego online.

Un montage propio organiza preparación, golpe y salida en secciones para cada elemento de la cadena. La habilidad coordina los enlaces, interrupciones y velocidades de reproducción para sincronizar los segmentos visuales con sus tiempos. Recovery representa el retorno del arma y puede coexistir con movimiento, continuación o NA; no es una segunda fuente de cooldown.

Los Anim Notifies se usan para presentación, como sonidos de movimiento y estelas. Los efectos de impacto o bloqueo responden al resultado real del contacto. Si las consultas de contacto siguen huesos o sockets, se evalúa la pose necesaria para esa geometría también en la simulación autorizada.

La primera implementación consulta el segmento respecto de la cápsula y la dirección guardada, sin sockets ni dependencia de una pose animada. `UDuelAnimInstance` expone velocidad, dirección, caída, fase y golpe al AnimBP propio. El montage utiliza `Hit1_Prepare`, `Hit1_Active`, `Hit1_Recovery`, y el mismo patrón para 2 y 3. El prefijo de preparación del primer golpe cubre también la ventana inicial. GAS decide cuándo saltar de fase y calcula su velocidad. Antes de avanzar, `PlayAttackSection` enlaza las tres secciones del golpe: preparación → activo → recovery → fin. Así Unreal no inicia el blend out en fases intermedias. Recovery no enlaza al siguiente golpe: la continuación sigue necesitando una pulsación aceptada por GAS. Una sección inexistente o un rechazo de `Montage_Play` genera una advertencia en `LogDuelAnimation`.

### Atributos y configuración

El `AttributeSet` contiene únicamente los atributos numéricos necesarios y su infraestructura: acceso, replicación y límites. La vida tiene una sola fuente de datos. La interpretación de inputs, las colisiones, el bloqueo y las animaciones pertenecen a sus respectivas acciones o funciones de reglas.

El daño, alcance y tiempos del movimiento se configuran en la definición del ataque, junto con la ventana de continuación y las duraciones de NA. La cobertura defensiva es un ángulo configurable por personaje, inicializado en 180 grados totales para ambos combatientes; la consulta compara el ángulo respecto del frente con la mitad de esa apertura. La fase, los identificadores, los tiempos de ejecución, el progreso de la cadena y su trayectoria son datos de acción, no atributos GAS. Los recursos y estadísticas adicionales se incorporan cuando una mecánica los requiere.

El acercamiento inicial utiliza el objetivo del lock-on y los límites del primer ataque. Los golpes posteriores conservan la trayectoria de continuación sin recalcularla hacia el objetivo; la orientación de cámara se gestiona por separado. La posición resultante participa en la detección real de contactos; el movimiento visual no concede un impacto fuera de alcance.

### Ampliación

Stances amplían las reglas y datos de las acciones. La movilidad ampliada y las skills utilizan habilidades. Se reutilizan Gameplay Effects para sus modificaciones numéricas y se extrae lógica común cuando aparezca uso compartido. Estas extensiones siguen el [alcance funcional](functional.md#expansiones-del-combate).

`GA_Dash` se incorpora después del MVP para gestionar desplazamiento e i-frames, coordinando la salida de las acciones sin cancelar `GA_NA`. Sus recursos y la barra de burst se incorporan como atributos cuando se implementan esas mecánicas.

## Accionables: núcleo local

- [ ] Resolver los casos de la primera versión del funcional. Entregable: condiciones de entrada, transiciones, resultados y parámetros de ataque y defensa.
- [ ] Implementar pulsaciones y buffer con vigencia configurable. Entregable: mantener el botón no repite acciones; los inputs anticipados se consumen solo al habilitarse su acción y no conceden defensa mientras esperan ni cambian silenciosamente una intención de defensa por ataque.
- [ ] Implementar `GA_Attack` y `GA_Guard` con sus fases y restricciones. Entregable: completar y cancelar ambas acciones sin dejar tareas, contactos o tags residuales.
- [ ] Implementar la ventana de continuación y el límite de tres golpes. Entregable: encadenar tras acierto, bloqueo o fallo con la misma ventana y el mismo conteo, y pasar a NA ofensivo al vencer la ventana o terminar el tercer golpe.
- [ ] Implementar bloqueos consecutivos, NA al salir de la secuencia defensiva y parry sin NA. Entregable: distinguir disponibilidad durante la secuencia, después de salir y después de hacer parry; verificar que el parry permite la reacción inmediata del defensor y conserva la continuación del atacante sin imponerle NA.
- [ ] Implementar la oportunidad breve de respuesta después de recibir daño. Entregable: reproducir salida mediante movimiento y bloqueo del siguiente golpe, comparar inputs dentro y fuera de la oportunidad y cubrir el caso de NA previo sin conceder impactos posteriores automáticamente.
- [ ] Implementar salidas mediante movimiento entre golpes. Entregable: cerrar la cadena y sus contactos pendientes, activar NA ofensivo al cortar el ataque y permitir movimiento durante recovery visual.
- [ ] Resolver contactos contra la fase defensiva vigente y convertir al perdedor de un clash. Entregable: probar fronteras temporales, cobertura angular y contactos muy próximos sin ventana de empate; verificar daño nulo en bloqueo/parry y ausencia de un impacto posterior del ataque interrumpido.
- [ ] Implementar pausa y reanudación de NA con espera configurable. Entregable: reproducir las líneas temporales de los ejemplos funcionales, verificar que cada golpe durante la pausa reinicia la espera sin acumularla ni modificar el NA restante, y que no permite ataque ni bloqueo durante la espera o el NA pendiente. Comprobar que un vencimiento anterior no reanuda el contador tras recibir un nuevo golpe.
- [ ] Implementar lock-on, seguimiento de cámara y acercamiento limitado para el duelo. Entregable: reproducir contactos y fallos según posición y alcance, sin redirigir los golpes posteriores al primero aunque el objetivo se desplace.
- [ ] Configurar `Health`, `MaxHealth` y la aplicación de daño mediante Gameplay Effects. Entregable: una única fuente de vida con límites consistentes y cambios visibles en la UI.
- [ ] Separar la resolución del contacto de su presentación. Entregable: reglas ejecutables sin depender de partículas, sonidos o del frame que dibuja cada cliente.
- [ ] Crear un rival de prueba que ataque y defienda con secuencias reproducibles, usando las mismas reglas que el jugador. Entregable: reproducción de cada caso funcional y reinicio del intercambio.

## Accionables: predicción y autoridad

- [ ] Ejecutar el mismo núcleo con dos clientes y un servidor dedicado local como ensayo de arquitectura. Entregable: solicitudes validadas y resultados autorizados visibles en ambos clientes.
- [ ] Definir para inicio de ataque, guardia, contacto y daño qué se anticipa localmente, con qué información y cómo se confirma o revierte. Entregable: tabla de predicciones y sus consecuencias dependientes.
- [ ] Validar la integración de GAS con ambas habilidades: activación predictiva, confirmación, rechazo y deduplicación de efectos. Entregable: verificar un rechazo sin efectos residuales ni repetidos y cubrir las correcciones que requieren lógica propia.
- [ ] Definir la línea temporal del combate y el tratamiento de una defensa recibida antes, al mismo tiempo o después del impacto. Entregable: reglas explícitas sobre recepción, historial, tolerancia y resultados que pueden revisarse.
- [ ] Implementar identificación y orden de acciones, validación temporal y corrección de estados. Entregable: ningún input duplicado aplica daño dos veces; entradas tardías o desordenadas siguen la política elegida.
- [ ] Resolver cruces de ataques sobre la línea temporal autorizada del contacto según la regla funcional acordada. Entregable: el orden de llegada de mensajes no sustituye el orden de contactos; la corrección incluye interrupciones, continuación de cadena o defensa y tiempo restante de NA.
- [ ] Separar la señal inmediata de acción del feedback de resultado. Entregable: comparar impacto anticipado y confirmado, medir demoras y registrar correcciones visibles sin retrasar accidentalmente las señales de ataque rival.

## Accionables: validación online

- [ ] Registrar por acción su identificador, input, recepción, decisión autorizada, resultado local y corrección. Medir diferencias temporales sin comparar directamente relojes de máquinas distintas.
- [ ] Repetir los casos funcionales con RTT medido de 0, 40, 80 y 120 ms. Agregar variación de latencia, pérdida y conexiones asimétricas en ensayos separados antes de combinarlos. Usar 150–200 ms como estrés adicional.
- [ ] Calibrar el emulador distinguiendo demora por sentido de RTT. Registrar configuración y RTT observado para poder reproducir cada prueba.
- [ ] Medir defensas locales rechazadas, resultados de contacto corregidos, demora input-feedback y tiempo de convergencia. Comparar iniciar ataque, esperar en guardia y repetir input bajo las mismas condiciones de red.
- [ ] Revisar los resultados antes de ampliar el combate. Entregable: límites de conexión aceptables y decisión sobre compensación adicional o rollback, basados en los casos fallidos.

Los [ensayos de rendimiento](../game/technical.md#accionables-rendimiento) miden el costo de este núcleo y su proyección a equipos.

## Referencias de implementación

- [Gameplay Attributes y Attribute Sets](https://dev.epicgames.com/documentation/en-us/unreal-engine/gameplay-attributes-and-attribute-sets-for-the-gameplay-ability-system-in-unreal-engine): datos numéricos y hooks de modificación.
- [Gameplay Effects](https://dev.epicgames.com/documentation/en-us/unreal-engine/gameplay-effects-for-the-gameplay-ability-system-in-unreal-engine): aplicación de modificaciones a atributos.
- [Predicción de GAS](https://dev.epicgames.com/documentation/en-us/unreal-engine/API/Plugins/GameplayAbilities/FPredictionKey): mecanismos y límites a verificar al integrar las habilidades.
- [Movimiento en red](https://dev.epicgames.com/documentation/en-us/unreal-engine/understanding-networked-movement-in-the-character-movement-component-for-unreal-engine): predicción y corrección del movimiento.
- [Emulación de red](https://dev.epicgames.com/documentation/unreal-engine/using-network-emulation-in-unreal-engine): configuración de los ensayos.

## Preguntas

1. ¿Qué tags y datos de fase necesita cada habilidad, y cuáles deben comunicarse a otros clientes para reconstruir la acción?
2. ¿Qué política temporal usaremos para aceptar una defensa tardía y qué máximo de historial podrá modificar? ¿Cómo validaremos el tiempo declarado por un cliente?
3. ¿Qué consecuencias de un impacto se predecirán y cómo se corregirán sus dependencias si cambia el resultado?
4. ¿Qué información viajará como estado replicado o evento y qué cadencia, entrega y deduplicación necesita cada una?
5. ¿Qué umbrales de correcciones y demora permitirán aceptar el primer duelo online?
