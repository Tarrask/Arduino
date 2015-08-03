#define AIR_PIN A1

void setup() {
  Serial.begin(9600);
  Serial.println("Starting...");
  // utilisation du pin en entrée
  pinMode(AIR_PIN, INPUT);
}

void loop() {
  // on lit la valeur et l'écrit sur la liaison série
  int value = analogRead(AIR_PIN);
  Serial.print("polution indice: ");
  Serial.println(value);
  
  // on attend quelques instant
  delay(500);
}
