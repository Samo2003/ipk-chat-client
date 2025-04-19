# IPK_2

## Introduction
This project is a chat client using the `IPK25-CHAT` protocol to communicate, implementing both its `TCP` and `UDP` variants. The program supports IPv4 network layer protocol. Upon execution, it uses the specified protocol variant and allows communication with the server.

## Table of Contents
- [IPK Project 2](#ipk_2)
    - [Table of Contents](#table-of-contents)
    - [Execution](#execution)
        - [Example Execution](#example-execution)
    - [Usage](#usage)
    - [Implementation Overview](#implementation-overview)
        - [File Overview](#file-overview)
    - [Testing](#testing)
    - [Biblipgraphy](#bibliography)

## Execution
The program can be compiled by running `make`, which will create the executable `ipk25chat-client`. Multiple options can be used; they are specified below.
```
./ipk25chat-client [-h] {-t protocol} {-s hostname} [-p server-port] [-d udp-timeout] [-r udp-retransmissions]
```

- `{}` - required
- `[]` - optional

| Option |Default | Description |
|--------|--------|-------------|
| `-h`   |-       | Prints usage instructions and exits |
| `-t`   |-       | Select protocl variant tcp/udp |
| `-s`   |-       | Server IP or hostname |
| `-p`   |`4567`  | Server port |
| `-d`   |`250`   | UDP confirmation timeout (miliseconds) |
| `-r`   |`3`     | Number of UDP retransmissions |

### Example Execution
```
./ipk25chat-client -t udp -s localhost -p 3456 -r 2
```

## Usage
The user must authenticate with the `/auth` command; after successful authentification, the user can send messages to the server and join server channels. The program can be terminated using `CRTL+C` or `CRTL+D`. Below is a summary of supported client commands.

| Command | Parameters | Description |
|---------|------------|-------------|
| `/auth` | `{username}` `{secret}` `{display-name}` | Send authenetification request |
| `/join` | `{channel-id}` | Send join request to join a server channel |
| `/rename` | `{display-name}` | Change local display name |
| `/help` | -          | Prints list of client commands |

## Implementation Overview
This section describes the project's implementation details, including its structure, key components, and how different files interact to perform communication with the server.

The project is implemented in **C** and has a modular structure. The source files are in `src/`, and their corresponding header files are in `lib/`. Below is a diagram illustrating how the files are structured within the project, showing their relationships and interactions. 

![Project File Hierarchy](doc/file_structure.png)

### File Overview
This section describes the contents and responsibilities of the project files:
- **main.c**: Program's main entry, processing arguments, and coordinating execution.
- **fsm.c**: Implements FSM describing client behavior.
- **prompt.c**: Utilizes `select` to wait either for user input or a message from server while taking care of the 5s timeout when waiting for a response when necessary.
- **comm.c**: Serves as an interface to send and receive messages based on the selected protocol variant.
- **comm_tcp.c**: Implements functions to send and receive `TCP` protocol variant messages.
- **comm_udp.c**: Implements functions to send and receive `UDP` protocol variant messages.
- **tcp_msg.c**: Implements functions to build and parse `TCP` protocol variant messages.
- **udp_msg.c**: Implements functions to build and parse `UDP` protocol variant messages.
- **input.c**: Implements functions to process user input and print client help.
- **buffer.c**: Implements functions to deal with `TCP` segmentation.
- **args.c**: Processes and validates command-line arguments.
- **list.c**: Implements functions to deal with receiving duplicate messages in the `UDP` protocol variant.
- **queue.c**: Implements functions to deal with messages received while waiting for confirmation in the `UDP` protocol variant.
- **common.c**: Implements utility functions to validate message parts.

**Note**: ChatGPT was used to implement some functions and generate comments in source code; this is also written in the source code comments of the corresponding functions.

## Testing

## Bibliography
- **[ChatGPT]** OpenAI. ChatGPT [online]. 2025. Available at: https://chat.openai.com
- **[Grammarly]** Grammarly Inc. Grammarly [online]. 2025. Available at: https://www.grammarly.com