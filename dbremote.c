#include "stdafx.h"

#include "calc.h"
#include "dbremote.h"
#include "dbreg.h"
#include "keys.h"

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"

#define ASC_STX 0x02

#define MON_BINARY_API_VERSION 0x02

#define MON_EVENT_ID 0xffffffff

static enum t_binary_command {
    e_MON_CMD_INVALID = 0x00,

    e_MON_CMD_MEM_GET = 0x01,
    e_MON_CMD_MEM_SET = 0x02,

    e_MON_CMD_CHECKPOINT_GET = 0x11,
    e_MON_CMD_CHECKPOINT_SET = 0x12,
    e_MON_CMD_CHECKPOINT_DELETE = 0x13,
    e_MON_CMD_CHECKPOINT_LIST = 0x14,
    e_MON_CMD_CHECKPOINT_TOGGLE = 0x15,

    e_MON_CMD_CONDITION_SET = 0x22,

    e_MON_CMD_REGISTERS_GET = 0x31,
    e_MON_CMD_REGISTERS_SET = 0x32,

    e_MON_CMD_DUMP = 0x41,
    e_MON_CMD_UNDUMP = 0x42,

    e_MON_CMD_RESOURCE_GET = 0x51,
    e_MON_CMD_RESOURCE_SET = 0x52,

    e_MON_CMD_ADVANCE_INSTRUCTIONS = 0x71,
    e_MON_CMD_KEY_FEED = 0x72,
    e_MON_CMD_EXECUTE_UNTIL_RETURN = 0x73,

    e_MON_CMD_PING = 0x81,
    e_MON_CMD_BANKS_AVAILABLE = 0x82,
    e_MON_CMD_REGISTERS_AVAILABLE = 0x83,
    e_MON_CMD_DISPLAY_GET = 0x84,
    e_MON_CMD_VICE_INFO = 0x85,

    e_MON_CMD_PALETTE_GET = 0x91,

    e_MON_CMD_JOYPORT_SET = 0xa2,

    e_MON_CMD_USERPORT_SET = 0xb2,

    e_MON_CMD_EXIT = 0xaa,
    e_MON_CMD_QUIT = 0xbb,
    e_MON_CMD_RESET = 0xcc,
    e_MON_CMD_AUTOSTART = 0xdd,
};
typedef enum t_binary_command BINARY_COMMAND;

static struct binary_command_s {
    unsigned char* body;
    uint32_t length;
    uint32_t request_id;
    uint8_t api_version;
    BINARY_COMMAND type;
};
typedef struct binary_command_s binary_command_t;

static enum t_binary_response {
    e_MON_RESPONSE_INVALID = 0x00,
    e_MON_RESPONSE_MEM_GET = 0x01,
    e_MON_RESPONSE_MEM_SET = 0x02,

    e_MON_RESPONSE_CHECKPOINT_INFO = 0x11,

    e_MON_RESPONSE_CHECKPOINT_DELETE = 0x13,
    e_MON_RESPONSE_CHECKPOINT_LIST = 0x14,
    e_MON_RESPONSE_CHECKPOINT_TOGGLE = 0x15,

    e_MON_RESPONSE_CONDITION_SET = 0x22,

    e_MON_RESPONSE_REGISTER_INFO = 0x31,

    e_MON_RESPONSE_DUMP = 0x41,
    e_MON_RESPONSE_UNDUMP = 0x42,

    e_MON_RESPONSE_RESOURCE_GET = 0x51,
    e_MON_RESPONSE_RESOURCE_SET = 0x52,

    e_MON_RESPONSE_JAM = 0x61,
    e_MON_RESPONSE_STOPPED = 0x62,
    e_MON_RESPONSE_RESUMED = 0x63,

    e_MON_RESPONSE_ADVANCE_INSTRUCTIONS = 0x71,
    e_MON_RESPONSE_KEYBOARD_FEED = 0x72,
    e_MON_RESPONSE_EXECUTE_UNTIL_RETURN = 0x73,

    e_MON_RESPONSE_PING = 0x81,
    e_MON_RESPONSE_BANKS_AVAILABLE = 0x82,
    e_MON_RESPONSE_REGISTERS_AVAILABLE = 0x83,
    e_MON_RESPONSE_DISPLAY_GET = 0x84,
    e_MON_RESPONSE_VICE_INFO = 0x85,

    e_MON_RESPONSE_PALETTE_GET = 0x91,

    e_MON_RESPONSE_JOYPORT_SET = 0xa2,

    e_MON_RESPONSE_USERPORT_SET = 0xb2,

    e_MON_RESPONSE_EXIT = 0xaa,
    e_MON_RESPONSE_QUIT = 0xbb,
    e_MON_RESPONSE_RESET = 0xcc,
    e_MON_RESPONSE_AUTOSTART = 0xdd,
};
typedef enum t_binary_response BINARY_RESPONSE;

static enum t_mon_error {
    e_MON_ERR_OK = 0x00,
    e_MON_ERR_OBJECT_MISSING = 0x01,
    e_MON_ERR_INVALID_MEMSPACE = 0x02,
    e_MON_ERR_CMD_INVALID_LENGTH = 0x80,
    e_MON_ERR_INVALID_PARAMETER = 0x81,
    e_MON_ERR_CMD_INVALID_API_VERSION = 0x82,
    e_MON_ERR_CMD_INVALID_TYPE = 0x83,
    e_MON_ERR_CMD_FAILURE = 0x8f,
};
typedef enum t_mon_error BINARY_ERROR;

static unsigned char* recvBuf;
static size_t recvBufSize;
static SOCKET listenSocket = INVALID_SOCKET;
static SOCKET clientSocket = INVALID_SOCKET;

static unsigned char* write_uint16(uint16_t input, unsigned char* output) {
    output[0] = input & 0xFFu;
    output[1] = (input >> 8) & 0xFFu;

    return output + 2;
}

static unsigned char* write_uint32(uint32_t input, unsigned char* output) {
    output[0] = input & 0xFFu;
    output[1] = (input >> 8) & 0xFFu;
    output[2] = (input >> 16) & 0xFFu;
    output[3] = (uint8_t)(input >> 24) & 0xFFu;

    return output + 4;
}

static unsigned char* write_string(uint8_t length, unsigned char* input, unsigned char* output) {
    output[0] = length;
    memcpy(&output[1], input, length);

    return output + length + 1;
}

static uint32_t little_endian_to_uint32(unsigned char* input) {
    return (input[3] << 24) + (input[2] << 16) + (input[1] << 8) + input[0];
}

static uint16_t little_endian_to_uint16(unsigned char* input) {
    return (input[1] << 8) + input[0];
}

static int transmit(const char* buffer, size_t buffer_length)
{
    int error = 0;

	size_t len = (size_t)send(clientSocket, buffer, buffer_length, 0);

	if (len != buffer_length) {
		error = -1;
	}
	else {
		error = (int)len;
	}

    return error;
}

static void send_response(uint32_t length, BINARY_RESPONSE response_type, BINARY_ERROR errorcode, uint32_t request_id, unsigned char* body)
{
    unsigned char response[12];

    response[0] = ASC_STX;
    response[1] = MON_BINARY_API_VERSION;
    write_uint32(length, &response[2]);
    response[6] = (uint8_t)response_type;
    response[7] = (uint8_t)errorcode;
    write_uint32(request_id, &response[8]);

    transmit((char*)response, sizeof response);

    if (body != NULL) {
        transmit((char*)body, length);
    }
}

static void send_error(BINARY_ERROR errorcode, uint32_t request_id)
{
    send_response(0, BINARY_RESPONSE::e_MON_RESPONSE_INVALID, errorcode, request_id, NULL);
}

static void emit_break(LPCALC lpCalc, LPVOID lpParam) {
    send_response(0, BINARY_RESPONSE::e_MON_RESPONSE_CHECKPOINT_INFO, BINARY_ERROR::e_MON_ERR_OK, MON_EVENT_ID, NULL);
}

static bool setup_socket() {
    WSADATA wsaData;
    int iResult;

    struct addrinfo *result = NULL;
    struct addrinfo hints;

    recvBuf = (unsigned char*)malloc(DEFAULT_BUFLEN);

    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed with error: %d\n", iResult);
        return false;
    }

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    // Resolve the server address and port
    iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
    if ( iResult != 0 ) {
        printf("getaddrinfo failed with error: %d\n", iResult);
        WSACleanup();
        return false;
    }

    // Create a SOCKET for the server to listen for client connections.
    listenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (listenSocket == INVALID_SOCKET) {
        printf("socket failed with error: %ld\n", WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
        return false;
    }

    // Setup the TCP listening socket
    iResult = bind(listenSocket, result->ai_addr, (int)result->ai_addrlen);
    if (iResult == SOCKET_ERROR) {
        printf("bind failed with error: %d\n", WSAGetLastError());
        freeaddrinfo(result);
        closesocket(listenSocket);
        WSACleanup();
        return false;
    }

    freeaddrinfo(result);

    iResult = listen(listenSocket, SOMAXCONN);
    if (iResult == SOCKET_ERROR) {
        printf("listen failed with error: %d\n", WSAGetLastError());
        closesocket(listenSocket);
        WSACleanup();
        return false;
    }

    return true;
}

static char keybuf[0xffff];
static int keybuf_cursor = 0;
static int keybuf_length = 0;
static int skipped_frames = 0;
static bool depressed = false;

static void drain_keybuf(LPCALC lpCalc, LPVOID lpParam) {
    CPU_t* cpu = &lpCalc->cpu;
    if (!skipped_frames++) {
        return;
    }
    else {
        skipped_frames = 0;
    }

    if (depressed) {
		keypad_release(cpu, keybuf[keybuf_cursor] >> 3 & 0b111, keybuf[keybuf_cursor] & 0b111);
        depressed = false;
        keybuf_cursor++;
    }
    else if(keybuf_cursor < keybuf_length) {
		BOOL changed;
		keypad_press(cpu, keybuf[keybuf_cursor] >> 3 & 0b111, keybuf[keybuf_cursor] & 0b111);
        depressed = true;
    }
    else {
        keybuf_cursor = 0;
        keybuf_length = 0;
    }
}

static void process_key_feed(binary_command_t* command, LPCALC lpCalc) {
    unsigned char* body = command->body;
    uint8_t length = body[0];

    if (command->length < 1 + length) {
        send_error(BINARY_ERROR::e_MON_ERR_CMD_INVALID_LENGTH, command->request_id);
        return;
    }

    memcpy(&keybuf[keybuf_length], &command->body[1], length);
    // FIXME Race
    keybuf_length += command->length;

    send_response(0, BINARY_RESPONSE::e_MON_RESPONSE_KEYBOARD_FEED, e_MON_ERR_OK, command->request_id, NULL);
}

static void process_ping(binary_command_t* command, LPCALC lpCalc) {
    calc_set_running(lpCalc, false);
	send_response(0, e_MON_RESPONSE_PING, e_MON_ERR_OK, command->request_id, NULL);
}

static void process_exit(binary_command_t* command, LPCALC lpCalc) {
    send_response(0, e_MON_RESPONSE_EXIT, e_MON_ERR_OK, command->request_id, NULL);
    calc_set_running(lpCalc, true);
}

static void send_register_info(LPCALC lpCalc, uint32_t request_id)
{
	register_info_t *regs;
	register_info_t *regs_cursor;
	unsigned char *response;
	unsigned char *response_cursor;
	uint32_t response_size = 2;
	uint16_t count = 0;
	uint8_t item_size = 3;

	regs = GetAllRegisters(lpCalc);
	regs_cursor = regs;

	for (; regs_cursor->id != -1; regs_cursor++) {
		++count;
	}

	response_size += count * (item_size + 1);
	response = (unsigned char*)malloc(response_size);
	response_cursor = response;

	regs_cursor = regs;

	response_cursor = write_uint16(count, response_cursor);

	for (; regs_cursor->id != -1; regs_cursor++) {
		*response_cursor = item_size;
		++response_cursor;

		*response_cursor = regs_cursor->id;
		++response_cursor;

		response_cursor = write_uint16(*(uint16_t*)(regs_cursor->data), response_cursor);
	}

	free(regs);

	send_response(response_size, e_MON_RESPONSE_REGISTER_INFO, e_MON_ERR_OK, request_id, response);

	free(response);
}

static void process_registers_get(binary_command_t* command, LPCALC lpCalc) {
	// FIXME or ignore???
	uint8_t requested_memspace = command->body[0];
	if (command->length < 1) {
		send_error(e_MON_ERR_CMD_INVALID_LENGTH, command->request_id);
		return;
	}

	send_register_info(lpCalc, command->request_id);
}

static void process_command(unsigned char* pbuffer, LPCALC lpCalc)
{
    BINARY_COMMAND command_type;
    binary_command_t command;

    ZeroMemory(&command, sizeof(command));

    command.api_version = (uint8_t)pbuffer[1];

    command.request_id = little_endian_to_uint32(&pbuffer[6]);

    if (command.api_version != 0x02) {
        send_error(BINARY_ERROR::e_MON_ERR_CMD_INVALID_API_VERSION, command.request_id);
        return;
    }

    command.length = little_endian_to_uint32(&pbuffer[2]);

    command.type = (BINARY_COMMAND)pbuffer[10];
    command.body = &pbuffer[11];

    command_type = command.type;
    if (command_type == e_MON_CMD_PING) {
        process_ping(&command, lpCalc);

    }
    else if (command_type == e_MON_CMD_MEM_GET) {
        send_error(BINARY_ERROR::e_MON_ERR_CMD_INVALID_TYPE, command.request_id);
    }
    else if (command_type == e_MON_CMD_MEM_SET) {
        send_error(BINARY_ERROR::e_MON_ERR_CMD_INVALID_TYPE, command.request_id);

    }
    else if (command_type == e_MON_CMD_CHECKPOINT_GET) {
        send_error(BINARY_ERROR::e_MON_ERR_CMD_INVALID_TYPE, command.request_id);
    }
    else if (command_type == e_MON_CMD_CHECKPOINT_SET) {
        send_error(BINARY_ERROR::e_MON_ERR_CMD_INVALID_TYPE, command.request_id);
    }
    else if (command_type == e_MON_CMD_CHECKPOINT_DELETE) {
        send_error(BINARY_ERROR::e_MON_ERR_CMD_INVALID_TYPE, command.request_id);
    }
    else if (command_type == e_MON_CMD_CHECKPOINT_LIST) {
        send_error(BINARY_ERROR::e_MON_ERR_CMD_INVALID_TYPE, command.request_id);
    }
    else if (command_type == e_MON_CMD_CHECKPOINT_TOGGLE) {
        send_error(BINARY_ERROR::e_MON_ERR_CMD_INVALID_TYPE, command.request_id);

    }
    else if (command_type == e_MON_CMD_CONDITION_SET) {
        send_error(BINARY_ERROR::e_MON_ERR_CMD_INVALID_TYPE, command.request_id);

    }
    else if (command_type == e_MON_CMD_REGISTERS_GET) {
		process_registers_get(&command, lpCalc);
    }
    else if (command_type == e_MON_CMD_REGISTERS_SET) {
        send_error(BINARY_ERROR::e_MON_ERR_CMD_INVALID_TYPE, command.request_id);

    }
    else if (command_type == e_MON_CMD_DUMP) {
        send_error(BINARY_ERROR::e_MON_ERR_CMD_INVALID_TYPE, command.request_id);
    }
    else if (command_type == e_MON_CMD_UNDUMP) {
        send_error(BINARY_ERROR::e_MON_ERR_CMD_INVALID_TYPE, command.request_id);

    }
    else if (command_type == e_MON_CMD_RESOURCE_GET) {
        send_error(BINARY_ERROR::e_MON_ERR_CMD_INVALID_TYPE, command.request_id);
    }
    else if (command_type == e_MON_CMD_RESOURCE_SET) {
        send_error(BINARY_ERROR::e_MON_ERR_CMD_INVALID_TYPE, command.request_id);

    }
    else if (command_type == e_MON_CMD_ADVANCE_INSTRUCTIONS) {
        send_error(BINARY_ERROR::e_MON_ERR_CMD_INVALID_TYPE, command.request_id);
    }
    else if (command_type == e_MON_CMD_KEY_FEED) {
        process_key_feed(&command, lpCalc);
    }
    else if (command_type == e_MON_CMD_EXECUTE_UNTIL_RETURN) {
        send_error(BINARY_ERROR::e_MON_ERR_CMD_INVALID_TYPE, command.request_id);

    }
    else if (command_type == e_MON_CMD_PALETTE_GET) {
        send_error(BINARY_ERROR::e_MON_ERR_CMD_INVALID_TYPE, command.request_id);

    }
    else if (command_type == e_MON_CMD_JOYPORT_SET) {
        send_error(BINARY_ERROR::e_MON_ERR_CMD_INVALID_TYPE, command.request_id);

    }
    else if (command_type == e_MON_CMD_USERPORT_SET) {
        send_error(BINARY_ERROR::e_MON_ERR_CMD_INVALID_TYPE, command.request_id);

    }
    else if (command_type == e_MON_CMD_BANKS_AVAILABLE) {
        send_error(BINARY_ERROR::e_MON_ERR_CMD_INVALID_TYPE, command.request_id);
    }
    else if (command_type == e_MON_CMD_REGISTERS_AVAILABLE) {
        send_error(BINARY_ERROR::e_MON_ERR_CMD_INVALID_TYPE, command.request_id);
    }
    else if (command_type == e_MON_CMD_DISPLAY_GET) {
        send_error(BINARY_ERROR::e_MON_ERR_CMD_INVALID_TYPE, command.request_id);
    }
    else if (command_type == e_MON_CMD_VICE_INFO) {
        send_error(BINARY_ERROR::e_MON_ERR_CMD_INVALID_TYPE, command.request_id);

    }
    else if (command_type == e_MON_CMD_EXIT) {
        process_exit(&command, lpCalc);
    }
    else if (command_type == e_MON_CMD_QUIT) {
        send_error(BINARY_ERROR::e_MON_ERR_CMD_INVALID_TYPE, command.request_id);
    }
    else if (command_type == e_MON_CMD_RESET) {
        send_error(BINARY_ERROR::e_MON_ERR_CMD_INVALID_TYPE, command.request_id);
    }
    else if (command_type == e_MON_CMD_AUTOSTART) {
        send_error(BINARY_ERROR::e_MON_ERR_CMD_INVALID_TYPE, command.request_id);

    }
    else {
        send_error(BINARY_ERROR::e_MON_ERR_CMD_INVALID_TYPE, command.request_id);
        printf(
            "network debugger command: unknown command %u, "
            "skipping command length of %u",
            command.type, command.length);
    }

    pbuffer[0] = 0;
}

DWORD WINAPI dbremote_thread(LPVOID lpParam) {	
    LPCALC lpCalc = (LPCALC)lpParam;
    setup_socket();
    calc_register_event(lpCalc, EVENT_TYPE::BREAKPOINT_EVENT, &emit_break, NULL);
    calc_register_event(lpCalc, EVENT_TYPE::LCD_ENQUEUE_EVENT, &drain_keybuf, NULL);
    while (true) {

        clientSocket = accept(listenSocket, NULL, NULL);
        if (clientSocket == INVALID_SOCKET) {
            printf("accept failed with error: %d\n", WSAGetLastError());
            continue;
        }

        recv: while (recv(clientSocket, (char *)recvBuf, 1, 0) != SOCKET_ERROR) {
            CPU_t* cpu = &lpCalc->cpu;
            uint8_t api_version;
            uint32_t body_length;
            unsigned int remaining_header_size = 5;
            unsigned int command_size;
			int n = 0;
			int o = 1;

            if (recvBuf[0] != ASC_STX) {
                continue;
            }

            n = 0;
            o = 1;

            while (n < sizeof(api_version) + sizeof(body_length)) {
                o = recv(clientSocket, (char*)&recvBuf[1 + n], (sizeof(api_version) + sizeof(body_length)) - n, 0);
                if (o <= 0) {
                    break;
                }

                n += o;
            }

            if (o <= 0) {
                continue;
            }

            api_version = (uint8_t)recvBuf[1];
            body_length = little_endian_to_uint32(&recvBuf[2]);

            if (api_version >= 0x01 && api_version <= 0x02) {
                remaining_header_size = 5;
            }
            else {
                continue;
            }

            command_size = sizeof(api_version) + sizeof(body_length) + remaining_header_size + body_length + 1;
            if (!recvBuf || recvBufSize < command_size + 1) {
                recvBuf = (unsigned char *)realloc(recvBuf, command_size + 1);
                recvBufSize = command_size + 1;
            }

            n = 0;
            o = 1;

            while (n < remaining_header_size + body_length) {
                o = recv(clientSocket, (char*)&recvBuf[6 + n], remaining_header_size + body_length - n, 0);
                if (o <= 0) {
                    break;
                }

                n += o;
            }

            if (o <= 0) {
                continue;
            }

            process_command(recvBuf, lpCalc);
        }
    }
}