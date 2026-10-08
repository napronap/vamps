# 008 — Bloqueo de generación de proyectos Visual Studio

## Alcance y resultado

Diagnóstico del fallo de **Generate Visual Studio project files** del 8 de octubre de 2026. La causa inicial confirmada es una política de control de aplicaciones de Windows que impide cargar `C:\Users\student\AppData\Local\UnrealEngine\Intermediate\Build\BuildRules\MarketplaceRules.dll`. La generación sigue pendiente de una nueva ejecución después de resolver ese bloqueo.

No hubo cambios de código, clases, Blueprints, assets, configuración del proyecto ni seguridad de Windows. Los cambios de C++ presentes al iniciar la revisión pertenecen al trabajo previo del usuario y no se modificaron.

## Evidencia y límites

- El log `Saved/Logs/UnrealVersionSelector-2026.10.08-13.37.08.log` contiene el mensaje japonés que indica bloqueo por una política de control de aplicaciones y el HRESULT `0x800711C7`.
- Windows registra eventos **3077** a las **13:37:09, hora de Japón**, para el `dotnet.exe` incluido en UE 5.8 al intentar cargar esa misma DLL. Identifican la política `{0283ac0f-fff1-49ae-ada1-8a933130cad6}`.
- Después del bloqueo de carga, UBT intenta recompilar el assembly y falla con una `IOException` por archivo en uso. No se identificó el propietario del bloqueo del archivo; no se atribuye a otro editor o build sin evidencia.
- El registro `HKLM\SYSTEM\CurrentControlSet\Control\CI\Policy` devuelve `VerifiedAndReputablePolicyState = 1`, correspondiente a Smart App Control activado. `CiTool --list-policies --json` devuelve acceso denegado; no se pudo asociar con certeza el GUID del evento a una política con nombre.
- En la consulta de procesos no aparecieron `UnrealEditor`, `dotnet`, `MSBuild` ni `LiveCodingConsole`; sí `UnrealVersionSelector`. Esto describe el momento de la consulta, no el momento del fallo.
- `vamps.uproject` está asociado a UE 5.8. El log detecta Visual Studio, MSVC y un SDK Win64 válido; el fallo registrado ocurre al cargar reglas de plugins, antes de compilar C++ del juego.
- No hay herramientas MCP directas de Unreal disponibles en esta sesión. No se ejecutó generación, compilación ni Live Coding, ni se automatizó la interfaz.

## Documentación y continuación

Se agregaron este reporte y la [guía manual](editor.md), se enlazaron desde `.docs/README.md` y se agregó un enlace desde la iteración 007. Esta iteración no cambia la configuración de gameplay de las anteriores.

La intervención de seguridad y la regeneración quedan pendientes del usuario, según la guía. No se borraron cachés ni archivos del proyecto.

Referencias: [eventos de App Control](https://learn.microsoft.com/en-us/windows/security/application-security/application-control/app-control-for-business/operations/event-id-explanations), [Smart App Control y sus opciones](https://support.microsoft.com/en-us/windows/security/threat-malware-protection/smart-app-control-frequently-asked-questions).
