CHAT
===
EN below

Консольное клиент-серверное приложение.
---
Таргеты:
- server_starter - запуск сервера.
- client_starter - запуск клиента.

Сначала запускается сервер, больше с ним ничего делать не надо.
<br>Клиент - выводит всю необходимую информацию в консоль.
<br>Для начала нужно подключиться к серверу: 'lh' для локалхоста или ввести ip хоста. Потом решить выводить ответы
 сервера или нет (ответить на вопрос поставленный консолью y/n).
<br>Доступные команды:
- HELLO - отсылает привет серверу.
- register - регистрация на сервере.
- login - вход.
- logout - выход.
- message - отсылка сообщения всем пользователям (анонимная или от имени никнейма).
- ping - пинг сервера (не печатает ответ).
- stop - остановить сервер.
- help - список комманд.

Все комманды (кроме ping) печатают ответ сервера в виде:
<br>Response:
<br>&nbsp; key1: 'value1'
<br>&nbsp; key2: 'value2'
<br>&nbsp; ...

---
Console client-server application.
---
Targets:
- server_starter - starting the server.
- client_starter - client launch.

<br>Firstly, you should run the server, nothing else needs to be done with it.
<br>Client - write in output all the necessary information to the console.
<br>First at all, you need to connect to the server: 'lh' for the localhost or enter the ip of the host. After that you 
should consider if you want to print responses (y/n answer for simple question).
<br>Available commands:
- HELLO - sends HELLO to the server.
- register - registration on the server.
- login - login.
- logout - logout.
- message - send message to all users (anonymous or with your login).
- ping - ping of the server (don't print response).
- stop - stop the server.
- help - list of commands.

After all commands (except ping) response will print as:
<br>Response:
<br>&nbsp; key1: 'value1'
<br>&nbsp; key2: 'value2'
<br>&nbsp; ...
