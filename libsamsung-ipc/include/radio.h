/**
 * This file is part of libsamsung-ipc.
 *
 * Copyright (C) 2010-2011 Joerie de Gram <j.de.gram@gmail.com>
 *
 * Modified for Jet - KB <kbjetdroid@gmail.com>
 *
 * libsamsung-ipc is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * libsamsung-ipc is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with libsamsung-ipc.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef __RADIO_H__
#define __RADIO_H__

#include <stdint.h>
#include <stdio.h>

#include "types.h"
#include "util.h"

#define IPC_CLIENT_TYPE_FMT      0
#define IPC_CLIENT_TYPE_RFS      1
#define IPC_CLIENT_TYPE_PACKET   2 //For Jet

#define IPC_DEVICE_CRESPO       0
#define IPC_DEVICE_ARIES        1
#define IPC_DEVICE_JET          2
#define IPC_DEVICE_WAVE         3

#define IPC_DEVICE_MAX          IPC_DEVICE_WAVE

#define IPC_COMMAND(f)  ((f->group << 8) | f->index)
#define IPC_GROUP(m)    (m >> 8)
#define IPC_INDEX(m)    (m & 0xff)

struct ipc_header {
    unsigned short length;
    unsigned char mseq, aseq;
    unsigned char group, index, type;
} __attribute__((__packed__));

struct ipc_message_info {
    unsigned char mseq;
    unsigned char aseq;
    unsigned char group;
    unsigned char index;
    unsigned char type;
    unsigned int length;
    unsigned char *data;
};

struct fifoPacketHeader
{
	uint32_t magic; //filled by modemctl
	uint32_t cmd;
	uint32_t datasize;
};

struct modem_io {
	uint32_t magic; //filled by modemctl
	uint32_t cmd;
	uint32_t datasize;
	uint8_t *data;
};

struct ipc_packet {
    unsigned char mseq;
    unsigned char aseq;
    unsigned char group;
    unsigned char index;
    unsigned char type;
    unsigned int length;
    unsigned char *data;
};

struct ipc_client;
struct ipc_handlers;

extern struct ipc_handlers ipc_default_handlers;

void ipc_init(void);
void ipc_shutdown(void);

typedef void (*ipc_client_log_handler_cb)(const char *message, void *user_data);

typedef int (*ipc_io_handler_cb)(void *data, unsigned int size, void *io_data);
typedef int (*ipc_handler_cb)(void *data);

struct ipc_client* ipc_client_new(int client_type);
struct ipc_client *ipc_client_new_for_device(int device_type, int client_type);
int ipc_client_free(struct ipc_client *client);

int ipc_client_set_log_handler(struct ipc_client *client, ipc_client_log_handler_cb log_handler_cb, void *user_data);

int ipc_client_set_handlers(struct ipc_client *client, struct ipc_handlers *handlers);
int ipc_client_set_io_handlers(struct ipc_client *client, 
                               ipc_io_handler_cb read, void *read_data,
                               ipc_io_handler_cb write, void *write_data);
int ipc_client_set_handlers_common_data(struct ipc_client *client, void *data);
void *ipc_client_get_handlers_common_data(struct ipc_client *client);
int ipc_client_create_handlers_common_data(struct ipc_client *client);
int ipc_client_destroy_handlers_common_data(struct ipc_client *client);
int ipc_client_set_handlers_common_data_fd(struct ipc_client *client, int fd);
int ipc_client_get_handlers_common_data_fd(struct ipc_client *client);

int ipc_client_bootstrap_modem(struct ipc_client *client);
int ipc_client_modem_operations(struct ipc_client *client, void *data, unsigned int cmd);
int ipc_client_open(struct ipc_client *client);
int ipc_client_close(struct ipc_client *client);
int ipc_client_power_on(struct ipc_client *client);
int ipc_client_power_off(struct ipc_client *client);

int ipc_client_recv(struct ipc_client *client, struct modem_io *response);

/* Convenience functions for ipc_send */
int ipc_client_send(struct ipc_client *client, struct modem_io *request);
void ipc_client_send_get(struct ipc_client *client, const unsigned short command, unsigned char mseq);
void ipc_client_send_exec(struct ipc_client *client, const unsigned short command, unsigned char mseq);

/* Utility functions */
const char *ipc_command_type_to_str(int command);
const char *ipc_response_type_to_str(int type);
const char *ipc_request_type_to_str(int type);
void ipc_hex_dump(struct ipc_client *client, void *data, int size);
void *ipc_mtd_read(struct ipc_client *client, char *mtd_name, int size, int block_size);
void *ipc_file_read(struct ipc_client *client, char *file_name, int size, int block_size);

#ifndef RIL_SHLIB

struct ipc_client *client_fmt;

static inline void ipc_fmt_send(struct modem_io *request)
{
	ipc_client_send(client_fmt, request);
}

static inline int ipc_fmt_modem_io(void *data, uint32_t cmd)
{
	return ipc_client_modem_operations(client_fmt, data, cmd);
}

#else
	extern void ipc_fmt_send(struct modem_io *request);
	extern int ipc_fmt_modem_io(void *data, uint32_t cmd);

#endif //RIL_SHLIB

#endif

// vim:ts=4:sw=4:expandtab