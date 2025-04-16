# Smart Home Control System

A client-server system for remotely controlling smart home functions ( stubs are used instead of function ) over TCP/IP connection.

## Features

- Heating control (on/off)
- Door control (open/close)
- Pet feeding system
- Real-time system status monitoring

## Requirements

- Windows OS (uses Winsock2)
- C++ compiler with C++11 support
- Visual Studio 2019 or newer (recommended)


## Usage

- ### Server

Run `smart_home_server.exe` to start listening on port 54001.

- ### Client

Run `smart_home_client.exe` to connect to the server at 127.0.0.1:54001.

To connect to a remote server, modify the IP address in the client source code.

## Commands

- **Heating**: `heating:0` (off) or `heating:1` (on)
- **Door**: `door:0` (close) or `door:1` (open)
- **Pet feeding**: `food:amount` (where amount is a portion of food)
- **Status**: `status` (displays current state of all systems)
- **Exit**: `exit` or `5` (terminate client)

## Implementation Details

- Single client connection at a time
- In-memory state (not persisted after restart)
- Uses blocking sockets for data exchange

## Troubleshooting

If connection fails:
- Ensure server is running
- Check Windows Firewall settings
- Verify port 54001 is available
- Check server IP address in client code

## License

MIT
