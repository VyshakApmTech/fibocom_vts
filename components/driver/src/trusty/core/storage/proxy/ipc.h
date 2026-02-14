/*
 * Copyright (C) 2016 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#pragma once

#include <stdint.h>
#include "../interface/include/trusty/interface/storage.h"

#define REQ_BUFFER_SIZE (1024 * 5)

int ipc_init(void);
void ipc_destroy(void);
int ipc_connect(const char *service_name);
void ipc_disconnect(void);
int ipc_get_msg(struct storage_msg *msg, void *req_buf, size_t req_buf_len);
int ipc_respond(struct storage_msg *msg, void *out, size_t out_size);
int ipc_send_message(struct storage_msg *msg, void *out, size_t out_size);

#if 0
int tee_ipc_get_msg(struct tee_storage_msg *msg, void *req_buf, size_t req_buf_len);
int tee_ipc_respond(struct tee_storage_msg *msg, struct tee_fs_rpc_operation *op, void *out, size_t out_size);
#endif
