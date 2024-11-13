void setup () {
  pinMode(18,INPUT_PULLUP);
  Serial.begin(115200);
}
void loop () {
  int a = digitalRead(18);
  Serial.print("SENSOR:");
  Serial.println(a);
}
