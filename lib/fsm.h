/**
 * @file fsm.h
 * @brief Finite State Machine (FSM) implementation for client-side protocol.
 *
 * This module implements a state-based control flow for handling client
 * communication. Each state represents a specific stage in the client's
 * interaction with the server, processing user inputs and server responses.
 *
 * Transitions are made by setting the global `state` function pointer.
 * The FSM continues executing states until communication ends or an error occurs.
 * 
 * @author Samuel Stefanik (xstefas00)
 * @date 2025-04-19
 */
#ifndef FSM_H
#define FSM_H

#include "prompt.h"

// Function pointer type for FSM state functions.
typedef int (*fsm_fn)(void);

/**
 * @brief Initial state of the client, waiting for AUTH command.
 * 
 * Handles user input to initiate authentication. If a valid AUTH message
 * is received, transitions to `state_auth`. Also handles unexpected server
 * messages by sending ERR and terminating on errors or BYE.
 */
int state_start(void);

/**
 * @brief Authentication state, sends AUTH and waits for REPLY/NREPLY.
 * 
 * On REPLY, proceeds to `state_open`. On NREPLY, stays in this state.
 * Terminates on ERR or BYE. Any unexpected message triggers ERR response.
 */
int state_auth(void);

/**
 * @brief Open communication state.
 * 
 * Allows the client to send messages or join a channel.
 * Transitions to `state_join` on JOIN command. Terminates on ERR/BYE.
 */
int state_open(void);

/**
 * @brief State while waiting for a response after JOIN command.
 * 
 * Transitions back to `state_open` after REPLY or NREPLY.
 * Terminates on ERR or BYE. Unexpected messages trigger ERR.
 */
int state_join(void);

/**
 * @brief Graceful termination state after user interrupt.
 * 
 * Sends BYE to server and ends the FSM.
 */
int state_interupt(void);

/**
 * @brief Runs the entire FSM starting in `state_start`.
 * 
 * Initializes the client's display name, checks for interrupt,
 * and runs state transitions until completion or error.
 */
int fsm(void);

#endif
