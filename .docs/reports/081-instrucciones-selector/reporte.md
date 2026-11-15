# 081 — Instrucciones en el selector

El selector de personajes incorpora **遊び方** en la esquina superior derecha. El modal abre en japonés, permite cambiar entre japonés e inglés y conserva el idioma elegido mientras existe el HUD. Se cierra con su botón o con Escape.

`SVampsMatchInstructions` contiene los textos y la interacción. `UVampsHUDWidget` lo monta en `SelectionView` de `/Game/Vamps/UI/WBP_VampsHUD` y lo cierra al salir de la selección. Usa la escala del HUD y la fuente compuesta de Slate. Los cambios están en C++.

Se consultó `/Game/Vamps/Input/IMC_Vamps` por MCP para confirmar WASD, RMB, F y Shift izquierdo. Las dos versiones explican el objetivo por puntos, la prioridad entre ataques activos, las continuaciones, la resistencia y la barra de guardia. Un párrafo destaca que hay que pulsar RMB repetidamente durante una ofensiva rival. La recuperación posterior al ataque se describe con la recomendación de usar dash o sprint para tomar distancia. [Guía del editor](editor.md).

Live Coding compiló y aplicó los textos actualizados con `Success`. Durante la integración del modal, `L_LocalMatch` completó `BeginPlay` en PIE y la sesión se cerró por MCP. La apertura del modal y la lectura de ambos idiomas quedan por comprobar manualmente.
