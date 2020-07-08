# MoCap
Here is non-optic motion capture system
Неоптическая система захвата движения. Чтобы запустить: подключаем два датчика акселерометра-гироскопа MPU-6050 к микроконтроллеру
ESP-8266. Заливаем прошивку из папки main на ESP-8266. Микроконтроллер поднимет сеть Wi-Fi с именем MoCap, подключаемся к ней.
После этого открываем браузером файл index.html из папки main. Манипулируя датчиками акселерометра-гироскопа 
можно вращать "руку" со сцены и ударять по мячику. Если по каким-то причинам под руком нет подходящей 
электроники, можно просто открыть файл index.html (открывать обязательно из папки client, чтобы 
скрипты прогрузились). Вы увидите землю (зелёная плоскость), на которую падает мяч. Он ударяется о руку, 
движение которой задаются из датчиков (то есть, пользователь вращает гироскоп-акселерометры в руках, и
вирутальная "рука" вращается в соответствии с движеним пользователя), дальше мячик просто укатится с земли.
Графика сделана с использованием three.js, физика - движка cannon.js