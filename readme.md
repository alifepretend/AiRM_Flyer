# AiRM Flyer
## MULTI CORE???
### Core 0
- Tudo que não tá no Core 1
### Core 1
- Display
- Luzis i ledis
- Buzzer

## Módulos
### Hardware
- BtnInput
- Display (I2C e barramento)
- PWM
- ADC

### High Level
- System

## Botões
| Função | GPIO | Periférico | Observações |
| :--- | :---: | :---: | ---: | 
| LCD 16x2 (SDA) | GP4 | I2C0 SDA | Display com módulo I2C |
| LCD 16x2 (SCL) | GP5 | I2C SCL | Display com módulo I2C |
| Botão 1: Liga/Desliga | GP6 | GPIO_IN (Pull-up) |  |
| Botão 2: Temp (+) | GP7 | GPIO_IN (Pull-up) |  |
| Botão 3: Temp (-) | GP8 | GPIO_IN (Pull-up) |  |
| Botão 4: Time (+) | GP9 | GPIO_IN (Pull-up) |  |
| Botão 5: Time (-) | GP10 | GPIO_IN (Pull-up) |  |
| Botão 6: Receitas | GP11 | GPIO_IN (Pull-up) |  |
| Sensor de Temp. (Potenciômetro) | GP26 | ADC0 |  |
| Sensor de Porta | GP12 | GPIO_IN (Pull-up) |  |
| Buzzer | GP13 | PWM |  |
| LED Status / Alerta | GP14 | GPIO_OUT | Colocar resistor para limitar corrente |
| LED Aquecedor / Porta | GP15 | GPIO_OUT | Extra pra deixar seu Rosnaldo feliz |
| PWM Ventilador | GP16 | PWM0 A | Período 8ms (125Hz) |
| PWM Aquecedor | GP18 | PWM1 A | Período 8ms (125Hz) |

## Eventos Previstos
### Botões
- Botão 1 (Liga/Desliga): 
    - void (*cb_on_off_btn)() - Se desligado, ativa depois de 2s com o botão pressionado. Se ligado, desliga após 3 segundos com o botão pressionado.
    - void (*cb_start_pause_btn)() - Se ligado, inicia/retoma ou pausa o processo caso o tempo não esteja em 0. É chamado se o botão for solto antes de fechar 3 segundos.
- Botão 2 (Temp (+)): void (*cb_plus_temp_btn)()
- Botão 3 (Temp (-)): void (*cb_minus_temp_btn)()
- Botão 4 (Time (+)): void (*cb_plus_time_btn)()
- Botão 5 (Time (-)): void (*cb_minus_time_btn)()
- Botão 6 (Recipes): void (*cb_recipes_btn)()

### Ações
- Porta foi aberta: void (*cb_door_open)() $\to$ Estático / Flash
- Porta foi fechada: void (*cb_door_close)() $\to$ Estático / Flash