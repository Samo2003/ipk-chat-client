# IPK_2

## Introduction
This project is a chat client using the `IPK25-CHAT` protocol to communicate, implementing both its `TCP` and `UDP` variants. Further information about this protocol can be found [here](https://git.fit.vutbr.cz/NESFIT/IPK-Projects/src/branch/master/Project_2). The program supports IPv4 network layer protocol. Upon execution, it uses the specified protocol variant and allows communication with the server.

## Table of Contents
- [IPK Project 2](#ipk_2)
    - [Table of Contents](#table-of-contents)
    - [Execution](#execution)
        - [Example Execution](#example-execution)
    - [Usage](#usage)
    - [Implementation Overview](#implementation-overview)
        - [File Overview](#file-overview)
    - [Testing](#testing)
        - [Testing Enviroment](#testing-environment)
        - [Test Suits](#test-suits)
        - [Valid Input Test Case Structure](#valid-input-test-case-structure)
        - [Python Server](#python-server)
        - [Wireshark](#wireshark)
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
The testing process ensures the client functions correctly across different scenarios, including handling invalid inputs and comparing results against expected results. The test script can be executed by running `make test`.

### Testing Environment
- **Host Machine**: MacBook Pro running Lima (`limactl version 0.23.2`).
- **Test VM**: Referential virtual machine with nix-shell setup.
- **Tools Used**: 
  - `ipk25chat-client` (chat client being tested)
  - Python-based test script
  - Python-based server
  - Public reference Discord server

**Note**: ChatGPT was used to create both Python-based scripts.

### Test Suits
- #### Invalid Inputs
    - **Objective**: Ensure the client correctly rejects incorrect arguments.
    - **Expected Outcome**: The client should return an error for each invalid input.

- #### Valid Inputs
    - **Objective**: Confirm correct client behavior according to the FSM describing its behavior.
    - **Expected Outcome**: Output is specified individually in each test case.

### Valid Input Test Case Structure
- This is the `test_10` test case:
```
Reply in open state
EXIT CODE: 1
/auth a b c
reply
====
Action Success: auth OK
Action Success: reply
ERROR: 
```
- The first line contains a short description of the test
The second line optionally contains the exit code if skipped; `0` is the default value
- Line starting with `=` serves as a divider between test input and test output
- The expected output is matched using the Python `startswith` function
- Part of the test inputs can be a line with `KILL`, which results in sending a `SIGINT` signal to the client

### Python Server
A local Python server, `test/server.py`, was used for testing. The server's implementation of the FSM describing its behavior is located in the `ClientBase` class, with `UDPClient` and `TCPClient` inheriting methods from this class and adding methods for parsing, creating, and sending messages. The server is for testing purposes only since it responds unexpectedly when it receives specific messages. This is used to test all of the transitions from client FSM. For example, from the test case shown above, when the server receives a message with the content `reply`, it responds with a `REPLY` message, as shown in the expected output part of the test.  
The server uses `select` to wait for incoming `UDP` messages or `TCP` connections. When such an action occurs, respectively, `TCPClient` or `UDPClient` is created, and a new thread is started to handle the client using the `handle_udp_client` or `handle_tcp_client` function.

### Wireshark
Wireshark played a crucial part in testing and debugging this project. A provided custom `IPK25-CHAT` protocol dissector plugin for Wireshark was used to capture the communication between the client and the server. The communication between the client and the server during the above-mentioned `test_10` test case is shown in the picture below.

![Client Server communication](doc/client_server_communication.jpg)

The first six packets captured show the `TCP` variant of the protocol, where it is visible that the `ABNF` grammar defining the `TCP` protocol variant is case-insensitive. The rest of the packets show the `UDP` protocol variant. Here, you can see that each message has its `ID` and has to be confirmed by the other side using a `CONFIRM` message.

## Bibliography
- **[RFC5234]** Crocker, D. and Overell, P. Augmented BNF for Syntax Specifications: ABNF [online]. January 2008. [cited 2025-04-10]. DOI: 10.17487/RFC5234. Available at: https://datatracker.ietf.org/doc/html/rfc5234
- **[ChatGPT]** OpenAI. ChatGPT [online]. 2025. Available at: https://chat.openai.com
- **[Grammarly]** Grammarly Inc. Grammarly [online]. 2025. Available at: https://www.grammarly.com