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
#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "log.h"
#include "ipc.h"
#include "vfs.h"
#include "storage.h"

static int open_fd;
static const char *ssdir_name;

static struct
{
    struct storage_file_read_resp hdr;
    uint8_t data[MAX_READ_SIZE];
} read_rsp;

static uint32_t insert_fd(int open_flags, int fd)
{
    if (fd < 0)
    {
        trusty_error(OSI_LOGPAR_SSI, "%s %s: fd= %d  fail \n",
                     STORAGE_STR, __func__, fd);
        open_fd = -1;
    }
    else
    {
        open_fd = fd;
    }
    return open_fd;
}

static int lookup_fd(uint32_t handle, bool dirty)
{
    if (handle != open_fd)
    {
        trusty_error(OSI_LOGPAR_SSI, "%s %s: fd= %d  fail \n",
                     STORAGE_STR, __func__, handle);
    }

    return open_fd;
}

static int remove_fd(uint32_t handle)
{
    if (handle != open_fd)
    {
        trusty_error(OSI_LOGPAR_SSI, "%s %s: fd= %d  fail \n",
                     STORAGE_STR, __func__, handle);
    }

    return open_fd;
}

enum storage_err translate_errno(int error)
{
    enum storage_err result;
    switch (error)
    {
    case 0:
        result = STORAGE_NO_ERROR;
        break;
    case EBADF:
    case EINVAL:
    case ENOTDIR:
    case EISDIR:
    case ENAMETOOLONG:
        result = STORAGE_ERR_NOT_VALID;
        break;
    case ENOENT:
        result = STORAGE_ERR_NOT_FOUND;
        break;
    case EEXIST:
        result = STORAGE_ERR_EXIST;
        break;
    case EPERM:
    case EACCES:
        result = STORAGE_ERR_ACCESS;
        break;
    default:
        result = STORAGE_ERR_GENERIC;
        break;
    }

    return result;
}

ssize_t write_with_retry(int fd, const void *buf_, size_t size, off_t offset)
{
    ssize_t rc;
    struct stat st;

    rc = vfs_fstat(fd, &st);
    if (rc < 0)
    {
        trusty_error(OSI_LOGPAR_SSII, "%s %s: vfs_fstat fd= %d rc = %d fail \n",
                     STORAGE_STR, __func__, fd, rc);
        return rc;
    }

    if (st.st_size < offset + size)
    {
        rc = vfs_ftruncate(fd, offset + size);
        if (rc < 0)
        {
            trusty_error(OSI_LOGPAR_SIII, "%s : vfs_ftruncate fd= %d lenth= %d rc = %d fail \n",
                         STORAGE_STR, fd, (offset + size), rc);
            return rc;
        }
    }

    if (vfs_lseek(fd, offset, SEEK_SET) != offset)
    {
        trusty_error(OSI_LOGPAR_SSII, "%s %s: vfs_lseek fd= %d, offset = %d fail \n",
                     STORAGE_STR, __func__, fd, offset);
        return -1;
    }

    rc = vfs_write(fd, buf_, size);
    if (rc < 0)
    {
        trusty_error(OSI_LOGPAR_SSII, "%s %s: vfs_write fd= %d rc = %d fail \n",
                     STORAGE_STR, __func__, fd, rc);
        return -1;
    }
    return rc;
}

ssize_t read_with_retry(int fd, void *buf_, size_t size, off_t offset)
{
    ssize_t rc;

    if (vfs_lseek(fd, offset, SEEK_SET) != offset)
    {
        trusty_error(OSI_LOGPAR_SSII, "%s %s: vfs_lseek fd= %d, offset = %d failed \n",
                     STORAGE_STR, __func__, fd, offset);
        return -1;
    }

    rc = vfs_read(fd, buf_, size);
    if (rc < 0)
    {
        trusty_error(OSI_LOGPAR_SSII, "%s %s: vfs_read fd= %d rc =%d failed \n",
                     STORAGE_STR, __func__, fd, rc);
        return -1;
    }
    return rc;
}

int storage_file_delete(struct storage_msg *msg,
                        const void *r, size_t req_len)
{
    char path[128];
    const struct storage_file_delete_req *req = r;

    if (req_len < sizeof(*req))
    {
        trusty_error(OSI_LOGPAR_SSII, "%s %s: invalid request length (%zd < %zd)\n",
                     STORAGE_STR, __func__, req_len, sizeof(*req));
        msg->result = STORAGE_ERR_NOT_VALID;
        goto err_response;
    }

    size_t fname_len = strlen(req->name);
    if (fname_len != req_len - sizeof(*req))
    {
        trusty_error(OSI_LOGPAR_SSII, "%s %s: invalid filename length (%zd != %zd)\n",
                     STORAGE_STR, __func__, fname_len, req_len - sizeof(*req));
        msg->result = STORAGE_ERR_NOT_VALID;
        goto err_response;
    }

    int rc = sprintf(path, "%s/%s", ssdir_name, req->name);
    if (rc < 0)
    {
        trusty_error(OSI_LOGPAR_SS, "%s %s: sprintf failed\n", STORAGE_STR, __func__);
        msg->result = STORAGE_ERR_GENERIC;
        goto err_response;
    }

    rc = vfs_unlink(path);
    if (rc < 0)
    {
        if (errno == ENOENT)
        {
            trusty_error(OSI_LOGPAR_SSIS, "%s %s: error (%d) unlinking file '%s'\n",
                         STORAGE_STR, __func__, rc, path);
        }
        else
        {
            trusty_error(OSI_LOGPAR_SSIS, "%s %s: error (%d) unlinking file '%s'\n",
                         STORAGE_STR, __func__, rc, path);
        }
        msg->result = translate_errno(rc);
        goto err_response;
    }

    trusty_error(OSI_LOGPAR_SSS, "%s %s: vfs_unlink \"%s\"\n", STORAGE_STR, __func__, path);
    msg->result = STORAGE_NO_ERROR;

err_response:
    return ipc_respond(msg, NULL, 0);
}

int storage_file_open(struct storage_msg *msg,
                      const void *r, size_t req_len)
{
    char path[128];
    const struct storage_file_open_req *req = r;
    struct storage_file_open_resp resp = {0};

    if (req_len < sizeof(*req))
    {
        trusty_error(OSI_LOGPAR_SSII, "%s %s: invalid request length (%zd < %zd)\n",
                     STORAGE_STR, __func__, req_len, sizeof(*req));
        msg->result = STORAGE_ERR_NOT_VALID;
        goto err_response;
    }

    size_t fname_len = strlen(req->name);
    if (fname_len != req_len - sizeof(*req))
    {
        trusty_error(OSI_LOGPAR_SSII, "%s %s: invalid filename length (%zd != %zd)\n",
                     STORAGE_STR, __func__, fname_len, req_len - sizeof(*req));
        msg->result = STORAGE_ERR_NOT_VALID;
        goto err_response;
    }

    int rc = sprintf(path, "%s/%s", ssdir_name, req->name);
    if (rc < 0)
    {
        trusty_error(OSI_LOGPAR_SS, "%s %s: sprintf failed\n", STORAGE_STR, __func__);
        msg->result = STORAGE_ERR_GENERIC;
        goto err_response;
    }

    int open_flags = O_RDWR;

    if (req->flags & STORAGE_FILE_OPEN_TRUNCATE)
        open_flags |= O_TRUNC;

    if (req->flags & STORAGE_FILE_OPEN_CREATE)
    {
        /* open or create */
        if (req->flags & STORAGE_FILE_OPEN_CREATE_EXCLUSIVE)
        {
            /* create exclusive */
            open_flags |= O_CREAT | O_EXCL;
            rc = vfs_open(path, open_flags, S_IRUSR | S_IWUSR);
        }
        else
        {
            /* try open first */
            rc = vfs_open(path, open_flags, S_IRUSR | S_IWUSR);
            if (rc == -1)
            {
                /* then try open with O_CREATE */
                open_flags |= O_CREAT;
                rc = vfs_open(path, open_flags, S_IRUSR | S_IWUSR);
            }
        }
    }
    else
    {
        /* open an existing file */
        rc = vfs_open(path, open_flags, S_IRUSR | S_IWUSR);
    }

    if (rc < 0)
    {
        if (errno == EEXIST || errno == ENOENT)
        {
            trusty_error(OSI_LOGPAR_SSS, "%s %s: failed to open file \"%s\"\n",
                         STORAGE_STR, __func__, path);
        }
        else
        {
            trusty_error(OSI_LOGPAR_SSS, "%s %s: failed to open file \"%s\"\n",
                         STORAGE_STR, __func__, path);
        }
        msg->result = translate_errno(rc);
        goto err_response;
    }

    /* at this point rc contains storage file fd */
    msg->result = STORAGE_NO_ERROR;
    resp.handle = insert_fd(open_flags, rc);
    trusty_debug(OSI_LOGPAR_SSII, "%s : \"%s\": fd = %u: handle = %d\n",
                 STORAGE_STR, path, rc, resp.handle);

    return ipc_respond(msg, &resp, sizeof(resp));

err_response:
    return ipc_respond(msg, NULL, 0);
}

int storage_file_close(struct storage_msg *msg,
                       const void *r, size_t req_len)
{
    const struct storage_file_close_req *req = r;

    if (req_len != sizeof(*req))
    {
        trusty_error(OSI_LOGPAR_SSII, "%s %s: invalid request length (%zd != %zd)\n",
                     STORAGE_STR, __func__, req_len, sizeof(*req));
        msg->result = STORAGE_ERR_NOT_VALID;
        goto err_response;
    }

    int fd = remove_fd(req->handle);
    trusty_error(OSI_LOGPAR_SSII, "%s %s: handle = %u: fd = %u\n", STORAGE_STR, __func__, req->handle, fd);

    int rc = vfs_fsync(fd);
    if (rc < 0)
    {
        rc = errno;
        trusty_error(OSI_LOGPAR_SSI, "%s %s: fsync failed for fd=%d\n",
                     STORAGE_STR, __func__, fd);
        msg->result = translate_errno(rc);
        goto err_response;
    }

    rc = vfs_close(fd);
    if (rc < 0)
    {
        rc = errno;
        trusty_error(OSI_LOGPAR_SSI, "%s %s: close failed for fd=%d: %s\n",
                     STORAGE_STR, __func__, fd);
        msg->result = translate_errno(rc);
        goto err_response;
    }

    msg->result = STORAGE_NO_ERROR;

err_response:
    return ipc_respond(msg, NULL, 0);
}

int storage_file_write(struct storage_msg *msg,
                       const void *r, size_t req_len)
{
    int rc;
    const struct storage_file_write_req *req = r;

    if (req_len < sizeof(*req))
    {
        trusty_error(OSI_LOGPAR_SSII, "%s %s: invalid request length (%zd < %zd)\n",
                     STORAGE_STR, __func__, req_len, sizeof(*req));
        msg->result = STORAGE_ERR_NOT_VALID;
        goto err_response;
    }

    int fd = lookup_fd(req->handle, true);
    rc = write_with_retry(fd, &req->data[0], req_len - sizeof(*req), req->offset);
    if (rc < 0)
    {

        trusty_error(OSI_LOGPAR_SSI, "%s %s: error writing file fd=%d\n",
                     STORAGE_STR, __func__, fd);
        msg->result = translate_errno(rc);
        goto err_response;
    }

    msg->result = STORAGE_NO_ERROR;

err_response:
    return ipc_respond(msg, NULL, 0);
}

int storage_file_read(struct storage_msg *msg,
                      const void *r, size_t req_len)
{
    const struct storage_file_read_req *req = r;

    if (req_len != sizeof(*req))
    {
        trusty_error(OSI_LOGPAR_SSII, "%s %s: invalid request length (%zd != %zd)\n",
                     STORAGE_STR, __func__, req_len, sizeof(*req));
        msg->result = STORAGE_ERR_NOT_VALID;
        goto err_response;
    }

    if (req->size > MAX_READ_SIZE)
    {
        trusty_error(OSI_LOGPAR_SSII, "%s %s: request is too large (%zd > %zd) - refusing\n",
                     STORAGE_STR, __func__, req->size, MAX_READ_SIZE);
        msg->result = STORAGE_ERR_NOT_VALID;
        goto err_response;
    }

    int fd = lookup_fd(req->handle, false);
    ssize_t read_res = read_with_retry(fd, read_rsp.hdr.data, req->size,
                                       (off_t)req->offset);
    if (read_res < 0)
    {
        trusty_error(OSI_LOGPAR_SSI, "%s %s: error reading file fd=%d\n",
                     STORAGE_STR, __func__, fd);
        msg->result = translate_errno(errno);
        goto err_response;
    }

    msg->result = STORAGE_NO_ERROR;
    return ipc_respond(msg, &read_rsp, (size_t)(read_res) + sizeof(read_rsp.hdr));

err_response:
    return ipc_respond(msg, NULL, 0);
}

int storage_file_get_size(struct storage_msg *msg,
                          const void *r, size_t req_len)
{
    const struct storage_file_get_size_req *req = r;
    struct storage_file_get_size_resp resp = {0};

    if (req_len != sizeof(*req))
    {
        trusty_error(OSI_LOGPAR_SSII, "%s %s: invalid request length (%zd != %zd)\n",
                     STORAGE_STR, __func__, req_len, sizeof(*req));
        msg->result = STORAGE_ERR_NOT_VALID;
        goto err_response;
    }

    struct stat stat;
    int fd = lookup_fd(req->handle, false);
    int rc = vfs_fstat(fd, &stat);
    if (rc < 0)
    {
        trusty_error(OSI_LOGPAR_SSI, "%s %s: error stat'ing file (fd=%d)\n",
                     STORAGE_STR, __func__, fd);
        msg->result = translate_errno(rc);
        goto err_response;
    }

    resp.size = stat.st_size;
    msg->result = STORAGE_NO_ERROR;
    return ipc_respond(msg, &resp, sizeof(resp));

err_response:
    return ipc_respond(msg, NULL, 0);
}

int storage_file_set_size(struct storage_msg *msg,
                          const void *r, size_t req_len)
{
    const struct storage_file_set_size_req *req = r;

    if (req_len != sizeof(*req))
    {
        trusty_error(OSI_LOGPAR_SSII, "%s %s: invalid request length (%zd != %zd)\n",
                     STORAGE_STR, __func__, req_len, sizeof(*req));
        msg->result = STORAGE_ERR_NOT_VALID;
        goto err_response;
    }

    int fd = lookup_fd(req->handle, true);
    int rc = vfs_ftruncate(fd, req->size);
    if (rc < 0)
    {
        trusty_error(OSI_LOGPAR_SSI, "%s %s: error truncating file (fd=%d)\n",
                     STORAGE_STR, __func__, fd);
        msg->result = translate_errno(errno);
        goto err_response;
    }

    msg->result = STORAGE_NO_ERROR;

err_response:
    return ipc_respond(msg, NULL, 0);
}

int storage_init(const char *dirname)
{

    vfs_mkdir(dirname, 0);

    ssdir_name = dirname;
    return 0;
}

int storage_sync_checkpoint(void)
{
    int rc;

    if (open_fd < 0)
    {
        trusty_error(OSI_LOGPAR_SSI, "%s %s: fd= %d  fail \n",
                     STORAGE_STR, __func__, open_fd);
        return 0;
    }
    /* need to sync individual fd */
    rc = vfs_fsync(open_fd);
    if (rc < 0)
    {
        trusty_error(OSI_LOGPAR_SI, "%s vfs_fsync for fd=%d failed\n", STORAGE_STR, open_fd);
        return rc;
    }

    return 0;
}
