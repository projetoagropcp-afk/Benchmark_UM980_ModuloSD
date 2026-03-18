/*
 * Arquitetura de Integração: UM980 GNSS + SD Card Logger para Benchmark
 * Autor: Pedro Vinicius
 * Plataforma Alvo: ESP32
 */

#include <SPI.h>
#include <SD.h>
#include <SparkFun_Unicore_GNSS_Arduino_Library.h>

// --- Configurações de Pinos ---
// GNSS UART
const int pin_UART1_RX = 16;
const int pin_UART1_TX = 17;

// Cartão SD SPI (Padrão VSPI do ESP32)
const int chipSelect = 5; 

// --- Objetos Globais ---
UM980 myGNSS;
HardwareSerial SerialGNSS(1); // Usa UART1 do ESP32
File logFile;

// --- Variáveis de Controle ---
unsigned long lastLogTime = 0;
const unsigned long LOG_INTERVAL_MS = 1000; // Taxa de gravação: 1Hz
const String filename = "/benchmark_gnss.csv"; // Arquivo salvo no SD

// --- Declaração da Função de Callback (Obrigatória) ---
void output(uint8_t * buffer, size_t length);

void setup() {
  Serial.begin(115200);
  delay(1000); // Aguarda estabilização do monitor serial
  
  Serial.println("\n--- Iniciando Sistema de Benchmark GNSS Dr. Archi-Tek ---");

  // 1. Inicialização do Cartão SD
  Serial.print("Montando o Cartão SD...");
  if (!SD.begin(chipSelect)) {
    Serial.println(" FALHA! Verifique as conexões SPI e se o cartão está inserido.");
    while (true); 
  }
  Serial.println(" SUCESSO!");

  // Criação do cabeçalho CSV se o arquivo não existir
  bool fileExists = SD.exists(filename);
  logFile = SD.open(filename, FILE_APPEND); // Abre em modo Append
  if (logFile) {
    if (!fileExists) {
      logFile.println("Millis,Latitude,Longitude,Altitude_m,SIV,PositionType");
    }
    logFile.close();
  } else {
    Serial.println("Erro ao abrir arquivo para gravar o cabeçalho.");
    while(true);
  }

  // 2. Inicialização do GNSS UM980
  Serial.print("Conectando ao UM980...");
  SerialGNSS.begin(115200, SERIAL_8N1, pin_UART1_RX, pin_UART1_TX);

  // CORREÇÃO: Passando a porta serial, a string identificadora e a função de callback 'output'
  if (myGNSS.begin(SerialGNSS, "SFE_Unicore_GNSS_Library", output) == false) {
    Serial.println(" FALHA! Módulo não detectado na UART1.");
    while (true);
  }
  Serial.println(" SUCESSO!");

  // Configurações do GNSS
  myGNSS.setModeRoverSurvey(); 
  myGNSS.saveConfiguration();

  Serial.println("Sistema Pronto. Coletando dados a 1Hz...");
}

void loop() {
  // Processa as mensagens de fundo do módulo
  myGNSS.update(); 

  // Algoritmo não-bloqueante baseado em millis()
  if (millis() - lastLogTime >= LOG_INTERVAL_MS) {
    lastLogTime = millis();

    // Extrair os dados
    double lat = myGNSS.getLatitude();
    double lon = myGNSS.getLongitude();
    float alt = myGNSS.getAltitude();
    int siv = myGNSS.getSIV(); 
    int posType = myGNSS.getPositionType(); 

    // Formatar a string CSV
    String dataString = String(millis()) + ",";
    dataString += String(lat, 9) + ",";
    dataString += String(lon, 9) + ",";
    dataString += String(alt, 3) + ",";
    dataString += String(siv) + ",";
    dataString += String(posType);

    // Gravação segura no Cartão SD
    logFile = SD.open(filename, FILE_APPEND);
    if (logFile) {
      logFile.println(dataString);
      logFile.close();
      
      Serial.print("Gravado: ");
      Serial.println(dataString);
    } else {
      Serial.println("Erro ao abrir o arquivo para gravar os dados!");
    }
  }
}

//----------------------------------------
// Função Output: Requisito arquitetural da biblioteca Unicore.
// Ela gerencia o despejo de bytes residuais não analisados pela biblioteca principal.
//----------------------------------------
void output(uint8_t * buffer, size_t length)
{
    size_t bytesWritten;
    if (Serial)
    {
        while (length)
        {
            // Aguarda até ter espaço no buffer FIFO da porta serial
            while (Serial.availableForWrite() == 0);

            // Cospe o caractere no console
            bytesWritten = Serial.write(buffer, length);
            buffer += bytesWritten;
            length -= bytesWritten;
        }
    }
}