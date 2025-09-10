# Just Another Pomodoro

This is just another (CLI) pomodoro timer. Simple and minimalistic but functional.

It can be said that it is used as internal tool for working on notes project. But you can try it out, maybe you will find it useful.

# Support platforms

For now it works only on Linux and requires `libnotify-bin` package for notifications. Terminal bell is used in combination with notification thus maybe you need to enable it too.

I plan to add support for Windows and MacOS in foreseeable future.

# Screenshots

<img width="577" height="107" alt="image" src="https://github.com/user-attachments/assets/ae5a51d0-cf98-45ba-bc89-948efd3101a5" />

Line "Pomodoros" contains info about completed sessions, i.e. completed pomodoros in a row (e.g. first was completed 4 pomodoros in a row, after that was a long break, then was completed 2 pomodoro and so on).

<img width="577" height="107" alt="image" src="https://github.com/user-attachments/assets/ae21fc44-e596-42a4-9f92-f30936818175" />

Work interval is 25 minutes and rest interval is 5. Long rest interval is not supported, feel free to rest whatever you want =)

<img width="577" height="107" alt="image" src="https://github.com/user-attachments/assets/0b2902ed-031d-4dd0-ac20-4970720a4583" />

Work and rest intervals follows each other and if you skip for example work interval than next will be rest interval.

<img width="516" height="153" alt="image" src="https://github.com/user-attachments/assets/24ac83a4-692c-4fa2-a3b1-6311564c448d" />

<img width="446" height="171" alt="image" src="https://github.com/user-attachments/assets/585361ad-24ae-4cf4-9eb4-e5689bc0f654" />

# Roadmap

- [x] add support for Windows
- [x] add support for MacOS
- [ ] add auto mode (work and rest intervals followed by each other without user actions)
- [ ] impl notifications on Windows
- [ ] impl notifications on MacOS
