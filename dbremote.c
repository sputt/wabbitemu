#include "stdafx.h"

#include "calc.h"
#include "dbremote.h"
#include "guidebug.h"
#include "dbreg.h"
#include "keys.h"

#define DEFAULT_BUFLEN 800
#define DEFAULT_PORT 27015

#define R_AF 0
#define R_BC 1
#define R_DE 2
#define R_HL 3
#define R_SP 4
#define R_PC 5

#define R_IX    6
#define R_IY    7
#define R_AF_   8
#define R_BC_   9
#define R_DE_   10
#define R_HL_   11
#define R_IR    12
#define R_LEN   13

#define BYTEFLIP(word) ((WORD)(word << 8) + (WORD)(word >> 8))

#define DBG_FEATURE_STR "<target version=\"1.0\">"\
"<feature name=\"org.gnu.gdb.z80.cpu\">"\
"<reg name=\"af\" bitsize=\"16\" type=\"int\"/>"\
"<reg name=\"bc\" bitsize=\"16\" type=\"int\"/>"\
"<reg name=\"de\" bitsize=\"16\" type=\"int\"/>"\
"<reg name=\"hl\" bitsize=\"16\" type=\"int\"/>"\
"<reg name=\"sp\" bitsize=\"16\" type=\"data_ptr\"/>"\
"<reg name=\"pc\" bitsize=\"16\" type=\"code_ptr\"/>"\
"<reg name=\"ix\" bitsize=\"16\" type=\"int\"/>"\
"<reg name=\"iy\" bitsize=\"16\" type=\"int\"/>"\
"<reg name=\"af'\" bitsize=\"16\" type=\"int\"/>"\
"<reg name=\"bc'\" bitsize=\"16\" type=\"int\"/>"\
"<reg name=\"de'\" bitsize=\"16\" type=\"int\"/>"\
"<reg name=\"hl'\" bitsize=\"16\" type=\"int\"/>"\
"<reg name=\"ir\" bitsize=\"16\" type=\"int\"/>"\
"</feature>"\
"<architecture>z80</architecture>"\
"</target>"

static SOCKET listenSocket = INVALID_SOCKET;
static SOCKET clientSocket = INVALID_SOCKET;

static int port = DEFAULT_PORT;
static bool waiting_for_stop = false;

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

static void send_nack() {
	transmit("-", 1);
}

static void send_ack() {
	transmit("+", 1);
}

static char* last_body = NULL;
static size_t last_body_length = 0;

static void send_response(char* body, size_t length) {
	unsigned char sum = 0;
	for (int i = 0; i < length; i++) {
		sum += body[i];
	}

	const char *fmt = "$%.*s#%02x";
    int size = snprintf(NULL, 0, fmt, length, body, sum);
	char *response = (char*)malloc(size + 1);
    sprintf(response, fmt, length, body, sum);

	transmit(response, size);

	if (last_body) {
		free(last_body);
	}

	last_body = response;
	last_body_length = size;
}

static void resend_response() {
	transmit(last_body, last_body_length);
}

static bool setup_socket() {
    WSADATA wsaData;
    int iResult;

    struct addrinfo *result = NULL;
    struct addrinfo hints;

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
	char portstr[6];
	sprintf(portstr, "%d", port);

    iResult = getaddrinfo(NULL, portstr, &hints, &result);
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

static void process_q(LPCALC lpCalc, char* buffer, size_t bufferSize) {
	if (strncmp(&buffer[2], "Supported", 9) == 0) {
		const char* fmt = "PacketSize=%04x;hwbreak+;qXfer:features:read+";
		int length = snprintf(NULL, 0, fmt, 0xffff);
		char *response = (char*)malloc(length + 1);
		sprintf(response, fmt, DEFAULT_BUFLEN);
		send_response(response, length);
		free(response);
	}
	else if (strncmp(&buffer[2], "Xfer:features:read:", 19) == 0) {
		send_response("l" DBG_FEATURE_STR, strlen("l" DBG_FEATURE_STR));
	}
}

static void process_G(LPCALC lpCalc, char* buffer, size_t bufferSize) {
	for (int i = 0; i < R_LEN && 2 + i * 4 < bufferSize; i++) {
		int scan;
		sscanf(&buffer[2 + i * 4], "%04x", &scan);
		WORD val = scan;
		val = BYTEFLIP(val);
		if (i == R_AF) {
			lpCalc->cpu.af = val;
		}
		else if (i == R_BC) {
			lpCalc->cpu.bc = val;
		}
		else if(i == R_DE) {
			lpCalc->cpu.de = val;
		}
		else if(i == R_HL) {
			lpCalc->cpu.hl = val;
		}
		else if(i == R_SP) {
			lpCalc->cpu.sp = val;
		}
		else if(i == R_PC) {
			lpCalc->cpu.pc = val;
		}
		else if(i == R_IX ) {
			lpCalc->cpu.ix = val;
		}
		else if(i == R_IY ) {
			lpCalc->cpu.iy = val;
		}
		else if(i == R_AF_) {
			lpCalc->cpu.afp = val;
		}
		else if(i == R_BC_) {
			lpCalc->cpu.bcp = val;
		}
		else if(i == R_DE_) {
			lpCalc->cpu.dep = val;
		}
		else if(i == R_HL_) {
			lpCalc->cpu.hlp = val;
		}
		else if (i == R_IR) {
			// I don't trust this one. Best not to do anything.
		}
	}

	send_response("OK", 2);
}

static void process_g(LPCALC lpCalc, char* buffer, size_t bufferSize) {
	char* item_fmt = "%04x";
	size_t item_len = snprintf(NULL, 0, item_fmt, 0xff, 0xffff);
	size_t items_len = item_len * R_LEN;

	size_t response_length = items_len;
	char* response = (char*)malloc(response_length + 1);

	for (int i = 0; i < R_LEN; i++) {
		WORD val;
		if (i == R_AF) {
			val = lpCalc->cpu.af;
		}
		else if (i == R_BC) {
			val = lpCalc->cpu.bc;
		}
		else if(i == R_DE) {
			val = lpCalc->cpu.de;
		}
		else if(i == R_HL) {
			val = lpCalc->cpu.hl;
		}
		else if(i == R_SP) {
			val = lpCalc->cpu.sp;
		}
		else if(i == R_PC) {
			val = lpCalc->cpu.pc;
		}
		else if(i == R_IX ) {
			val = lpCalc->cpu.ix;
		}
		else if(i == R_IY ) {
			val = lpCalc->cpu.iy;
		}
		else if(i == R_AF_) {
			val = lpCalc->cpu.afp;
		}
		else if(i == R_BC_) {
			val = lpCalc->cpu.bcp;
		}
		else if(i == R_DE_) {
			val = lpCalc->cpu.dep;
		}
		else if(i == R_HL_) {
			val = lpCalc->cpu.hlp;
		}
		else if (i == R_IR) {
			val = (((WORD)lpCalc->cpu.i) << 8) + ((WORD)lpCalc->cpu.r);
		}

		sprintf(&response[i * item_len], item_fmt, BYTEFLIP(val));
	}

	send_response(response, response_length);
	free(response);
}

static void process_question(LPCALC lpCalc) {
	char* item_fmt = "%02x:%04x;";
	size_t item_len = snprintf(NULL, 0, item_fmt, 0xff, 0xffff);
	size_t items_len = item_len * R_LEN;

	size_t response_length = 3 + items_len;
	char* response = (char*)malloc(response_length + 1);

	//signal
	sprintf(response, "T%02x", 5);
	
	for (int i = 0; i < R_LEN; i++) {
		WORD val;
		if (i == R_AF) {
			val = lpCalc->cpu.af;
		}
		else if (i == R_BC) {
			val = lpCalc->cpu.bc;
		}
		else if(i == R_DE) {
			val = lpCalc->cpu.de;
		}
		else if(i == R_HL) {
			val = lpCalc->cpu.hl;
		}
		else if(i == R_SP) {
			val = lpCalc->cpu.sp;
		}
		else if(i == R_PC) {
			val = lpCalc->cpu.pc;
		}
		else if(i == R_IX ) {
			val = lpCalc->cpu.ix;
		}
		else if(i == R_IY ) {
			val = lpCalc->cpu.iy;
		}
		else if(i == R_AF_) {
			val = lpCalc->cpu.afp;
		}
		else if(i == R_BC_) {
			val = lpCalc->cpu.bcp;
		}
		else if(i == R_DE_) {
			val = lpCalc->cpu.dep;
		}
		else if(i == R_HL_) {
			val = lpCalc->cpu.hlp;
		}
		else if (i == R_IR) {
			val = (((WORD)lpCalc->cpu.i) << 8) + ((WORD)lpCalc->cpu.r);
		}

		sprintf(&response[3 + i * item_len], item_fmt, i, val);
	}

	send_response(response, response_length);
	free(response);
}

static void process_zZ(LPCALC lpCalc, char* buffer, size_t bufferSize) {
	int type;
	int addr;
	int kind;
	int read = sscanf(&buffer[2], "%x,%x,%x", &type, &addr, &kind);
	bool set = buffer[1] == 'Z';
	if (set) {
		set_break(&lpCalc->mem_c, MAKE_WADDR((uint16_t)addr, 1, 1));
	}
	else {
		clear_break(&lpCalc->mem_c, MAKE_WADDR((uint16_t)addr, 1, 1));
	}

	send_response("OK", 2);
}

static void process_s(LPCALC lpCalc, char* buffer, size_t bufferSize) {
	int addr = -1;
	int read = sscanf(&buffer[2], "%x", addr);
	if (read != 1) {
		addr = -1;
	}

	if (addr >= 0) {
		lpCalc->cpu.pc = BYTEFLIP(addr);
	}

	CPU_step(&lpCalc->cpu);

	process_question(lpCalc);
}

static void process_c(LPCALC lpCalc, char* buffer, size_t bufferSize) {
	int addr = -1;
	int read = sscanf(&buffer[2], "%x", addr);
	if (read != 1) {
		addr = -1;
	}

	if (addr >= 0) {
		lpCalc->cpu.pc = BYTEFLIP(addr);
	}

	waiting_for_stop = true;
	CPU_step(&lpCalc->cpu);
	calc_set_running(lpCalc, TRUE);
}

static void process_M(LPCALC lpCalc, char* buffer, size_t bufferSize) {
	int addr;
	int len;

	sscanf(&buffer[2], "%x,%x", &addr, &len);
	char *data = strchr(&buffer[2], ':') + 1;

	memory_context_t *mem = &lpCalc->mem_c;
	for (int i = 0; i < len && i * 2 < bufferSize - (buffer - data); i++) {
		int scan;
		sscanf(&data[i * 2], "%02x", &scan);
		char val = scan;
		mem_write(mem, addr + i, val);
	}

	send_response("OK", 2);
}

static void process_m(LPCALC lpCalc, char* buffer, size_t bufferSize) {
	int addr;
	int len;

	sscanf(&buffer[2], "%x,%x", &addr, &len);
	size_t response_length = 2 * len;
	char *response = (char*)malloc(response_length + 1);

	memory_context_t *mem = &lpCalc->mem_c;
	for (int i = 0; i < len; i++) {
		sprintf(&response[2 * i], "%02x", mem_read(mem, addr + i));
	}

	send_response(response, response_length);
	free(response);
}

static void do_process(LPCALC lpCalc, char* buffer, size_t bufferSize) {
	buffer[bufferSize] = '\0';
	char command = buffer[1];
	if (command == 'q') {
		process_q(lpCalc, buffer, bufferSize);
	}
	else if (command == 'g') {
		process_g(lpCalc, buffer, bufferSize);
	}
	else if (command == 'G') {
		process_G(lpCalc, buffer, bufferSize);
	}
	else if (command == 's') {
		process_s(lpCalc, buffer, bufferSize);
	}
	else if (command == 'z') {
		process_zZ(lpCalc, buffer, bufferSize);
	}
	else if (command == 'Z') {
		process_zZ(lpCalc, buffer, bufferSize);
	}
	else if (command == 'c') {
		process_c(lpCalc, buffer, bufferSize);
	}
	else if (command == 'm') {
		process_m(lpCalc, buffer, bufferSize);
	}
	else if (command == 'M') {
		process_M(lpCalc, buffer, bufferSize);
	}
	else if (command == '?') {
		process_question(lpCalc);
	}
	else {
		send_response("E01", 3);
	}
}

static void emit_break(LPCALC lpCalc, LPVOID lpParam) {
	if (!lpCalc->running && waiting_for_stop) {
		process_question(lpCalc);
		waiting_for_stop = false;
	}
}

DWORD WINAPI dbremote_thread(LPVOID lpParam) {	
    LPCALC lpCalc = (LPCALC)lpParam;
	if (_Module.GetParsedCmdArgs()->gdb_port > 0 && _Module.GetParsedCmdArgs()->gdb_port <= INTERNET_MAX_PORT_NUMBER_VALUE) {
		port = _Module.GetParsedCmdArgs()->gdb_port;
	}
    setup_socket();
    while (true) {
        clientSocket = accept(listenSocket, NULL, NULL);
        if (clientSocket == INVALID_SOCKET) {
            printf("accept failed with error: %d\n", WSAGetLastError());
            continue;
        }

		calc_set_running(lpCalc, FALSE);
		calc_register_event(lpCalc, EVENT_TYPE::BREAKPOINT_EVENT, &emit_break, NULL);
		calc_register_event(lpCalc, EVENT_TYPE::ROM_RUNNING_EVENT, &emit_break, NULL);

		bool receiving = false;
		char recvBuf[DEFAULT_BUFLEN];
		size_t recvBufSize = 0;
        while (recv(clientSocket, (char *)&recvBuf[recvBufSize++], 1, 0) != SOCKET_ERROR) {
			char thisChar = recvBuf[recvBufSize - 1];
			if (recvBufSize == 1) {
				if (thisChar == '\x03') {
					waiting_for_stop = TRUE;
					recvBufSize = 0;
					calc_set_running(lpCalc, FALSE);
				}
				else if (thisChar == '+') {
					send_nack();
					recvBufSize = 0;
				}
				else if(thisChar == '-') {
					resend_response();
					recvBufSize = 0;
				}
				else if (thisChar == '$') {
					receiving = true;
				}
			}
			else if (thisChar == '#') {
				receiving = false;
				recv(clientSocket, (char*)&recvBuf[recvBufSize], 2, 0);
				recvBufSize += 2;

				unsigned char sum = 0;
				for (int i = 1; i < recvBufSize - 3; i++) {
					sum += recvBuf[i];
				}

				int compare_sum;
				sscanf(&recvBuf[recvBufSize - 2], "%02x", &compare_sum);

				if (compare_sum != sum) {
					send_nack();
					recvBufSize = 0;
					continue;
				}
				else {
					send_ack();
				}

				do_process(lpCalc, recvBuf, recvBufSize);
				recvBufSize = 0;
			}
			else if (!receiving) {
				recvBufSize = 0;
			}
        }

		calc_unregister_event(lpCalc, EVENT_TYPE::BREAKPOINT_EVENT, &emit_break, NULL);
		calc_unregister_event(lpCalc, EVENT_TYPE::ROM_RUNNING_EVENT, &emit_break, NULL);
		CPU_step(&lpCalc->cpu);
		calc_set_running(lpCalc, TRUE);
    }
}