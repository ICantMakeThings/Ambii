#include <Arduino.h>
#include <FS.h>
#include <SD.h>
#include <driver/i2s.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define BUTTON_PIN 0

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

unsigned long startMillis = 0;

#define SD_CS 33
#define SD_MOSI 21
#define SD_MISO 5
#define SD_SCK 17
SPIClass sdSPI(FSPI);

#define SAMPLE_RATE 48000
#define BITS_PER_SAMPLE I2S_BITS_PER_SAMPLE_32BIT

#define BUFFER_SIZE 16384
uint8_t *buffer1;
uint8_t *buffer2;
uint8_t *packed;
#define MAX_FILE_SIZE_MB 3900
#define MAX_BYTES (MAX_FILE_SIZE_MB * 1024UL * 1024UL)

#define I2S_PORT_1 I2S_NUM_0
#define I2S_PORT_2 I2S_NUM_1

File file;
bool recording = false;
uint32_t bytesWritten = 0;

struct WAVHeader
{
  char riff[4];
  uint32_t size;
  char wave[4];
  char fmt[4];
  uint32_t fmt_size;
  uint16_t audio_fmt;
  uint16_t num_channels;
  uint32_t sample_rate;
  uint32_t byte_rate;
  uint16_t block_align;
  uint16_t bits_per_sample;
  char data[4];
  uint32_t data_size;
};

void writeWavHeader(File &f, uint32_t pcm_bytes)
{
  WAVHeader h;
  memcpy(h.riff, "RIFF", 4);
  h.size = pcm_bytes + 36;
  memcpy(h.wave, "WAVE", 4);
  memcpy(h.fmt, "fmt ", 4);
  h.fmt_size = 16;
  h.audio_fmt = 1;
  h.num_channels = 4;
  h.sample_rate = SAMPLE_RATE;
  h.bits_per_sample = 24;
  h.byte_rate = SAMPLE_RATE * h.num_channels * (h.bits_per_sample / 8);
  h.block_align = h.num_channels * (h.bits_per_sample / 8);
  memcpy(h.data, "data", 4);
  h.data_size = pcm_bytes;
  f.seek(0);
  f.write((uint8_t *)&h, sizeof(h));
}

void i2sInit()
{
  i2s_config_t config = {
      .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
      .sample_rate = SAMPLE_RATE,
      .bits_per_sample = BITS_PER_SAMPLE,
      .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
      .communication_format = I2S_COMM_FORMAT_STAND_I2S,
      .intr_alloc_flags = 0,
      .dma_buf_count = 8,
      .dma_buf_len = 512,
      .use_apll = true,
      .tx_desc_auto_clear = false,
      .fixed_mclk = SAMPLE_RATE * 256};

  i2s_pin_config_t pin_config_1 = {
      .mck_io_num = GPIO_NUM_35,
      .bck_io_num = GPIO_NUM_38,
      .ws_io_num = GPIO_NUM_18,
      .data_out_num = I2S_PIN_NO_CHANGE,
      .data_in_num = GPIO_NUM_16};

  i2s_pin_config_t pin_config_2 = {
      .mck_io_num = GPIO_NUM_13,
      .bck_io_num = GPIO_NUM_2,
      .ws_io_num = GPIO_NUM_12,
      .data_out_num = I2S_PIN_NO_CHANGE,
      .data_in_num = GPIO_NUM_4};

  i2s_driver_install(I2S_PORT_1, &config, 0, NULL);
  i2s_set_pin(I2S_PORT_1, &pin_config_1);
  i2s_set_clk(I2S_PORT_1, SAMPLE_RATE, BITS_PER_SAMPLE, I2S_CHANNEL_STEREO);
  i2s_zero_dma_buffer(I2S_PORT_1);

  i2s_driver_install(I2S_PORT_2, &config, 0, NULL);
  i2s_set_pin(I2S_PORT_2, &pin_config_2);
  i2s_set_clk(I2S_PORT_2, SAMPLE_RATE, BITS_PER_SAMPLE, I2S_CHANNEL_STEREO);
  i2s_zero_dma_buffer(I2S_PORT_2);
}

int getNextFileNumber()
{
  int num = 1;
  while (SD.exists("/record" + String(num) + ".wav"))
  {
    num++;
  }
  return num;
}

void setup()
{
  Serial.begin(115200);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  Wire.begin(11, 10);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
  {
    Serial.println("SSD1306 init failed!");
    while (1)
      ;
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("Ambii by icmt.cc");
  display.println("v1.0");
  display.display();
  delay(2000);
  display.clearDisplay();
  display.display();

  if (!psramFound())
  {
    Serial.println("PSRAM ran away");
    while (1)
      ;
  }

  buffer1 = (uint8_t *)ps_malloc(BUFFER_SIZE);
  buffer2 = (uint8_t *)ps_malloc(BUFFER_SIZE);
  packed = (uint8_t *)ps_malloc(BUFFER_SIZE * 3 / 2);

  if (!buffer1 || !buffer2 || !packed)
  {
    Serial.println("PSRAM died!");
    while (1)
      ;
  }

  Serial.println("PSRAM works, yay!");

  sdSPI.begin(SD_SCK, SD_MISO, SD_MOSI, SD_CS);
  if (!SD.begin(SD_CS, sdSPI))
  {
    Serial.println("No SD Card");

    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("No SD Card");
    display.println("Or not formatted.");
    display.display();

    while (1)
    {
      delay(250);
    }
  }

  Serial.println("SD card yay good");
  i2sInit();
}

void loop()
{
  static int fileNum = 0;

  static bool lastButtonState = HIGH;

  bool buttonState = digitalRead(BUTTON_PIN);

  if (lastButtonState == HIGH && buttonState == LOW)
  {
    if (!recording)
    {
      fileNum = getNextFileNumber();
      String filename = "/record" + String(fileNum) + ".wav";
      file = SD.open(filename, FILE_WRITE);

      if (!file)
      {
        Serial.println("sumfin rong with file");
        display.clearDisplay();
        display.setCursor(0, 0);
        display.println("Check SD Card");
        display.display();
        delay(2000);
        return;
      }

      WAVHeader blank = {0};
      file.write((uint8_t *)&blank, sizeof(blank));

      bytesWritten = 0;
      recording = true;
      startMillis = millis();

      Serial.println("rec...");

      display.clearDisplay();
      display.setCursor(0, 0);
      display.print("Recording nr. ");
      display.println(fileNum);
      display.display();
      delay(1000);
    }
    else
    {
      writeWavHeader(file, bytesWritten);
      file.flush();
      file.close();
      recording = false;

      Serial.println("Saved");
      display.clearDisplay();
      display.setCursor(0, 0);
      display.println("Saved");
      display.println(fileNum);
      display.display();
      delay(1000);
      display.clearDisplay();
      display.display();

      uint64_t total = SD.totalBytes();
      uint64_t used = SD.usedBytes();
      uint64_t free = total - used;
      uint32_t freeMB = free / (1024 * 1024);

      display.clearDisplay();
      display.setCursor(0, 0);
      display.println("Free space:");
      display.print(freeMB);
      display.println(" MB");
      display.display();
      delay(10000);
      display.clearDisplay();
      display.display();
    }

    delay(200);
  }

  lastButtonState = buttonState;

  if (recording)
  {
    size_t bytesRead1 = 0, bytesRead2 = 0;
    i2s_read(I2S_PORT_1, buffer1, BUFFER_SIZE, &bytesRead1, portMAX_DELAY);
    i2s_read(I2S_PORT_2, buffer2, BUFFER_SIZE, &bytesRead2, portMAX_DELAY);

    if (bytesRead1 > 0 && bytesRead2 > 0)
    {
      int32_t *samples1 = (int32_t *)buffer1;
      int32_t *samples2 = (int32_t *)buffer2;
      size_t frames = min(bytesRead1, bytesRead2) / 8;

      for (size_t i = 0; i < frames; i++)
      {
        int32_t L1 = samples1[i * 2 + 0];
        int32_t R1 = samples1[i * 2 + 1];
        int32_t L2 = samples2[i * 2 + 0];
        int32_t R2 = samples2[i * 2 + 1];

        size_t base = i * 12;

        packed[base + 0] = (L1 >> 8) & 0xFF;
        packed[base + 1] = (L1 >> 16) & 0xFF;
        packed[base + 2] = (L1 >> 24) & 0xFF;

        packed[base + 3] = (R1 >> 8) & 0xFF;
        packed[base + 4] = (R1 >> 16) & 0xFF;
        packed[base + 5] = (R1 >> 24) & 0xFF;

        packed[base + 6] = (L2 >> 8) & 0xFF;
        packed[base + 7] = (L2 >> 16) & 0xFF;
        packed[base + 8] = (L2 >> 24) & 0xFF;

        packed[base + 9] = (R2 >> 8) & 0xFF;
        packed[base + 10] = (R2 >> 16) & 0xFF;
        packed[base + 11] = (R2 >> 24) & 0xFF;
      }

      size_t packedBytes = frames * 12;

      if (bytesWritten + packedBytes <= MAX_BYTES)
      {
        file.write(packed, packedBytes);
        bytesWritten += packedBytes;
      }
      else
      {
        uint64_t total = SD.totalBytes();
        uint64_t used = SD.usedBytes();
        uint64_t free = total - used;

        if (free < 50UL * 1024UL * 1024UL)
        {
          Serial.println("SD card low space!");
          display.clearDisplay();
          display.setCursor(0, 0);
          display.println("SD space low!");
          display.display();

          writeWavHeader(file, bytesWritten);
          file.flush();
          file.close();
          recording = false;

          delay(2000);
          return;
        }

        Serial.println("Max file size reached.");
        writeWavHeader(file, bytesWritten);
        file.flush();
        file.close();
        recording = false;
        return;
      }
    }

    static unsigned long lastFlush = 0;
    if (millis() - lastFlush > 1000)
    {
      file.flush();
      lastFlush = millis();
    }

    static unsigned long lastDisplayUpdate = 0;
    if (millis() - lastDisplayUpdate > 1000)
    {
      unsigned long elapsedMillis = millis() - startMillis;
      unsigned long totalSeconds = elapsedMillis / 1000;
      unsigned int hours = totalSeconds / 3600;
      unsigned int minutes = (totalSeconds % 3600) / 60;
      unsigned int seconds = totalSeconds % 60;

      display.clearDisplay();
      display.setCursor(0, 0);
      display.print("Time: ");
      if (hours < 10)
        display.print("0");
      display.print(hours);
      display.print(":");
      if (minutes < 10)
        display.print("0");
      display.print(minutes);
      display.print(":");
      if (seconds < 10)
        display.print("0");
      display.print(seconds);

      if ((totalSeconds % 2) == 0)
      {
        display.setCursor(SCREEN_WIDTH - 30, 0);
        display.print("REC");
      }

      display.display();
      lastDisplayUpdate = millis();
    }
  }
}