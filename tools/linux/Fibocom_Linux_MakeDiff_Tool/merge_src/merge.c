#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

int g_sign_flag=1;                   // 1 to sign delta firmware package. 0 to no_sign.

#define OUT_FW_FILE                 "./out/fw.bin"
#define OUT_APP_DELTA_FILE          "./out/appfota.bin"
#define OUT_APP_FW_FILE             "./out/appfw_merge.bin"

#define BUFF_SIZE                   512
#define MAX_SIZE                    4*1024*1024
#define SPACE_FW_RESERVED_8910      130*1024    // old 8910 baseline need 130KB to upgrade fw
#define SPACE_SYSTEM_RESERVED_8910  16*1024     // 8910 baseline need 16KB for system running

const char* platforms_list[] = {
    "8850", "8850BM-C1", "8910", "8910V1.6", "8910C3", NULL
};

const char* usage_firmware_delta =          "\t./merge -p platform -f oldfw.pac newfw.pac\n";
const char* usage_app_delta =               "\t./merge -p platform -a oldapp.img newapp.img [-b blocsize]\n";
const char* usage_app_firmware_delta =      "\t./merge -p platform -a oldapp.img newapp.img [-b blocsize] -f oldfw.pac newfw.pac\n";
const char* usage_app_firmware_delta_auto_optimal = \
    "\t./merge -p platform -a oldapp.img newapp.img -f oldfw.pac newfw.pac \
--auto-optimal simple_type internal_filesystem_available_space external_filesystem_available_space memory_space\n\
- simple_type is 1 means fw and app delta package will both stored in internal flash, upgrade use internal flash.\n\
- simple_type is 2 means fw delta package will stored in internal flash, upgrade use internal flash, \n\
                         app delta package will stored in external flash, upgrade use external flash.\n";

static void print_version_logs(void)
{
    printf("---------------------------------------\n");
    printf("|       Tool Version : V1.1.0.13      |\n");
    printf("|       Compile Time : 2025/07/01     |\n");
    printf("---------------------------------------\n");
}

static void usage_print(void)
{
    printf("**Description**:\n");
    printf("Used to generate ota package.\n\n");

    // print platform list
    printf("**Options**:\n");
    printf("  -p    Platform name (refer to the platform list below).\n");
    printf("  -a    Make app delta package.\n");
    printf("        **Usage**: `-a <oldapp.img> <newapp.img>`\n");
    printf("  -f    Make firmware delta package.\n");
    printf("        **Usage**: `-f <oldfw.pac> <newfw.pac>`\n");
    printf("  -b    App block size for cutting `app.img`.\n");
    printf("        - New and old `app.img` must be divided into the **same number of blocks**.\n");
    printf("        - If omitted, a minimum value will be automatically generated.\n");
    printf("  -s    Sign the delta firmware package.\n");
    printf("        - `1`: Enable signing (default).\n");
    printf("        - `0`: Disable signing.\n");
    printf("\n");

    printf("**Details**:\n");

    // print platform
    printf("- Platform List:\n\t[");
    for (int i = 0; platforms_list[i] != NULL; i++) {
        printf("%s  ", platforms_list[i]);
    }
    printf("]\n");

    // print usage firmware delta
    printf("- Make firmware delta package:\n%s", usage_firmware_delta);

    // print usage app delta
    printf("- Make app delta package:\n%s", usage_app_delta);

    // print usage app&firmware delta
    printf("- Make app&firmware delta package:\n%s", usage_app_firmware_delta);

    printf("\n**For example**:\n");
    printf("\t./merge -p 8850 -a app_v1.img app_v2.img -f fw_v1.pac fw_v2.pac -b 120 -s 1\n");

    printf("\n**Not recommended commands**:");
    printf("\nFor compatibility with older version commands,the following commands are also support:\n");
    printf("\t./merge --platform -af oldapp.img newapp.img oldfw.pac newfw.pac");
    printf("\nFor example:\n");
    printf("\t./merge --8910 -af app_v1.img app_v2.img fw_v1.pac fw_v2.pac\n");

    //print for auto optimal app&firmware delta
    //printf("\n--------------------------------reserved command---------------------------------------\n");
    //printf("- Make app&firmware auto optimal delta package:\n%s", usage_app_firmware_delta_auto_optimal);
    //printf("For example:\n");
    //printf("\t./merge -p 8910 -a app_v1.img app_v2.img -f fw_v1.pac fw_v2.pac --auto-optimal 1 904400 2097152 524288\n");
    //printf("\n");
}

static int file_exists(const char* filename) {
    int true = 1;
    int false = 0;
    FILE *file = fopen(filename, "r");

    if (file) {
        fclose(file);
        return true;
    }
    else {
        return false;
    }
}

static int fw_delta_file_sign(char* platform, char *src_fw_file)
{
    char pdata[BUFF_SIZE] = {0};
    char *sign_fw_file = "./out/sign.bin";

    if(!memcmp(platform, "8910", strlen(platform))) {
        printf("Info: platform 8910 doesn't have sign.\n");
        return 0;
    }

    memset(pdata, 0, BUFF_SIZE);
    sprintf(pdata, "cp %s %s", src_fw_file, "./out/fw_no_sign_bak.bin");
    printf("exceuting : %s\n", pdata);
    system(pdata);

    memset(pdata, 0, BUFF_SIZE);
    printf("Info: will sign delta firmware package.\n");
    if(!memcmp(platform, "8910V1.6", strlen("8910V1.6")) || !memcmp(platform, "8910C3", strlen("8910C3"))) {
        sprintf(pdata, "./bin/%s/dtools sign8910 --pn test --pw 12345678 %s %s", platform, src_fw_file, sign_fw_file);
    }
    else {
        sprintf(pdata, \
                "./bin/%s/dtools fotasign88xx ./bin/%s/setting/secp256r1_prv.pem ./bin/%s/setting/secp256r1_pub.pem %s %s", \
                platform, platform, platform, src_fw_file, sign_fw_file);
    }

    printf("exceuting : %s\n", pdata);
    system(pdata);
    if (!file_exists(sign_fw_file)) {
        printf("Error: %s generate error.\n", sign_fw_file);
        goto error;
    }

    if (rename(sign_fw_file, src_fw_file)) {
        printf("Error: rename %s %s\n", sign_fw_file, src_fw_file);
    }

    return 0;

error:
    unlink(sign_fw_file);
    return -1;
}

static int make_fw_delta_package(char* platform, char *file1, char *file2)
{
    char pdata[BUFF_SIZE] = {0};
    char *fw_file = OUT_FW_FILE;

    unlink(fw_file);

    sprintf(pdata, "./bin/%s/dtools fotacreate2 --pac %s,%s,bin/%s/setting/fota.xml %s -d v", platform, file1, file2, platform, OUT_FW_FILE);

    printf("exceuting : %s\n", pdata);
    system(pdata);
    if (!file_exists(fw_file)) {
        printf("Error: %s generate error.\n", fw_file);
        goto error;
    }

    if (g_sign_flag == 1) {
        printf("Info: will sign delta firmware package.\n");
        if (fw_delta_file_sign(platform, fw_file)) {
            printf("Error: fw_delta_file_sign fail!\n");
            goto error;
        }
    }

    return 0;

error:
    unlink(fw_file);
    return -1;
}

int merge_file(char *file1, char *file2)
{
    char    *mem = NULL;
    int     fd1 = -1;
    int     fd2 = -1;
    int     fd_out = -1;
    int     read_len = 0;
    int     write_len = 0;
    char    *appfw_path = OUT_APP_FW_FILE;

    unlink(appfw_path);

    mem = (char *)malloc(MAX_SIZE);
    if(mem == NULL){
        printf("Error: malloc error!\n");
        return -1;
    }
    memset(mem, 0, sizeof(MAX_SIZE));

    fd1 = open(file1, O_RDONLY, 0);
    if(fd1 <= 0){
        printf("Error: open %s failed!\n", file1);
        goto error;
    }

    fd2 = open(file2, O_RDONLY, 0);
    if(fd2 <= 0){
        printf("Error: open %s failed!\n", file2);
        goto error;
    }

    fd_out = open(appfw_path, O_RDWR | O_CREAT, 0644);
    if(fd_out <= 0){
        printf("Error: open %s failed!\n", appfw_path);
        goto error;
    }

    read_len = read(fd1, mem, MAX_SIZE);
    if(read_len <= 0){
        printf("Error: read %s file failed.\n", file1);
        goto error;
    }

    write_len = write(fd_out, mem, read_len);
    if(write_len != read_len){
        printf("Error: write fd_out failed.\n");
        goto error;
    }

    memset(mem, 0, sizeof(MAX_SIZE));

    read_len = read(fd2, mem, MAX_SIZE);
    if(read_len <= 0){
        printf("Error: read %s file failed.\n", file1);
        goto error;
    }

    write_len = write(fd_out, mem, read_len);
    if(write_len != read_len){
        printf("Error: write fd_out failed.\n");
        goto error;
    }

    printf("merge file SUCCESS! ++++\n");

    if (mem) {
        free(mem);
        mem = NULL;
    }
    if (fd1 > 0)
        close(fd1);
    if (fd2 > 0)
        close(fd2);
    if (fd_out > 0)
        close(fd_out);

    return 0;

error:
    if (mem) {
        free(mem);
        mem = NULL;
    }

    if (fd1 > 0)
        close(fd1);
    if (fd2 > 0)
        close(fd2);
    if (fd_out > 0)
        close(fd_out);

    exit(1);
    return -1;
}

int calculate_blocksize(char *file1, char *file2)
{
    struct stat file_1_t;
    struct stat file_2_t;
    int         file_size1 = 0;
    int         file_size2 = 0;
    int         blocksize = 0;
    int         block1 = 0;
    int         block2 = 0;

    if(stat(file1,&file_1_t)) {
        printf("Error: %s open error\n", file1);
        goto error;
    }
    file_size1 = file_1_t.st_size;

    if(stat(file2,&file_2_t)) {
        printf("Error: %s open error\n", file2);
        goto error;
    }
    file_size2 = file_2_t.st_size;

    if(file_size1 <= 104*1024 && file_size2 <= 104*1024)
        blocksize = 104;
    else{
        for(blocksize=104; blocksize<1000; blocksize+=4) {
            block1 = file_size1/(blocksize*1024) + (file_size1 % (blocksize*1024)? 1 : 0);
            block2 = file_size2/(blocksize*1024) + (file_size1 % (blocksize*1024)? 1 : 0);
            if(block1 == block2){
                printf("calculate best blocksize : %d, block number : %d\n", blocksize, block1);
                break;
            }
        }
    }
    if(blocksize >= 1000) {
        printf("Error: calculate blocksize fail! %d, %d\n", file_size1, file_size2);
        goto error;
    }

    return blocksize;

error:
    exit(1);
    return -1;
}

static unsigned int simple_crc(const char *data, int size)
{
    unsigned int crc = 0;

    for (int i = 0; i < size; i++) {
        crc += (unsigned char)data[i];
    }

    return crc;
}

/*
 * appfota.bin Tail Structure (512 bytes by default)
 * Maximum capacity: 31 block entries
 * 
 * ---------------------------------------------------------------------------------------|-------|
 * | "APPOTATAIL" magic header (10 bytes) | Version: 01 (2 bytes) | CRC checksum (4 bytes)|       |
 * ---------------------------------------------------------------------------------------|       |
 * | Old block 1 CRC (4 bytes) | New block 1 CRC (4 bytes) |        Reserved (8 bytes)    | TOTAL |
 * ---------------------------------------------------------------------------------------|       |
 * | Old block 2 CRC (4 bytes) | New block 2 CRC (4 bytes) |        Reserved (8 bytes)    |  512  |
 * ---------------------------------------------------------------------------------------|       |
 * |                   ...                  ...                  ...                   ...| BYTES |
 * ---------------------------------------------------------------------------------------|       |
 * | Old block n CRC (4 bytes) | New block n CRC (4 bytes) |        Reserved (8 bytes)    |       |
 * ---------------------------------------------------------------------------------------|-------|
 * 
 * Each block information includes:
 * - Old block CRC checksum (4 bytes)
 * - New block CRC checksum (4 bytes)
 * - Reserved field (8 bytes)
 */
#define APPFOTA_TAIL_HEADER "APPOTATAIL01"
#define RESERVED_BYTES      8
#define TOTAL_SIZE          512
int update_appfota_file(char *appfota_file)
{
    FILE        *file = NULL;
    int         file_size = 0;
    char        *buffer = NULL;
    unsigned int crc = 0;

    file = fopen(appfota_file, "rb+");
    fseek(file, 0, SEEK_END);
    file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    buffer = (char *)malloc(file_size);
    if (!buffer) {
        printf("Error: Memory allocation failed\n");
        goto error;
    }
    memset(buffer, 0, file_size);

    if (fread(buffer, 1, file_size, file) != (size_t)file_size) {
        printf("Error: Failed to read file content\n");
        goto error;
    }
    unsigned int total_length = (unsigned int)file_size;
    *((unsigned int *)buffer) = total_length;

    crc = 0;
    crc += simple_crc(buffer+30, file_size-30);
    *((unsigned int *)(buffer + 4)) = crc;

    fseek(file, 0, SEEK_SET);
    if (fwrite(buffer, 1, file_size, file) != (size_t)file_size) {
        printf("Error: Failed to write back to file\n");
        goto error;
    }

    // update header crc
    fseek(file, 0, SEEK_SET);
    if (fread(buffer, 1, file_size, file) != (size_t)file_size) {
        printf("Error: Failed to read file content\n");
        goto error;
    }
    crc = 0;
    crc += *(unsigned int *)(buffer + 0);
    crc += *(unsigned int *)(buffer + 4);
    crc += *(unsigned int *)(buffer + 8);
    crc += *(unsigned int *)(buffer + 12);
    crc += *(unsigned int *)(buffer + 16);
    printf("header crc : %u\n", crc);
    *((unsigned int *)(buffer + 20)) = crc;
    fseek(file, 0, SEEK_SET);
    if (fwrite(buffer, 1, file_size, file) != (size_t)file_size) {
        printf("Error: Failed to write back to file\n");
        goto error;
    }

    if (file) {
        fclose(file);
    }

    if (buffer) {
        free(buffer);
        buffer = NULL;
    }

    printf("File header updated successfully.\n");
    return 0;

error:
    if (file) {
        fclose(file);
    }

    if (buffer) {
        free(buffer);
        buffer = NULL;
    }

    printf("Error: File header updated failed!\n");
    exit(1);
    return -1;
}

int add_appfota_tail(char *source_file, char *target_file, int blocksize, char *appfota_file)
{
    FILE    *fs = NULL;
    FILE    *ft = NULL;
    FILE    *fa = NULL;

    char    *s_buf = NULL;
    char    *t_buf = NULL;

    int     fs_size = 0;
    int     ft_size = 0;

    int     blocks = 0;
    int     block_max = 31;
    int     count_size = 0;

    fs = fopen(source_file, "rb");
    if (!fs) {
        printf("Error: Failed to open source file! \n");
        goto error;
    }
 
    ft = fopen(target_file, "rb");
    if (!ft) {
        printf("Error: Failed to open target file");
        goto error;
    }
 
    fa = fopen(appfota_file, "ab");
    if (!fa) {
        printf("Error: Failed to open appfota file");
        goto error;
    }

    fseek(fs, 0, SEEK_END);
    fs_size = ftell(fs);
    fseek(fs, 0, SEEK_SET);

    fseek(ft, 0, SEEK_END);
    ft_size = ftell(ft);
    fseek(ft, 0, SEEK_SET);

    //add tail header
    char header[16] = {0};
    strncpy(header, APPFOTA_TAIL_HEADER, strlen(APPFOTA_TAIL_HEADER));
    fwrite(header, 1, sizeof(header), fa);
    count_size += sizeof(header);

    // 512 bytes can store up to 31 block crc information
    blocks = (fs_size + blocksize * 1024 - 1) / (blocksize * 1024);
    if (blocks > block_max) { 
        printf("Error: Too many blocks for the tail\n");
        goto error;
    }

    s_buf = malloc(blocksize * 1024);
    t_buf = malloc(blocksize * 1024);
    if (!s_buf || !t_buf) {
        printf("Error: Memory allocation failed");
        goto error;
    }
    memset(s_buf, 0, blocksize * 1024);
    memset(t_buf, 0, blocksize * 1024);

    for (int i = 0; i < blocks; i++) {
        // read source file
        size_t s_read = fread(s_buf, 1, blocksize * 1024, fs);
        if (s_read < blocksize * 1024 && !feof(fs)) {
            printf("Error: reading source file");
            goto error;
        }
 
        // read target file
        size_t t_read = fread(t_buf, 1, blocksize * 1024, ft);
        if (t_read < blocksize * 1024 && !feof(ft)) {
            printf("Error: reading target file");
            goto error;
        }
 
        // caculate crc
        unsigned int s_crc = simple_crc(s_buf, s_read);
        unsigned int t_crc = simple_crc(t_buf, t_read);
        unsigned int crc_pair[2] = {s_crc, t_crc};
        fwrite(crc_pair, sizeof(unsigned int), 2, fa);
        count_size += sizeof(crc_pair);
        printf("block %d old crc: %u\n", i, s_crc);
        printf("block %d new crc: %u\n", i, t_crc);

        // Write 8 NULL bytes for RESERVED
        char null_bytes[RESERVED_BYTES] = {0};
        fwrite(null_bytes, 1, RESERVED_BYTES, fa);
        count_size += sizeof(null_bytes);
    }

    printf("fwrite total count_size : %d\n", count_size);
    if (count_size > TOTAL_SIZE) {
        printf("Error: count_size too long!");
        goto error;
    }
    else {
        char padding[TOTAL_SIZE] = {0};
        fwrite(padding, 1, TOTAL_SIZE - count_size, fa);
    }

    printf("add_appfota_tail successful!");

    if (fs)
        fclose(fs);
    if (ft)
        fclose(ft);
    if (fa)
        fclose(fa);
    if (s_buf)
        free(s_buf);
    if (t_buf)
        free(t_buf);

    //update appfota.bin content crc, len, total crc
    update_appfota_file(appfota_file);
    return 0;

error:
    printf("add_appfota_tail error!");

    if (fs)
        fclose(fs);
    if (ft)
        fclose(ft);
    if (fa)
        fclose(fa);
    if (s_buf)
        free(s_buf);
    if (t_buf)
        free(t_buf);
    exit(1);
    return -1;
}

int make_app_delta_package(char *file1, char *file2, int blocksize_in)
{
    char    pdata[BUFF_SIZE] = {0};
    FILE    *fp;
    char    *result = NULL;
    char    buf[BUFF_SIZE] = {0};
    int     blocksize = 0;
    int     ret = -1;

    if (blocksize_in > 0) {
        blocksize = blocksize_in;
    }
    else {
        blocksize = calculate_blocksize(file1, file2);
    }

    unlink(OUT_APP_DELTA_FILE);
    unlink("./out/3.bin");

    sprintf(pdata, "./bin/bsdiff %s %s %s %d", file1, file2, OUT_APP_DELTA_FILE, blocksize);
    printf("exceuting : %s\n", pdata);
    system(pdata);

    //record each block old/new crc in appfota.bin tail for crc mismatch phenomena
    printf("exceuting : add_appfota_tail\n");
    ret = add_appfota_tail(file1, file2, blocksize, OUT_APP_DELTA_FILE);
    if (ret) {
        printf("Error: add_appfota_tail fail!\n");
        goto error;
    }

    memset(pdata, 0, BUFF_SIZE);
    sprintf(pdata, "./bin/bspatch %s ./out/3.bin %s %d", file1, OUT_APP_DELTA_FILE, blocksize);
    printf("exceuting : %s\n", pdata);
    system(pdata);

    //check if 3.bin is same as file2
    memset(pdata, 0, BUFF_SIZE);
    result = malloc(BUFF_SIZE);
    if (result == NULL) {
        printf("Error: malloc fail!\n");
        goto error;
    }

    memset(result, 0, BUFF_SIZE);
    sprintf(pdata, "diff %s %s", file2, "./out/3.bin");
    fp = popen(pdata, "r");
    if (!fp) {
        printf("Error: execute \"%s\" fail!\n", pdata);
        goto error;
    }

    while(fgets(buf, BUFF_SIZE, fp))
        strcat(result, buf);

    pclose(fp);

    if (*result){
        printf("Error: appfota.bin is wrong\n");
        goto error;
    }

    unlink("./out/3.bin");

    if (result) {
        free(result);
        result=NULL;
    }
    return 0;

error:
    unlink(OUT_APP_DELTA_FILE);
    unlink("./out/3.bin");

    if (result) {
        free(result);
        result=NULL;
    }

    exit(1);
    return -1;
}

int make_app_delta_package_with_rule(
    char    *platform,
    char    *file1,
    char    *file2,
    int     type,
    int     inner_fs_space,
    int     ext_fs_space,
    int     memory_space
)
{
    int     inner_fs_space_need = 0;
    int     ext_fs_space_need = 0;
    int     memory_need = 0;
    int     block_size = 100;
    char    *fw_file = OUT_FW_FILE;
    char    *app_file = OUT_APP_DELTA_FILE;
    struct  stat file_1_t;
    struct  stat file_2_t;
    struct  stat file_fw_t;
    struct  stat file_app_t;

    if (0 != strcmp(platform, "8910")) {
        printf("Error: platform only supports \"8910\", exit!\n");
        goto error;
    }

    if (type == 1) {
        printf("Type: 1\n");
        printf("fw and app delta package will both stored in internal flash, upgrade use internal flash.\n");
    }
    else if (type == 2) {
        printf("Type: 2\n");
        printf("fw delta package will stored in internal flash, upgrade use internal flash.\n");
        printf("app delta package will stored in external flash, upgrade use external flash.\n");
    }
    else {
        printf("Error: not support this type(%d) now, exit!\n", type);
        goto error;
    }

    if (stat(fw_file, &file_fw_t) == -1) {
        printf("Error: stat %s error, exit!\n", fw_file);
        goto error;
    }

    if(stat(file1, &file_1_t)) {
        printf("Error: stat %s error, exit!\n", file1);
        goto error;
    }

    if(stat(file2, &file_2_t)) {
        printf("Error: stat %s error, exit!\n", file2);
        goto error;
    }

    // make app delta package
    while (1) {
        inner_fs_space_need = 0;
        ext_fs_space_need = 0;
        memory_need = 0;
        block_size += 4;

        printf("\nAttempting suitable app delta blocksize(%d)\n", block_size);

        if (block_size > 1000) {
            printf("Error: block_size > 1000, but still unable to meet the conditions. exit!\n");
            goto error;
        }

        if (((int)file_1_t.st_size/(block_size*1024) + ((int)file_1_t.st_size % (block_size*1024)? 1 : 0)) != \
            ((int)file_2_t.st_size/(block_size*1024) + ((int)file_2_t.st_size % (block_size*1024)? 1 : 0))) {
            printf("Warning: block size (%d) not suitable, continue next attempt.\n", block_size);
            continue;
        }

        if (0 == make_app_delta_package(file1, file2, block_size)) {
            printf("app delta package successful!\n");
        }
        else {
            printf("\nError: app delta package error, exit!\n\n");
            goto error;
        }

        if (stat(app_file, &file_app_t) == -1) {
            printf("Error: stat %s error, exit!\n", app_file);
            goto error;
        }

        if (type == 1) {
            inner_fs_space_need += SPACE_SYSTEM_RESERVED_8910;

            inner_fs_space_need += (int)file_fw_t.st_size;
            inner_fs_space_need += SPACE_FW_RESERVED_8910;

            inner_fs_space_need += (int)file_app_t.st_size;
            inner_fs_space_need += block_size*1024;

            memory_need += (int)file_fw_t.st_size;
            memory_need += (int)file_app_t.st_size;

        }
        else if (type == 2) {
            inner_fs_space_need += SPACE_SYSTEM_RESERVED_8910;

            inner_fs_space_need += (int)file_fw_t.st_size;
            inner_fs_space_need += SPACE_FW_RESERVED_8910;

            ext_fs_space_need += (int)file_app_t.st_size;
            ext_fs_space_need += block_size*1024;

            memory_need += (int)file_fw_t.st_size;
            memory_need += (int)file_app_t.st_size;
        }

        printf("inner_fs_space_need(%d), memory_need(%d), ext_fs_space_need(%d)\n", \
                inner_fs_space_need, memory_need, ext_fs_space_need);

        if ((inner_fs_space_need < inner_fs_space) && \
            (memory_need < memory_space) && \
            (ext_fs_space_need <= ext_fs_space)) {
            printf("============congratulation!============\n");
            break;
        }
        else{
            printf("inner_fs_space_need(%d) or memory_need(%d) or ext_fs_space_need(%d) too big! exit!\n", \
                    inner_fs_space_need, memory_need, ext_fs_space_need);
            printf("continue...\n");
            continue;
        }
    }

    return 0;

error:
    exit(1);
    return -1;
}

int main(int argc, char *argv[])
{
    int i = 0;                  // just use for params match
    int j = 0;                  // just use for params check

    char  *platform = NULL;     // platform string, reference const char* platforms_list[]
    char  *old_app_img = NULL;  // old app image path
    char  *new_app_img = NULL;  // new app image path

    int   block_size = 0;       // app block size, used to separate app to generate delta upgrade package

    char  *old_fw_img = NULL;   // old firmware image path
    char  *new_fw_img = NULL;   // new firmware image path

    int   simple_type = 0;      // Type of simple production. Generate available upgrade packages based on app*fw fs space
    int   inner_fs_space = 0;   // internal filesystem available space
    int   ext_fs_space = 0;     // external filesystem available space
    int   memory_space = 0;     // firmware memory available space, should more than package size, just for old upgrade api

    print_version_logs();

    // display usage.
    if (argc == 1) {
        usage_print();
        return 0;
    }

    // For compatibility with older version commands
    if (strncmp(argv[1], "--", 2) == 0) {
        printf("Warning, customer is using an old command. \n");
        goto old_param_logic;
    }

    // platform match.
    for (i=1; i<argc-1; i++) {
        if (strcmp(argv[i], "-p") == 0) {
            platform = argv[i + 1];
            break;
        }
    }

    // platform must have a value.
    if (platform == NULL) {
        printf("Error: platform parameter not found, exit!\n\n");
        goto error;
    }
    else{
        // Check the legality of the platform
        for (j=0; platforms_list[j] != NULL; j++) {
            if (strcmp(platforms_list[j], platform) == 0) {
                break;
            }
        }

        if (platforms_list[j] == NULL) {
            printf("Error: platform parameter(%s) not in platforms_list!\n\n", platform);
            goto error;
        }
    }

    // app image match.
    for (i = 1; i<argc-1; i++) {
        if (strcmp(argv[i], "-a") == 0) {
            if (argc < i+2) {
                printf("Error: app image parameters lack.\n");
                goto error;
            }

            old_app_img = argv[i + 1];
            new_app_img = argv[i + 2];
            if (old_app_img == NULL || new_app_img == NULL) {
                printf("Error: app image parameters not found.\n");
                goto error;
            }
            else {
                // Check whether the app file exists
                if (!file_exists(old_app_img) || !file_exists(new_app_img)) {
                    printf("Error: old_app_img(%s) or new_app_img(%s) not exist.\n\n", old_app_img, new_app_img);
                    goto error;
                }
            }
            break;
        }
    }

    // app block size match.
    for (i = 1; i<argc-1; i++) {
        if (strcmp(argv[i], "-b") == 0) {
            block_size = atoi(argv[i + 1]);
            if (block_size < 0) {
                printf("Error: block_size(%d) error, exit!\n\n", block_size);
                goto error;
            }
            else {
                if (block_size < 104 || block_size > 1000 || (block_size%4 != 0)) {
                    printf("block_size(%d) error : 100<block_size<1000, and should be a multiple of 4 !\n\n", block_size);
                    goto error;
                }
            }
            break;
        }
    }

    // firmware image match.
    for (i = 1; i<argc-1; i++) {
        if (strcmp(argv[i], "-f") == 0) {
            old_fw_img = argv[i + 1];
            new_fw_img = argv[i + 2];
            if (old_fw_img == NULL || new_fw_img == NULL) {
                printf("Error: firmware pac parameters not found\n");
            }
            else {
                // Check whether the firmware file exists
                if (!file_exists(old_fw_img) || !file_exists(new_fw_img)) {
                    printf("Error: old_fw_img(%s) or file_exists(%s) not exist.\n\n", old_fw_img, new_fw_img);
                    goto error;
                }
            }
            break;
        }
    }

    // match g_sign_flag
    for (i = 1; i<argc-1; i++) {
        if (strcmp(argv[i], "-s") == 0) {
            g_sign_flag = atoi(argv[i + 1]);
            if (g_sign_flag!=0 || g_sign_flag!=1) {
                printf("Error: g_sign_flag(%d) error, exit!\n\n",g_sign_flag);
                goto error;
            }
            break;
        }
    }

    // auto-optimal match simple_type inner_fs_space ext_fs_space
    for (i = 1; i<argc-1; i++) {
        if (strcmp(argv[i], "--auto-optimal") == 0) {
            simple_type = atoi(argv[i + 1]);
            inner_fs_space = atoi(argv[i + 2]);
            ext_fs_space = atoi(argv[i + 3]);
            memory_space = atoi(argv[i + 4]);
            if (simple_type<=0 || inner_fs_space<=0 || ext_fs_space<=0 || memory_space<=0) {
                printf("Error: simple_type(%d) inner_fs_space(%d), ext_fs_space(%d), memory_space(%d)error, exit!\n\n",
                               simple_type, inner_fs_space, ext_fs_space, memory_space);
                goto error;
            }
            break;
        }
    }

    goto make_package;

old_param_logic:
    printf("Info : old_param_logic in.\n");

    if(argc == 1)
        goto error;

    if(!memcmp(argv[1], "--8850", strlen(argv[1])))
        platform="8850";
    else if(!memcmp(argv[1], "--8850BM-C1", strlen(argv[1])))
        platform="8850BM-C1";
    else if(!memcmp(argv[1], "--8910", strlen(argv[1])))
        platform="8910";
    else if(!memcmp(argv[1], "--8910V1.6", strlen(argv[1])))
        platform="8910V1.6";
    else
        goto error;

    //App&firmware union delta package
    if(!memcmp(argv[2], "-af", 3) && (strlen(argv[2]) == 3)){
        if(argc != 7)
            goto error;

        old_app_img = argv[2 + 1];
        new_app_img = argv[2 + 2];
        old_fw_img =  argv[2 + 3];
        new_fw_img =  argv[2 + 4];
    }
    else {
        printf("Unknow command! Error!\n");
        goto error;
    }

make_package:
    // params match finished, just printf for good looking.
    printf("\n========== params match finished ==========\n");
    printf("  platform: %s      \n", platform);
    printf("  old_app_img: %s   \n", old_app_img);
    printf("  new_app_img: %s   \n", new_app_img);
    printf("  block_size: %d    \n", block_size);
    printf("  old_fw_img: %s    \n", old_fw_img);
    printf("  new_fw_img: %s    \n", new_fw_img);
    printf("-------------- reserved command -----------\n");
    printf("  simple_type: %d   \n", simple_type);
    printf("  inner_fs_space: %d  \n", inner_fs_space);
    printf("  ext_fs_space: %d   \n", ext_fs_space);
    printf("  memory_space: %d   \n", memory_space);

    if (!old_app_img && !new_app_img && !old_fw_img && !new_fw_img) {
        printf("\nError: parameters error, exit!\n\n");
        goto error;
    }

    // environmental preparation
    system("rm -rf ./out");
    system("mkdir out");

    // firmware delta package generation.
    if (old_fw_img && new_fw_img) {
        printf("========== firmware delta package ==========\n");
        if (0 == make_fw_delta_package(platform, old_fw_img, new_fw_img)) {
            printf("========== firmware delta package successful! ==========\n");
        }
        else {
            printf("\nError: firmware delta package error, exit!\n\n");
            goto error;
        }
    }

    // app delta package generation.
    if (old_app_img && new_app_img) {
        printf("========== app delta package ==========\n");
        if (simple_type <= 0) {
            printf("========== app delta package normal mode. ==========\n");
            if (0 == make_app_delta_package(old_app_img, new_app_img, block_size)) {
                printf("========== app delta package successful! ==========\n");
            }
            else {
                printf("\nError: app delta package error, exit!\n\n");
                goto error;
            }
        }
        else {
            printf("========== app delta package cusomized mode. ==========\n");
            if (0 == make_app_delta_package_with_rule(platform, old_app_img, new_app_img, simple_type, inner_fs_space, ext_fs_space, memory_space)) {
                printf("========== app delta package successful! ==========\n");
            }
            else {
                printf("\nError: app delta package error, exit!\n\n");
                goto error;
            }
        }
    }

    // app&fw merge
    char *fw_file = OUT_FW_FILE;
    char *app_file = OUT_APP_DELTA_FILE;
    if (file_exists(fw_file) && file_exists(app_file)) {
        printf("========== app&fw delta package ==========\n");
        if(0 == merge_file(app_file, fw_file)) {
            printf("========== merge app&fw delta package successful! ==========\n");
        }
        else {
            printf("\nError: merge app&fw delta package error, exit!\n\n");
            goto error;
        }
    }

    return 0;

error:
    printf("Note: run \"./merge\" to display usage!\n");
    exit(1);
    return -1;
}
