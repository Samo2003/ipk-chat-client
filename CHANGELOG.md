# CHANGELOG

## Implemented Funcionality
- argument processing
- IPv4 network communication
- `IPK25-CHAT` protocol `TCP` variant
- `IPK25-CHAT` protocol `UDP` variant

## Known Problems
After receiving a valid `ERR` or `BYE` message from the server the client does not wait for the possible retransmission.