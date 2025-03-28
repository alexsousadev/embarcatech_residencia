#include <stdio.h>
#include "pico/stdlib.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

// Definição dos pinos (LED e Botão)
#define PIN_BUTTON_A 5
#define PIN_LED_BLUE 12

// Envio do estado do botão a cada 100 ms
#define BUTTON_STATE_UPDATE_INTERVAL 100

// Filas para comunicação entre tarefas
typedef struct
{
    QueueHandle_t buttonQueue;
    QueueHandle_t ledQueue;
} QueueHandles;

QueueHandles queues;

// Estrutura para armazenar o estado dos botões
typedef struct state_buttons
{
    bool previousState;
    bool currentState;
} StateButtons;

StateButtons stateButtons;

// Inicialização dos pinos
void init_gpios(void)
{
    stdio_init_all();
    gpio_init(PIN_LED_BLUE);
    gpio_set_dir(PIN_LED_BLUE, GPIO_OUT);
    gpio_init(PIN_BUTTON_A);
    gpio_set_dir(PIN_BUTTON_A, GPIO_IN);
    gpio_pull_up(PIN_BUTTON_A);
}

// Tarefa 1: Leitura do botão e envio do estado para a fila
void vTaskButtonRead(void *pvParameters)
{
    while (1)
    { // pega o estado atual do botão e manda para a fila
        stateButtons.currentState = !gpio_get(PIN_BUTTON_A);
        xQueueSend(queues.buttonQueue, &stateButtons.currentState, portMAX_DELAY);
        stateButtons.previousState = stateButtons.currentState;
        vTaskDelay(pdMS_TO_TICKS(BUTTON_STATE_UPDATE_INTERVAL)); // Envia a cada 100ms
    }
}

// Tarefa 2: Processamento do estado do botão e envio para a fila do LED
void vTaskButtonProcess(void *pvParameters)
{
    bool btnState;
    while (1)
    {
        // Se recebeu estado do botão acionado, atualiza a fila do led também
        if (xQueueReceive(queues.buttonQueue, &btnState, portMAX_DELAY) == pdPASS)
        {
            xQueueSend(queues.ledQueue, &btnState, portMAX_DELAY);
        }
    }
}

// Tarefa 3: Controle do LED com base na fila do processador
void vTaskLedControl(void *pvParameters)
{
    bool ledState;
    while (1)
    {
        // Recebe dados da fila do LED e liga/desliga ele
        if (xQueueReceive(queues.ledQueue, &ledState, portMAX_DELAY) == pdPASS)
        {
            gpio_put(PIN_LED_BLUE, ledState);
        }
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}

int main()
{
    // Inicializando os pinos
    init_gpios();

    // Criando as filas
    queues.buttonQueue = xQueueCreate(5, sizeof(bool));
    queues.ledQueue = xQueueCreate(5, sizeof(bool));

    // Criando as tarefas
    xTaskCreate(vTaskButtonRead, "ButtonRead", 256, NULL, 1, NULL);
    xTaskCreate(vTaskButtonProcess, "ButtonProcessing", 256, NULL, 2, NULL);
    xTaskCreate(vTaskLedControl, "LEDControl", 256, NULL, 3, NULL);

    // Inicia o escalonador do FreeRTOS
    vTaskStartScheduler();

    while (1)
        ;
    return 0;
}
