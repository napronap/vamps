# 082 — Pausa y salida de partida

`AVampsMatchPlayerController` recibe la acción digital `IA_Pause` con Escape y avisa a `UVampsHUDWidget`. El HUD abre `SVampsPauseMenu` durante la cuenta regresiva o la partida activa. La pausa nativa de Unreal congela la ronda y conserva su fase. El botón **Resume** o una nueva pulsación de Escape reanudan el juego y devuelven el foco al viewport.

El selector incorpora **Quit Game** junto al acceso a las instrucciones. El selector y la pausa usan la misma acción de salida. El modal de instrucciones incluye Escape en ambos idiomas.

[IMC_Vamps](input-despues.json) agrega un único mapping de Escape y conserva las siete entradas previas en el mismo orden. `BP_VampsMatchPlayerController` referencia `IA_Pause` en `PauseAction`. Se verificaron por MCP ambos assets y la acción después del guardado. Mientras la pausa está abierta, Slate maneja el Escape para reanudar con foco en el modal.

Live Coding aplicó el parche con `Success`. `L_LocalMatch` completó `BeginPlay` en PIE con el mapping y la clase configurados. La prueba interactiva de pausa, reanudación y salida queda en el flujo manual de Standalone.

[Guía del editor](editor.md).
