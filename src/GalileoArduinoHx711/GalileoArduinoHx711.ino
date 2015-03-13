#define m_sckPinCtx 2
#define m_dataPinCtx  3

#define SAMPLES_LOG2 8
#define SAMPLES 4

int32_t adcRead()
{
  int32_t v = 0;

  while (digitalRead(m_dataPinCtx) == HIGH);
  
  for (int i=0; i<24; i++)
  {
    digitalWrite(m_sckPinCtx, 1);
    
    v <<= 1;
    v |= (digitalRead(m_dataPinCtx) == HIGH) ? 1 : 0;
    
    digitalWrite(m_sckPinCtx, 0); 
  }
  
  digitalWrite(m_sckPinCtx, 1);
  digitalWrite(m_sckPinCtx, 0);
  
  v |= (v & 0x00800000) ? 0xff000000 : 0x00000000; // sign extend
  
  return v;
}

int32_t preciseReading()
{
  int32_t v = 0;
  for (int i=0; i<SAMPLES; i++)
  {
    v += adcRead();
  }
  return ((v/SAMPLES) >> SAMPLES_LOG2);
}

int32_t calibration = 0;

void setup() {
  Serial.begin(9600);
  Serial.println("hx711 v0.1");

  pinMode(m_sckPinCtx, OUTPUT);
  pinMode(m_dataPinCtx, INPUT);
  
  calibration = preciseReading();
  Serial.print("initial calibration: ");
  Serial.println(calibration);
}

void loop() 
{
  int32_t v = preciseReading();
  Serial.print("v: ");
  Serial.println(v - calibration);
}
