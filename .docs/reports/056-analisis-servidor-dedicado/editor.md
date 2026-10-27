# Preparación — Paso 7 y servidor dedicado

Antes de implementar quedan el [plazo de reserva, mínimo humano y propuestas de despliegue/compensación](reporte.md#preguntas-antes-de-implementar). Control por bot al desconectar, transferencia de organizador y 30 s para decidir revancha ya están acordados. Se prioriza infraestructura sencilla en un proveedor global, sin desarrollar ni desplegar un servicio adicional para salas. La recomendación es usar botones con conexión directa a hasta tres endpoints fijos. No hace falta contratar todavía un VPS para completar este diseño. No se desplegó nada.

## 1. Dirección de red

**Hecho — usuario.** Servidor dedicado, partidas privadas de portafolio, jugadores en Japón, dos o tres sesiones simultáneas como máximo y unos meses de operación con coste bajo. Equipos A/B con bots, ready y comienzo por creador. Quien no responde a revancha/salir queda fuera del siguiente ciclo tras 30 s. Al salir el creador, organiza el humano conectado más antiguo. Durante la ronda solo se admite reconexión de un participante anterior, con estadísticas vigentes y control temporal por bot mientras está ausente; la ronda continúa.

**Pendiente — usuario.** Resolver los casos concretos todavía abiertos al final del reporte. Las propuestas técnicas son clientes Windows, servidor Linux x86_64 y una VM en Tokio con hasta tres salas/procesos y compensación defensiva limitada. El control durante la desconexión ya está definido; no se eligieron proveedor/plan ni los parámetros finales de compensación.

## 2. Preparar motor y targets cuando se implemente

**Pendiente — usuario, preparación externa al editor.** Acordar plataforma, disponer de acceso a fuentes de Unreal y preparar un build 5.8 apto para servidor, según la [guía oficial de Epic](https://dev.epicgames.com/documentation/en-us/unreal-engine/setting-up-dedicated-servers-in-unreal-engine). La instalación actual está marcada InstalledBuild y el proyecto no tiene target Server. No se ejecutó una descarga, compilación del motor ni cambio de asociación del proyecto en esta revisión.

**Pendiente — Codex, durante la implementación.** Añadir el target Server propio y configurar el pipeline de build/cook según la plataforma acordada; revisar compatibilidad de módulos/plugins. Conocer la ruta real del motor nuevo antes de ajustar scripts o asociación. No copiar lógica del ejemplo usado por la guía de Epic.

Si se cambian clases/structs de DA, se hará primero búsqueda, captura durable y guardado por MCP de los assets afectados. No corresponde restaurar desde defaults ni asumir que las capturas de 051 siguen describiendo todos los valores actuales.

## 3. Conexiones y comprobación dedicada local

**Pendiente — Codex por MCP, durante la implementación y cuando esté disponible.** Descubrir herramientas/schemas y revisar configuración de modo, GameState/PlayerState propios, clases de participantes y mapas. No hay herramientas MCP directas de Unreal disponibles en esta revisión; las operaciones sin soporte quedarán como instrucciones manuales exactas al conocer sus assets y valores.

**Pendiente — usuario, después de implementar.** Ejecutar un servidor dedicado y dos o más clientes separados. Comprobar equipo A/B y límites de plazas, relleno con bots, ready/START por organizador, combate bidireccional, sincronización de HUD/plazos/puntos, muerte/respawn y revancha con salida de quien no participa. Verificar que una persona nueva no entra durante la ronda y que un participante desconectado sí recupera su plaza/estadísticas. Comprobar contactos y notifies sin render y la compensación acordada con latencia/pérdida controladas manualmente. No se ejecuta nada de esto como parte del análisis ni se piden tests automatizados.

**Pendiente — usuario, solo si elige usar la Mac como servidor local.** Preparar Unreal/Xcode compatibles y una build del servidor para macOS ARM64, revisando los plugins del proyecto. Ejecutar inicialmente una sala y conectar clientes Windows por la IP local de la Mac, permitiendo la conexión en el firewall cuando corresponda. Revisar consumo y tiempos de tick antes de intentar tres salas. La compatibilidad de plataforma está documentada en el [reporte](reporte.md#comprobación-local-en-mac-apple-silicon); no se verificó una build de este proyecto en esa máquina. No se necesita una VM Linux para esta ruta. Puede omitirse el target Mac si se comprueba primero con servidor y clientes Windows; el paquete Linux final se verificará aparte.

## 4. Despliegue posterior

**Pendiente — usuario.** Elegir proveedor/plan y presupuesto máximo tras medir el paquete dedicado. La recomendación es VM Linux x86_64 en Tokio con IP pública y UDP, hasta tres procesos con puertos separados y un servicio que mantenga su disponibilidad. No existe aún el paquete Server ni una configuración de proveedor; no hay un comando de despliegue listo que se pueda dar por verificado.

Antes de la contratación final, ejecutar tres procesos juntos y observar dos o tres rondas activas: consumo total, tiempos de tick, memoria y estabilidad. Comparar el candidato económico de 4 GB con el escalón de 8 GB si fuera necesario; no prometer tres sesiones por el solo número de vCPU/RAM. Los precios mensuales de referencia y fuentes están en el [reporte](reporte.md#hosting-en-japón-y-coste). No contratar un año para reducir una cuota si solo se necesitan unos meses.

Priorizar VM sencilla en Linode/Akamai Tokio o AWS Lightsail Tokio. DigitalOcean no lista Tokio en su disponibilidad oficial actual; Singapur requiere aceptar/verificar otra latencia. Configurar hasta tres ejecuciones del mismo paquete Unreal, cada una con su puerto. Las unidades del sistema solo indican cómo arrancar y supervisar esos ejecutables; no requieren escribir otro programa. El menú conecta por botones a endpoints fijos, sin consulta previa de disponibilidad, búsqueda automática ni reserva de instancias. Equipo, ready, permisos, reconexión y acceso privado se resuelven dentro del servidor del juego, sin backend adicional.

El puerto estándar mostrado por la [guía de Epic](https://dev.epicgames.com/documentation/en-us/unreal-engine/setting-up-dedicated-servers-in-unreal-engine) es 7777, configurable. Al desplegar se debe permitir el UDP del puerto realmente elegido en firewall del proveedor/sistema y publicar el endpoint que escuche el proceso; revisar por separado cualquier puerto auxiliar necesario.

Si se usa subdominio en Cloudflare, configurar su A/AAAA hacia la IP pública como **DNS only**, sin proxy HTTP. El nombre no oculta ni protege ese tráfico mediante el proxy web. [Referencia oficial](https://developers.cloudflare.com/dns/proxy-status/). Este paso no se realizó ni se necesita un dominio real para comenzar la verificación local.

Containers no sustituye esa VM para el tráfico nativo; Spectrum es una contratación/proxy distinta y no está asumida para esta entrega. Tras el despliegue, comprobar conexión desde otra red, logs y consumo real antes de decidir capacidad adicional.
