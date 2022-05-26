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
| 5 - 12 | 0x05 - 0x0C | Sensores activos por cable | 0 , 255 |
| 13 - 20 | 0x0D - 0x14 | Temperatura cable 0 - Sensor 0 al 7  | 0 <= x <= 4096 |
| 21 - 28 | 0x15 - 0x1C | Temperatura cable 1 - Sensor 0 al 7  | 0 <= x <= 4096 |
| 29 - 36 | 0x1D - 0x24 | Temperatura cable 2 - Sensor 0 al 7  | 0 <= x <= 4096 |
| 37 - 44 | 0x25 - 0x2C | Temperatura cable 3 - Sensor 0 al 7  | 0 <= x <= 4096 |
| 45 - 52 | 0x2D - 0x34 | Temperatura cable 4 - Sensor 0 al 7  | 0 <= x <= 4096 |
| 53 - 60 | 0x35 - 0x3C | Temperatura cable 5 - Sensor 0 al 7  | 0 <= x <= 4096 |
| 61 - 68 | 0x5D - 0x44 | Temperatura cable 6 - Sensor 0 al 7  | 0 <= x <= 4096 |
| 69 - 76 | 0x45 - 0x4C | Temperatura cable 7 - Sensor 0 al 7  | 0 <= x <= 4096 |

Los valores de temperatura devueltos tienen el formato del TMP100, para realizar la conversion:

<img src="https://latex.codecogs.com/svg.image?\inline&space;\huge&space;\bg{white}\textbf{temperatura}\left&space;(&space;x&space;\right&space;)&space;=&space;&space;\left\{\begin{matrix}x&space;*&space;0,0625&space;\Rightarrow&space;x&space;\leq&space;2047&space;\\(4096-x)&space;*&space;(-0,0625)&space;&space;\Rightarrow&space;x&space;>&space;2047\end{matrix}\right."  />
