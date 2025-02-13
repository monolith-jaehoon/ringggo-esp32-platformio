#include "mp3.h"


void Mp3Init()
{
  log_i("Init Mp3");

  volume = MAX_VOLUME;

  xQueueMp3 = xQueueCreate(10, sizeof(uint8_t[2]));
  
  Serial2.begin(9600);
  mp3Player.begin(Serial2);

  mp3Player.setTimeOut(500); //Set serial communictaion time out 500ms
  mp3Player.volume(volume);  //Set volume value. From 0 to 30
  mp3Player.enableDAC();          //Enable On-chip DAC
  mp3Player.outputSetting(true, 0);
  mp3Player.EQ(DFPLAYER_EQ_NORMAL);
  mp3Player.outputDevice(DFPLAYER_DEVICE_SD);

}

void Mp3Play(int number)
{
  log_i("Mp3 play");
  mp3Player.playMp3Folder(number);
}

void Mp3Stop()
{
  log_i("Mp3 stop");
  mp3Player.stop();
}

void Mp3Effect(int number)
{
  log_i("Mp3 effect");
  mp3Player.advertise(number);
}

void Mp3Pause()
{
  log_i("Mp3 pause");
  mp3Player.pause();
}

void Mp3Loop(int number)
{
  log_i("Mp3 loop");
  mp3Player.loop(number);
}


void Mp3Volume(int number)
{
  mp3Player.volume(number);
  log_i("Mp3 Volume: %d", volume);
}

void Mp3Task(void* parameter)
{
  BaseType_t xStatus;

  uint8_t packetBody[2] = { 0, };

  Mp3Init();

  Mp3Stop();

  for (;;)
  {
    // Serial.println("Mp3 Task");

    // Command by serial
    while(Serial.available())
    {
      char readSerial = (char)Serial.read();
      
      if(readSerial == '0') {
        Mp3Stop();
      } else if (readSerial == '1') {
        Mp3Play(1);
      } else if (readSerial == '2') {
        Mp3Play(2);
      } else if (readSerial == '3') {
        Mp3Play(3);
      } else if (readSerial == '4') {
        Mp3Effect(1);
      } else if (readSerial == '5') {
        Mp3Play(6);
      } else if (readSerial == '6') {
        Mp3Play(7);
      } else if (readSerial == '7') {
        Mp3Play(5);
      } else if (readSerial == '+') {
        volume++;
        Mp3Volume(volume);
      } else if (readSerial == '-') {
        volume--;
        Mp3Volume(volume);
      } else {
        log_i("Mp3 invalid cmd (0: stop, 1: play, 2: effect ah, 3: pause, 4: effect ome, +: volume up, -: volume down)");
      }

    }

    xStatus = xQueueReceive(xQueueMp3, packetBody, 100);
    if(xStatus == pdPASS)
    {
      log_i("mp3 queue received: %d, %d\n", packetBody[0], packetBody[1]);
      
      if(packetBody[0] == STOP) {
        Mp3Stop();
      } else if(packetBody[0] == PLAY) {
        Mp3Play(packetBody[1]);
      } else if(packetBody[0] == EFFECT) {
        Mp3Effect(packetBody[1]);
      } else if(packetBody[0] == VOLUME) {
        Mp3Volume(packetBody[1]);
      } else {
        log_i("invalid mp3 cmd");
      }
    }

    vTaskDelay(1000);

  }
  vTaskDelete(NULL);
}
