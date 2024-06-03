# OS_hw8 - Семафоры POSIX
Работа выполнена на 9 баллов. 
Результат работы программы: 
<img width="1747" alt="Снимок экрана 2024-05-30 в 17 38 15" src="https://github.com/danikd1/OS_HW/assets/36849026/0d605f22-5661-4432-a44b-bb4f0880f1e8">

# OS_hw9 - Семафоры UNIX SYSTEM V
Работа выполнена на 10 баллов. Результат работы программы: 
<img width="1785" alt="Снимок экрана 2024-05-30 в 19 09 26" src="https://github.com/danikd1/OS_HW/assets/36849026/49204300-9218-4dc3-bc04-3527b2c5566c">

# OS_hw10 - TCP
Работа выполнена на 10 баллов. Клиент №1 и Клиент №2 используют один и тот же код client.c, но запускаются поочередно, сначала Клиент №1(отправитель), затем Клиент №2(получатель). Cервер обрабатывает одновременно два подключения, а клиенты поочередно подключаются и отправляют данные. Результат работы программы: 
<img width="1782" alt="Снимок экрана 2024-05-30 в 19 59 14" src="https://github.com/danikd1/OS_HW/assets/36849026/a34fe19f-a139-42b3-bba5-3790c268772d">

# OS_hw11 - UDP
Работа выполнена на 10 баллов. Сервер создает UDP-сокет для отправки широковещательных сообщений. Сервер настраивает свой сокет на возможность отправки широковещательных сообщений через использование опции SO_BROADCAST.
Сервер конфигурирует структуру адреса с IP-адресом для броадкаста, указанным в командной строке. Пользователь вводит сообщение, которое затем сервер отправляет всем клиентам в локальной сети. Отправка сообщений происходит каждые 3 секунды, чтобы избежать засорения сети. Клиент привязывает сокет к порту, указанному в командной строке, и настраивает его на прослушивание широковещательных сообщений.
<img width="1792" alt="Снимок экрана 2024-06-03 в 23 15 17" src="https://github.com/danikd1/OS_HW/assets/36849026/e82ff1c7-7ba2-43cd-bd1e-065c1a5c9f65">
