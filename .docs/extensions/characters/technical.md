# Implementación de la identidad de personajes

Comportamiento: [identidad de personajes](functional.md). Base de implementación: [arquitectura de combate](../../combat/technical.md). Etapa: [extensiones Vamps](../technical.md), posterior a la base FGOA adaptada.

## Datos y acciones compartidas

Los perfiles reutilizan la arquitectura de combate. Las diferencias de alcance, acercamiento, pushback y tiempos se expresan en la configuración de sus ataques. Una diferencia numérica no requiere una clase o un componente por personaje.

Los recursos que las mecánicas necesiten utilizan los atributos de GAS descritos en la arquitectura de combate. Las habilidades ejecutan sus reglas; el `AttributeSet` conserva sus responsabilidades numéricas. Los perfiles no agregan wrappers ni duplican el estado de las acciones.

Las skills incorporan comportamientos específicos sobre las mismas reglas de activación, cancelación y autoridad. La lógica común se extrae cuando varios personajes realmente la necesitan.

## Accionables después de la base de referencia

- [ ] Parametrizar las diferencias del primer contraste de perfiles sobre el núcleo de referencia validado, preservando las ventanas defensivas comunes.
- [ ] Comprobar que alcance, desplazamiento y señales de animación coinciden con los contactos resueltos por el juego.
- [ ] Reutilizar los escenarios locales y online de combate para comparar distancias de entrada y salida, oportunidades de respuesta y consumo de recursos entre perfiles.
- [ ] Evaluar las herramientas de intervención del protector y las interacciones entre presión de varios atacantes y salidas defensivas sobre la partida por equipos de la base.

## Preguntas

1. ¿Qué formato de configuración facilita ajustar y comparar personajes sobre la implementación de la base de referencia?
2. ¿Qué diferencias requieren una habilidad propia y cuáles se resuelven con parámetros de una acción compartida?
