#include "battery.h"


void BatteryInit()
{
    log_i("Init Battery");

    g_fBatteryVolt = 0;
    g_dBatteryValue = 0;
}

void BatteryTask(void* parameter)
{
  BatteryInit();

  BaseType_t xStatus;
  
  uint16_t l_udBatteryADValue = 0;
  float l_fBatteryVolt = 0.0f;

  float g_xPredict = 0;
  float g_x = 0;
  float g_PPredict = 0;
  float g_P = 0;
  float g_K = 0;

  int i = 0;
  int l_dDispCount = 0;

  pinMode(A5, ANALOG);

  // 초기 포화까지 진행
  for (i = 0; i < 1000; i++)
  {
      l_udBatteryADValue = analogRead(A5);
      l_fBatteryVolt = l_udBatteryADValue * 0.000805;        // AD Reference 보정

      // predict..
      g_xPredict = g_x;
      g_PPredict = g_P + EXTERNAL_NOISE;
      // predict..

      // Correct..
      g_K = g_PPredict / (g_PPredict + MEASUREMENT_NOISE);
      g_x = g_xPredict + g_K * (l_fBatteryVolt - g_xPredict);
      g_P = (1 - g_K) * g_PPredict;
      // Correct..               
  }
  // 초기 포화까지 진행

  g_fBatteryVolt = g_x;
 
  for (;;)
  {
    // TODO
    // Serial.println("Battery Task");

    l_udBatteryADValue = analogRead(A5);
    l_fBatteryVolt = l_udBatteryADValue * 0.000805;        // AD Reference 보정
    //l_fBatteryVolt = l_udBatteryADValue * 0.0008056640625;
    
    // predict..
    g_xPredict = g_x;
    g_PPredict = g_P + EXTERNAL_NOISE;
    // predict..
    
    // Correct..
    g_K = g_PPredict / (g_PPredict + MEASUREMENT_NOISE);
    g_x = g_xPredict + g_K * (l_fBatteryVolt - g_xPredict);
    g_P = (1 - g_K) * g_PPredict;
    // Correct..               
    
    g_fBatteryVolt = g_x;        // g_x => filtered value 
    
    g_dBatteryValue = (int)(22.22222f * g_fBatteryVolt - 488.8889f);
    
    l_dDispCount++;
    if (l_dDispCount > BATTERY_DELAY)      // 60초에 한번씩 송출
    {
      l_dDispCount = 0;
      log_i("AD: %d, Voltage: %f, Percent: %d", l_udBatteryADValue, g_fBatteryVolt, g_dBatteryValue);
      SendBattery(g_dBatteryValue);
    }
    
    vTaskDelay(1000);

  }
  vTaskDelete(NULL);
}
