# 🚨IoT Servidor Alarma 🚨
 
## Table of Contents
- [Descripción](#descripción)
- [Características](#características)
- [Componentes del Sistema](#componentes-del-sistema)
- [Fortalezas del Código](#fortalezas-del-código)
- [Áreas de Mejora](#áreas-de-mejora)
- [Requisitos de Hardware](#requisitos-de-hardware)
- [Requisitos de Software](#requisitos-de-software)
- [Instalación](#instalación)
- [Configuración smtp.gmail.com](#configuracion-smtp.gmail.com)
- [Uso](#uso)
- [Endpoints de la API](#puntos-finales-de-la-api)
- [Nota de Seguridad](#nota-de-seguridad)
- [Mejoras Futuras](#mejoras-futuras)
- [Contribuciones](#contribuciones)
- [Licencia](#licencia)
- [Contacto](#contacto)

## 📄Descripción
Este proyecto implementa un servidor web en un microcontrolador ESP32 que permite activar una respuesta externa como demostración de control remoto IoT en una red local. 

El sistema utiliza un relé para controlar una alarma de 12 voltios, ilustrando cómo los dispositivos IoT pueden interactuar con el mundo físico a través de la red.

El código proporcionado es un ejemplo funcional y educativo. Para aquellos interesados en implementaciones profesionales o versiones optimizadas para uso en producción, se invita a ponerse en contacto directamente con el desarrollador.


## 🌟Características
- Conectividad WiFi
- Servidor web para control remoto de la alarma
- Control de relé para activar una alarma de 12V
- Control del LED incorporado como indicador visual
- Manejo de errores para la conexión WiFi
- Notificaciones via email smtp.

## 🛠️Componentes del Sistema
- Microcontrolador ESP32(y su fuente 5v)
- Relé (conectado al pin 26)
- Alarma de 12V (conectada al relé y su fuente 12v)
- LED incorporado del ESP32 (generalmente en el pin 2)

## 💪Fortalezas del Código
1. **Simplicidad**: El código es directo y fácil de entender.
2. **Manejo de Errores**: La conexión WiFi incluye un mecanismo de tiempo de espera y reinicio automático en caso de fallo.
3. **Flexibilidad**: El código utiliza `LED_BUILTIN` con una definición de respaldo, haciéndolo adaptable a diferentes placas ESP32.
4. **Control Dual**: Activa tanto el relé (para la alarma) como el LED incorporado, proporcionando múltiples indicadores.

## 🔧Áreas de Mejora
1. **Seguridad**: La autenticación está comentada. Para uso en producción, debe ser reimplementada.
2. **Actualizaciones OTA**: La funcionalidad OTA está comentada. Implementarla facilitaría futuras actualizaciones.
3. **Interfaz Web**: Agregar una interfaz HTML simple mejoraría la facilidad de uso para pruebas basadas en navegador.
4. **Configuración**: Las credenciales WiFi y otras constantes están codificadas. Considere usar un archivo de configuración o EEPROM para una gestión más fácil.

## 📦Requisitos de Hardware
- Placa de desarrollo ESP32
- Relé compatible con ESP32
- Alarma de 12V
- Fuente de alimentación de 12V para la alarma

## 💻Requisitos de Software
- Arduino IDE o PlatformIO
- Paquete de soporte para placas ESP32
- Biblioteca WiFi (generalmente incluida con el paquete de placas ESP32)

## 📝Instalación
1. Clone este repositorio o descargue el código fuente.
2. Abra el proyecto en Arduino IDE o PlatformIO.
3. Instale las bibliotecas requeridas y el paquete de soporte de placa.
4. Conecte el relé al pin 26 del ESP32.
5. Conecte la alarma de 12V al relé y a una fuente de alimentación adecuada.
6. Actualice las credenciales WiFi en el código:
   ```cpp
   const char *SSID = "su_ssid_wifi";
   const char *PASSWORD = "su_contraseña_wifi";
   ```
7. Suba el código a su placa ESP32.

## Configuración smtp.gmail.com
- Abrir cuenta google https://myaccount.google.com/
- Activar google 2 factor auth en la pestaña de seguridad.
- Obtener contraseña de applicacion "IoT-Alarm" en http://myaccount.google.com/apppasswords 

## 🚀Uso
1. Encienda su placa ESP32 y la fuente de alimentación de la alarma.
2. La placa se conectará a la red WiFi especificada e imprimirá su dirección IP en el Monitor Serial.
3. Para activar la alarma, envíe una solicitud GET a `http://<DIRECCION_IP_ESP32>/trigger`.
4. La alarma se activará, el relé se cerrará y el LED incorporado se encenderá durante 5 segundos.

## 🌐Endpoints de la API
- `/trigger` (GET): Activa la alarma y el LED durante 5 segundos.

## 🔐Nota de Seguridad
Esta versión del código tiene la autenticación comentada. Para uso en producción, se recomienda implementar una autenticación adecuada y usar HTTPS.

## ⏱️Mejoras Futuras
- Implementar actualizaciones OTA (Over-The-Air)
- Agregar una interfaz web para un control más fácil
- Mejorar las características de seguridad
- Implementar gestión de configuración para una configuración más fácil
- Añadir opciones para personalizar la duración de la alarma

## 🤝Contribuciones
Las contribuciones a este proyecto son bienvenidas. Por favor, haga un fork del repositorio y envíe un pull request con sus cambios.

## 📜Licencia

Este proyecto está licenciado bajo la Licencia MIT - consulta el archivo [LICENSE](LICENSE) para más detalles.


## 📞Contacto
Para mas información o ayuda, favor comunicarse por [LinkedIn](https://www.linkedin.com/in/arland-michelena/).

To get more info or help, fell free to contact me over [LinkedIn](https://www.linkedin.com/in/arland-michelena/).