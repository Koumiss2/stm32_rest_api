# STM32 REST API Framework

Легковесный REST API фреймворк для STM32, работающий поверх **LwIP** и **FreeRTOS**. 
Предназначен для создания HTTP-эндпоинтов на встраиваемых системах с использованием современного C++ подхода.

## Архитектура

Проект разделен на несколько логических уровней:

- **`http/`**: Низкоуровневая обертка над сокетами LwIP. Обрабатывает входящие TCP-соединения и парсит HTTP-заголовки.
- **`api/`**: Ядро фреймворка. Содержит `Router` (маршрутизатор) и определения типов (`Request`, `Response`).
- **`endpoints/`**: Бизнес-логика. Здесь располагаются контроллеры ресурсов.
- **`jsmn/`**: Ультра-легкая библиотека для парсинга JSON.

## Как добавить новый эндпоинт

Фреймворк использует паттерн **Restful Controller**. Один класс отвечает за один путь (URI) и обрабатывает различные HTTP-методы.

### 1. Создайте класс контроллера
Унаследуйте свой класс от `IController` и переопределите нужные методы (`get`, `post`, `put`, `del`).

```cpp
#include "IController.hpp"

class MyResourceController : public IController {
public:
    void get(const Request& req, Response& res) override {
        res.status = HttpStatus::OK;
        res.body = "{\"message\":\"Hello from MyResource\"}";
    }
};
```

### 2. Зарегистрируйте контроллер
В файле запуска (обычно `http_server.cpp` или `main.cpp`) создайте экземпляр контроллера и добавьте его в роутер:

```cpp
static MyResourceController my_controller;

void http_server_task(void *argument) {
    // ...
    Router::instance().register_controller("/my-resource", &my_controller);
    // ...
}
```

## Тестирование

Для тестирования эндпоинтов используйте `curl`.

### GET запрос
```bash
curl http://<device-ip>/status
```

### POST запрос с JSON
```bash
curl -X POST -d '{"key": "value"}' http://<device-ip>/status
```

## Особенности реализации

- **Zero-copy (по возможности)**: Используется `std::string_view` для работы со строками без лишних аллокаций.
- **Минимальный объем памяти**: Парсинг JSON через `jsmn` не использует кучу (heap).
- **Расширяемость**: Роутер поддерживает как контроллеры, так и простые лямбда-функции.

## Зависимости
- LwIP (Sockets API)
- FreeRTOS
- C++17 (или выше)

## Integration

In the main project:

```cpp
extern "C" void http_server_task(void *argument);
```

```cpp
void MX_FREERTOS_Init(void) {
  const osThreadAttr_t httpTaskAttr = {
    .name = "http_server",
    .stack_size = 1024 * 4,
    .priority = (osPriority_t) osPriorityNormal,
  };

  osThreadNew(http_server_task, nullptr, &httpTaskAttr);
}
```

The module now registers the mock `/status` route automatically.
