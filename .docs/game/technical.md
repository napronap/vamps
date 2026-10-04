# Base técnica del juego

El juego utiliza Unreal Engine 5.

Comportamiento de referencia: [juego y partida](functional.md).

La arquitectura del personaje para V1 se define en [implementación del combate](../combat/technical.md#arquitectura-de-la-primera-versión).

## Código y recursos del MVP

Las clases, Blueprints, Animation Blueprints y lógica del MVP se crean desde cero sobre las clases nativas de Unreal. No se reutiliza, hereda ni copia código o Blueprints del ejemplo Third Person ni de sus variantes. Del contenido del ejemplo solo se reutilizan recursos como modelos, esqueletos, animaciones, materiales y efectos; las reglas, eventos y conexiones de esos recursos se configuran para el MVP.

El núcleo de reglas se implementa en C++; Blueprints y assets configuran y presentan el juego. `VampsDuelCharacter.h/.cpp` define `AVampsDuelCharacter` sobre `ACharacter`. `DuelPlayerController.h/.cpp` define `ADuelPlayerController` sobre `APlayerController` para controles, interfaz y solicitud de reinicio. `DuelGameMode.h/.cpp` define `ADuelGameMode` sobre `AGameModeBase` para preparar y reiniciar los dos combatientes y el escenario.

Los Blueprints de esas clases, el Animation Blueprint, la interfaz, el mapa y los assets de input del duelo son propios del MVP. El módulo existente se configura para incorporar GAS sin adoptar las clases de ejemplo como base.

La implementación local usa UE 5.8.3 en Windows. `ADuelHUD` dibuja una interfaz de diagnóstico nativa, sin Widget Blueprint: vida, fase, combo, NA, pausa y resultado. `ADuelPlayerController` recibe seis Input Actions y un Mapping Context propios; el shared button se enlaza a `Started`, sin repetición por mantenimiento. `ADuelGameMode` expone las clases, la configuración compartida y las transformaciones de reinicio. Los pasos de conexión de assets están en la [guía del MVP](../reports/mvp-duelo-editor.md).

`ADuelAIController` recibe las mismas solicitudes que el jugador y ejecuta los [modos de prueba](functional.md#duelo-local-de-prueba) con decisiones cada 0,03 s y pulsaciones separadas al menos 0,12 s. No altera la disponibilidad ni aplica daño directamente. El juego online sigue siendo una etapa posterior.

## Accionables de preparación

- [ ] Seleccionar versión exacta de UE5 y plataformas.
- [ ] Crear las clases y Blueprints propios del duelo e integrar GAS en el módulo.
- [ ] Preparar el ensayo cliente-servidor indicado en el [plan técnico de combate](../combat/technical.md), conservando el mismo comportamiento en la prueba contra IA.
- [ ] Evaluar la arquitectura de partida con los resultados de ese ensayo y el objetivo de equipos antes de ampliar funcionalidades.

## Accionables: rendimiento

- [ ] Instrumentar tiempo de paso del servidor, hilos críticos, colisiones, entidades activas, tráfico por cliente y frecuencia de correcciones. Separar solicitudes, eventos, pasos de simulación y paquetes en las métricas.
- [ ] Medir primero el duelo y luego una carga sintética de seis combatientes con las mismas acciones. Repetir ataques y defensas simultáneos para detectar picos, sin implementar todavía reglas de equipo.
- [ ] Usar como objetivos iniciales del ensayo 60 Hz de simulación y 60 FPS estables en clientes. Para el servidor, buscar tiempo habitual inferior a 8 ms y percentil 99 inferior a 12 ms; registrar además todos los pasos que excedan los 16,67 ms disponibles.
- [ ] Medir al cliente por separado: tiempo del hilo de juego, renderizado y GPU, colas de presentación y respuesta al input. Comparar condiciones estables y saturadas de CPU/GPU sin atribuir automáticamente los tirones a la red.
- [ ] Comparar cadencias de envío de estado de 30 y 60 Hz, midiendo el retraso añadido a señales de combate. Elegir por dato qué se actualiza periódicamente y qué necesita comunicación oportuna como evento.
- [ ] Inspeccionar payload, bytes reales enviados/recibidos, paquetes y retransmisiones. Reducir repetición, precisión innecesaria y efectos cosméticos replicados; agrupar datos cuando no perjudique las señales críticas.
- [ ] Activar consultas de impacto solo durante las ventanas necesarias y comprobar su costo en ráfagas. Mantener fuera del dedicado el trabajo visual que no interviene en la resolución del combate.
- [ ] Usar Networking Insights y perfiles de CPU para localizar el costo dominante de cada prueba. Entregable: resultados reproducibles por escenario, con promedio, percentiles y picos.

### Presupuesto de tráfico para contrastar con mediciones

Con seis clientes, 100 bytes de payload por envío de input a 60 envíos/s representan 36 kB/s de entrada al servidor. Un conjunto de cambios de 1.000 bytes por cliente a 30 envíos/s representa 180 kB/s de salida total y 30 kB/s recibidos por cada cliente.

Estas cifras son un ejemplo de cálculo, no un límite ni una medición del motor. Las capturas deben incluir cabeceras, retransmisiones y otros costos del transporte. Un conjunto de cambios puede ocupar varios paquetes.

## Referencias de implementación

- [Networking Insights](https://dev.epicgames.com/documentation/en-us/unreal-engine/networking-insights-in-unreal-engine): análisis del tráfico.
- [RPC](https://dev.epicgames.com/documentation/unreal-engine/remote-procedure-calls-in-unreal-engine): entrega, fiabilidad y restricciones.
- [Sincronización de baja latencia](https://dev.epicgames.com/documentation/unreal-engine/low-latency-frame-syncing-in-unreal-engine): relación entre presentación y respuesta local.

## Preguntas

1. ¿Qué versión exacta de UE5 y qué plataformas de cliente vamos a soportar?
2. ¿Qué modelo de autoridad usaremos para la partida y dónde se ejecutará: servidor dedicado, host jugador u otra arquitectura?
3. ¿Qué límites de tiempo de paso, frame time del cliente y tráfico adoptaremos después de medir el núcleo?
