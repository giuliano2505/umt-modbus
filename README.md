# MODBUS-UMT
##Version funcional de la UMT con comunicacion MODBUS Esclavo
###Hardware
Este firmware se implemento sobre el hardware provisto por D'Ascanio, basado en un PIC18F242, con comunicacion RS485, direccionamiento mediante DIPSwitchs para hasta 16 dispositivos, multiplexores para las lineas de Data y Clock de I2C y una entrada logica para la instalacion de un sensor de "Silo lleno".
La unidad se comunica hasta con 8 "cables", numerados desde el cable 0 (usualmente al centro del silo) hasta el 7. Cada uno de los cables puede tener hasta 8 sensores TMP100, numerados desde el fondo del silo hacia arriba desde el 0 al 7. Los sensores TMP100 son de resolucion configurable, pero en esta implementacion se utilizaron con 12 bits de resolucion por practicidad, ya que los tiempos no son problema. A futuro se puede agregar un Holding Register para esto.
Por otro lado, si bien el registro de Silo Lleno está presente y es funcional, no se encuentra agregado en el Hardware de prueba, por lo que no se pudo verificar completamente.
El sistema soporta la lectura de multiples registros (Comando 0x03) y la escritura de registros por separado (0x06), pero no la escritura de multiples registros (0x0F).
Se incluye una hoja de calculo con el mapeo de esos holding registers:
| Numero | Direccion(HEX) | Funcion  | Valores Posibles |
| - | - | - | - |
| 0 | 0x00 | Baudrate seleccionado   | 1200, 2400, 9600, 19200 |
| 1 | 0x01 | Tiempo entre muestras (Segundos)   | 60 <= t <= 21600 |
| 2 | 0x02 | Iniciar una medicion    | x > 0 Para iniciar|
| 3 | 0x03 | Silo lleno | 0, 1 |
