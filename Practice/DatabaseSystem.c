#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <io.h>
#include <direct.h>

// constant definition
#define true 1
#define false 0
#define data_t int
#define option_t int
#define bool unsigned int
typedef char string[256];
string rootDir = ".";
string databaseDirName = "database";
string wildcard = "*";
string currentDatabase = {'\0'};

// structure definition 
typedef struct {
    string      tableName;      // 表的名称
    int         itemCount;      // 表的条目的数量
    int         attrCount;      // 表的属性的数量
    string      *attrName;      // 表的属性名的列表
    data_t      **attrValue;    // 表的所有条目的所有属性值
} Table;
typedef struct {
    size_t fileCount;   // 文件数量
    string *fileList;   // 文件列表篇
} Dir;

// funciton declaration
Table *loadTable(string tableName);
bool saveTable(Table *table);
int findAttribute(Table *table, string attrName);
int *filterItem(Table *table, string attrName, data_t value, int *itemCount);
bool existsFile(Dir *dir, string fileName);
void freeTable(Table *table);

bool createDatabase(string databaseName);
bool deleteDatabase(string databaseName);

bool createTable(string tableName, string *attrNameList, int attrNameCount);
bool deleteTable(string tableName);

bool addAttribute(Table *table, string attrName, data_t value);
bool dropAttribute(Table *table, string attrName);

bool insertItem(Table *table, data_t *valueList);
size_t deleteItem(Table *table, int *indexList, int indexCount);
size_t updateItem(Table *table, int *indexList, int indexCount, string attrName, data_t value);
size_t selectItem(Table *table, int *indexList, int indexCount, string *attrNameList, int attrNameCount);

int *range(int start, int stop);
bool inRange(option_t option, option_t leftEnd, option_t rightEnd);
Dir *getDir(string dirPath);
void freeDir(Dir *dir);
bool removeDir(string dirPath);

// function definition
/**
 * @brief   加载给定的数据库的给定的表
 * @param   tableName 表名
 * @return  加载数据表成功返回 Table 对象指针，若加载数据表失败，返回 NULL 
 */
Table* loadTable(string tableName) {
    int i;
    string databasePath, tablePath;
    sprintf(databasePath, "%s/%s/%s", rootDir, databaseDirName, currentDatabase);
    sprintf(tablePath, "%s/%s", databasePath, tableName);
    FILE *tableFile = NULL;
    Table *table = NULL;
    if (access(databasePath, F_OK) || access(tablePath, F_OK))
        return NULL;
    table = (Table *)calloc(1, sizeof(Table));
    tableFile = fopen(tablePath, "rb");
    fread(table->tableName, sizeof(string), 1, tableFile);
    fread(&(table->itemCount), sizeof(table->itemCount), 1, tableFile);
    fread(&(table->attrCount), sizeof(table->attrCount), 1, tableFile);
    if (table->attrCount > 0) {
        table->attrName = (string *)malloc(table->attrCount * sizeof(string));
        fread(table->attrName, sizeof(string), table->attrCount, tableFile);
        if (table->itemCount > 0) {
            table->attrValue = (data_t **)malloc(table->itemCount * sizeof(data_t *));
            for (i = 0; i < table->itemCount; ++i) {
                table->attrValue[i] = (data_t *)malloc(table->attrCount * sizeof(data_t));
                fread(table->attrValue[i], sizeof(data_t), table->attrCount, tableFile);
            }
        }
    }
    fclose(tableFile);
    return table;
}
/**
 * @brief   保存给定的数据表到给定的数据库
 * @param   databaseName 表所在的数据库名
 * @param   table 表指针对象
 * @return  保存数据表成功返回 true ，若指针为 NULL 或保存数据表失败，返回 false 
 */
bool saveTable(Table *table) {
    int i;
    string databasePath, tablePath;
    sprintf(databasePath, "%s/%s/%s", rootDir, databaseDirName, currentDatabase);
    sprintf(tablePath, "%s/%s", databasePath, table->tableName);
    FILE *tableFile = NULL;
    if (access(databasePath, F_OK))
        return false;
    tableFile = fopen(tablePath, "wb");
    fwrite(table->tableName, sizeof(string), 1, tableFile);
    fwrite(&(table->itemCount), sizeof(table->itemCount), 1, tableFile);
    fwrite(&(table->attrCount), sizeof(table->attrCount), 1, tableFile);
    fwrite(table->attrName, sizeof(string), table->attrCount, tableFile);
    for (i = 0; i < table->itemCount; ++i)
        fwrite(table->attrValue[i],  sizeof(data_t), table->attrCount, tableFile);
    fclose(tableFile);
    return true;
}
/**
 * @brief   搜索给定的表的给定的属性的索引
 * @param   table 表对象指针
 * @param   attrName 需要搜索的属性名
 * @return  若表指针不为 NULL 且给定的属性名存在于给定的表内，返回属性的索引 ，否则返回 -1 
 */
int findAttribute(Table *table, string attrName) {
    int i;
    for (i = 0; i < table->attrCount; ++i)
        if (!strcmp(table->attrName[i], attrName))
            return i;
    return -1;
}
/**
 * @brief   通过属性筛选表中的条目，可使用通配符
 * @param   table 表对象指针
 * @param   attrName 需要筛选的属性属性名
 * @param   value 需要筛选的属性名对应的属性值
 * @param   itemCountDst 写入筛选得到的条目的数量的地址
 * @return  经过筛选得到的给定表的所有条目的索引的列表（使用完毕需要 free ），若指针为 NULL 或属性不存在或筛选结果数量为 0 ，返回 NULL
 */
int *filterItem(Table *table, string attrName, data_t value, int *indexCountDst) {
    int i, j, count = 0, attrIndex = -1;
    int *tmpIndexList = NULL, *indexList = NULL;
    if (!strcmp(attrName, "*")) {
        *indexCountDst = table->itemCount;
        indexList = range(0, table->itemCount);
        return indexList;
    }
    for (i = 0; i < table->attrCount; ++i) {
        if (!strcmp(table->attrName[i], attrName)) {
            attrIndex = i;
            break;
        }
    }
    if (attrIndex < 0) {
        *indexCountDst = 0;
        return NULL;
    }
    tmpIndexList = (int *)malloc(table->itemCount * sizeof(int));
    for (i = 0; i < table->itemCount; ++i) {
        if (table->attrValue[i][attrIndex] == value) {
            tmpIndexList[count++] = i;
        }
    }
    if (!count) {
        free(tmpIndexList);
        *indexCountDst = 0;
    } else {
        indexList = (int *)malloc(count * sizeof(int));
        memcpy(indexList, tmpIndexList, count * sizeof(int));
        *indexCountDst = count;
        free(tmpIndexList);
    }
    return indexList;
}
/**
 * @brief   判断文件夹内是否存在指定文件
 * @param   dir 指定文件夹的 Dir 结构体
 * @return  若文件存在 Dir 对应的文件夹返回 true ，否则返回 false
 */
bool existsFile(Dir *dir, string fileName) {
    int i;
    for (i = 0; i < dir->fileCount; ++i)
        if (!strcmp(dir->fileList[i], fileName))
            return true;
    return false;
}
/**
 * @brief   释放 Table 结构体指针的内存
 * @param   table 表指针对象
 * @return  无返回值
 */ 
void freeTable(Table *table) {
    int i;
    if (table) {
        free(table->attrName);
        for (i = 0; i < table->itemCount; ++i)
            free(table->attrValue[i]);
        free(table->attrValue);
        free(table);
    }
}
/**
 * @brief   创建空的数据库
 * @param   databaseName 需要创建的数据库名
 * @return  创建成功返回 true，若该数据库名已经存在，则返回 false 
 */
bool createDatabase(string databaseName) {
    string databasePath;
    sprintf(databasePath, "%s/%s/%s", rootDir, databaseDirName, databaseName);
    if (!access(databasePath, F_OK))
        return false;
    mkdir(databasePath);
    return true;
}
/**
 * @brief   删除已存在的数据库
 * @param   databaseName 需要删除的数据库名
 * @return  删除成功返回 true，若该数据库名不存在，则返回 false 
 */
bool deleteDatabase(string databaseName) {
    string databasePath;
    sprintf(databasePath, "%s/%s/%s", rootDir, databaseDirName, databaseName);
    removeDir(databasePath);
    return true;
}
/**
 * @brief   在给定的数据库创建空的表
 * @param   tableName 表名
 * @return  创建成功返回 true ，若在给定的数据库中该表名已经存在，则创建数据表失败，返回 false 
 */
bool createTable(string tableName, string *attrNameList, int attrNameCount) {
    string databasePath, tablePath;
    sprintf(databasePath, "%s/%s/%s", rootDir, databaseDirName, currentDatabase);
    sprintf(tablePath, "%s/%s", databasePath, tableName);
    Table *table;
    FILE *tableFile = NULL;
    if (access(databasePath, F_OK) || !access(tablePath, F_OK))
        return false;
    table = (Table *)calloc(1, sizeof(Table));
    strcpy(table->tableName, tableName);
    table->attrCount = attrNameCount;
    tableFile = fopen(tablePath, "wb");
    fwrite(table->tableName, 1, sizeof(table->tableName), tableFile);
    fwrite(&(table->itemCount), 1, sizeof(table->itemCount), tableFile);
    fwrite(&(table->attrCount), 1, sizeof(table->attrCount), tableFile);
    fwrite(attrNameList, sizeof(string), attrNameCount, tableFile);
    fclose(tableFile);
    free(table);
    return true;
}
/**
 * @brief   删除给定的数据库中的给定的表
 * @param   tableName 表名
 * @return  删除数据表成功返回 true ，若在给定的数据库中该表名不存在，则删除数据表失败，返回 false
 */
bool deleteTable(string tableName) {
    string databasePath, tablePath;
    sprintf(databasePath, "%s/%s/%s", rootDir, databaseDirName, currentDatabase);
    sprintf(tablePath, "%s/%s", databasePath, tableName);
    if (access(databasePath, F_OK) || access(tablePath, F_OK))
        return false;
    if (!remove(tablePath))
        return true;
    return false;
}
/**
 * @brief   增加数据表的属性
 * @param   table 表对象指针
 * @param   attrName 需要增加的属性名
 * @param   attrValue 需要增加的属性名对应的初始属性值
 * @return  增加属性成功返回 true ，若指针为 NULL 或增加属性失败，返回 false 
 */
bool addAttribute(Table *table, string attrName, data_t attrValue) {
    int i;
    if (findAttribute(table, attrName) >= 0)
        return false;
    table->attrName = (string *)realloc(table->attrName, (table->attrCount + 1) * sizeof(string));
    strcpy(table->attrName[table->attrCount], attrName);
    for (i = 0; i < table->itemCount; ++i) {
        table->attrValue[i] = (data_t *)realloc(table->attrValue[i], (table->attrCount + 1) * sizeof(data_t));
        table->attrValue[i][table->attrCount] = attrValue;
    }
    table->attrCount++;
    if (saveTable(table))
        return true;
    return false;
}
/**
 * @brief   删除数据表的属性
 * @param   table 表对象指针
 * @param   attrName 需要删除的属性名
 * @return  删除属性成功返回 true ，若指针为 NULL 或删除属性失败，返回 false 
 */
bool dropAttribute(Table *table, string attrName) {
    int i, j, attrIndex;
    attrIndex = findAttribute(table, attrName);
    if (attrIndex < 0)
        return false;
    for (i = attrIndex + 1; i < table->attrCount; ++i)
        strcpy(table->attrName[i - 1], table->attrName[i]);
    table->attrName = (string *)realloc(table->attrName, (table->attrCount - 1) * sizeof(string));
    for (i = 0; i < table->itemCount; ++i) {
        for (j = attrIndex + 1; j < table->attrCount; ++j)
            table->attrValue[i][j - 1] = table->attrValue[i][j];
        table->attrValue[i] = (data_t *)realloc(table->attrValue[i], (table->attrCount - 1) * sizeof(data_t));
    }
    table->attrCount--;
    if (saveTable(table))
        return true;
    return false;
}
/**
 * @brief   增加数据表的条目
 * @param   table 表对象指针
 * @param   valueList 需要添加的条目的所有属性值的列表
 * @return  增加条目成功返回 true ，若指针为 NULL 或增加条目失败，返回 false 
 */
bool insertItem(Table *table, data_t *valueList) {
    table->attrValue = (data_t **)realloc(table->attrValue, (table->itemCount + 1) * sizeof(data_t *));
    table->attrValue[table->itemCount] = (data_t *)malloc(table->attrCount * sizeof(data_t));
    memcpy(table->attrValue[table->itemCount], valueList, table->attrCount * sizeof(data_t));
    table->itemCount++;
    if (saveTable(table))
        return true;
    return false;
}
/**
 * @brief   删除数据表的条目
 * @param   table 表对象指针
 * @param   indexList 需要更新的条目所在的索引的列表
 * @param   indexCount 需要更新的条目的数量
 * @return  成功删除的条目数量
 */
size_t deleteItem(Table *table, int *indexList, int indexCount) {
    int i, j, itemIndex;
    for (i = indexCount - 1; i >= 0; --i) {
        itemIndex = indexList[i];
        for (j = itemIndex + 1; j < table->itemCount; ++j)
            memcpy(table->attrValue[j - 1], table->attrValue[j], table->attrCount * sizeof(data_t));
        table->attrValue = (data_t **)realloc(table->attrValue, (table->itemCount - 1) * sizeof(data_t *));
        table->itemCount--;
    }
    if (saveTable(table))
        return indexCount;
    return 0;
}
/**
 * @brief   更新数据表的多个给定条目的属性值
 * @param   table 表对象指针
 * @param   indexList 需要更新的条目所在的索引的列表
 * @param   indexCount 需要更新的条目的数量
 * @param   attrName 需要更新的条目的属性名
 * @param   value 需要更新的条目的属性名对应的属性值
 * @return  更新的条目的数量
 */
size_t updateItem(Table *table, int *indexList, int indexCount, string attrName, data_t value) {
    int i, j, attrIndex, itemIndex;
    attrIndex = findAttribute(table, attrName);
    if (attrIndex < 0)
        return 0;
    for (i = 0; i < indexCount; ++i) {
        itemIndex = indexList[i];
        table->attrValue[itemIndex][attrIndex] = value;
    }
    if (saveTable(table))
        return indexCount;
    return 0;
}
/**
 * @brief   选择数据表的多个给定条目的给定属性值并显示
 * @param   table 表对象指针
 * @param   indexList 需要选择显示的条目所在的索引的列表
 * @param   indexCount 需要选择显示的条目的数量
 * @param   attrNameList 需要选择显示的条目的属性名的列表
 * @param   attrNameCount 需要选择显示的条目的属性名的数量
 * @return  显示成功返回 true ，若指针为 NULL 或显示失败，返回 false
 */
size_t selectItem(Table* table, int *indexList, int indexCount, string *attrNameList, int attrNameCount) {
    int i, j, itemIndex, attrIndex, existsAttrCount = 0, *attrIndexList = NULL;
    if (attrNameList != table->attrName) {
        attrIndexList = (int *)malloc(attrNameCount * sizeof(int));
        for (i = 0; i < attrNameCount; ++i)
            attrIndexList[i] = findAttribute(table, attrNameList[i]);
    } else {
        attrIndexList = range(0, table->attrCount);
    }
    for (j = 0; j < attrNameCount; ++j) {
        attrIndex = attrIndexList[j];
        if (attrIndex >= 0)
            existsAttrCount++;
    }
    if (existsAttrCount > 0) {
        for (j = 0; j < attrNameCount; ++j) {
            attrIndex = attrIndexList[j];
            if (attrIndex >= 0)
                printf("%s\t", attrNameList[attrIndex]);
        }
        printf("\n");
        for (i = 0; i < indexCount; ++i) {
            itemIndex = indexList[i];
            for (j = 0; j < attrNameCount; ++j) {
                attrIndex = attrIndexList[j];
                if (attrIndex >= 0)
                    printf("%d\t", table->attrValue[itemIndex][attrIndex]);
            }
            printf("\n");
        }
    }
    if (attrIndexList)
        free(attrIndexList);
    if (existsAttrCount > 0)
        return indexCount;
    return 0;
}
/**
 * @brief   生成公差为 1 的左闭右开的等差数列
 * @param   start 数列开始的值
 * @param   stop 数列结束的值
 * @return  等差数列指针，若 stop <= start 返回 NULL 
 */
int *range(int start, int stop) {
    int i, j, length, *array = NULL;
    length = stop - start;
    if (length < 1)
        return NULL;
    j = start;
    array = (int *)malloc(length * sizeof(int));
    for (i = 0; i < length; ++i) {
        array[i] = j;
        j++;
    }
    return array;
}
/**
 * @brief   判断选项范围
 * @param   option 选项
 * @param   leftEnd 选项的左端点
 * @param   rightEnd 选项的右端点
 * @return  选项在给定范围内返回 true ，否则返回 false 
 */
bool inRange(option_t option, option_t leftEnd, option_t rightEnd) {
    return leftEnd <= option && option <= rightEnd;
}
/**
 * @brief   获取文件夹结构
 * @param   dirPath 文件夹路径
 * @return  Dir 结构体指针，包含文件数量和文件列表
 */
Dir *getDir(string dirPath) {
    int i;
    long handle;
    string currentDirAllFile;
    struct _finddata_t fileInfo;
    Dir *dir = (Dir *)calloc(1, sizeof(Dir));
    sprintf(currentDirAllFile, "%s/*", dirPath);
    handle = _findfirst(currentDirAllFile, &fileInfo);
    if (handle != -1) {
        do {
            dir->fileCount++;
        } while(!_findnext(handle, &fileInfo));
        _findclose(handle);
        dir->fileList = (string *)malloc(dir->fileCount * sizeof(string));
        handle = _findfirst(currentDirAllFile, &fileInfo);
        _findnext(handle, &fileInfo);
        _findnext(handle, &fileInfo);
        dir->fileCount -= 2;
        for (i = 0; i < dir->fileCount; i++) {
            strcpy(dir->fileList[i], fileInfo.name);
            _findnext(handle, &fileInfo);
        }
        _findclose(handle);
    }
    return dir;
}
/**
 * @brief   释放 Dir 结构体指针内存
 * @param   dir Dir 结构体指针
 * @return 无返回值
 */
void freeDir(Dir *dir) {
    if (dir)
        free(dir->fileList);
    free(dir);
}
/**
 * @brief   移除文件夹，包括空文件夹和非空文件夹
 * @param   dirPath 需要移除的文件夹路径
 * @return  移除文件夹成功返回 true ，否则返回 false
 */
bool removeDir(string dirPath) {
    int flag;
	long handle = 0;
	struct _finddata_t fb;
	string path;
    sprintf(path, "%s/*", dirPath);
	handle = _findfirst(path, &fb);
	if (handle != 0) {
        _findnext(handle, &fb);
        flag = _findnext(handle, &fb);
		while (!flag) {
            sprintf(path, "%s/%s", dirPath, fb.name);
            if (fb.attrib == 16)
                removeDir(path);	
            else
                remove(path);
            _findnext(handle, &fb);
		}
		flag = _findclose(handle);
	}
    if (!rmdir(dirPath))
        return true;
    return false;
}

/**
 * @brief   初始化程序
 * @return  无返回值
 */
void init() {
    string databaseDirPath;
    sprintf(databaseDirPath, "%s/%s", rootDir, databaseDirName);
    if (access(databaseDirPath, F_OK))
        mkdir(databaseDirPath);
}
/**
 * @brief   输出程序开始或结束时的提示词
 * @param   mode mode == 0 时输出欢迎词，mode == 1 时输出结束词
 * @return  无返回值
 */
void prompt(int mode) {
    if (mode == 0)
        printf("\nWelcome to DBS (Database System).\n\n");
    else if (mode == 1)
        printf("\nGood bye.\n\n");
}
/**
 * @brief   获取用户在数据库层的选择
 * @return  用户在数据库层的选择（所有的选择类型见函数内部代码）
 */
option_t showDatabaseOptions() {
    int i;
    option_t optionCount = 4;
    option_t option = -1;
    string databaseDirPath;
    sprintf(databaseDirPath, "%s/%s", rootDir, databaseDirName);
    Dir *dir = getDir(databaseDirPath);
    if (dir->fileCount > 0) {
        printf("\nThe following database(s) exist:\n");
        for (i = 0; i < dir->fileCount; i++)
            printf("%s\n", dir->fileList[i]);
    } else {
        printf("\nThere have no database exists:\n");
    }
    printf(
        "\n"
        "Please select one of the following commands:\n"
        "1. create database <databaseName>\n"
        "2. delete database <databaseName>\n"
        "3. access to <databaseName>\n"
        "4. exit the program\n"
    );
    while (!scanf("%d", &option) || !inRange(option, 1, optionCount)) {
        printf("Please select a number between 1 and %d:\n", optionCount);
        fflush(stdin);
    }
    freeDir(dir);
    return option;
}
/**
 * @brief   处理用户在数据库层的选择
 * @param   option 用户在数据库层的选择
 * @return  若选择退回上一级返回 -1 ，否则返回 0 
 */
int handleDatabaseOption(option_t option) {
    string databaseName;
    string databaseDirPath;
    Dir *dir;
    if (option != 4) {
        sprintf(databaseDirPath, "%s/%s", rootDir, databaseDirName);
        dir = getDir(databaseDirPath);
    } else {
        return -1;
    }
    switch (option) {
        case 1:
            printf("Please enter a database name:\n");
            while (!scanf("%s", databaseName)) {
                printf("Please enter correct data.\n");
                fflush(stdin);
            }
            if (createDatabase(databaseName)) {
                printf("Database created successfully.\n");
            } else {
                printf("Database failed to create, this database name may already exist.\n");
            }
            break;
        case 2:
            printf("Please enter a database name:\n");
            while (!scanf("%s", databaseName)) {
                printf("Please enter correct data.\n");
                fflush(stdin);
            }
            if (deleteDatabase(databaseName)) {
                printf("Database deleted successfully.\n");
            } else {
                printf("Database failed to delete, this database name may not exist.\n");
            }
            break;
        case 3:
            if (dir->fileCount == 0) {
                printf("There is no database.\n");
            } else {
                printf("Please enter a database name:\n");
                while (!scanf("%s", databaseName)) {
                    printf("Please enter correct data.\n");
                    fflush(stdin);
                }
                if (!existsFile(dir, databaseName)) {
                    printf("Database %s does not exist.\n", databaseName);
                } else {
                    strcpy(currentDatabase, databaseName);
                }
            }
            break;
    }
    freeDir(dir);
}
/**
 * @brief   获取用户在数据表层的选择
 * @return  用户在数据表层的选择（所有的选择类型见函数内部代码）
 */
option_t showTableOptions() {
    int i;
    option_t optionCount = 9;
    option_t option = -1;
    string tableDirPath;
    sprintf(tableDirPath, "%s/%s/%s", rootDir, databaseDirName, currentDatabase);
    Dir *dir = getDir(tableDirPath);
    if (dir->fileCount > 0) {
        printf("\nDatabase %s has the following table(s):\n", currentDatabase);
        for (i = 0; i < dir->fileCount; i++)
            printf("%s\n", dir->fileList[i]);
    } else {
        printf("\nDatabase %s does not have any tables.\n", currentDatabase);
    }
    printf(
        "\n"
        "Please select one of the following commands:\n"
        "1. create table {<attributeName1, attributeName2, ...>}\n"
        "2. delete table <tableName>\n"
        "3. insert into <tableName> values (value1, value2, ...)\n"
        "4. delete from <tableName> where <attributeName>=<attributeValue>\n"
        "5. update <tableName> set <attributeName>=<attributeValue> where <attributeName>=<attributeValue>\n"
        "6. select <attributeName1, attributeName2, ...> from <tableName> where <attributeName>=<attributeValue>\n"
        "7. alter table <tableName> add column <attributeName>\n"
        "8. alter table <tableName> drop column <attributeName>\n"
        "9. back to the previous menu\n"
    );
    while (!scanf("%d", &option) || !inRange(option, 1, optionCount)) {
        printf("Please select a number between 1 and %d:\n", optionCount);
        fflush(stdin);
    }
    freeDir(dir);
    return option;
}
/**
 * @brief   处理用户在数据表层的选择
 * @param   option 用户在数据表层的选择
 * @return  若选择退回上一级返回 -1 ，否则返回 0 
 */
int handleTableOption(option_t option) {
    bool success = false;
    int i, itemCount, indexCount, attributeNameCount, *indexList = NULL;
    data_t attributeValue, *valueList = NULL;
    string tableDirPath, tableName, attributeName, *attributeNameList = NULL;
    Dir *dir = NULL;
    Table *table = NULL;
    if (option != 9) {
        sprintf(tableDirPath, "%s/%s/%s", rootDir, databaseDirName, currentDatabase);
        dir = getDir(tableDirPath);
        printf("Please enter table name:\n");
        while (!scanf("%s", tableName)) {
            printf("Please enter correct data.\n");
            fflush(stdin);
        }
        table = loadTable(tableName);
        if (!table && option != 1) {
            printf("Table %s does not exist.\n", tableName);
            return 0;
        }
        if (table && option == 1) {
            printf("Table %s already exists.\n", tableName);
            freeTable(table);
            return 0;
        }
    } else {
        return -1;
    }
    switch (option) {
        case 1:
            printf("Please enter the number of attributes to be set:\n");
            while (!scanf("%d", &attributeNameCount) || attributeNameCount <= 0) {
                printf("Please enter correct data.\n");
                fflush(stdin);
            }
            printf("Please enter %d attribute name(s) separated by spaces:\n", attributeNameCount);
            attributeNameList = (string *)malloc(attributeNameCount * sizeof(string));
            for (i = 0; i < attributeNameCount; ++i)
                scanf("%s", attributeNameList[i]);
            if (createTable(tableName, attributeNameList, attributeNameCount)) {
                printf("Table %s created successfully.\n", tableName);
            } else {
                printf("Failed to create table %s.\n", tableName);
            }
            if (attributeNameList)
                free(attributeNameList);
            break;
        case 2:
            if (deleteTable(tableName)) {
                printf("Table %s deleted successfully.\n", tableName);
            } else {
                printf("Failed to delete table %s.\n", tableName);
            }
            break;
        case 3:
            valueList = (data_t *)malloc(table->attrCount * sizeof(data_t));
            printf("Please enter %d attribute value(s) separated by spaces:\n", table->attrCount);
            for (i = 0; i < table->attrCount; ++i)
                scanf("%d", valueList + i);
            if (insertItem(table, valueList)) {
                printf("Successfully inserted an item to table %s.\n", tableName);
            } else {
                printf("Failed to insert an item to table %s.\n", tableName);
            };
            if (valueList)
                free(valueList);
            break;
        case 4:
            printf("Please enter an attribute name and an attribute value separated by spaces, you can use '*' as a wildcard:\n");
            while (!scanf("%s", attributeName) || !scanf("%d", &attributeValue)) {
                printf("Please enter correct data.\n");
                fflush(stdin);
            }
            indexList = filterItem(table, attributeName, attributeValue, &indexCount);
            if (!indexCount) {
                printf("The attribute does not exist or no items are filtered out, no items are deleted");
            } else {
                itemCount = deleteItem(table, indexList, indexCount);
                printf("%d item(s) are deleted in this operation.\n", itemCount);
                if (indexList)
                    free(indexList);
            }
            break;
        case 5:
            printf("Please enter an attribute name and an attribute value separated by spaces, these values are used to filter item(s):\n");
            while (!scanf("%s", attributeName) || !scanf("%d", &attributeValue)) {
                printf("Please enter correct data.\n");
                fflush(stdin);
            }
            indexList = filterItem(table, attributeName, attributeValue, &indexCount);
            if (!indexCount) {
                printf("The attribute does not exist or no items are filtered out, no items are updated");
            } else {
                printf("Please enter an attribute name and an attribute value separated by spaces, these values are used to update item(s):\n");
                while (!scanf("%s", attributeName) || !scanf("%d", &attributeValue)) {
                    printf("Please enter correct data.\n");
                    fflush(stdin);
                }
                itemCount = updateItem(table, indexList, indexCount, attributeName, attributeValue);
                printf("%d item(s) are updated in this operation.\n", itemCount);
                if (indexList)
                    free(indexList);
            }
            break;
        case 6:
            printf("Please enter an attribute name and an attribute value separated by spaces, these values are used to filter item(s), you can use '*' as a wildcard:\n");
            while (!scanf("%s", attributeName) || !scanf("%d", &attributeValue)) {
                printf("Please enter correct data.\n");
                fflush(stdin);
            }
            indexList = filterItem(table, attributeName, attributeValue, &indexCount);
            if (!indexList && !indexCount) {
                printf("The attribute does not exist or no items are filtered out, no items are selected.\n");
            } else {
                printf("Please enter the number of attributes to be selected:\n");
                while (!scanf("%d", &attributeNameCount) || attributeNameCount <= 0) {
                    printf("Please enter correct data.\n");
                    fflush(stdin);
                }
                printf("Please enter %d attribute name(s) separated by spaces:\n", attributeNameCount);
                attributeNameList = (string *)malloc(attributeNameCount * sizeof(string));
                for (i = 0; i < attributeNameCount; ++i) {
                    scanf("%s", attributeNameList[i]);
                }
                for (i = 0; i < attributeNameCount; ++i) {
                    if (!strcmp(attributeNameList[i], wildcard)) {
                        free(attributeNameList);
                        attributeNameList = table->attrName;
                        attributeNameCount = table->attrCount;
                        break;
                    }
                }
                itemCount = selectItem(table, indexList, indexCount, attributeNameList, attributeNameCount);
                printf("%d item(s) are selected in this operation.\n", itemCount);
            }
            if (attributeNameList != table->attrName)
                free(attributeNameList);
            if (indexList)
                free(indexList);
            break;
        case 7:
            printf("Please enter an attribute name and an attribute value separated by spaces:\n");
            while (!scanf("%s", attributeName) || !scanf("%d", &attributeValue)) {
                printf("Please enter correct data.\n");
                fflush(stdin);
            }
            if (addAttribute(table, attributeName, attributeValue)) {
                printf("Successfully added attribute %s for table %s.\n", attributeName, table->tableName);
            } else {
                printf("Failed to add attribute %s for table %s, perhaps the attribute already exists.\n", attributeName, table->tableName);
            }
            break;
        case 8:
            printf("Please enter an attribute name:\n");
            while (!scanf("%s", attributeName)) {
                printf("Please enter correct data.\n");
                fflush(stdin);
            }
            if (dropAttribute(table, attributeName)) {
                printf("Successfully dropped attribute %s for table %s.\n", attributeName, table->tableName);
            } else {
                printf("Failed to drop attribute %s for table %s, perhaps the attribute does not exist.\n", attributeName, table->tableName);
            }
            break;
    }
    if (table)
        freeTable(table);
    freeDir(dir);
    return 0;
}

// main function
int main() {
    char *databaseName;
    int statusCode;
    option_t databaseOption, tableOption;
    init();
    prompt(0);
    while (true) {
        databaseOption = showDatabaseOptions();
        statusCode = handleDatabaseOption(databaseOption);
        if (statusCode == -1)
            break;
        if (strlen(currentDatabase) > 0) {
            while (true) {
                tableOption = showTableOptions();
                statusCode = handleTableOption(tableOption);
                if (statusCode == -1)
                    break;
            }
            memset(currentDatabase, 0, sizeof(currentDatabase));
        }
    }
    prompt(1);
}