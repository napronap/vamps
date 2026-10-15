# Convenciones C++ del proyecto

Usar las convenciones de [Epic para Unreal](https://dev.epicgames.com/documentation/unreal-engine/epic-cplusplus-coding-standard-for-unreal-engine), con las decisiones de formato concretas de [.clang-format](../.clang-format) y [.editorconfig](../.editorconfig). Una versión del lenguaje, como C++20, no determina cómo organizar o presentar el código.

## Organización

Las declaraciones `.h` van en `Source/vamps/Public/`; sus implementaciones `.cpp`, en `Source/vamps/Private/`. Ambos árboles reflejan las mismas responsabilidades:

| Carpeta | Contenido |
|---|---|
| `Combat/` | Reglas, tags y trazas del arma |
| `Combat/Abilities/` | Habilidades GAS y su base común |
| `Combat/Attributes/` | Attribute Sets |
| `Combat/Effects/` | Gameplay Effects |
| `Combat/Data/` | Configuración y definiciones de golpes |
| `Characters/` | Personajes y sus componentes nativos |
| `AI/` | Controladores del rival |
| `Player/` | Input y cámara del jugador |
| `GameModes/` | Preparación y reinicio de la partida |
| `Animation/` | Datos de presentación para AnimBP y notifies de ventanas |
| `UI/` | HUD e interfaz |

Ejemplo: `Public/Combat/Abilities/GA_Attack.h` y `Private/Combat/Abilities/GA_Attack.cpp`. El include es `"Combat/Abilities/GA_Attack.h"`; no incluye `Public` ni `Private`. Unreal Build Tool descubre estas carpetas dentro del módulo.

El nombre de carpeta organiza archivos; no cambia el identificador de clase `/Script/vamps.*` que referencian los Blueprints. No renombrar clases o propiedades reflejadas solo por estética.

## Legibilidad

- Código, comentarios y textos incluidos en código en inglés. Documentación interna en español.
- Tabs de cuatro columnas; llaves en líneas separadas; bloques completos incluso para una sentencia.
- Una declaración por línea. Separar funciones y bloques con objetivos diferentes mediante líneas en blanco.
- `UPROPERTY` y `UFUNCTION` encima de su declaración. Mantener `.generated.h` como último include del header.
- Nombres de Unreal con sus prefijos `A`, `U`, `F`, `E` y `b` donde correspondan; nombres locales descriptivos.
- Límite orientativo de 120 columnas, establecido para este repo. Dividir llamadas largas y condiciones complejas para poder recorrerlas visualmente.
- Comentarios para explicar responsabilidad, restricciones y motivos; no narrar cada sentencia.

```cpp
void AVampsCharacter::RequestAttack()
{
	if (HasAuthority())
	{
		AbilitySystem->TryActivateAbility(AttackHandle);
	}
}
```

## Nombres y miembros heredados

No ocultar tipos, atributos ni métodos de Unreal con variables locales, parámetros, atributos o métodos propios. Antes de introducir un nombre, revisar la jerarquía completa de clases base y las interfaces implementadas en los headers del motor instalado. No limitar la búsqueda a la clase base inmediata ni a las funciones virtuales.

Declarar un método con el mismo nombre y otra firma también oculta sobrecargas heredadas: que compile no garantiza que el nombre sea correcto. Para lógica distinta, elegir un nombre que describa su responsabilidad. Por ejemplo, `AVampsCharacter::ReceiveCombatHit` resuelve contactos de combate; `AActor::ReceiveHit` ya pertenece a la notificación de colisiones físicas y no debe ocultarse con una firma propia. Dentro de un PlayerController, usar `ControlledPawn` para una variable local porque `AController` ya declara el miembro `Character`.

Reutilizar un nombre de función virtual heredada únicamente al implementar deliberadamente ese contrato, con firma compatible y `override`. No agregar wrappers, aliases ni declaraciones `using` para conservar un nombre propio que oculta una API del motor: renombrar la función y actualizar sus consumidores.

## Formato y revisión

Aplicar `clang-format --style=file -i <archivos>` únicamente a los archivos propios que se editan. La configuración se comprobó con clang-format 19.1.5; Rider puede usarla al habilitar su integración con ClangFormat. `.editorconfig` fija la indentación básica para editores compatibles.

El formateador mantiene la presentación; la agrupación por responsabilidad y la claridad de nombres se revisan al editar. Revisar nombres de métodos, atributos, parámetros y variables contra las clases base e interfaces, además de comprobar includes y compilar por MCP cuando esté disponible. No agregar ni ejecutar tests automatizados, según [AGENTS.md](../AGENTS.md#sin-tests-automatizados).
