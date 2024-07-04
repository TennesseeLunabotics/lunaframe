![](https://tennesseelunabotics.com/images/backDrop2.png)

# Tennessee Lunabotics Framework
This repo contains framework code for Tennessee Lunabotics robots. It is derived from the 2023-2024 robot codebase and does not contain code for specific robot functions besides a drivetrain. For info and documentation, visit the [Wiki Pages](https://wiki.tennesseelunabotics.com/)

DO NOT .gitignore THE .vscode IT CONTAINS THE FORMATTING SPECIFIER.

# How to setup the repo
To use the repo you will first need to download OpenSSL and OpenCV. Both downloads are in the bottom.

Run the opencv-4.9.0-windows.exe executable and move the folder it makes to `C:\Program Files\`.
Add the `build\x64\vc16\`, `build\x64\vc16\bin`, and `build\x64\vc16\lib` to your PATH.

Extract cert.7z to `C:\lunakey\cert.pem`

# Layout
The client side code runs on a laptop and communicates with the robot via tcp.

The server side is run on a NVIDIA Jetson Nano 2GB which communicates with an esp32 microcontroller to handle the movement of the robot.

# NANO Headless Mode
Connect a micro-usb to the nano and to your computer. You will be able to use it like ethernet to talk to the nano in headless mode. During this the Jetson has a static IP of `192.168.55.1`.


## Command to generate private key and public certificate on nano
openssl req -x509 -nodes -days 365 -newkey rsa:2048 -keyout private.pem -out cert.pem
# Useful Links
- [ANSI Escape Codes](https://gist.github.com/fnky/458719343aabd01cfb17a3a4f7296797)
- TCP Connections
  - [General Explanation](https://www.khanacademy.org/computing/computers-and-internet/xcae6f4a7ff015e7d:the-internet/xcae6f4a7ff015e7d:transporting-packets/a/transmission-control-protocol--tcp)
  - [GeeksforGeeks](https://www.geeksforgeeks.org/tcp-server-client-implementation-in-c/#)
  - [Educative](https://www.educative.io/answers/how-to-implement-tcp-sockets-in-c)
  - [IBM](https://www.ibm.com/docs/en/zos/2.1.0?topic=programs-c-socket-tcp-client)
- [XInput General Info](https://learn.microsoft.com/en-us/windows/win32/xinput/getting-started-with-xinput)
- [Variadic Macro Arguments in C++ (GNU)](https://gcc.gnu.org/onlinedocs/cpp/Variadic-Macros.html)
- [Simple C Syntax](https://www.w3schools.com/c/c_syntax.php)
- [Simple C++ Syntax](https://www.w3schools.com/cpp/cpp_syntax.asp)
- [OpenCV Documentation](https://docs.opencv.org/4.x/)
- [UART Overview](https://www.seeedstudio.com/blog/2022/09/08/uart-communication-protocol-and-how-it-works/)
- [OpenSSL Wiki](https://wiki.openssl.org/index.php/Main_Page)
- [OpenCV Download Link](https://sourceforge.net/projects/opencvlibrary/files/4.9.0/opencv-4.9.0-windows.exe/download)
- [OpenSSL Download Link](https://slproweb.com/download/Win64OpenSSL-3_2_0.exe)
