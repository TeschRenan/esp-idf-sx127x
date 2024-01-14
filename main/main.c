#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "driver/spi_common.h"

#include "lora.h"

#define SPI_PORT    SPI2_HOST
#define SPI_MISO 	GPIO_NUM_16
#define SPI_SCK 	GPIO_NUM_5
#define SPI_MOSI 	GPIO_NUM_17
#define SPI_CS 		GPIO_NUM_18
#define SPI_RST		GPIO_NUM_4

void task_tx(void *p)
{
   for(;;) {

	vTaskDelay(pdMS_TO_TICKS(5000));
	lora_send_packet((uint8_t*)"Hello", 5);
	ESP_LOGI(__func__,"packet sent...");

	int lost = lora_packet_lost();
	if (lost != 0) {
		ESP_LOGW(pcTaskGetName(NULL), "%d packets lost", lost);
	}

   }
}
void task_rx(void *pvParameters)
{
   uint8_t buf[256]; // Maximum Payload size of SX1276/77/78/79 is 255
   for(;;) {

	if (lora_received()) {

		int rxLen = lora_receive_packet(buf, sizeof(buf));
		int rssi = lora_packet_rssi();
		float snr = lora_packet_snr();
		ESP_LOGI(__func__, "RSSI: %d, SNR: %.2f", rssi, snr);
		ESP_LOGI(__func__, "%d byte packet received:[%.*s]", rxLen, rxLen, buf);

	}

   }

}

void app_main()
{
   if (lora_init(SPI_PORT,SPI_MISO,SPI_MOSI,SPI_SCK,SPI_CS,SPI_RST) == 0) {
		ESP_LOGE(__func__, "Does not recognize the module");
	   	esp_restart();
	}

	ESP_LOGI(__func__, "Frequency is 915MHz");
	lora_set_frequency(915e6); // 915MHz


	lora_enable_crc();

	int cr = 1;
	int bw = 7;
	int sf = 7;

	lora_set_coding_rate(cr);

	//cr = lora_get_coding_rate();
	ESP_LOGI(__func__, "coding_rate=%d", cr);

	lora_set_bandwidth(bw);

	//int bw = lora_get_bandwidth();
	ESP_LOGI(__func__, "bandwidth=%d", bw);

	lora_set_spreading_factor(sf);

	//int sf = lora_get_spreading_factor();
	ESP_LOGI(__func__, "spreading_factor=%d", sf);

   	//Can you remove commentaries to set the task
	xTaskCreate(&task_tx, "task_tx", 2048, NULL, 5, NULL);
   	//xTaskCreate(&task_rx, "task_rx", 2048, NULL, 5, NULL);
}