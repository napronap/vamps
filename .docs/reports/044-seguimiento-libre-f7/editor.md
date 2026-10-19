# Editor — Seguimiento libre y F7

**Para el usuario:** primero completar un build con el editor cerrado y reabrirlo; Live Coding compiló pero el editor se cerró al recargar las clases. Después observar en Play el acompañamiento lateral y comprobar F7. No hay assets ni teclas que asignar. Esta guía sustituye los comandos y el rumbo fijo de [043](../043-camara-sin-target/editor.md).

## 1. Preparar y compilar

**Hecho — Codex por MCP.** EditorAppToolset.IsPIERunning detectó Play activo; StopPIE lo detuvo e IsPIERunning confirmó false antes de compilar. Para reproducirlo, detener Play en el editor.

**Hecho — Codex por MCP, aplicación fallida.** LiveCodingToolset.LiveCodingToolset.CompileLiveCoding produjo una compilación UBT Succeeded y enlace del parche, pero la recarga mostró un tamaño incompatible del manager y cerró el editor durante la reinstanciación. No fue NoChanges y tampoco se confirma un parche aplicado correctamente. [Diagnóstico](reporte.md#verificación). No se modifican clases de Data Asset, propiedades de sus structs ni assets.

**Pendiente — usuario.** Con el editor de vamps cerrado, ejecutar desde PowerShell en C:\dev\vamps:

```powershell
.\build.bat
```

Esperar código de salida 0 y Result: Succeeded. Después ejecutar:

```powershell
.\editor.bat
```

build.bat compila vampsEditor Win64 Development con UE 5.8 y -NoHotReload; editor.bat abre vamps.uproject. No abrir otra instancia mientras exista una del mismo proyecto ni continuar si el build falla. La ejecución de estos scripts no está cubierta por MCP; queda manual según AGENTS.md. No se da por comprobado el arranque limpio.

## 2. Arranque

**Pendiente — usuario.** Después del build y reapertura, abrir /Game/Vamps/Maps/L_CharacterBasic e iniciar Play. Comprobar controller propio, manager y selección normal; consultar Output Log por LogVampsCombat. El arranque no verifica la pulsación de F7 ni la sensación de movimiento. MCP dejó de estar disponible al cerrarse el editor; esta comprobación podrá retomarse por MCP cuando vuelva a conectarse.

## 3. Observar la cámara

**Pendiente — usuario.** En /Game/Vamps/Maps/L_CharacterBasic:

1. Iniciar Play y dar foco al juego. Pulsar F7 una vez, sin mantener Shift/Ctrl/Alt: el panel existente debe indicar LOCK ON: None. Output Log, categoría LogVampsCombat, informa debug player lock-on disabled (F7). Los rivales y su IA siguen activos.
2. Mantener A/D, soltar e invertir. La vista acompaña suavemente el desplazamiento con una desviación acotada; no copia el giro corporal ni continúa orbitando al mantener el lateral. Al detenerse recupera el rumbo con suavidad.
3. Repetir con W/S, diagonales, sprint y Shift con dirección. Comprobar que el desplazamiento vertical no inclina lateralmente la vista y que atacar al aire conserva la transición de encuadre. Sin selección real, Shift sin dirección conserva su rechazo existente.
4. Mientras el modo esté activo, F y una guardia efectiva no deben recuperar selección. Pulsar F7 de nuevo: el log indica enabled y se readquiere un rival elegible con la transición de cámara existente.
5. Alternar mientras se mueve y durante un ataque/bloqueo. Pulsar R con el modo desactivado y activado: el reinicio de combate conserva su estado. Detener Play y comenzar otra sesión: lock-on habilitado de nuevo.

F7 está enlazada por el controller de pruebas en builds que no sean Shipping. F6 y R mantienen sus funciones. No se usa consola para alternar el modo.

Las herramientas directas descubiertas no ejecutan pulsaciones de juego; estos pasos quedan manuales, sin automatización de interfaz.
