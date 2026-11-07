# Arena de `L_LocalMatch` en Unreal

**Queda para el usuario.** Jugar una ronda y comprobar el desvanecimiento al pasar junto a los muros interiores, el dash y los empujes contra el perímetro.

## 1. Plataforma y presentación

**Hecho — Codex por MCP.** En `/Game/Vamps/Maps/L_LocalMatch`, ajustar `Arena_Deck` a 36 × 28 m, con centro `(200, 0)` y superficie en Z=0. Usar `BlockAll` en su componente estático. Construir la base, los bordes del suelo y las marcas de aparición con `Cube`, y el círculo central con dos `Cylinder` superpuestos. Dejar las marcas y el zócalo en `NoCollision`. Las piezas quedan agrupadas en `Arena/Structure` y `Arena/Markings` en el Outliner. Mover `TeamSpawn_A` a `(-700, 0, 100)` y `TeamSpawn_B` a `(1100, 0, 100)`, con yaw de 0° y 180° respectivamente. Alinear las marcas de cada equipo al mismo X, en Y=±100.

**Hecho — Codex por MCP.** Crear las cuatro instancias en `/Game/Vamps/Materials/` con padre `/Engine/BasicShapes/BasicShapeMaterial`. `MaterialInstanceTools` configuró y releyó los parámetros, y `AssetTools.save_assets` guardó las instancias.

| Instancia | Color lineal RGB | Roughness |
|---|---|---:|
| `MI_ArenaDeck` | `(0.19, 0.205, 0.22)` | 0.82 |
| `MI_ArenaTrim` | `(0.055, 0.075, 0.096)` | 0.79 |
| `MI_ArenaAmber` | `(0.55, 0.25, 0.065)` | 0.65 |
| `MI_ArenaCyan` | `(0.055, 0.32, 0.42)` | 0.65 |

## 2. Paredes y límites

**Hecho — Codex por MCP.** Usar `SceneTools` y `ActorTools` para colocar los cubos y volúmenes. Las cuatro paredes tienen 100 cm de grosor y 120 cm de alto. Los pilares de 120 × 120 × 205 cm ocupan las cuatro combinaciones X/Y de los centros de las paredes. En todos esos cubos, `ObjectTools` configuró `Custom / WorldStatic / QueryAndPhysics`, `Pawn=Block`, `Camera=Ignore`, `Can Character Step Up On=No` y `Use Default Collision=false`. Aplicar `/Game/Vamps/Materials/M_CameraOccluder`, Component Tag `VampsCameraFade` y Custom Primitive Data `[0]`.

| Pared | Centro XYZ en cm | Dimensiones XYZ en cm |
|---|---|---|
| Oeste / Este | `(-1490, 0, 60)` / `(1890, 0, 60)` | `100 × 2480 × 120` |
| Norte / Sur | `(200, 1290, 60)` / `(200, -1290, 60)` | `3280 × 100 × 120` |

**Hecho — Codex por MCP.** Agregar cuatro cubos interiores en `Arena/Structure/Cover`, con la misma configuración de colisión y cámara que el perímetro. Los muros quedan desplazados hacia los flancos para dejar libre el acceso central desde ambos equipos.

| Cobertura | Centro XYZ en cm | Dimensiones XYZ en cm | Yaw |
|---|---|---|---:|
| Suroeste / Noreste | `(-190, -520, 77.5)` / `(590, 520, 77.5)` | `510 × 85 × 155` | 20° |
| Noroeste / Sureste | `(-630, 640, 90)` / `(1030, -640, 90)` | `340 × 95 × 180` | −58° |

**Hecho — Codex por MCP.** Colocar cuatro `BlockingVolume` en `Arena/Boundaries`. En el Brush Component usar `Custom / WorldStatic / QueryAndPhysics`, `Pawn=Block`, `Camera=Ignore`, `Visibility=Ignore` y `Can Character Step Up On=No`. Las dimensiones de la tabla son finales en el mundo, con los centros a Z=220 y los límites verticales de −40 a 480 cm.

| Volumen | Centro XYZ en cm | Dimensiones XYZ en cm |
|---|---|---|
| Oeste / Este | `(-1480, 0, 220)` / `(1880, 0, 220)` | `60 × 2700 × 520` |
| Norte / Sur | `(200, 1280, 220)` / `(200, -1280, 220)` | `3500 × 60 × 520` |

## 3. Comprobación de ronda

**Hecho — Codex por MCP.** `SceneTools.trace_world` confirmó las cuatro paredes y la altura del piso bajo las marcas. `SceneTools.find_actors` con `WorldStatic` confirmó por consulta de física que los cuatro lados tienen volumen y que cada esquina tiene dos. `AssetTools.save_assets` guardó `L_LocalMatch` y `is_dirty` devolvió `false`.

**Hecho — Codex por MCP.** `EditorAppToolset.StartPIE` abrió la selección en el mapa actualizado. En el mundo PIE se confirmaron los dos spawns, los carriles de salida y el solapamiento en las cuatro esquinas. Se usó `StopPIE` y `IsPIERunning` devolvió `false`.

**Hecho — Codex por MCP.** `ObjectTools` configuró temporalmente `Arena_Cover_Southwest` con Custom Primitive Data `[1]`. `CaptureViewport` mostró que desapareció. Se restauró y verificó `[0]`, y se guardó `L_LocalMatch`.

**Hecho — Codex por MCP.** `LiveCodingToolset.LiveCodingToolset.CompileLiveCoding` aplicó el parche de cámara. Se abrió `/Game/Vamps/Maps/L_CharacterBasic` en PIE y se activó temporalmente `VeryVerbose` para `LogVampsCombat` con `LogsToolset`. El log confirmó que `CameraFadeWall` llegó a 1.00 cuando estaba entre el jugador y la cámara. Se detuvo PIE, se volvió a `L_LocalMatch` y se restauró la verbosidad `Log`.

**Pendiente — usuario.** Abrir `L_LocalMatch`, iniciar PIE, elegir personaje y pulsar **START**. Comprobar que las dos formaciones aparecen sobre sus marcas. Pasar junto a una cobertura con la cámara del otro lado. Comprobar que se desvanece cuando tapa al jugador o al objetivo y reaparece al despejar la vista. Repetir por una esquina del muro y comprobar que conserva el bloqueo del personaje. Probar correr y hacer dash contra el perímetro y recibir un empuje cerca del borde. Detener PIE al terminar.
