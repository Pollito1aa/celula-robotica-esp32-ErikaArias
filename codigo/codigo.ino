#define LedA_Proc 4
#define LedV_Insp 16
#define LedR_Ready 17
#define AUTH 18
#define CLASS 19
#define IDLE 21

volatile int ticks = 0;
volatile int counter = 0;

int estado = 1;

hw_timer_t * timer = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

void IRAM_ATTR onTimer()
{
  portENTER_CRITICAL_ISR(&timerMux);
  ticks = 1;
  counter++;
  portEXIT_CRITICAL_ISR(&timerMux);
}

void IRAM_ATTR isrAuth()
{
  if (estado == 1) estado = 2;
}

void IRAM_ATTR isrClass()
{
  if (estado == 2) estado = 3;
}

void IRAM_ATTR isrIdle()
{
  if (estado == 6)
  {
    Serial.println("Sistema reiniciado");
    estado = 1;
  }
}

void setup()
{
  Serial.begin(9600);

  pinMode(LedA_Proc, OUTPUT);
  pinMode(LedV_Insp, OUTPUT);
  pinMode(LedR_Ready, OUTPUT);
  pinMode(AUTH, INPUT_PULLUP);
  pinMode(CLASS, INPUT_PULLUP);
  pinMode(IDLE, INPUT_PULLUP);

  attachInterrupt(AUTH, isrAuth, FALLING);
  attachInterrupt(CLASS, isrClass, FALLING);
  attachInterrupt(IDLE, isrIdle, FALLING);

  timer = timerBegin(1000000);
  timerAttachInterrupt(timer, &onTimer);
  timerAlarm(timer, 1000000, true, 0);
}

void loop()
{
  if (ticks == 1)
  {
    ticks = 0;

    if (estado == 1)
    {
      Serial.println("Estado 1 - IDLE/ REPOSO");
      digitalWrite(LedA_Proc, LOW);
      digitalWrite(LedV_Insp, LOW);
      digitalWrite(LedR_Ready, LOW);
    }
    else if (estado == 2)
    {
      Serial.println("Estado 2 - AUTH/ DETECCIÓN DE PIEZA");
    }
    else if (estado == 3)
    {
      Serial.println("Estado 3 - CLASS");
      estado = 4;
      counter = 0;
    }
    else if (estado == 4)
    {
      Serial.print("Estado 4 (LED AMARILLO) - PROCESS / Tiempo: ");
      Serial.println(counter);

      digitalWrite(LedA_Proc, HIGH);

      if (counter >= 5)
      {
        digitalWrite(LedA_Proc, LOW);
        estado = 5;
        counter = 0;
      }
    }
    else if (estado == 5)
    {
      Serial.print("Estado 5 (LED VERDE) - INSPECT / Tiempo: ");
      Serial.println(counter);
      digitalWrite(LedV_Insp, HIGH);

      if (counter >= 3)
      {
        digitalWrite(LedV_Insp, LOW);
        estado = 6;
        counter = 0;
      }
    }
    else if (estado == 6)
    {
      Serial.println("Estado 6 (LED ROJO) - READY");
      digitalWrite(LedR_Ready, HIGH);
    }
  }
}