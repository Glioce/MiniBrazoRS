# MiniBrazoRS
Mini Brazo robótico con matriz de LEDs.

Basado en\
https://www.instructables.com/id/IoT-Motion-Controlled-Servos/
https://www.pubnub.com/blog/motion-controlled-servos-with-leap-motion-raspberry-pi/

Modelo 3D
https://www.thingiverse.com/thing:2373939
https://www.trossenrobotics.com/robotgeek-snapper-mini-9g-3d-printed-robot-arm.aspx

## Pasos de Instalación en RPi

1. Instalar Node-RED desde el menu Inicio --> Preferencias --> Recommended Software
2. Actualizar Node-RED, Node.js e instalar npm con el comando que aparece en la terminal
3. Habilitar puertos I2C y SPI. En la terminal ejecutar sudo raspi-config,
   en el menu ir a Interfacing Options --> SPI, responder Si a la pregunta "Would you like the SPI interface to be enabled?"
   hacer lo mismo con Interfacing Options --> I2C
4. Instalar Dashboard desde el menu Manage palette
   https://flows.nodered.org/node/node-red-dashboard
5. OPCIONAL Instalar biblioteca i2c para usar el nodo I2C Scan (Manage palette)
   https://flows.nodered.org/node/node-red-contrib-i2c
6. Instalar biblioteca pca9685 para controlar el driver de los servos. Usar nodo PWM Output con ancho de pulso en microsegundos
   https://flows.nodered.org/node/node-red-contrib-pca9685

## Pasos de Instalación en Dispositivo Remoto

1. Instalar Node-RED
2. Instalar nodos Dashboard
3. Instalar nodos SerialPort
   https://flows.nodered.org/node/node-red-node-serialport
4. Instalar nodos PubNub
   https://flows.nodered.org/node/node-red-contrib-pubnub

## Opciones para Controlar MAX7219

Algunas opciones se muestran en este tema\
https://discourse.nodered.org/t/how-to-use-max7219-8x8-led-matrix-display/2967

La primera opcion es instalar esta biblioteca, pero ya no es compatible, hay errores de compilacion

Lo que es util es el comando para ver los puertos SPI disponibles\
https://www.npmjs.com/package/max7219

```
$ ls -l /dev/spi*
crw-rw---- 1 root spi 153, 0 oct 24 09:26 /dev/spidev0.0
crw-rw---- 1 root spi 153, 1 oct 24 09:26 /dev/spidev0.1
```
La biblioteca anterior depende de node-spi. Se puede instalar con npm install spi

Esta biblioteca podria ser suficiente para controlar el MAX7219
https://github.com/russtheaerialist-retired-projects/node-spi

USO BÁSICO
```js
var SPI = global.get("spi");

var spi = new SPI.Spi('/dev/spidev0.0', {
    'mode': SPI.MODE['MODE_0'],  // always set mode as the first option
    'chipSelect': SPI.CS['none'] // 'none', 'high' - defaults to low
  }, function(s){s.open();});

var txbuf = new Buffer([ 0x23, 0x48, 0xAF, 0x19, 0x19, 0x19 ]);
var rxbuf = new Buffer([ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 ]);

spi.transfer(txbuf, rxbuf, function(device, buf) {
    // rxbuf and buf should be the same here
    var s = "";
    for (var i=0; i < buf.length; i++)
        s = s + buf[i] + " ";
        console.log(s + "- " + new Date().getTime());
  });
return;
```
La segunda y ultima opcion que se presenta es\
https://www.npmjs.com/package/node-max7219-led-matrix

USO BÁSICO
```js
var max7219LedMatrix = global.get("max7219LedMatrix");
var max7219 = new max7219LedMatrix.max7219("/dev/spidev0.0");
max7219.letter('A');
return;
```

https://github.com/SebSchwartz/node-max7219-led-matrix/blob/master/index.js
Este es el codigo fuente de la biblioteca, aqui podemos ver todos los metodos:
max7219(device); //constructor
_init(); //se usa dentro del constructor
. . .

Para usar las bibliotecas anteriores dentro de Node-RED se puede usar un nodo que instala paquetes de npm temporalmente

https://flows.nodered.org/node/node-red-contrib-function-npm
Los inconvenientes son que necesita conexion a internet y los paquetes se descargan cada vez que se inicia el flujo.

(Las variables globales funcionan!) Una mejor opcion es declarar funciones globales. 
En el archivo settings.js se agregaron las lineas 220 y 221
```js
functionGlobalContext: {
  // ...
  spi : require('spi'),
  max7219LedMatrix : require('node-max7219-led-matrix')
},
```
## Interfaz para detectar movimiento de las manos
Se usa un PIC16F877A con 3 sensores ultrasónicos. Los valores obtenidos con los sensores se envían a través de UART a un módulo Serial-USB. En Node-RED se usa el nodo Serial Input para capturar los datos.

Cuando un objeto está cerca del sensor ultrasónico (y se mide una distancia corta), el brazo se contrae y la matriz de LEDs apunta hacia abajo. Si se mide una distancia grande el brazo se extiende y la matriz de LEDs apunta hacia arriba. El ángulo del primer servomotor se establece al comparar las distancias medidas por los 3 sensores; Si el sensor central mide la menor distancia, el brazo se coloca centrado; Si uno de los sensores laterales mide la menor distancia, el brazo gira hacia el lado correspondiente.

## Hacer
- [x] @mentions, #refs, [links](), **formatting**, and <del>tags</del> supported
- [x] Ver si se puede usar un nodo funcion normal
- [x] Cargar bibliotecas en archivo settings.js (en objeto de variables globales)
- [x] Conectar matriz de LEDs
- [x] Probar funciones ocultas
- [x] Obtener ip para usar dashboard en dispositivo remoto
- [x] Pegar cuernos
- [ ] Acomodar cables de servos
- [ ] Fijar pca
- [x] Voltear matrix
- [ ] Soldar pines y cable flexible
- [ ] Reordenar jumpers en pi
- [ ] Escribir instrucciones para acceder sobre LAN
- [x] Acomodar dashboard

## OTRAS REFERENCIAS
RPi Pinout
https://pinout.xyz

PCA9685 Guia
https://learn.adafruit.com/16-channel-pwm-servo-driver

SG90 Datos
https://servodatabase.com/servo/towerpro/sg90

MAX7219 Datasheet
https://cdn-shop.adafruit.com/datasheets/MAX7219.pdf

Obtener IP para acceso remoto
https://www.raspberrypi.org/documentation/remote-access/ip-address.md

Iniciar Node-RED en el arranque
https://nodered.org/docs/getting-started/raspberrypi
