#include <stdio.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>

#define LOG(format, ...) do { \
                printf("%s:%s/%s|%d::%d::"format,"BattleShip",__FILE__,__func__,__LINE__,getpid(),##__VA_ARGS__); \
                printf("\n"); \
}while(0)

#define PARENT_LOG(format, ...) do { \
                printf("[%d %s]"format,getpid(),"parent",##__VA_ARGS__); \
                printf("\n"); \
}while(0)

#define CHILD_LOG(format, ...) do { \
                printf("[%d %s ]"format,getpid(),"child",##__VA_ARGS__); \
                printf("\n"); \
}while(0)

#define SHM_SIZE (1024)


/* 位置所对应的坐标 */
int position_detail[16][2] = {
    {1,1},{1,2},{1,3},{1,4},
    {2,1},{2,2},{2,3},{2,4},
    {3,1},{3,2},{3,3},{3,4},
    {4,1},{4,2},{4,3},{4,4}
};
/* 位置图，战舰是2X1，这里用于列出战舰可能的位置 */
/*
    13 14 15 16
    9  10 11 12
    5  6  7  8
    1  2  3  4
*/
int position_map[16][2] = {
    {1,2},{1,5},{2,6},{2,3},
    {3,7},{3,4},{4,8},{5,6},
    {5,9},{6,10},{6,7},{7,11},
    {7,8},{8,12},{9,13},{9,10},
    {10,14},{10,11},{11,15},{11,12},
    {12,16}
};

/* 战舰的位置 */
typedef struct
{
    int x1;
    int y1;
    int x2;
    int y2;
}ST_BOAT_POSITION;

/* 炸弹的位置 */
typedef struct
{
    int x;
    int y;
}ST_BOMB_POSITION;

/* 两个进程共享的数据 */
typedef struct
{
    int step;        /* 当前需要操作的指令 */
    int parent_ready; /* 父进程当前是否正确就绪 */
    int child_ready;  /* 子进程当前是否正确就绪 */
    int parent_bomb_num;  /* 父进程轰炸次数 */
    int child_bomb_num;   /* 子进程轰炸次数 */
    int victory;         /* 1：父进程胜利 2：子进程胜利 */
    ST_BOMB_POSITION bomb_pos;
}ST_BATTLE_DATA;

typedef enum
{
    GENERATE_BOAT_POSITION = 1,
    GENERATE_BOMB_POSITION = 2,
    CHECK_BOMB_POSITION    = 3,
    GAME_OVER = 4,
    GAME_EXIT = 5,
}E_STEP;

int shmid;
//ST_BATTLE_DATA* shmaddr;
struct shmid_ds buf;

ST_BATTLE_DATA* gstBattleData;

//获取真随机数
unsigned int myrand(void) {
    int ret;
    unsigned int randnum = 0;
    int fd = open("/dev/urandom", O_RDONLY);
    if(-1 == fd)
    {
        LOG("myrand error\n");
        return 0;
    }

    ret = read(fd, (char *)&randnum, sizeof(int));
    if(ret < 0)
    {
        LOG("read failed");
    }
    close(fd);

    return randnum;
}

void generate_boat_position(ST_BOAT_POSITION* pos)
{
    srand(time(NULL));
    unsigned int s = myrand() % 16 ;
    //LOG("myrand:%d",s);
    int selected_1 = position_map[s][0];
    int selected_2 = position_map[s][1];

    pos->x1 = position_detail[selected_1][0];
    pos->y1 = position_detail[selected_1][1];
    pos->x2 = position_detail[selected_2][0];
    pos->y2 = position_detail[selected_2][1];
}

void generate_bomb_position(ST_BOMB_POSITION* pos)
{
    static int record[16] = {0};
    unsigned int s = 0;
    //srand(time(NULL));
    while(1)
    {
        s = myrand() % 16 ;
        if(0 == record[s]) break;
    }
    record[s] = 1;
    pos->x = position_detail[s][0];
    pos->y = position_detail[s][1];
}

/* 初始化共享内存 */
int init_shm()
{
    int flag = 0;

    flag = shmctl(shmid,IPC_STAT,&buf);
    if(flag == -1)
    {
        LOG("shmctl err:%d",errno);
        return -1;
    }
    //LOG("shm size:%d",buf.shm_segsz);
    if(buf.shm_segsz != sizeof(ST_BATTLE_DATA))
    {
        LOG("shm size is err,read:%d,actual:%d",buf.shm_segsz,sizeof(ST_BATTLE_DATA));
        return -1;
    }

    gstBattleData = (ST_BATTLE_DATA*)shmat(shmid,NULL,0);
    if(gstBattleData == (void*)(-1))
    {
        LOG("shmat err:%d",errno);
        return -1;
    }

    memset(gstBattleData,0,sizeof(ST_BATTLE_DATA));

    return 0;
}

/* 步进式设计，每步执行一次操作，操作完之后再选择下一步需要执行的操作 */
int parent_process()
{
    ST_BOAT_POSITION position;
    ST_BOMB_POSITION bomb_pos;
    int bomb_flag = 0;

    gstBattleData->step = GENERATE_BOAT_POSITION;
    gstBattleData->parent_ready = 1;
    while(1)
    {
        if(0 == gstBattleData->parent_ready)
        {
            continue;
        }
        gstBattleData->parent_ready = 0;
        //LOG("step:%d",gstBattleData->step);

        if(GAME_EXIT == gstBattleData->step)  /* 游戏退出 */
        {
            LOG("bye");
            break;
        }

        switch(gstBattleData->step)
        {
            case GENERATE_BOAT_POSITION:   /* 生成战舰位置 */
            {
                generate_boat_position(&position);
                PARENT_LOG("the gunboat (%d,%d)(%d,%d)",position.x1,position.y1,position.x2,position.y2);
                gstBattleData->step = GENERATE_BOAT_POSITION;
                gstBattleData->child_ready = 1;
                break;
            }

            case GENERATE_BOMB_POSITION:   /* 生成炸弹位置 */
            {
                generate_bomb_position(&bomb_pos);
                PARENT_LOG("the bombing (%d,%d)",bomb_pos.x,bomb_pos.y);
                gstBattleData->bomb_pos.x = bomb_pos.x;
                gstBattleData->bomb_pos.y = bomb_pos.y;

                gstBattleData->parent_bomb_num ++;
                gstBattleData->step = CHECK_BOMB_POSITION;
                gstBattleData->child_ready = 1;
                break;
            }

            case CHECK_BOMB_POSITION:    /* 检查炸弹位置 */
            {
                if(gstBattleData->bomb_pos.x == position.x1 && gstBattleData->bomb_pos.y == position.y1)
                {
                    PARENT_LOG("hit");
                    bomb_flag |= 0x01;
                }else if(gstBattleData->bomb_pos.x == position.x2 && gstBattleData->bomb_pos.y == position.y2)
                {
                    PARENT_LOG("hit");
                    bomb_flag |= 0x02;
                }
                else
                {
                    PARENT_LOG("missing");
                }
                if((bomb_flag&0x01)&&(bomb_flag&0x02)) /* 全部命中 */
                {
                    gstBattleData->step = GAME_OVER;
                    gstBattleData->victory = 2;
                    gstBattleData->parent_ready = 1;
                }else{
                    gstBattleData->step = GENERATE_BOMB_POSITION;
                    gstBattleData->parent_ready = 1;
                }

                break;
            }

            case GAME_OVER:       /* 游戏结束 */
            {
                if(2 == gstBattleData->victory)
                {
                    PARENT_LOG("child win with %d bombs",gstBattleData->child_bomb_num);
                    gstBattleData->step = GAME_EXIT;
                    gstBattleData->parent_ready = 1;
                    gstBattleData->child_ready = 1;
                }
                else if(1 == gstBattleData->victory)
                {
                    PARENT_LOG("parent win %d bombs",gstBattleData->parent_bomb_num);
                    gstBattleData->step = GAME_EXIT;
                    gstBattleData->child_ready = 1;
                }
            }
        }
    }

    PARENT_LOG("parent exit..");
    wait(NULL);
    shmdt(gstBattleData);
    shmctl(shmid,IPC_RMID,NULL);
    return 0;
}

/* 步进式设计，每步执行一次操作，操作完之后再选择下一步需要执行的操作 */
int child_process()
{
    gstBattleData = (char*)shmat(shmid,NULL,0);
    if((void*)(-1) == gstBattleData)
    {
        LOG("shmat add err:%d",errno);
        return -1;
    }
    ST_BOAT_POSITION position;
    ST_BOMB_POSITION bomb_pos;
    int bomb_flag = 0;

    gstBattleData->step = GENERATE_BOAT_POSITION;
    while(1)
    {
        if(0 == gstBattleData->child_ready)
        {
            continue;
        }
        gstBattleData->child_ready = 0;
        //LOG("step:%d",gstBattleData->step);

        if(gstBattleData->step == GAME_EXIT)  /* 游戏结束 */
        {
            gstBattleData->parent_ready = 1;
            break;
        }

        switch(gstBattleData->step)
        {
            case GENERATE_BOAT_POSITION:  /* 生成战舰位置 */
            {
                generate_boat_position(&position);
                CHILD_LOG("the gunboat (%d,%d)(%d,%d)",position.x1,position.y1,position.x2,position.y2);
                gstBattleData->step = GENERATE_BOMB_POSITION;
                gstBattleData->parent_ready = 1;
                break;
            }

            case GENERATE_BOMB_POSITION:  /* 生成炸弹位置 */
            {
                generate_bomb_position(&bomb_pos);
                CHILD_LOG("the bombing (%d,%d)",bomb_pos.x,bomb_pos.y);

                gstBattleData->child_bomb_num ++;
                gstBattleData->step = CHECK_BOMB_POSITION;
                gstBattleData->parent_ready = 1;
                break;
            }

            case CHECK_BOMB_POSITION:   /* 检查炸弹位置 */
            {
                if(gstBattleData->bomb_pos.x == position.x1 && gstBattleData->bomb_pos.y == position.y1)
                {
                    CHILD_LOG("hit");
                    bomb_flag |= 0x01;
                }else if(gstBattleData->bomb_pos.x == position.x2 && gstBattleData->bomb_pos.y == position.y2)
                {
                    CHILD_LOG("hit");
                    bomb_flag |= 0x02;
                }else
                {
                    CHILD_LOG("missing");
                }
                if((bomb_flag&0x01)&&(bomb_flag&0x02))  /* 全部命中 */
                {
                    gstBattleData->step = GAME_OVER;
                    gstBattleData->victory = 1;
                    gstBattleData->parent_ready = 1;
                }else{
                    gstBattleData->step = GENERATE_BOMB_POSITION;
                    gstBattleData->child_ready = 1;
                }

            }

        }
    }

    CHILD_LOG("child will exit..");
    shmdt(gstBattleData);
    exit(0);
    return 0;
}

int main()
{
    pid_t fpid;

    shmid = shmget(IPC_PRIVATE,sizeof(ST_BATTLE_DATA),IPC_CREAT|0600);
    if(shmid < 0)
    {
        LOG("get shm id err:%d",errno);
        return -1;
    }

    /* 初始化共享内存 */
    init_shm();

    //fork child process
    fpid = fork();
    if(fpid < 0)
    {
        LOG("fork err");
        shmctl(shmid,IPC_RMID,NULL);
        return -1;
    }
    else if(0 == fpid)
    {
        //LOG("this is child process ");
        child_process();
    }else
    {
        //LOG("this is parent process,child pid is :%d",fpid);
        parent_process();
    }

    return 0;
}


