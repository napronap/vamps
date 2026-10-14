# Escenas de referencia

Índice de evidencia para [el análisis consolidado](analisis.md). Los tiempos son posiciones en cada MP4. Las láminas conservan juego, controles relevantes, tiempo real del fotograma y número de frame; ayudan a localizar secuencias, no sustituyen su movimiento.

## newdemo1 — cámara, locomoción y guardia

| Tiempo | Evidencia útil | Límite de lectura |
|---|---|---|
| 00:05,20–00:08,40 | Shift sostenido con A, A+S y D; cambia dirección y posición del cuerpo conservando selección. Fondo y encuadre se adaptan. [N01](evidencia/N01-camara-recorrido.jpg) | No permite recuperar una matriz de movimiento ni un umbral numérico |
| 00:08,60–00:14,90 | Carrera prolongada y varias pulsaciones F; cambia el objetivo y la cámara reencuadra | No atribuir todo giro a seguimiento del mismo target |
| 00:15–00:17 | LMB repetido durante juego | Corrige la ausencia de LMB jugable observada en el primer video; no implica ataque separado |
| 00:43–00:48 | Primera acción ofensiva a distancia, termina sin cadena completa y con draw | Útil como repetición del corte; efectos de otros personajes impiden atribuir todo número de daño |
| 00:51,50–00:52,25 | Termina el draw previo y se prepara una nueva ofensiva | Separa redraw previo de la defensa siguiente |
| 00:52,50–00:56,00 | Barreras repetidas, GUARD, pérdidas de HP y cartas todavía seleccionadas; no hay draw entre contactos. [N04](evidencia/N04-guardia-cartas.jpg) | Las cartas giran durante preparación/defensa; distinguir animación de consumo persistente |
| 00:56,25–00:58 | HP 77; aparece CHANCE y sale una acción ofensiva después de la presión | No impone que toda guardia conserve o reanude automáticamente el ataque sin una solicitud posterior |
| 01:23–01:27 | Shift breve sin WASD bajo golpes y, al comienzo, draw; no se reconoce esquiva inmóvil | No es una prueba limpia desde neutral disponible |
| 01:57–02:10 | Sprint prolongado con laterales/diagonales, selección y encuadre amplios. [N02](evidencia/N02-camara-encuadre.jpg) | Cambia el target y uno muere; no usar para medir histéresis |
| 02:27–02:40 | BATTLE FINISH y LOSE después del tramo final | No establece por sí solo la fórmula de derrota ni agotamiento de reservas |

## newdemo2 — preparación defensiva y daño sin draw

### N03: preparación seguida de guardia

En 00:20,50 se organiza una cadena Arts. En 00:20,70 aparece `1st Attack` y la pose de preparación; 00:20,90 muestra barrera y HP 13.851 frente al rival seleccionado. Siguen contactos defendidos en 00:21,30, 00:21,90 y 00:22,50, con HP que llega a 13.544. No hay redraw entre ellos. [Lámina N03](evidencia/N03-preparacion-guardia.jpg).

Confirma visualmente preparación ofensiva que termina en defensa antes de observar la salida propia. El HUD `1st Attack` no certifica que ya se activó el contacto ofensivo. Este episodio enfrenta al seleccionado; no prueba por sí mismo el ataque lateral recordado.

### N05: conservar cartas después de perder HP

| Tiempo | HP | Entrada y HUD |
|---|---:|---|
| 00:36,30 | 13.544 | Cinco cartas; movimiento previo |
| 00:36,33–00:36,57 | 13.544 al comienzo | Shift + A breve |
| 00:36,90 | 12.874 | Golpe, reacción y prompt de pulsaciones rápidas; no barrera GUARD reconocible |
| 00:37,10 | 12.108 | Más daño; cinco cartas y RMB sin pulsar |
| 00:37,70 / 00:37,90 | 11.845 / 11.220 | Continúa daño; no draw |
| 00:38,03 en adelante | — | Se retoman pulsaciones RMB |
| 00:38,10 | 10.292 | Misma mano visible |
| 00:38,90 | 9.789 | Contacto con barrera/GUARD |
| 00:39,50–00:40,50 | 9.789 | Salida de presión y locomoción; cinco cartas, sin redraw |

[Lámina N05](evidencia/N05-danio-sin-draw.jpg). No se observa una ofensiva propia en curso al comienzo. Esto contradice una regla universal HP perdido → cartas consumidas → draw inmediato; no resuelve qué ocurre al recibir daño durante una cadena propia.

### N06: búsqueda de la defensa lateral

En 00:54,33 comienzan pulsaciones RMB y aparece preparación/selección. Se mantiene Master E seleccionado. Entre 00:55,70 y 00:56,30, Master C cruza desde el lateral y realiza una acción de efecto verde mientras persiste presión de Master E; se ven barreras defensivas. [N06](evidencia/N06-presion-lateral.jpg).

El episodio es compatible con presión no seleccionada, pero se superponen contactos. Las aclaraciones sitúan esta preparación/guardia con cartas listas alrededor de 00:51; en el archivo se localiza desde 00:54,33. No se equipara selección a consumo definitivo. [N14](evidencia/N14-guardia-cambia-target.jpg) y [N15](evidencia/N15-retarget-davinci.jpg) aíslan mejor defensa con cambio forzado.

### N10: golpes durante un draw que ya comenzó

En 02:18,40 Gilgamesh ya muestra NA/draw después de su ofensiva. En 02:19–02:20,40 recibe impactos y repite RMB sin barrera. Hacia 02:20,80, con cartas restauradas y terminada la restricción, aparece guardia. [N10](evidencia/N10-draw-impide-guardia.jpg). Este episodio no muestra que el daño haya causado NA ni una conversión lateral de preparación: el draw precede a los golpes.

Las cadenas de proyectiles de 00:08–00:18 y 01:50–01:54 permiten comparar acciones numeradas, varios impactos y redraw sin exigir un acercamiento melee universal. No se incorporan estos repertorios ranged al primer alcance.

## newdemo3 — acercamiento, fallo y otro target

### N08: un ataque fuera de alcance se ejecuta y corta la cadena

1. 00:02,97: primera solicitud RMB después del movimiento manual; el overlay registra W hasta aproximadamente 00:02,87 y D hasta 00:02,90, antes del desplazamiento ofensivo.
2. 00:03,10–00:03,30: `1st Attack`, preparación y avance propio hacia Master A todavía lejano.
3. 00:03,50–00:03,90: efecto del golpe alrededor de Holmes; no alcanza al grupo lejano. La primera carta pasa a usada.
4. Continúan pulsaciones entre aproximadamente 00:03,47 y 00:04,83, pero no se ejecuta `2nd Attack`.
5. 00:04,70: NA y `Command Card: Draw`; HP sigue en 13.161. Las otras acciones no se ejecutaron.

[N08](evidencia/N08-fallo-primer-ataque.jpg). La secuencia separa fallo ofensivo de interrupción por daño, solicitud de continuación de aceptación, y avance permitido de contacto efectivo.

### N09: dos pasos, sin tercero

En 00:06,30 comienza otra acción; hay contactos y defensa próximos con Master A seleccionado a distancia. En 00:07,30 aparece `2nd Attack`. Hacia 00:08,70 quedan dos casillas vacías y la tercera carta disponible. Siguen pulsaciones, pero a 00:09,30 ya se ve draw y no hubo `3rd Attack`. HP permanece en 13.161 durante esta cadena. [N09](evidencia/N09-cadena-parcial.jpg).

Confirma consumo parcial y fin de cadena. No se etiqueta el segundo ataque como fallo aislado por las intervenciones. R03 identifica el primer contacto con otro enemigo como habilitante del segundo paso, conservando Master A seleccionado: no se exige acertar al target para continuar.

### N07/N13: guardia durante la secuencia ofensiva

En 00:10,30 termina draw y se organiza otra secuencia con Master A lejos a la derecha. En 00:10,50 Holmes pasa de 13.161 a 13.005 HP y en 00:10,70 aparece barrera manteniendo lock. [N07](evidencia/N07-guardia-en-secuencia.jpg). Esta muestra no aísla la procedencia del contacto; se corrige su uso anterior como prueba concluyente de enemigo distinto.

Luego aparece `1st Attack` (00:11,10), el gesto ofensivo (00:11,50) y otra barrera (00:11,70–00:11,90), con RMB repetido y sin draw entre ellos. [N13](evidencia/N13-defensa-tras-gesto-ofensivo.jpg). El contacto entrante ranged se defiende durante una acción ya iniciada, sin probar un frame interno de tracing ni defensa automática por animación.

### N14/N15: retarget por guardia efectiva

En `newdemo3`, 00:13,70, Master A sigue seleccionado a la derecha y Master E entra por la izquierda. Hay defensa en 00:14,10 y lock sobre E en 00:14,30, sin F durante ese tramo. Después se ataca a E y se continúa la cadena. [N14](evidencia/N14-guardia-cambia-target.jpg).

En `newdemo2`, F alrededor de 01:05,93 cambia el target manualmente; el daño de E próximo llega manteniendo otra selección. Barrera en 01:07,90–01:08,10 y lock sobre E hacia 01:08,30, sin otra pulsación F. [N15](evidencia/N15-retarget-davinci.jpg). La escena indicada aproximadamente en 01:05 se extiende hasta esa defensa efectiva.

### N16/N17/N18: visibilidad, amenaza y Extra original

`newdemo2`, 02:12–02:14, muestra vegetación próxima perdiendo opacidad con trama visible; permite ver al personaje y el frente del combate. [N16](evidencia/N16-desvanecimiento.jpg). Desvanecimiento visual no demuestra cambio de colisión.

En `newdemo2`, alrededor de 00:46,80–00:47,40, el círculo de proximidad muestra flecha roja de amenaza y luego hay golpe abierto; no hay bloqueo exitoso en ese episodio. [N17](evidencia/N17-aviso-direccional.jpg). El aviso no certifica cobertura trasera.

En `newdemo2`, 02:25,60, hay Buster/Arts/Buster y `Extra Attack Gained`; hacia 02:26,40 aparece la cuarta posición. [N18](evidencia/N18-extra-bab.jpg). La exclusión del Extra en Vamps no depende de asignarle una condición exclusiva Q/A/B que este ejemplo contradice.

### N11 y N12: contraste ranged y Grial

Larva/Tiamat aparece como segundo personaje después de Holmes, aproximadamente en 01:05. En 01:25–01:27 ejecuta tres acciones desde su posición sin WASD, con cámara que se acerca. [N11](evidencia/N11-ataque-estatico.jpg). No corresponde a `newdemo2`.

En 02:06 tiene 8.305 HP durante la tercera acción. En 02:07 aparece Grail Obtained después de la baja del portador y tiene 13.434 HP; luego se mueve durante draw. [N12](evidencia/N12-grial-cura.jpg). Corrobora la restauración vista con Dantès, sin aislar todas las reglas del Grial.

## full demo — referencias conservadas

| Tiempo | Evidencia original |
|---|---|
| 00:17–00:19 | Evasión direccional con Shift. [E02](evidencia/E02-esquiva-001750.jpg) |
| 00:29,20–00:29,40 | Cambio Master C → Master E con F. [E01](evidencia/E01-objetivo-002940.jpg) |
| 00:54–00:56 / 02:02–02:04 | Entrada de reservas d'Eon y Dantès. [E09](evidencia/E09-personaje-005450.jpg), [E10](evidencia/E10-personaje-020250.jpg) |
| 00:57–01:01 | Cadena de tres acciones, varios impactos por acción. [E03](evidencia/E03-cadena-005890.jpg) |
| 01:01–01:02 | Draw/NA con locomoción. [E06](evidencia/E06-redraw-010150.jpg) |
| 01:04–01:07 | Daño inicial, posterior defensa con pérdida de HP. [E04](evidencia/E04-guardia-010550.jpg), [E05](evidencia/E05-guardia-010590.jpg) |
| 01:44–01:47 | Reacción, presión durante indisponibilidad y Stunned explícito. [E07](evidencia/E07-reaccion-014450.jpg), [E08](evidencia/E08-stunned-014750.jpg) |
| 02:22–02:24 | Cuarta acción / Extra Attack y posterior Stunned sobre rival. [E11](evidencia/E11-extra-022290.jpg) |
| 02:42,70–02:42,90 | Dantès 6.633 → 12.071 HP con Grail Obtained. [E12](evidencia/E12-grial-024290.jpg) |
| 02:52–02:55 | NP Ready, BATTLE FINISH y cierre antes del fin del reloj. [E13](evidencia/E13-np-025200.jpg), [E14](evidencia/E14-fin-025500.jpg) |

Los intervalos de input proceden del overlay visible, no de eventos internos del juego. La falta de calibración y las acciones simultáneas se mantienen como límites de las conclusiones.
