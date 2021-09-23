#include <NTPClient.h>
#include <WiFiUdp.h>
#include <ESP8266WiFi.h>


// Autor: RAPHAEL KENNEDY
// FUNCAO: Acionamento autonomo de irrigação em horário pré-definido;

char rede [] = "id_da_rede";
char dados[] = "senha";

WiFiUDP udp;
NTPClient ntp(udp, "1.br.pool.ntp.org", -10800, 60000);//Cria um objeto "NTP" com as configurações.utilizada no Brasil
String hora;
int Qirrigacao = 0; //Controle de Irrigação

WiFiServer server(80);

const byte pinoRele = D0;

void setup() {
  Serial.begin(115200);
  pinMode(pinoRele, OUTPUT);
  digitalWrite(pinoRele, LOW);
  delay(10);

  conectaWiFi(rede, dados);

  ntp.begin();          // Inicia o protocolo ntp
  ntp.forceUpdate();    // Atualização

  server.begin();
  Serial.println("Serviço WEB iniciado.");
}

void autoIrrigacao() {

//Serial.println(hora);

  if ((hora > "07:30:00") && (hora < "07:30:10")) { // definido aqui 10 segundos de acionamento.
    Qirrigacao=+1;
    digitalWrite(pinoRele, HIGH);
  } else if ((hora > "19:30:00") && (hora < "19:30:10")) {
    Qirrigacao=+1;
    digitalWrite(pinoRele, HIGH);
  } else {
    digitalWrite(pinoRele, LOW);
    
  }
}

void conectaWiFi(char REDE[], char DADOS[]) {

  Serial.print("Conectando a rede: ");
  Serial.println(REDE);

  WiFi.begin(REDE, DADOS);
  WiFi.hostname("RoboPlanta");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("conectando...");
  }

  Serial.println("");
  Serial.println("WiFi Conectado!");
  Serial.print("Endereço IP: ");
  Serial.println(WiFi.localIP());
}

void loop() {

  hora = ntp.getFormattedTime();
  autoIrrigacao();

  WiFiClient client = server.available();

  if (client) {
    while (client.connected()) {
      {
        client.println("HTTP/1.1 200 OK");
        client.println("Content-Type: text/html");
        client.println("");
        client.println("<!DOCTYPE HTML>");
        client.println("<html>");
        client.println("<title>Robot-Irrigacao</title>");
        client.println("<meta http-equiv='refresh' content='5'>");
        client.println("<meta http-equiv='Content-Type' content='text/html' charset='utf-8'>");
        client.println("<h2>Irrigaçao autonoma - ESP8266</h2>");
        client.println("Horário: ");
        client.println(hora);
        client.println("<br>");
        client.println("Numero de irrigações: ");
        client.println(Qirrigacao);
        client.println("<br>");
        client.println("<br>");
        client.println("</html>");
        client.stop();
      }
    }
  }
}
