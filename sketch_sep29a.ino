#include <WiFi.h>
#include <PubSubClient.h>
#include <vector>


const char* ssid = "TP-Link_E716"; //Nome da rede WiFI
const char* password = "08334404"; // Senha da rede
const char* IP_MQTT = "4.228.225.124"; //IP do servidor do MQTT broker




WiFiClient espClient; 
PubSubClient client(espClient);

std::vector<int> velocidade_max; //lista das velocidades maximas

//variaveis potenciometros

int p1 = 32; 
int p2 = 33;
int p3 = 35;


//função conexão ao wifi

void connectWifi(){

  if (WiFi.status() != WL_CONNECTED){
    WiFi.begin(ssid, password);

    if (WiFi.status() == WL_CONNECTED){
      Serial.println("Conectado a Rede WIFI");
    }
  }


}



void setup() {
  pinMode(p1, INPUT);
  pinMode(p2, INPUT);
  pinMode(p3,INPUT);
  Serial.begin(115200);
  connectWifi();
  velocidade_max.push_back(0);
  velocidade_max.push_back(0);
  velocidade_max.push_back(0);
  client.setServer(IP_MQTT, 1883);
}

void loop() {

  //conexão ao broker
  if (!client.connected()){
    Serial.print(".");
    client.connect("device001");
    delay(1000);
    if (client.connected()){
      Serial.println("Dispositivo conectado ao broker");
    }
  }


  //leitura dos potenciometros

  int velocidade = analogRead(p1);
  int velocidade_2 = analogRead(p2);
  int velocidade_3 = analogRead(p3);


  //ajustando valores
  velocidade_2 = map(velocidade_2, 0, 4095, 0, 320);
  velocidade_3 = map(velocidade_3, 0, 4095, 0, 320);
  velocidade = map(velocidade, 0, 4095, 0, 320);


  for (int i = 0; i < velocidade_max.size(); i++){
    Serial.println(velocidade_max[i]);
  }

  //Armazenamento dos picos de velocidade

  if (velocidade > velocidade_max[0]){
    velocidade_max[0] = velocidade;
  }
  if (velocidade_2 > velocidade_max[1]){
    velocidade_max[1] = velocidade_2;
  }
  if (velocidade_3 > velocidade_max[2]){
    velocidade_max[2] = velocidade_3;
  }

  Serial.println("Velocidade 1: " + String(velocidade) + " Velocidade 2: " + String(velocidade_2) + " Velocidade 3: " + String(velocidade_3));


  //Convertendo os valores
  String mensagem_2 = String(velocidade_2);
  String mensagem_3 = String(velocidade_3);
  String mensagem = String(velocidade);
  String velocidade_max_1 = String(velocidade_max[0]);
  String velocidade_max_2 = String(velocidade_max[1]);
  String velocidade_max_3 = String(velocidade_max[2]);


  //publicando valores ao broker
  client.publish("/TEF/device001/attrs/velocidade_2", mensagem_2.c_str());
  client.publish("/TEF/device001/attrs/velocidade_3", mensagem_3.c_str());
  client.publish("/TEF/device001/attrs/velocidade_1", mensagem.c_str());
  client.publish("/TEF/device001/attrs/velocidade_max_1", velocidade_max_1.c_str());
  client.publish("/TEF/device001/attrs/velocidade_max_2", velocidade_max_2.c_str());
  client.publish("/TEF/device001/attrs/velocidade_max_3", velocidade_max_3.c_str());

  delay(500);
  client.loop();
}
