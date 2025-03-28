# Sistema de Monitoramento com FreeRTOS

Esta aplicação embarcada utiliza o **FreeRTOS** para criar um sistema de monitoramento de 3 tarefas que simulam o funcionamento de sensores (um botão e um LED). As tarefas cooperam entre si para realizar diferentes funções, como ler o estado do botão e controlar o LED.

## Lista de Tarefas

### 1. Tarefa 1 - Leitura do Botão  
- Responsável por fazer a leitura do botão.  
- Executada a cada **100ms**.  
- Envia o estado do botão para a próxima tarefa através da **fila** (Queue).

### 2. Tarefa 2 - Processamento do Botão  
- Recebe o estado do botão da **Tarefa 1** pela fila.  
- Se o botão estiver pressionado, aciona a **Tarefa 3**.  
- Caso contrário, aguarda o próximo ciclo.  

### 3. Tarefa 3 - Controle do LED  
- Controla um LED, ligando se o botão estiver pressionado.  
- **Desliga** o LED caso contrário.  
- Executada sempre que acionada pela **Tarefa 2**.  