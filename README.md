Simple TCP echo server and client

Files:
- server.c — multi-threaded (pthread) TCP echo server for IPv4.
- client.c — simple TCP client that connects, sends lines, and prints echoed lines.

Build (Linux/frios2):

    gcc -Wall -Wextra -O2 server.c -pthread -o server
    gcc -Wall -Wextra -O2 client.c -o client

Run:

1. Start server (port optional, default 42069):

    ./server 42069

2. Connect with client:

    ./client 127.0.0.1 42069

Type messages in the client; the server will echo them back. Send a line starting with `quit` to close the client connection.

## Build with CMake (recommended on frios2 remote host)

The project includes CMake targets for `server` and `client`. The targets are only added on UNIX-like systems to avoid Windows/Mingw errors in CLion.

Configure and build with CMake:

    cmake -S . -B cmake-build-debug
    cmake --build cmake-build-debug --target server -- -j
    cmake --build cmake-build-debug --target client -- -j

Run (on frios2 or any Linux remote):

1. Start server (default port 42069):

    ./cmake-build-debug/server 42069

2. In another shell on the same host, connect with the client:

    ./cmake-build-debug/client 127.0.0.1 42069

You can also run `./cmake-build-debug/server` and `nc 127.0.0.1 42069` from another machine to test.

Notes:
- Build and run on the remote `frios2` host using CLion's Remote Host toolchain or by SSHing into frios2 and running the above commands.
- On Windows the `server` and `client` targets are not added and the source files contain stubs that print a short message.

- The code uses POSIX sockets and pthreads; on Windows you'll need to adapt to Winsock and replace pthreads (or use MinGW/MSYS).
- For quick testing you can also use `telnet` or `nc` to connect to the server.
