# rest_api

Легковесный HTTP/REST-модуль для STM32 на базе LwIP и FreeRTOS. Подходит для простых диагностических и сервисных endpoint'ов без динамической маршрутизации и тяжелого JSON-стека.

## Структура

- `http/`: TCP-сервер и запуск RTOS-задачи
- `api/`: типы `Request`/`Response`, роутер и точки входа
- `endpoints/`: контроллеры
- `jsmn/`: компактный JSON-парсер для простых `POST`

## Что есть

- `rest_api_start_server_task()`: старт HTTP-задачи
- `rest_api_register_default_routes()`: регистрация встроенных маршрутов
- `Router::instance()`: singleton-роутер
- `IController`: базовый интерфейс контроллера
- встроенный `StatusController` для `/status`

## Как использовать

```cpp
#include "rest_api.hpp"

void StartDefaultTask(void *argument) {
    MX_LWIP_Init();
    rest_api_start_server_task();

    for (;;) {
        osDelay(1000);
    }
}
```

После старта модуль ждет, пока сетевой интерфейс станет готов, и затем запускает HTTP-сервер на порту `80`.

## Встроенные endpoint'ы

- `GET /status`
- `POST /status`

## Как добавить свой контроллер

```cpp
#include "IController.hpp"
#include "router.hpp"

class MyController : public IController {
public:
    void get(const Request& req, Response& res) override {
        (void)req;
        res.status = HttpStatus::OK;
        res.body = "{\"message\":\"hello\"}";
    }
};

static MyController g_my_controller;

void register_routes() {
    Router::instance().register_controller("/my", &g_my_controller);
}
```

## Основные моменты

- Роутер статический и без heap: максимум маршрутов задается через `REST_API_MAX_ROUTES`, по умолчанию `4`.
- `Response::body` имеет тип `std::string_view`, поэтому строка должна жить дольше, чем выполняется отправка ответа.
- Буфер приема небольшой: `HTTP_RECV_BUF_SZ = 384`, значит большие HTTP-заголовки и тела пока не поддерживаются.
- Сервер разбирает только метод, URI и тело после `\r\n\r\n`; полноценный HTTP parser не реализован.
- Для неподдерживаемого метода в найденном контроллере базовый `IController` отвечает `405`.
- Для неизвестного маршрута роутер возвращает `404` и `{"error":"route not found"}`.

## На что обратить внимание

- В `send_response()` текст статуса сейчас различается только между `200 OK` и всем остальным как `Not Found`, даже если фактический код `400`, `405` или `500`.
- Контроллеры лучше держать статическими или глобальными: роутер хранит сырой указатель.
- Модуль рассчитан на простые REST-сценарии, а не на высокую нагрузку или большие JSON-документы.
