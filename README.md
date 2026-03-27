Pinagem Hardware

1. Módulo GNSS UM980 (Comunicação UART)
O código configura a comunicação serial (SerialGNSS) usando os pinos 16 e 17. Lembre-se que em conexões UART, o RX de um dispositivo sempre vai no TX do outro, e vice-versa.

ESP32 Pino 16 (RX): Conecte ao pino TX do UM980.

ESP32 Pino 17 (TX): Conecte ao pino RX do UM980.

2. Módulo de Cartão SD (Comunicação SPI)
O código define explicitamente o pino chipSelect (CS) como 5 e os comentários mencionam o uso do barramento padrão "VSPI" da biblioteca do ESP32. As ligações padrão VSPI são:

ESP32 Pino 5 (CS/SS): Conecte ao pino CS (Chip Select) do módulo SD.

ESP32 Pino 23 (MOSI): Conecte ao pino MOSI (Master Out, Slave In) do módulo SD.

ESP32 Pino 19 (MISO): Conecte ao pino MISO (Master In, Slave Out) do módulo SD.

ESP32 Pino 18 (SCK/CLK): Conecte ao pino SCK (Clock) do módulo SD.

3. Alimentação (Essencial para o funcionamento)
Embora não esteja escrito no código, você precisará alimentar ambos os módulos:

GND: Conecte os pinos GND do UM980 e do módulo SD a qualquer pino GND disponível no seu ESP32.

VCC / Alimentação: Conecte o VCC dos módulos à saída de energia do ESP32. Atenção: Verifique a tensão (3.3V ou 5V) suportada pelas placas (breakouts) específicas que você está usando antes de ligar para evitar danos aos componentes.
