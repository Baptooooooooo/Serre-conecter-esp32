#include <WiFi.h>
#include <ArduinoWebsockets.h>
#include <DHTesp.h>
#include <ArduinoJson.h>


#include <ESP32Servo.h>




const char* ssid = "tufaitquoila";
const char* password = "admin123";
const char* serverAddress = "45.145.167.182";
const int serverPort = 81; // Port du serveur WebSocket

Servo myservo;
#define SERVO_PIN 27  // Broche à laquelle est connecté le servo
#define DHTPIN 14 // Pin pour le capteur H/T
#define AROSAGEon 13 // Pin pour l'arrosage on
#define DHTTYPE DHTesp::DHT11 // initialise le capteur H/T
#define LIGHTPIN 35 // capteur de luminositer 
#define MAX_LIGHT_VALUE 4095 // valeur max capteur liminositer 


//rubant led 
#define RED_PIN 15
#define GREEN_PIN 2
#define BLUE_PIN 0
#define MOTEUR 4 


int aro = 0; // Ajout d'un point-virgule ici

DHTesp dht; // Ajout d'un point-virgule ici
bool connected = false;

// pour les calcule de pourcentage dde la lumière 
int lastLightPercentage = -1;
float lastTemperature = -999.0;
float lastHumidity = -999.0;

using namespace websockets;

WebsocketsClient client;

void onMessageCallback(WebsocketsMessage message) {
    Serial.print("Message reçu: ");
    Serial.println(message.data());

    // Convertir le message JSON en objet JSON
    DynamicJsonDocument jsonMessage(200);
    DeserializationError error = deserializeJson(jsonMessage, message.data());
    if (error) {
        Serial.println("Erreur lors de la désérialisation du JSON");
        return;
    }

    // Vérifier le contenu du message et agir en conséquence
    if (jsonMessage.containsKey("arosage")) {
        String arrosage = jsonMessage["arosage"];
        if (arrosage == "on") {
            Serial.println("Démarrage de l'arrosage");

            if (aro == 0) { // Correction de la syntaxe ici

                digitalWrite(AROSAGEon, HIGH);
                aro = 1; // Correction de la syntaxe ici
            }

        } else if (arrosage == "off") {
            Serial.println("Arrêt de l'arrosage");

            if (aro == 1) { // Correction de la syntaxe ici

                digitalWrite(AROSAGEon, LOW);
                aro = 0; // Correction de la syntaxe ici
            }
        }
    } else if (jsonMessage.containsKey("feunètre")) {
        String feunetre = jsonMessage["feunètre"];

        if (feunetre == "on") {
            Serial.println("Allumer le feunètre");
            // Ajoutez ici le code pour allumer le feu nètre
            myservo.write(0);
            digitalWrite(MOTEUR, HIGH);
        } else if (feunetre == "off") {
            Serial.println("Éteindre le feunètre");
            myservo.write(30);
            digitalWrite(MOTEUR, LOW);
            // Ajoutez ici le code pour éteindre le feu nètre
        }
    }else if (jsonMessage.containsKey("color")) {
    JsonArray colors = jsonMessage["color"];

    // Assurez-vous que le tableau a au moins 3 éléments
    if (colors.size() >= 3) {
      int red = colors[0];
      int green = colors[1];
      int blue = colors[2];

      setColor(red , green , blue);
    }
    }
}

void setColor(int red, int green, int blue) {
  analogWrite(RED_PIN, red);
  analogWrite(GREEN_PIN, green);
  analogWrite(BLUE_PIN, blue);
}



void onEventsCallback(WebsocketsEvent event, String data) {
    if (event == WebsocketsEvent::ConnectionOpened) {
        Serial.println("Connnection Opened");
        connected = true;
    } else if (event == WebsocketsEvent::ConnectionClosed) {
        Serial.print(".");
        connected = false;
    } else if (event == WebsocketsEvent::GotPing) {
        Serial.print(".");
    } else if (event == WebsocketsEvent::GotPong) {
        Serial.print(".");
    }
}
void stop() {

    digitalWrite(AROSAGEon, LOW);
}

void setup() {
    Serial.begin(115200);
    WiFi.begin(ssid, password);

    pinMode(AROSAGEon, OUTPUT);  // Définir le pin de l'arosage on comme sortie
    pinMode(MOTEUR, OUTPUT);  // Définir le pin du moteur
    stop();


    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("\nConnecté au réseau WiFi");

    dht.setup(DHTPIN, DHTTYPE);

    client.onMessage(onMessageCallback);
    client.onEvent(onEventsCallback);

    // Connecter au serveur WebSocket
    String serverUrl = "ws://" + String(serverAddress) + ":" + String(serverPort);
    client.connect(serverUrl);
    myservo.attach(SERVO_PIN);  // Attacher le servo à la broche
    myservo.write(30);
}

void loop() {
    client.poll(); // Gestion des événements WebSocket

    if (!connected) {
        // Si la connexion n'est pas établie, essayer de se reconnecter
        Serial.println("Reconnecting...");
        String serverUrl = "ws://" + String(serverAddress) + ":" + String(serverPort);
        client.connect(serverUrl);
        delay(5000); // Attendre un court instant avant de réessayer
        return;
    }

    // Mesurer la lumière
    int lightValue = analogRead(LIGHTPIN);
    int lightPercentage = round((lightValue / (float)MAX_LIGHT_VALUE) * 100);

    // Mesurer la température et l'humidité
    float temperature = dht.getTemperature();
    float humidity = dht.getHumidity();
    humidity = humidity-40;
    // Vérifier si les valeurs des capteurs ont changé
    if (temperature != lastTemperature || humidity != lastHumidity || lightPercentage != lastLightPercentage) {
        // Créer un objet JSON avec les nouvelles données
        DynamicJsonDocument jsonData(200);
        jsonData["temperature"] = temperature;
        jsonData["humidity"] = humidity;
        jsonData["light"] = lightPercentage;

        // Convertir l'objet JSON en chaîne
        String jsonString;
        serializeJson(jsonData, jsonString);

        // Envoyer les données au serveur WebSocket
        client.send(jsonString);

        // Mettre à jour les valeurs précédentes
        lastTemperature = temperature;
        lastHumidity = humidity;
        lastLightPercentage = lightPercentage;
    }




    delay(1000); // Attente de 500 ms avant la prochaine lecture des capteurs et envoi des données
}