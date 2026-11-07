# 069 — Arena de la partida local

- `/Game/Vamps/Maps/L_LocalMatch` tiene una plataforma rectangular de 36 × 28 m con base oscura, muros bajos y cuatro pilares. Cuatro muros interiores en diagonal y escalonados dan cobertura a los flancos y dejan libre el trayecto central de salida. Las marcas del suelo dibujan el perímetro, el círculo central y las posiciones de los equipos en ámbar y cian. Se reutilizaron el piso de plantilla y tres cubos de prueba del mapa. Las piezas nuevas usan `Cube` y `Cylinder` de `/Engine/BasicShapes`.
- Las paredes y los pilares bloquean a los personajes, impiden que trepen e ignoran el canal `Camera`. `AVampsPlayerCameraManager` reconoce su etiqueta de componente `VampsCameraFade` y el índice 0 de `M_CameraOccluder` para desvanecerlos cuando ocultan al jugador o al objetivo. Las piezas decorativas tienen colisión desactivada.
- Cuatro `BlockingVolume` invisibles cubren el perímetro hasta Z=480 cm y se solapan en las esquinas. Bloquean a los personajes durante movimiento y empuje, e ignoran `Camera` y `Visibility`. `TeamSpawn_A` y `TeamSpawn_B` quedan a 18 m entre sí, frente a frente. Las cuatro marcas se desplazaron con las posiciones de la formación.
- Se agregaron `MI_ArenaDeck`, `MI_ArenaTrim`, `MI_ArenaAmber` y `MI_ArenaCyan` en `/Game/Vamps/Materials/`, instancias de `BasicShapeMaterial` del motor. La geometría y su colisión se configuran en el mapa.

## Corrección de la oclusión

`AVampsPlayerCameraManager` barre desde la cámara publicada hasta la cápsula del jugador y del objetivo. Usa la cápsula con 8 cm menos de radio y semialtura, e incluye los solapamientos iniciales para detectar una pared cuando la cámara ya está dentro. Los muros con `VampsCameraFade` se desvanecen con una respuesta más rápida al entrar en la vista y recuperan su opacidad al despejarla. El canal `Camera` conserva su función en el barrido de retracción separado.

## Verificación

- Las trazas horizontales detectaron las cuatro paredes en los planos interiores previstos. Las trazas verticales llegaron a Z=0 en el centro, las cuatro posiciones de aparición y una marca lateral. Una consulta de física a Z=320–350 cm detectó cada volumen en su lado y dos en cada esquina, con el centro libre.
- Después de mover los equipos, se confirmó la altura del piso en sus cuatro marcas. Las trazas detectaron los cuatro muros interiores y llegaron al perímetro a través de los carriles Y=−100, 0 y 100. Una consulta de física confirmó que las coberturas quedan fuera de ese corredor. La captura del viewport mostró la distribución completa.
- Se guardó `L_LocalMatch` por MCP y se confirmó `dirty=false`. También se confirmó el guardado de las cuatro instancias.
- En PIE se abrió la selección, se localizaron los 29 actores de la arena, se releyeron los spawns separados 18 m y se repitieron las consultas de esquinas y las trazas por los carriles de salida. Se detuvo PIE y se confirmó su cierre. La ronda jugada queda pendiente.
- Se verificó visualmente que `M_CameraOccluder` oculta el muro suroeste al recibir `VampsCameraFade = 1`. Se restauró y releyó el valor original `[0]` del muro, y se confirmó el guardado del mapa.
- Live Coding compiló y aplicó el parche de `AVampsPlayerCameraManager`. En PIE de `L_CharacterBasic`, el muro entre la cámara y el jugador alcanzó `VampsCameraFade = 1.00`, confirmado por `LogVampsCombat`. La reproducción del recorrido concreto en `L_LocalMatch` queda en la guía.

Las dimensiones, los valores y las comprobaciones del nivel se detallan en la [guía del editor](editor.md).
