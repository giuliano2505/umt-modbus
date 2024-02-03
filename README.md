# MODBUS-UMT
## Version funcional de la UMT con comunicacion MODBUS Esclavo
### Hardware
Este firmware se implemento sobre el hardware provisto por D'Ascanio, basado en un PIC18F242, con comunicacion RS485, direccionamiento mediante DIPSwitchs para hasta 16 dispositivos, multiplexores para las lineas de Data y Clock de I2C y una entrada logica para la instalacion de un sensor de "Silo lleno".
La unidad se comunica hasta con 8 "cables", numerados desde el cable 0 (usualmente al centro del silo) hasta el 7. Cada uno de los cables puede tener hasta 8 sensores TMP100, numerados desde el fondo del silo hacia arriba desde el 0 al 7. Los sensores TMP100 son de resolucion configurable, pero en esta implementacion se utilizaron con 12 bits de resolucion por practicidad, ya que los tiempos no son problema. A futuro se puede agregar un Holding Register para esto.
Por otro lado, si bien el registro de Silo Lleno está presente y es funcional, no se encuentra agregado en el Hardware de prueba, por lo que no se pudo verificar completamente.
El sistema soporta la lectura de multiples registros (Comando 0x03) y la escritura de registros por separado (0x06), pero no la escritura de multiples registros (0x0F).
Se incluye una hoja de calculo con el mapeo de esos holding registers.
### Mapa de registros
| Numero | Direccion(HEX) | Funcion  | Valores Posibles |
| - | - | - | - |
| 1 | 0x01 | Baudrate seleccionado   | 1200, 2400, 9600, 19200 |
| 2 | 0x02 | Tiempo entre muestras (Segundos)   | 60 <= t <= 21600 |
| 3 | 0x03 | Iniciar una medicion    | x > 0 Para iniciar|
| 4 | 0x04 | Silo lleno | 0, 1 |
| 5 | 0x05 | Sensores activos desde ultima lectura | 0 <= x <= 64 |
| 6 | 0x06 | Temperatura maxima de silo | -2048 <= x <= 2047 |
| 7 - 14 | 0x07 - 0x0E| Temperatura maxima por cable | -2048 <= x <= 2047 |
| 15 - 22 | 0x0F - 0x16 | Sensores activos por cable | 0 , 255 |
| 23 - 30 | 0x17 - 0x1E | Temperatura cable 0 - Sensor 0 al 7  | -2048 <= x <= 2047 |
| 31 - 38 | 0x1F - 0x26 | Temperatura cable 1 - Sensor 0 al 7  | -2048 <= x <= 2047 |
| 39 - 46 | 0x27 - 0x2E | Temperatura cable 2 - Sensor 0 al 7  | -2048 <= x <= 2047 |
| 47 - 54 | 0x2F - 0x36 | Temperatura cable 3 - Sensor 0 al 7  | -2048 <= x <= 2047 |
| 55 - 62 | 0x37 - 0x3E | Temperatura cable 4 - Sensor 0 al 7  | -2048 <= x <= 2047 |
| 63 - 70 | 0x3F - 0x46 | Temperatura cable 5 - Sensor 0 al 7  | -2048 <= x <= 2047 |
| 71 - 78 | 0x47 - 0x4E | Temperatura cable 6 - Sensor 0 al 7  | -2048 <= x <= 2047 |
| 79 - 86 | 0x4F - 0x56 | Temperatura cable 7 - Sensor 0 al 7  | -2048 <= x <= 2047 |

Los valores de temperatura devueltos tienen una longitud de 2 bytes en complemento a 2 y se puede utilizar un int16_t para multiplicarlo luego por 0.0625 para obtener la temperatura leida.
Si el valor leido es -2828 (0xF4F4) se considera un valor erroneo
