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
| 0 | 0x00 | Baudrate seleccionado   | 1200, 2400, 9600, 19200 |
| 1 | 0x01 | Tiempo entre muestras (Segundos)   | 60 <= t <= 21600 |
| 2 | 0x02 | Iniciar una medicion    | x > 0 Para iniciar|
| 3 | 0x03 | Silo lleno | 0, 1 |
| 4 | 0x04 | Sensores activos desde ultima lectura | 0 <= x <= 64 |
| 5 | 0x05 | Temperatura maxima de silo | -2048 <= x <= 2047 |
| 6 - 13 | 0x06 - 0x0D| Temperatura maxima por cable | -2048 <= x <= 2047 |
| 14 - 21 | 0x0E - 0x15 | Sensores activos por cable | 0 , 255 |
| 22 - 29 | 0x16 - 0x1D | Temperatura cable 0 - Sensor 0 al 7  | -2048 <= x <= 2047 |
| 30 - 37 | 0x1E - 0x25 | Temperatura cable 1 - Sensor 0 al 7  | -2048 <= x <= 2047 |
| 38 - 45 | 0x26 - 0x2D | Temperatura cable 2 - Sensor 0 al 7  | -2048 <= x <= 2047 |
| 46 - 53 | 0x2E - 0x35 | Temperatura cable 3 - Sensor 0 al 7  | -2048 <= x <= 2047 |
| 54 - 61 | 0x36 - 0x3D | Temperatura cable 4 - Sensor 0 al 7  | -2048 <= x <= 2047 |
| 62 - 69 | 0x3E - 0x45 | Temperatura cable 5 - Sensor 0 al 7  | -2048 <= x <= 2047 |
| 70 - 77 | 0x46 - 0x4D | Temperatura cable 6 - Sensor 0 al 7  | -2048 <= x <= 2047 |
| 78 - 85 | 0x4E - 0x55 | Temperatura cable 7 - Sensor 0 al 7  | -2048 <= x <= 2047 |

Los valores de temperatura devueltos tienen una longitud de 2 bytes en complemento a 2 y se puede utilizar un int16_t para multiplicarlo luego por 0.0625 para obtener la temperatura leida.
Si el valor leido es -2828 (0xF4F4) se considera un valor erroneo
