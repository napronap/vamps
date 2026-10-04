# Preparación del MVP: guía del editor

**Pendiente para el usuario:** nada para cerrar esta preparación. Los pasos siguientes ya se completaron; sirven para reproducirla si se abre otra sesión. La creación y configuración de assets del MVP quedan fuera de esta iteración.

Proyecto: `C:\dev\vamps\vamps.uproject`. Editor comprobado: UE 5.8.3. [Reporte de cambios y límites de la verificación](preparacion-mvp.md).

## 1. Abrir el proyecto

**Estado: Hecho — usuario.**

Abrir `vamps.uproject` con su instalación de Unreal 5.8, directamente o desde Rider. Esperar a que termine de cargar el editor antes de conectar MCP.

**Resultado esperado:** editor del proyecto `vamps` disponible.

## 2. Habilitar los plugins

**Estado: Hecho — usuario; configuración verificada por Codex.**

1. En `Edit > Plugins`, habilitar los plugins `ModelContextProtocol`, `AllToolsets` y `LiveCodingToolset`.
2. Si el editor solicita reiniciarse, guardar el trabajo y reiniciarlo antes de continuar.
3. Confirmar que los tres figuran habilitados en el proyecto.

**Resultado comprobado:** los tres tienen `Enabled: true` en `vamps.uproject`; la herramienta de Live Coding está disponible en la sesión MCP.

## 3. Iniciar el servidor MCP

**Estado: Hecho — usuario.**

1. Abrir el panel `Output Log` del editor.
2. En su campo de comandos `Cmd`, ejecutar `ModelContextProtocol.StartServer`.
3. Revisar la confirmación de inicio del servidor en el log.

**Resultado comprobado:** servidor iniciado en `127.0.0.1:8000` y conexión MCP funcional. Al reiniciar el editor, repetir este paso si el servidor no se inicia automáticamente.

## 4. Verificar las herramientas disponibles

**Estado: Hecho — Codex por MCP.**

Con el editor y el servidor abiertos, solicitar a Codex que consulte `list_toolsets` y `describe_toolset` para `LiveCodingToolset.LiveCodingToolset`. Este paso se hace desde el cliente MCP; no requiere navegación por la UI del editor.

**Resultado comprobado:** aparece `CompileLiveCoding` con sus parámetros y formato de resultado. No alcanza con que el plugin figure habilitado en el archivo del proyecto.

## 5. Ejecutar y comprobar Live Coding

**Estado: Hecho — Codex por MCP; prueba sin cambios pendientes.**

1. Solicitar a Codex una compilación mediante `LiveCodingToolset.LiveCodingToolset.CompileLiveCoding`.
2. Esperar su resultado y revisar los mensajes devueltos.
3. Para reproducir manualmente la compilación desde el editor, ejecutar `LiveCoding.Compile` en el campo `Cmd` de `Output Log` y revisar allí el resultado. Esta alternativa manual corresponde al usuario.

**Resultado comprobado:** `Result: NoChanges` y `Live coding succeeded, no code changes detected`. No se aplicó un parche nuevo porque no había cambios de código pendientes.

## Próxima iteración

Cuando se autorice la implementación del MVP, su propia guía deberá incluir la creación de assets, asignaciones, valores, guardado y pruebas que efectivamente requiera. Cada operación sin soporte MCP directo quedará como **Pendiente — usuario**, con sus instrucciones; no se intentará resolver mediante UI automatizada.
