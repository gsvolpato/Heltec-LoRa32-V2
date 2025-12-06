# Estrutura de Diretórios do Projeto

Este documento explica a organização dos arquivos do projeto, separando **aplicações (apps)** de **configurações do sistema**.

## Estrutura Geral

```
src/
├── apps/          # Aplicações do usuário
├── system/        # Configurações e módulos do sistema
└── main.cpp       # Ponto de entrada principal
```

## Diretório `apps/` - Aplicações

Contém aplicações que o usuário pode acessar através do menu principal. Cada app é um módulo independente.

### Apps Disponíveis:

- **`ir/`** - Aplicação de controle por infravermelho
  - `ir_config.h` / `ir_config.cpp`
  
- **`notepad/`** - Editor de texto (bloco de notas)
  - `notepad_config.h` / `notepad_config.cpp`

- **`settings/`** - Configurações do sistema
  - `settings_config.h` / `settings_config.cpp`
  - `keyboard_test.h` / `keyboard_test.cpp` - Teste do teclado
  - `deepsleep_config.h` / `deepsleep_config.cpp` - Modo deep sleep

### Regras para Apps:

1. Cada app deve ter sua própria pasta em `apps/`
2. Apps são módulos independentes que podem ser ativados/desativados
3. Apps aparecem no menu principal
4. Apps podem usar componentes do sistema (`system/`)

## Diretório `system/` - Sistema e Configurações

Contém módulos de sistema, configurações de hardware, e componentes reutilizáveis.

### Módulos de Hardware:

- **`bluetooth/`** - Configuração e controle Bluetooth
- **`wifi/`** - Configuração e controle WiFi
- **`lora/`** - Configuração e controle LoRa
- **`keyboard/`** - Sistema de teclado e entrada de texto
- **`gpios.h` / `gpios.cpp`** - Configuração de GPIOs

### Componentes de Sistema:

- **`display_config.h` / `display_config.cpp`** - Configuração do display OLED
- **`homescreen_config.h` / `homescreen_config.cpp`** - Tela inicial
- **`menu_config.h` / `menu_config.cpp`** - Sistema de menus
- **`skull_logo.h`** - Logo de inicialização

### Componentes Reutilizáveis:

- **`text_input/`** - Componente de entrada de texto (single-line)
  - Usado para senhas, nomes de arquivo, etc.
  
- **`text_editor/`** - Componente de editor de texto (multi-line)
  - Usado para notepad e edição de texto completa

### FreeRTOS:

- **`freertos/`** - Configuração e tarefas do FreeRTOS
  - `tasks_config.h` / `tasks_config.cpp` - Configuração de tarefas
  - `keyboard_queue.h` / `keyboard_queue.cpp` - Fila de teclado
  - `utils.h` / `utils.cpp` - Utilitários FreeRTOS

### Regras para System:

1. Módulos de hardware são configurações do sistema
2. Componentes reutilizáveis devem estar em `system/`
3. Configurações globais ficam em `system/`
4. Não devem depender de apps específicos

## Separação de Responsabilidades

### Apps (`apps/`)
- Funcionalidades específicas do usuário
- Podem ser ativadas/desativadas
- Aparecem no menu principal
- Exemplos: Notepad, IR, Settings

### System (`system/`)
- Funcionalidades de baixo nível
- Configurações de hardware
- Componentes reutilizáveis
- Infraestrutura do sistema
- Exemplos: WiFi, Bluetooth, LoRa, Keyboard, Display

## Adicionando Novos Apps

Para adicionar um novo app:

1. Criar pasta em `src/apps/nome_do_app/`
2. Criar arquivos `nome_do_app_config.h` e `nome_do_app_config.cpp`
3. Adicionar função `nomeDoAppSetup()` no menu
4. Adicionar entrada no menu principal em `menu_config.cpp`

## Adicionando Novos Módulos de Sistema

Para adicionar um novo módulo de sistema:

1. Criar pasta em `src/system/nome_do_modulo/` (se necessário)
2. Criar arquivos de configuração
3. Incluir no `main.cpp` se necessário na inicialização
4. Documentar no README.md

## Exemplos de Uso

### Incluir um App:
```cpp
#include "apps/ir/ir_config.h"
```

### Incluir um Módulo do Sistema:
```cpp
#include "system/wifi/wifi_config.h"
```

### Incluir um Componente Reutilizável:
```cpp
#include "system/text_input/text_input.h"
```
