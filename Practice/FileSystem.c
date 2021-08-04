/**
 * author: cc
 * time:  2021-06-22
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <time.h>
#include <unistd.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/types.h>

#define Ki (1 << 10)
#define Mi (1 << 20)
#define BLOCK_SIZE 4096
#define BLOCKID_SIZE 2
#define INODE_SIZE 128
#define BLOCKID_OFFSET_SIZE 4

#define INODE_NAME_LEN 56
#define INODE_LIST_LEN 20

typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned int uint;
typedef unsigned long ulong;

typedef struct {
    uchar type;                     // 0表示目录, 1表示文件
    uchar permission;               // bits[0]表示读权限, bits[1]表示写权限
    ushort link;
    ushort id;
    ushort offset;
    ushort pid;
    ushort p_offset;
    uint size;
    time_t creationTime;
    time_t modificationTime;
    ushort list[INODE_LIST_LEN];
    uchar name[INODE_NAME_LEN];
} inode;

typedef struct _paths_ {
    inode *node;
    struct _paths_ *prev;
    struct _paths_ *next;
} paths;

// 全局变量定义
static const ulong SHMEM_SIZE = 100UL * Mi;
static const uint BLOCK_NUM = (uint)(SHMEM_SIZE / BLOCK_SIZE);
static const uint INODE_NUM_PER_BLOCK = BLOCK_SIZE / INODE_SIZE;
static const uint BLOCKID_NUM_PER_BLOCK = BLOCK_SIZE / BLOCKID_SIZE;
static const uint BLOCKID_OFFSET_NUM_PER_BLOCK = BLOCK_SIZE / BLOCKID_OFFSET_SIZE;
static const uint BLOCKID_IN_LIST_LEN = INODE_LIST_LEN;
static const uint BLOCKID_OFFSET_IN_LIST_LEN = INODE_LIST_LEN >> 1;
static const char bkp[] = ".bk";
static key_t shmkey = -1;
static int shmid = -1;
static char *shmptr = NULL;
static inode *cdptr = NULL;
static FILE *bk = NULL;
static paths *curpaths;
static char cmd[256] = { 0 };
static char *tks[32] = { 0 };

// 函数声明
void main_init();
void main_done();

ushort addr_to_block_id (char *ptr);
void * block_id_to_addr (ushort block_id);
void * addr_to_block_start_addr (char *ptr);
inode * block_id_offset_to_inode (ushort block_id, ushort offset);
inode * path_to_inode (inode *start_iptr, const char *path, int type);

void init_inode_list (inode *iptr_start, ushort id, ushort pid);

void set_bit (char *ptr, ushort offset, char bit);
void set_bit_of_bitmap (ushort block_id, char bit);
void set_bit_of_inode_list (inode *iptr_start, ushort offset, char bit);

int find_free_bit (ulong *ptr, uint bit_num);
ushort find_free_block_id ();
ushort find_free_inode (ushort *offset);
ushort * find_free_dir_list_addr (inode *diptr, char *from_link);
inode * find_dir_inode (inode *diptr, const char *name, int type);
ushort * find_block_id_offset_item (inode *diptr, ushort block_id, ushort offset);

void free_block (ushort block_id);
void free_inode_item (inode *iptr);

int add_inode_to_dir (inode *diptr, const char *name, int type);
int name_is_exists (inode *diptr, const char *name);
char * process_multi_dir (inode *start_iptr, const char *path, int *status);
void change_inode_dir (inode *iptr, inode *diptr);
char * get_last_dir (const char *path);
inode * get_current_dir ();
void get_inode_path (inode *iptr, char *buffer, char delim);
void get_inode_semkey (inode *iptr, char *buffer, char op);
void prompt ();

void read_file (inode *iptr);
void edit_file (inode *iptr);
void clear_file_content (inode *iptr);
void copy_inode_to_content (inode *iptr, const char *tmp_file_path);
void copy_content_to_inode (const char *tmp_file_path, inode *iptr);

int get_cmd ();
int analysis_cmd ();

int exec_mkdir (inode *start_iptr, const char *path);
int exec_touch (inode *start_iptr, const char *path);
int exec_rmdir (inode *start_iptr, const char *path);
int exec_rm (inode *start_iptr, const char *path);
int exec_ls (inode *start_iptr);
int exec_mv (inode *start_iptr, const char *src, const char *dst);
int exec_cat (inode *start_iptr, const char *path);
int exec_gedit (inode *start_iptr, const char *path);
int exec_cd (inode *start_iptr, const char *path);
int exec_exit ();
int exec_nocommand ();

void __disp_cmd ();
void __disp_cp ();

void load_shm ();
void save_shm ();

// 函数定义

/* 主函数相关函数 */
void main_init () {
    char src[BLOCK_SIZE];
    char *dst = NULL;
    int count;
    inode *iptr = NULL;
    // 获取key
    shmkey = ftok(".", 0);
    if (shmkey == -1) {
        printf("\e[0;31mError: ftok error.\e[0m\n");
        exit(-1);
    }
    // 获取共享内存
    shmid = shmget(shmkey, SHMEM_SIZE, IPC_CREAT | 0600);
    if (shmid == -1) {
        printf("\e[0;31mError: shmget error.\e[0m\n");
        exit(-1);
    }
    // 连接共享内存
    shmptr = (char *)shmat(shmid, 0, 0);
    if (shmptr == (void *)-1) {
        printf("\e[0;31mError: shmat error.\e[0m\n");
        exit(-1);
    }
    // 尝试加载.bk的数据
    dst = shmptr;
    if (!access(bkp, F_OK) ) {
        // .bk存在，从.bk载入数据
        load_shm();
    } else {
        // .bk不存在，写入数据到.bk
        // 写入0到共享内存
        bk = fopen(bkp, "w+");
        for (int i = 0; i < BLOCK_NUM; i++) {
            memset(dst, 0, BLOCK_SIZE);
            dst += BLOCK_SIZE;
        }
        // 块0和块1都被占用
        dst = shmptr;
        *dst = 0b11;
        // inode表的首个块的块号是1，更新共享内存数据
        iptr = (inode *)(shmptr + BLOCK_SIZE);
        init_inode_list(iptr, 1, 0);
        set_bit_of_inode_list(iptr, 0, 1);
        iptr[0].id = 1;
        iptr[0].permission = 0b11;
        // 将数据从共享内存复制到.bk
        save_shm();
    }
    curpaths = (paths *)malloc(sizeof(paths));
    curpaths->node = (inode *)(shmptr + BLOCK_SIZE);
    curpaths->prev = NULL;
    curpaths->next = NULL;
}
void main_done () {
    // detach shared memory
    if (shmdt(shmptr) == -1) {
        printf("shmdt error.\n");
        exit(-1);
    }
}

/* 转换相关函数 */
// 将地址映射为block_id
ushort addr_to_block_id (char *ptr) {
    uint diff_bytes = ptr - shmptr;
    ushort block_id = diff_bytes / BLOCK_SIZE;
    return block_id;
}
// 将vlock_id映射为地址
void * block_id_to_addr (ushort block_id) {
    return (shmptr + (uint)block_id * BLOCK_SIZE);
}
// 将地址映射为块起始地址
void * addr_to_block_start_addr (char *ptr) {
    uint diff_bytes = ptr - shmptr;
    ushort block_id = diff_bytes / BLOCK_SIZE;
    return (shmptr + (uint)block_id * BLOCK_SIZE);
}
// 将block_id和offset映射为inode地址
inode * block_id_offset_to_inode (ushort block_id, ushort offset) {
    return (inode *)(shmptr + (uint)block_id * BLOCK_SIZE) + offset;
}
// 以传入的inode目录为当前目录，以path为相对路径，将路径映射为inode地址
inode * path_to_inode (inode *start_iptr, const char *path, int type) {
    // 复制路径
    char pbk[256];
    strncpy(pbk, path, sizeof(pbk));
    // 获取当前目录inode
    inode *tiptr, *iptr = start_iptr;
    // 分割目录
    char *delim = "/";
    char *token = strtok(pbk, delim);
    // 遍历目录
    while (token != NULL) {
        // 检查路径类型
        if (strcmp(token, ".")) {
            // 不是重定位到当前目录，需要进行处理，先检查是否父目录
            if (strcmp(token, "..") == 0) {
                // 重定向到父目录
                if (iptr->pid) {
                    // 父目录存在
                    iptr = block_id_offset_to_inode(iptr->pid, iptr->p_offset);
                } else {
                    // 不存在父目录
                    return NULL;
                }
            } else {
                // 重定向到子目录
                tiptr = find_dir_inode(iptr, token, 0);
                if (!tiptr) tiptr = find_dir_inode(iptr, token, 1);
                if (!tiptr) return NULL;
                iptr = tiptr;
            }
        }
        token = strtok(NULL, delim);
        // 文件类型inode不可进行进一步重定向
        if (token && iptr->type != 0)  return NULL;
    }
    if (iptr->type == type) return iptr;
    return NULL;
}

/* 初始化相关函数 */
// 初始化新扩展的inode表
void init_inode_list (inode *iptr_start, ushort id, ushort pid) {
    inode *iptr = iptr_start + INODE_NUM_PER_BLOCK - 1;
    iptr->size = (uint)0x8000;
    iptr->id = id;
    iptr->pid = pid;
}

/* 置位相关函数 */
// 通过传入起始地址和偏移量确定需要置位的地址，通过bit确定需要设置的位
void set_bit (char *ptr, ushort offset, char bit) {
    ptr += offset >> 3;
    uchar mask = 1 << (offset & 0x07);
    uchar byte = *ptr;
    uchar filter = byte & mask;
    if (filter && !bit) byte &= ~mask;
    else if (!filter && bit) byte |= mask;
    *ptr = byte;
}
// 通过传入block_id在块位图确定需要置位的地址，通过bit确定需要设置的位
void set_bit_of_bitmap (ushort block_id, char bit) {
    char *ptr = shmptr + (block_id >> 3);
    ushort offset = block_id & 0x07;
    set_bit(ptr, offset, bit);
    // 请求新块，需要清空块内容
    if (bit == 1) {
        ptr = (char *)block_id_to_addr(block_id);
        memset(ptr, 0, BLOCK_SIZE);
    }
}
// 通过传入inode表起始地址和inode偏移确定inode的地址，将inode表的末尾块的对应bit置位，通过bit确定需要设置的位
void set_bit_of_inode_list (inode *iptr_start, ushort offset, char bit) {
    set_bit((char *)&(iptr_start[INODE_NUM_PER_BLOCK - 1].size), offset, bit);
}

/* 寻找空闲位置函数 */
// 在指定长度bit中寻找0并返回偏移量
int find_free_bit (ulong *ptr, uint bit_num) {
    ulong mask, bits = ~*ptr;
    int i, offset = 0;
    for (i = bit_num >> 1; i >= 1; i >>= 1) {
        mask = (1UL << i) - 1;
        if (!(bits & mask)) {
            offset += i;
            bits >>= i;
        }
    }
    if (!(bits & 1)) offset = -1;
    return offset;
}
// 在块位图中找到bit为0的偏移作为块号返回
ushort find_free_block_id () {
    uchar mask, found_byte;
    ulong *ptr8 = (ulong *)shmptr;
    int i, loop_count = BLOCK_SIZE / sizeof(ulong);
    ushort block_id = 0;
    for (i = 0; i < loop_count; i++) {
        if (~*ptr8) {
            block_id += (ushort)find_free_bit(ptr8, 64);
            break;
        }
        ptr8++;
        block_id += (ushort)sizeof(ulong) << 3;
    }
    if (i == loop_count) block_id = 0;
    return block_id;
}
// 通过inode列表找到空闲的inode位置，返回block_id并写入偏移offset
ushort find_free_inode (ushort *offset) {
    inode *iptr = (inode *)(shmptr + BLOCK_SIZE);
    inode *last_item_ptr;
    ushort inode_offset, block_id, parent_block_id;
    uint i, mask, found_4_bytes;
    while (1) {
        // 检查当前列表是否存在空闲inode项位置
        last_item_ptr = iptr + INODE_NUM_PER_BLOCK - 1;
        found_4_bytes = last_item_ptr->size;
        if (~found_4_bytes) {
            // 当前列表有空闲inode项位置
            block_id = last_item_ptr->id;
            inode_offset = (ushort)find_free_bit((ulong *)&found_4_bytes, INODE_NUM_PER_BLOCK);
            break;
        } else {
            // 当前列表无空闲inode项位置，检查下一列表块
            parent_block_id = last_item_ptr->id;
            block_id = last_item_ptr->link;
            if (block_id) {
                // 下一列表块存在，更新iptr，并且重新开始循环
                iptr = (inode *)block_id_to_addr(block_id);
            } else {
                // 下一列表块不存在，创建新的列表块
                block_id = find_free_block_id();
                if (block_id) {
                    // 成功找到空闲块，初始化为列表块
                    set_bit_of_bitmap(block_id, 1);
                    iptr = (inode *)block_id_to_addr(block_id);
                    init_inode_list(iptr, block_id, parent_block_id);
                    inode_offset = 0;
                    break;
                } else {
                    // 找不到空闲块，退出程序
                    printf("\e[0;31mError: can not find any free block when finding free inode id.\e[0m\n");
                    exit(-1);
                }
                break;
            }
        }
    }
    *offset = inode_offset;
    return block_id;
}
// 在inode中找到空闲的存储子目录或子文件的信息的地址返回，并写入是否通过链接得到
ushort * find_free_dir_list_addr (inode *diptr, char *from_link) {
    int i;
    ushort block_id, block_id_of_item, used_item_num;
    ushort *ptr2, *block_id_offset_ptr;
    *from_link = 0;
    // 检查list中是否有空闲item位置
    ptr2 = diptr->list;
    for (i = 0; i < BLOCKID_OFFSET_IN_LIST_LEN; i++) {
        if (!ptr2[0]) return ptr2;
        ptr2 += 2;
    }
    // list中无空闲item位置。检查link中是否有空闲item位置
    *from_link = 1;
    block_id = diptr->link;
    if (block_id) {
        // 找到下一item列表块
        ptr2 = (ushort *)block_id_to_addr(block_id);
        while (1) {
            block_id_offset_ptr = ptr2 + ((BLOCKID_OFFSET_NUM_PER_BLOCK - 1) << 1);
            used_item_num = block_id_offset_ptr[1];
            if (used_item_num >= BLOCKID_OFFSET_NUM_PER_BLOCK - 1) {
                // 当前列表块无剩余位置，尝试寻找下一列表块
                block_id_of_item = block_id_offset_ptr[0];
                if (block_id_of_item) {
                    ptr2 = (ushort *)block_id_to_addr(block_id_of_item);
                    continue;
                } else {
                    // 下一列表块不存在，创建新列表块
                    block_id = find_free_block_id();
                    if (block_id) {
                        // 成功创建新列表块
                        set_bit_of_bitmap(block_id, 1);
                        block_id_offset_ptr[0] = block_id;
                        ptr2 = (ushort *)block_id_to_addr(block_id);
                        return ptr2;
                    } else {
                        // 创建新列表块失败，退出程序
                        printf("\e[0;31mError: can not find a free block when creating blockid-offset-list.\e[0m\n");
                        exit(-1);
                    }
                }
            } else {
                // 当前列表块有剩余位置，寻找该位置
                block_id_offset_ptr = ptr2;
                for (i = 0; i < BLOCKID_OFFSET_NUM_PER_BLOCK - 1; i++) {
                    block_id_of_item = block_id_offset_ptr[0];
                    if (!block_id_of_item) return block_id_offset_ptr;
                    block_id_offset_ptr += 2;
                }
            }
        }
    } else {
        // 下一列表块不存在，创建新列表块
        block_id = find_free_block_id();
        if (block_id) {
            // 成功创建新列表块
            set_bit_of_bitmap(block_id, 1);
            diptr->link = block_id;
            ptr2 = (ushort *)block_id_to_addr(block_id);
            return ptr2;
        } else {
            // 创建新列表块失败，退出程序
            printf("\e[0;31mError: can not find a free block when creating blockid-offset-list.\e[0m\n");
            exit(-1);
        }
    }
}
// 通过名字找到传入inode目录中存在的目录或文件并返回inode，通过type确定需要寻找的类型
inode * find_dir_inode (inode *diptr, const char *name, int type) {
    ushort *list = diptr->list;
    ushort *block_id_offset_last_item_ptr, *block_id_offset_ptr = list;
    ushort block_id_of_item, offset_of_item;
    uint inode_count = 0, inode_num = diptr->size;
    inode *iptr;
    // 尝试在list内寻找inode
    for (int i = 0; i < BLOCKID_OFFSET_IN_LIST_LEN && inode_count < inode_num; i++) {
        block_id_of_item = block_id_offset_ptr[0];
        if (block_id_of_item) {
            inode_count++;
            offset_of_item = block_id_offset_ptr[1];
            iptr = block_id_offset_to_inode(block_id_of_item, offset_of_item);
            if (strcmp(iptr->name, name) == 0 && iptr->type == type) return iptr;
        }
        block_id_offset_ptr += 2;
    }
    // 尝试通过link寻找inode
    ushort next_list_block_id, used_item_count, used_item_num;
    if (inode_count < inode_num) {
        // 有剩余inode未检测
        block_id_offset_ptr = (ushort *)block_id_to_addr(diptr->link);
        block_id_offset_last_item_ptr = block_id_offset_ptr + ((BLOCKID_OFFSET_NUM_PER_BLOCK - 1) << 1);
        while (1) {
            // 在每个块只检查used_item_num个有效item即可
            next_list_block_id = block_id_offset_last_item_ptr[0];
            used_item_num = block_id_offset_last_item_ptr[1];
            used_item_count = 0;
            for (int i = 0; i < BLOCKID_OFFSET_NUM_PER_BLOCK - 1 && used_item_count < used_item_num; i++) {
                block_id_of_item = block_id_offset_ptr[0];
                if (block_id_of_item) {
                    used_item_count++;
                    offset_of_item = block_id_offset_ptr[1];
                    iptr = block_id_offset_to_inode(block_id_of_item, offset_of_item);
                    if (strcmp(iptr->name, name) == 0 && iptr->type == type) return iptr;
                }
            }
            // 检查是否有下一个列表块
            if (next_list_block_id) {
                // 更新item指针，后重新循环
                block_id_offset_ptr = (ushort *)block_id_to_addr(next_list_block_id);
                block_id_offset_last_item_ptr = block_id_offset_ptr + ((BLOCKID_OFFSET_NUM_PER_BLOCK - 1) << 1);
            } else {
                break;
            }
        }
    }
    // 查不到该inode，返回NULL
    return NULL;
}
// 在inode中通过block_i和offset确定特定的信息对所在的地址并返回
ushort * find_block_id_offset_item (inode *diptr, ushort block_id, ushort offset) {
    ushort *last_item_ptr, *item_ptr = diptr->list;
    ushort block_id_of_item, offset_of_item, link;
    // 尝试在list寻找item
    for (int i = 0; i < BLOCKID_OFFSET_IN_LIST_LEN; i++) {
        block_id_of_item = item_ptr[0];
        offset_of_item = item_ptr[1];
        if (block_id_of_item == block_id && offset_of_item == offset) return item_ptr;
        item_ptr += 2;
    }
    // list中未找到，通过link进行寻找
    ushort next_list_block_id, used_item_count, used_item_num;
    link = diptr->link;
    while (1) {
        item_ptr = (ushort *)block_id_to_addr(link);
        last_item_ptr = item_ptr + ((BLOCKID_OFFSET_NUM_PER_BLOCK - 1) << 1);
        next_list_block_id = last_item_ptr[0];
        used_item_num = last_item_ptr[1];
        used_item_count = 0;
        for (int i = 0; i < BLOCKID_OFFSET_NUM_PER_BLOCK - 1 && used_item_count < used_item_num; i++) {
            block_id_of_item = item_ptr[0];
            offset_of_item = item_ptr[1];
            if (block_id_of_item) {
                used_item_count++;
                if (block_id_of_item == block_id && offset_of_item == offset) return item_ptr;
            }
            item_ptr += 2;
        }
        if (next_list_block_id) {
            link = next_list_block_id;
        } else {
            break;
        }
    }
    return NULL;
}

/* 释放相关函数 */
// 通过block_id定位block并释放块内存
void free_block (ushort block_id) {
    char *ptr = (char *)block_id_to_addr(block_id);
    memset(ptr, 0, BLOCK_SIZE);
    set_bit_of_bitmap(block_id, 0);
}
// 清除inode在其所在目录的记录信息
void free_inode_item (inode *iptr) {
    inode *diptr = block_id_offset_to_inode(iptr->pid, iptr->p_offset);
    ushort *item_ptr = find_block_id_offset_item(diptr, iptr->id, iptr->offset);
    // 检查是否在inode列表或链接中找到子inode信息
    if (item_ptr == NULL) {
        printf("\e[0;31mError: cannot find child inode information in inode list or link.\e[0m\n");
        return;
    }
    // 找到item对应位置，将信息清零
    item_ptr[0] = item_ptr[1] = 0;
    // 减少文件数量
    diptr->size--;
    // 更新inode表的最后项的信息
    inode *iptr_start = (inode *)block_id_to_addr(iptr->id);
    set_bit_of_inode_list(iptr_start, iptr->offset, 0);
}

/* 其他函数 */
// 将inode添加到某已存在的inode节点上
int add_inode_to_dir(inode *diptr, const char *name, int type) {
    // type = 0 表示添加目录，type = 1 表示添加文件
    // 检查是否已存在重名文件或文件夹
    inode *dbl_name_iptr = find_dir_inode(diptr, name, type);
    if (dbl_name_iptr) {
        printf("\e[0;31mError: a %s '%s' with the same name already exists.\e[0m\n", (type ? "file" : "directory"), name);
        return -1;
    }
    // 计算需要复制的名称长度
    int name_len = strlen(name);
    int max_name_len = INODE_NAME_LEN - 1;
    name_len = name_len > max_name_len ? max_name_len : name_len;
    // 获取空闲的inode的位置
    ushort block_id, inode_offset;
    block_id = find_free_inode(&inode_offset);
    // 获取当前时间
    time_t now;
    time(&now);
    // 设定初始值
    inode *iptr = (inode *)block_id_to_addr(block_id);
    inode *niptr = iptr + inode_offset;
    niptr->type = !!type;
    niptr->permission = 0b11;
    niptr->id = block_id;
    niptr->offset = inode_offset;
    niptr->pid = diptr->id;
    niptr->p_offset = diptr->offset;
    niptr->creationTime = now;
    niptr->modificationTime = now;
    strncpy(niptr->name, name, name_len);
    // 更新父目录inode的文件数量
    diptr->size++;
    // 获取空闲的item项的位置
    char from_link;
    ushort *block_id_offset_ptr = find_free_dir_list_addr(diptr, &from_link);
    // 将inode信息添加至当前目录的空闲item项
    block_id_offset_ptr[0] = niptr->id;
    block_id_offset_ptr[1] = niptr->offset;
    // 若item位置在link处，更新item所在的列表块信息
    ushort *item_ptr;
    if (from_link) {
        item_ptr = (ushort *)addr_to_block_start_addr((char *)block_id_offset_ptr);
        item_ptr += (BLOCKID_OFFSET_NUM_PER_BLOCK - 1) << 1;
        item_ptr[1] += 1;
    }
    // 更新inode表的最后项信息
    set_bit_of_inode_list((inode *)block_id_to_addr(niptr->id), niptr->offset, 1);
    return 0;
}
// 判断文件(夹)名是否存在于某个inode节点内
int name_is_exists (inode *diptr, const char *name) {
    ushort *list = diptr->list;
    ushort *block_id_offset_last_item_ptr, *block_id_offset_ptr = list;
    ushort block_id_of_item, offset_of_item;
    uint inode_count = 0, inode_num = diptr->size;
    inode *iptr;
    // 尝试在list内寻找inode
    for (int i = 0; i < BLOCKID_OFFSET_IN_LIST_LEN && inode_count < inode_num; i++) {
        block_id_of_item = block_id_offset_ptr[0];
        if (block_id_of_item) {
            inode_count++;
            offset_of_item = block_id_offset_ptr[1];
            iptr = block_id_offset_to_inode(block_id_of_item, offset_of_item);
            if (strcmp(iptr->name, name) == 0) return 1;
        }
        block_id_offset_ptr += 2;
    }
    // 尝试通过link寻找inode
    ushort next_list_block_id, used_item_count, used_item_num;
    if (inode_count < inode_num) {
        // 有剩余inode未检测
        block_id_offset_ptr = (ushort *)block_id_to_addr(diptr->link);
        block_id_offset_last_item_ptr = block_id_offset_ptr + ((BLOCKID_OFFSET_NUM_PER_BLOCK - 1) << 1);
        while (1) {
            // 在每个块只检查used_item_num个有效item即可
            next_list_block_id = block_id_offset_last_item_ptr[0];
            used_item_num = block_id_offset_last_item_ptr[1];
            used_item_count = 0;
            for (int i = 0; i < BLOCKID_OFFSET_NUM_PER_BLOCK - 1 && used_item_count < used_item_num; i++) {
                block_id_of_item = block_id_offset_ptr[0];
                if (block_id_of_item) {
                    used_item_count++;
                    offset_of_item = block_id_offset_ptr[1];
                    iptr = block_id_offset_to_inode(block_id_of_item, offset_of_item);
                    if (strcmp(iptr->name, name) == 0) return 1;
                }
            }
            // 检查是否有下一个列表块
            if (next_list_block_id) {
                // 更新item指针，后重新循环
                block_id_offset_ptr = (ushort *)block_id_to_addr(next_list_block_id);
                block_id_offset_last_item_ptr = block_id_offset_ptr + ((BLOCKID_OFFSET_NUM_PER_BLOCK - 1) << 1);
            } else {
                break;
            }
        }
    }
    // 查不到该inode，返回NULL
    return 0;
}
// 处理多层路径，status为-1表示路径不合法，为-2表示上层目录树不存在，为0且返回指针为NULL表示不是多级目录，反之返回指向最后一个路径节点的名称指针
char * process_multi_dir (inode *start_iptr, const char *path, int *status) {
    char *item_name = get_last_dir(path);
    inode *diptr;
    *status = 0;
    // 检查是否存在多级目录
    if (item_name) {
        // 存在多级目录，检查路径是否合法
        if (!*item_name) {
            *status = -1;
            return NULL;
        }
        // 判断上层目录树是否存在
        item_name[-1] = '\0';
        diptr = path_to_inode(start_iptr, path, 0);
        if (!diptr) {
            *status = -2;
            return NULL;
        }
        return item_name;
    } else {
        // 多级目录不存在，不处理
        *status = 0;
        return NULL;
    }
}
// 修改inode的所在目录
void change_inode_dir (inode *iptr, inode *diptr) {
    // 移除在原目录的记录
    inode *piptr = block_id_offset_to_inode(iptr->pid, iptr->p_offset);
    ushort *item_ptr = find_block_id_offset_item(piptr, iptr->id, iptr->offset);
    item_ptr[0] = item_ptr[1] = 0;
    piptr->size--;
    // 修改指向为新的目录
    iptr->pid = diptr->id;
    iptr->p_offset = diptr->offset;
    // 增加信息至新目录中
    diptr->size++;
    char from_link;
    item_ptr = find_free_dir_list_addr(diptr, &from_link);
    item_ptr[0] = iptr->id;
    item_ptr[1] = iptr->offset;
    // 更新item表尾信息
    if (from_link) {
        item_ptr = (ushort *)addr_to_block_start_addr((char *)item_ptr);
        item_ptr += (BLOCKID_OFFSET_NUM_PER_BLOCK - 1) << 1;
        item_ptr[1] += 1;
    }
}
// 获取路径的最后一个文件(夹)名，若不是多级目录返回NULL
char * get_last_dir (const char *path) {
    char *delim = strrchr(path, '/');
    if (!delim) return NULL;
    return delim + 1;
}
// 获取当前工作目录
inode * get_current_dir () {
    paths *cp = curpaths;
    while (cp->next) cp = cp->next;
    return cp->node;
}
// 获取inode的路径
void get_inode_path (inode *iptr, char *buffer, char delim) {
    char *buf = buffer;
    paths *t, *p = (paths *)malloc(sizeof(paths));
    int len;
    p->node = iptr;
    p->prev = p->next = NULL;
    while (iptr->pid) {
        iptr = block_id_offset_to_inode(iptr->pid, iptr->p_offset);
        t = (paths *)malloc(sizeof(paths));
        t->node = iptr;
        t->prev = NULL;
        t->next = p;
        p = t;
    }
    while (p) {
        len = strlen(p->node->name);
        if (len) {
            strncpy(buf, p->node->name, len);
            buf += len;
        }
        *buf++ = delim;
        p = p->next;
    }
    buf[-1] = '\0';
}
// 计算操作(r/w)特定的inode对应的信号量的key写入buffer
void get_inode_semkey (inode *iptr, char *buffer, char op) {
    *buffer++ = op;
    get_inode_path(iptr, buffer, '~');
}
// 输出提示
void prompt () {
    paths *cp = curpaths;
    do {
        printf("\e[33m%s/", cp->node->name);
        cp = cp->next;
    } while (cp);
    printf("\e[0m: $ ");
}

/* 读写文件相关函数 */
// 读inode文件
void read_file (inode *iptr) {
    uint file_size = iptr->size;
    ushort block_id;
    int block_num = file_size / BLOCK_SIZE + 1;
    int i;
    char *bptr, buffer[BLOCK_SIZE] = { 0 };
    printf("\n");
    // 读取list的块
    for (i = 0; i < block_num && i < BLOCKID_IN_LIST_LEN; i++) {
        block_id = iptr->list[i];
        bptr = (char *)block_id_to_addr(block_id);
        memcpy(buffer, bptr, BLOCK_SIZE);
        printf("%s", buffer);
    }
    // 检查是否还有link中的块
    ushort *bidptr;
    if (i < block_num) {
        block_num -= BLOCKID_IN_LIST_LEN;
        block_id = iptr->link;
        bidptr = (ushort *)block_id_to_addr(block_id);
        // 开始读取link所在块表的块
        for (i = 0; i < block_num && i < BLOCKID_NUM_PER_BLOCK; i++) {
            block_id = bidptr[i];
            bptr = (char *)block_id_to_addr(block_id);
            memcpy(buffer, bptr, BLOCK_SIZE);
            printf("%s", buffer);
        }
    }
}
// 编辑inode文件
void edit_file (inode *iptr) {
    pid_t pid;
    char tmp_file_path[300] = { 0 }, file_path[256] = { 0 };
    char command[512] = { 0 };
    // 创建临时文件夹
    system("[ -d .tmp ] || mkdir .tmp");
    // 计算临时文件路径
    get_inode_path(iptr, file_path, '~');
    sprintf(tmp_file_path, "./.tmp/.%s", file_path);
    // 创建临时文件
    sprintf(command, "touch %s", tmp_file_path);
    system(command);
    // 创建子进程
    pid = fork();
    if (pid < 0) {
        printf("\e[0;31mError: an unexpected error occurred while creating a child process.\e[0m\n");
    } else if (pid == 0) {
        // 子进程，先将inode文件的内容复制到临时文件
        copy_inode_to_content(iptr, tmp_file_path);
        // 调用gedit
        sprintf(command, "gedit %s", tmp_file_path);
        exit(system(command));
    } else {
        // 父进程，等待子进程
        wait(NULL);
        // 复制临时文件内容到inode文件
        clear_file_content(iptr);
        copy_content_to_inode(tmp_file_path, iptr);
        sprintf(command, "rm %s", tmp_file_path);
        system(command);
        // 更新修改时间
        time_t now;
        time(&now);
        iptr->modificationTime = now;
    }
}
// 清除inode文件内容
void clear_file_content (inode *iptr) {
    uint file_size = iptr->size;
    ushort block_id;
    int block_num = file_size / BLOCK_SIZE + 1;
    int i;
    // 清空list的块
    for (i = 0; i < block_num && i < BLOCKID_IN_LIST_LEN; i++) {
        block_id = iptr->list[i];
        free_block(block_id);
    }
    // 检查是否还有link中的块
    ushort *bidptr;
    if (i < block_num) {
        block_num -= BLOCKID_IN_LIST_LEN;
        block_id = iptr->link;
        bidptr = (ushort *)block_id_to_addr(block_id);
        // 开始清空link所在块表的块
        for (i = 0; i < block_num && i < BLOCKID_NUM_PER_BLOCK; i++) {
            block_id = bidptr[i];
            free_block(block_id);
        }
        // 清空link指向的块表
        block_id = iptr->link;
        free_block(block_id);
    }
}
// 将inode文件的内容复制到临时文件
void copy_inode_to_content (inode *iptr, const char *tmp_file_path) {
    uint file_size = iptr->size;
    ushort block_id;
    int block_num = file_size / BLOCK_SIZE;
    int rest_byte = file_size % BLOCK_SIZE;
    int i;
    char *bptr, buffer[BLOCK_SIZE] = { 0 };
    FILE *fp = fopen(tmp_file_path, "wb+");
    if (fp == NULL) {
        printf("\e[0;31mError: an unexpected error occurred while loading file content.\e[0m\n");
        return;
    }
    // 复制list的块
    for (i = 0; i < block_num && i < BLOCKID_IN_LIST_LEN; i++) {
        block_id = iptr->list[i];
        bptr = (char *)block_id_to_addr(block_id);
        memcpy(buffer, bptr, BLOCK_SIZE);
        fwrite(buffer, 1, BLOCK_SIZE, fp);
    }
    block_id = iptr->list[i];
    bptr = (char *)block_id_to_addr(block_id);
    // 检查是否还有link中的块
    ushort *bidptr;
    if (i < block_num) {
        // link中还存在块
        block_num -= BLOCKID_IN_LIST_LEN;
        block_id = iptr->link;
        bidptr = (ushort *)block_id_to_addr(block_id);
        // 开始复制link所在块表的块
        for (i = 0; i < block_num && i < BLOCKID_NUM_PER_BLOCK; i++) {
            block_id = bidptr[i];
            bptr = (char *)block_id_to_addr(block_id);
            memcpy(buffer, bptr, BLOCK_SIZE);
            fwrite(buffer, 1, BLOCK_SIZE, fp);
        }
        block_id = bidptr[i];
        bptr = (char *)block_id_to_addr(block_id);
    }
    memcpy(buffer, bptr, rest_byte);
    fwrite(buffer, 1, rest_byte, fp);
    // 关闭文件
    fclose(fp);
}
// 复制临时文件内容到inode文件
void copy_content_to_inode (const char *tmp_file_path, inode *iptr) {
    FILE *fp = fopen(tmp_file_path, "rb+");
    // 检查文件指针
    if (fp == NULL) {
        printf("\e[0;31mError: an unexpected error occurred while saving file content.\e[0m\n");
        return;
    }
    char buffer[BLOCK_SIZE];
    // 获取文件大小
    fseek(fp, 0, SEEK_END);
    uint file_size = (uint)ftell(fp);
    fseek(fp, 0, SEEK_SET);
    // 复制文件
    int block_num = file_size / BLOCK_SIZE;
    int rest_byte = file_size % BLOCK_SIZE;
    int i;
    ushort block_id, offset;
    char *bptr;
    // 先复制最多前20个块内容
    for (i = 0; i < block_num && i < BLOCKID_IN_LIST_LEN; i++) {
        block_id = find_free_block_id();
        set_bit_of_bitmap(block_id, 1);
        iptr->list[i] = block_id;
        bptr = block_id_to_addr(block_id);
        fread(buffer, 1, BLOCK_SIZE, fp);
        memcpy(bptr, buffer, BLOCK_SIZE);
    }
    // 若有剩余块，通过链接获得地址
    ushort *bidptr;
    int has_rest = 0;
    if (i < block_num) {
        // 计数减少20个
        block_num -= BLOCKID_IN_LIST_LEN;
        // 获取链接
        block_id = find_free_block_id();
        set_bit_of_bitmap(block_id, 1);
        iptr->link = block_id;
        bidptr = (ushort *)block_id_to_addr(block_id);
        // 开始写入
        for (i = 0; i < block_num && i < BLOCKID_NUM_PER_BLOCK - 1; i++) {
            block_id = find_free_block_id();
            set_bit_of_bitmap(block_id, 1);
            bidptr[i] = block_id;
            bptr = block_id_to_addr(block_id);
            fread(buffer, 1, BLOCK_SIZE, fp);
            memcpy(bptr, buffer, BLOCK_SIZE);
        }
        // 检查是否全部写入
        if (i < block_num) {
            printf("\e[0;31mError: the file is too large to save completely.\e[0m\n");
            has_rest = 1;
        }
        // 将剩余不足一块的空间进行分配
        if (!has_rest) {
            block_id = find_free_block_id();
            set_bit_of_bitmap(block_id, 1);
            bidptr[i] = block_id;
            bptr = block_id_to_addr(block_id);
        }
    } else {
        // 整数块小于等于20
        if (i == BLOCKID_IN_LIST_LEN) {
            // 刚好用完list的块位置，先分类list的块
            block_id = find_free_block_id();
            set_bit_of_bitmap(block_id, 1);
            iptr->link = block_id;
            bidptr = (ushort *)block_id_to_addr(block_id);
            // 给块表的第一个分配一个block_id
            block_id = find_free_block_id();
            set_bit_of_bitmap(block_id, 1);
            bidptr[0] = block_id;
            bptr = block_id_to_addr(block_id);
        } else {
            // 还有剩余的list的块位置
            block_id = find_free_block_id();
            set_bit_of_bitmap(block_id, 1);
            iptr->list[i] = block_id;
            bptr = block_id_to_addr(block_id);
        }
    }
    // bptr已经初始化，写入剩余内容
    if (!has_rest) {
        fread(buffer, 1, rest_byte, fp);
        memcpy(bptr, buffer, rest_byte);
    }
    // 写入文件大小
    iptr->size = file_size;
    if (has_rest) {
        iptr->size = (BLOCKID_IN_LIST_LEN + BLOCKID_NUM_PER_BLOCK) * BLOCK_SIZE;
    }
    // 关闭文件
    fclose(fp);
}

/* 命令相关函数 */
// 获取命令
int get_cmd () {
    int count = 0;
    char *token = NULL, *enter = NULL;
    const char *delim = " ";
    if (fgets(cmd, sizeof(cmd), stdin) == NULL) return count;
    enter = strchr(cmd, '\n');
    if (enter) *enter = '\0';
    if (strlen(cmd) == 0) return count;
    token = strtok(cmd, delim);
    while (token != NULL) {
        tks[count++] = token;
        token = strtok(NULL, delim);
    }
    tks[count] = NULL;
    return count;
}
// 分析命令
int analysis_cmd () {
    char *cmd_name, *path, *src, *dst;
    cmd_name = tks[0];
    src = path = tks[1];
    dst = tks[2];
    inode *diptr = get_current_dir();
    if (strcmp(cmd_name, "mkdir") == 0) exec_mkdir(diptr, path);
    else if (strcmp(cmd_name, "touch") == 0) exec_touch(diptr, path);
    else if (strcmp(cmd_name, "rmdir") == 0) exec_rmdir(diptr, path);
    else if (strcmp(cmd_name, "rm") == 0) exec_rm(diptr, path);
    else if (strcmp(cmd_name, "mv") == 0) exec_mv(diptr, src, dst);
    else if (strcmp(cmd_name, "cat") == 0) exec_cat(diptr, path);
    else if (strcmp(cmd_name, "gedit") == 0) exec_gedit(diptr, path);
    else if (strcmp(cmd_name, "cd") == 0) exec_cd(diptr, path);
    else if (strcmp(cmd_name, "ls") == 0) exec_ls(diptr);
    else if (strcmp(cmd_name, "exit") == 0) exec_exit();
    else exec_nocommand();
    return 0;
}

/* 执行相关函数 */
int exec_mkdir (inode *start_iptr, const char *path) {
    // 判断是否传入路径
    if (!path || strlen(path) == 0) {
        printf("\e[0;31mError: a directory name needs to be passed in.\e[0m\n");
        return -1;
    }
    // 处理多级目录，diptr和dir_name将被初始化
    int status;
    inode *diptr;
    char *dir_name = process_multi_dir(start_iptr, path, &status);
    if (status == -1) {
        printf("\e[0;31mError: path '%s' is illegal.\e[0m\n", path);
        return -1;
    } else if (status == -2) {
        printf("\e[0;31mError: no such directory '%s'.\e[0m\n", path);
        return -1;
    } else {
        if (dir_name) {
            diptr = path_to_inode(start_iptr, path, 0);
        } else {
            diptr = start_iptr;
            dir_name = (char *)path;
        }
    }
    // 检查是否有同名文件或文件夹
    if (name_is_exists(diptr, dir_name)) {
        printf("\e[0;31mError: a file or directory with the same name already exists.\e[0m\n");
        return -1;
    }
    return add_inode_to_dir(diptr, dir_name, 0);
}
int exec_touch (inode *start_iptr, const char *path) {
    // 判断是否传入路径
    if (!path || strlen(path) == 0) {
        printf("\e[0;31mError: a file name needs to be passed in.\e[0m\n");
        return -1;
    }
    // 处理多级目录，diptr和file_name将被初始化
    int status;
    inode *diptr;
    char *file_name = process_multi_dir(start_iptr, path, &status);
    if (status == -1) {
        printf("\e[0;31mError: path '%s' is illegal.\e[0m\n", path);
        return -1;
    } else if (status == -2) {
        printf("\e[0;31mError: no such directory '%s'.\e[0m\n", path);
        return -1;
    } else {
        if (file_name) {
            diptr = path_to_inode(start_iptr, path, 0);
        } else {
            diptr = start_iptr;
            file_name = (char *)path;
        }
    }
    // 检查是否有同名文件或文件夹
    if (name_is_exists(diptr, file_name)) {
        printf("\e[0;31mError: a file or directory with the same name already exists.\e[0m\n");
        return -1;
    }
    return add_inode_to_dir(diptr, file_name, 1);
}
int exec_rmdir (inode *start_iptr, const char *path) {
    // 检查是否传入目录名
    if (!path || strlen(path) == 0) {
        printf("\e[0;31mError: a directory name needs to be passed in.\e[0m\n");
        return -1;
    }
    // 获取目录的inode
    inode *iptr = path_to_inode(start_iptr, path, 0);
    // 目录不存在则退出
    if (iptr == NULL) {
        printf("\e[0;31mError: no such directory '%s'.\e[0m\n", path);
        return -1;
    }
    // 目录存在，检查是否为空
    if (iptr->size != 0) {
        printf("\e[0;31mError: the directory '%s' is not empty and cannot be removed.\e[0m\n", path);
        return -1;
    }
    // 可以安全移除文件夹
    ushort *list = iptr->list;
    ushort link = iptr->link;
    ushort *item_ptr;
    ushort block_id, offset;
    // 释放链接页的空间
    while (link) {
        item_ptr = (ushort *)block_id_to_addr(link);
        item_ptr += (BLOCKID_OFFSET_NUM_PER_BLOCK - 1) << 1;
        block_id = item_ptr[0];
        offset = item_ptr[1];
        free_block(link);
        link = block_id;
    }
    // 删除父目录的inode信息，并更新其所在inode列表的信息
    free_inode_item(iptr);
    return 0;
}
int exec_rm (inode *start_iptr, const char *path) {
    // 检查是否传入文件名
    if (!path || strlen(path) == 0) {
        printf("\e[0;31mError: a file name needs to be passed in.\e[0m\n");
        return -1;
    }
    // 处理多级目录，diptr和file_name将被初始化
    int status;
    inode *diptr;
    char *file_name = process_multi_dir(start_iptr, path, &status);
    if (status == -1) {
        printf("\e[0;31mError: path '%s' is illegal.\e[0m\n", path);
        return -1;
    } else if (status == -2) {
        printf("\e[0;31mError: no such directory '%s'.\e[0m\n", path);
        return -1;
    } else {
        if (file_name) {
            diptr = path_to_inode(start_iptr, path, 0);
        } else {
            diptr = start_iptr;
            file_name = (char *)path;
        }
    }
    // 搜索文件
    inode *iptr = find_dir_inode(diptr, file_name, 1);
    // 文件不存在则退出
    if (iptr == NULL) {
        printf("\e[0;31mError: no such file '%s'.\e[0m\n", file_name);
        return -1;
    }
    // 移除文件
    ushort *list = iptr->list;
    ushort link = iptr->link;
    ushort *item_ptr;
    ushort block_id;
    // 释放链接页的空间
    while (link) {
        item_ptr = (ushort *)block_id_to_addr(link);
        item_ptr += BLOCKID_NUM_PER_BLOCK - 1;
        block_id = item_ptr[0];
        free_block(link);
        link = block_id;
    }
    // 删除父目录的inode信息，自减文件数量，并更新其所在inode列表的信息
    free_inode_item(iptr);
    return 0;
}
int exec_mv (inode *start_iptr, const char *src, const char *dst) {
    // 解析src，src必须存在
    inode *siptr = path_to_inode(start_iptr, src, 0);
    if (!siptr) siptr = path_to_inode(start_iptr, src, 1);
    if (!siptr) {
        printf("\e[0;31mError: no such file or directory '%s'.\e[0m\n", src);
        return -1;
    }
    // 解析dst，dst可以不存在
    inode *diptr = path_to_inode(start_iptr, dst, 0);
    if (!diptr) diptr = path_to_inode(start_iptr, dst, 1);
    // 预先进行上层目录的存在性检查
    char *item_name = NULL;
    int status;
    inode *uiptr = NULL;
    if (!diptr) {
        item_name = process_multi_dir(start_iptr, dst, &status);
        if (status == -2) {
            // 不存在上层目录
            printf("\e[0;31mError: no such directory '%s'.\e[0m\n", dst);
            return -1;
        } else {
            // 存在上层目录，存为inode
            if (item_name) {
                // 传入的是多级路径
                item_name[-1] = '\0';
                uiptr = path_to_inode(start_iptr, dst, 0);
                item_name[-1] = '/';
            } else {
                // 传入的不是多级路径
                uiptr = get_current_dir();
                item_name = (char *)dst;
            }
        }
    }
    // 获取时间
    time_t now;
    time(&now);
    // 分类讨论
    if (siptr->type == 0) {
        // src为目录
        if (!diptr) {
            // dst不存在，上层目录存在，修改指向关系，并重命名
            change_inode_dir(siptr, uiptr);
            strncpy(siptr->name, item_name, INODE_NAME_LEN);
            siptr->modificationTime = now;
        } else if (diptr->type != 0) {
            // dst存在，但不是目录
            printf("\e[0;31mError: '%s' is a file, not a directory.\e[0m\n", dst);
            return -1;
        } else if (siptr == diptr) {
            // dst存在，且是目录，但是与源路径相同
            printf("\e[0;31mError: cannot move a directory to its own subdirectory.\e[0m\n");
            return -1;
        } else {
            // dst存在，且是目录，修改指向关系
            change_inode_dir(siptr, diptr);
            siptr->modificationTime = now;
        }
    } else {
        // src为文件
        if (!diptr) {
            // dst不存在，上层目录存在，dst末尾名称解析为文件名，修改src相关链接信息
            change_inode_dir(siptr, uiptr);
            strncpy(siptr->name, item_name, INODE_NAME_LEN);
            siptr->modificationTime = now;
        } else if (diptr->type == 0) {
            // dst存在，且为目录，直接修改src相关链接信息
            change_inode_dir(siptr, diptr);
            siptr->modificationTime = now;
        } else if (siptr == diptr) {
            // dst存在，但为文件，且与src相同，退出
            printf("\e[0;31mError: Error: cannot operate on the same file '%s'.\e[0m\n", dst);
            return -1;
        } else {
            // dst存在，但为文件，需要释放dst文件，并修改src相关链接信息
            exec_rm(uiptr, item_name);
            change_inode_dir(siptr, uiptr);
            siptr->modificationTime = now;
        }
    }
    return 0;
}
int exec_cat (inode *start_iptr, const char *path) {
    // 检查是否传入文件名
    if (!path || strlen(path) == 0) {
        printf("\e[0;31mError: a file name needs to be passed in.\e[0m\n");
        return -1;
    }
    // 处理多级目录，diptr和file_name将被初始化
    int status;
    inode *diptr;
    char *file_name = process_multi_dir(start_iptr, path, &status);
    if (status == -1) {
        printf("\e[0;31mError: path '%s' is illegal.\e[0m\n", path);
        return -1;
    } else if (status == -2) {
        printf("\e[0;31mError: no such directory '%s'.\e[0m\n", path);
        return -1;
    } else {
        if (file_name) {
            diptr = path_to_inode(start_iptr, path, 0);
        } else {
            diptr = start_iptr;
            file_name = (char *)path;
        }
    }
    // 搜索文件
    inode *iptr = find_dir_inode(diptr, file_name, 1);
    // 文件不存在则退出
    if (iptr == NULL) {
        printf("\e[0;31mError: no such file '%s'.\e[0m\n", file_name);
        return -1;
    }
    // 获取信号量
    char rdkey[256] = { 0 }, wrkey[256] = { 0 };
    get_inode_semkey(iptr, rdkey, 'r');
    get_inode_semkey(iptr, wrkey, 'w');
    sem_t *rd_sem = sem_open(rdkey, O_CREAT, 0666, 1);
    sem_t *wr_sem = sem_open(wrkey, O_CREAT, 0666, 1);
    // 读
    sem_wait(rd_sem);
    int sem_stat = sem_trywait(wr_sem);
    sem_post(rd_sem);
    read_file(iptr);
    if (sem_stat != -1) sem_post(wr_sem);
    printf("\n");
    return 0;
}
int exec_gedit (inode *start_iptr, const char *path) {
    // 检查是否传入文件名
    if (!path || strlen(path) == 0) {
        printf("\e[0;31mError: a file name needs to be passed in.\e[0m\n");
        return -1;
    }
    // 处理多级目录，diptr和file_name将被初始化
    int status;
    inode *diptr;
    char *file_name = process_multi_dir(start_iptr, path, &status);
    if (status == -1) {
        printf("\e[0;31mError: path '%s' is illegal.\e[0m\n", path);
        return -1;
    } else if (status == -2) {
        printf("\e[0;31mError: no such directory '%s'.\e[0m\n", path);
        return -1;
    } else {
        if (file_name) {
            diptr = path_to_inode(start_iptr, path, 0);
        } else {
            diptr = start_iptr;
            file_name = (char *)path;
        }
    }
    // 搜索文件
    inode *iptr = find_dir_inode(diptr, file_name, 1);
    // 文件不存在则创建空文件
    if (iptr == NULL) {
        exec_touch(diptr, file_name);
        iptr = find_dir_inode(diptr, file_name, 1);
    }
    // 获取信号量
    char rdkey[256] = { 0 }, wrkey[256] = { 0 };
    get_inode_semkey(iptr, rdkey, 'r');
    get_inode_semkey(iptr, wrkey, 'w');
    sem_t *rd_sem = sem_open(rdkey, O_CREAT, 0666, 1);
    sem_t *wr_sem = sem_open(wrkey, O_CREAT, 0666, 1);
    int sem_stat;
    // 写
    while (1) {
        sem_wait(wr_sem);
        sem_stat = sem_trywait(rd_sem);
        if (sem_stat == -1) {
            sem_post(wr_sem);
            continue;
        }
        break;
    }
    edit_file(iptr);
    sem_post(rd_sem);
    sem_post(wr_sem);
    return 0;
}
int exec_cd (inode *start_iptr, const char *path) {
    // 检查是否传入路径
    if (!path || strlen(path) == 0) {
        printf("\e[0;31mError: a path needs to be passed in.\e[0m\n");
        return -1;
    }
    // 解析路径为目录inode
    inode *diptr = path_to_inode(start_iptr, path, 0);
    // 若找不到目录inode，退出
    if (!diptr) {
        printf("\e[0;31mError: no such directory '%s'.\e[0m\n", path);
        return -1;
    }
    // 释放原链表
    paths *pnode;
    while (curpaths) {
        pnode = curpaths->next;
        free(curpaths);
        curpaths = pnode;
    }
    // 创建新链表
    paths *ndptr;
    curpaths = (paths *)malloc(sizeof(paths));
    curpaths->node = diptr;
    curpaths->prev = curpaths->next = NULL;
    while (diptr->pid) {
        diptr = block_id_offset_to_inode(diptr->pid, diptr->p_offset);
        ndptr = (paths *)malloc(sizeof(paths));
        ndptr->node = diptr;
        ndptr->next = curpaths;
        ndptr->prev = NULL;
        curpaths->prev = ndptr;
        curpaths = ndptr;
    }
    return 0;
}
int exec_ls (inode *start_iptr) {
    inode *diptr = start_iptr;
    inode *uiptr;
    ushort *block_id_offset_ptr = diptr->list;
    ushort link = diptr->link;
    ushort block_id, offset;
    char buffer[20];
    struct tm *timeinfo;
    // statistics info
    printf("all item(s) count: %u\n\n", diptr->size);
    printf("  permission    size                    time    name\n");
    // details of list
    for (int i = 0; i < BLOCKID_OFFSET_IN_LIST_LEN; i++) {
        block_id = block_id_offset_ptr[0];
        offset = block_id_offset_ptr[1];
        if (block_id) {
            uiptr = block_id_offset_to_inode(block_id, offset);
            printf("         ");
            printf("%c", uiptr->type ? '-' : 'd');
            printf("%c", (uiptr->permission & 0x1) ? 'r' : '-');
            printf("%c", (uiptr->permission & 0x2) ? 'w' : '-');
            uiptr->type ? printf("%8u", uiptr->size) : printf("        ");
            strftime(buffer, sizeof(buffer), "%F %T", localtime(&(uiptr->modificationTime)));
            printf("%24s", buffer);
            printf("    ");
            uiptr->type ? printf("%s\n", uiptr->name) : printf("\e[0;34m%s\e[0m\n", uiptr->name);
        }
        block_id_offset_ptr += 2;
    }
    printf("\n");
    return 0;
}
int exec_exit () {
    save_shm();
    exit(0);
}
int exec_nocommand () {
    printf("\e[0;31mError: '%s': no this command.\e[0m\n", tks[0]);
    return 0;
}

/* 调试用函数 */
void __disp_cmd() {
    int i = 0;
    while (tks[i]) printf("[%s] ", tks[i++]);
    printf("\n");
}
void __disp_cp () {
    paths *p = curpaths;
    while (p) {
        printf("[%s]->", p->node->name);
        p = p->next;
    }
    printf("\n");
}

/* 存取相关函数 */
// 加载数据至共享内存
void load_shm () {
    int count;
    char src[BLOCK_SIZE];
    char *dst = shmptr;
    FILE *fp = fopen(bkp, "r");
    for (int i = 0; i < BLOCK_NUM; i++) {
        count = fread(src, 1, BLOCK_SIZE, fp);
        if (count != BLOCK_SIZE) {
            fclose(fp);
            printf("\e[0;31mError: read error.\e[0m\n");
            exit(-1);
        }
        memcpy(dst, src, BLOCK_SIZE);
        dst += BLOCK_SIZE;
    }
    fclose(fp);
}
// 保存共享内存的数据
void save_shm () {
    int count;
    char *dst = shmptr;
    FILE *fp = fopen(bkp, "w+");
    for (int i = 0; i < BLOCK_NUM; i++) {
        count = fwrite(dst, 1, BLOCK_SIZE, fp);
        if (count != BLOCK_SIZE) {
            fclose(bk);
            printf("\e[0;31mError: write error.\e[0m\n");
            exit(-1);
        }
        dst += BLOCK_SIZE;
    }
    fclose(fp);
}

/* 主函数 */
int main () {
    main_init();
    while (1) {
        prompt();
        if (get_cmd()) analysis_cmd();
    }
    main_done();
    return 0;
}
