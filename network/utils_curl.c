#include <curl/curl.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <cJSON.h>

#include "osadapter.h"
#include "commons_log.h"

#define LOG(...) COMMONS_LOG("CURL",__VA_ARGS__);

#define URL_LOGIN  "http://2yuanyy.com/Personal/Login"
#define URL_MAIN   "http://2yuanyy.com/Home/Index"
#define URL_DOCTOR_MAIN "http://2yuanyy.com/Clinic/GetClinicRegistList"
#define URL_VERFY_CODE "http://2yuanyy.com/verify_Image.ashx"
#define URL_PYC    "http://2yuanyy.com/Passport/LoginMethod?LoginName=340881199308270615&LoginPwd=pp265358&LoginType=0&VerCode=1742"
#define URL_POST_PWD  "http://2yuanyy.com/Passport/LoginMethod"
#define URL_BOOK_MODEL "http://2yuanyy.com/Clinic/GetBookSureModel"
#define URL_BOOK_TICKET "http://2yuanyy.com/ajax.aspx"

#define URL_PWD_USRNAME "340881199308270615"
#define URL_PWD_PWD     "pp265358"

#define PATH_MAIN_INTERFACE "./jiaoda.html"
#define PATH_VER_CODE "./verfycode.jpg"
#define PATH_LOGIN_INTERFACE "./login.html"
#define PATH_DOCTOR_INTERFACE "./doctor.html"

#define DOCTOR_ID_TEST "6101001710337018"
#define DOCTOR_ID_USED "6101001710473007"

#define SELECT_ID      "340881199308270615"
#define SELECT_ID_USED "610222199403231123"

#define PREFER_TIME    "09:00~09:30"

size_t write_data(void *buffer, size_t size, size_t nmemb, void *userp)
{
    commons_println("get data size:%d,nmemb:%d",size,nmemb);

    FILE *fp = (FILE *)userp;
    size_t return_size = fwrite(buffer, size, nmemb, fp);
    return return_size;
}

typedef struct
{
    char TimePart[32];
    char TimePartDate[32];
    char ScheduleItemCode[32];
}st_time_part;

typedef struct
{
    char PersonId[32];  //医院分配的id
    char IdNumber[64];   //身份证号
}st_person_info;

typedef struct
{
    int appoint_status_len;
    char* appoint_status_content;
    char* clinic_label_id;
    int time_part_type;
    int channel_type_id;
    int rsv_model;
    int visit_type;     //1--初诊 2-复诊 3-术后复诊
    int pay_channel;    //2--到院支付
    char* schedule_item_code;
    char* his_start_time;
    char* his_end_time;
    char* clinic_date;
    char* rQhis_count;
    char* book_model;
    int time_part_num;
    st_time_part time_part[10];
    int person_num;
    st_person_info person_info[10];
    pthread_rwlock_t lock;
}st_curl_ctx;


st_curl_ctx* curl_ctx_acquire(BOOL write)
{
    static st_curl_ctx gstCurlCtx;
    st_curl_ctx* ctx = &gstCurlCtx;

    if(write)
        pthread_rwlock_wrlock(&ctx->lock);
    else
        pthread_rwlock_rdlock(&ctx->lock);

    return ctx;
}

st_curl_ctx* curl_ctx_release(st_curl_ctx* ctx)
{
    pthread_rwlock_unlock(&ctx->lock);
    return NULL;
}


/* parse headers for Content-Length */
static size_t get_content_length_func(void *ptr, size_t size, size_t nmemb,
                                   void *stream)
{
    int r;
    long len = 0;

    r = sscanf(ptr, "Content-Length: %ld\n", &len);
    //if(r)
    // *((long *) stream) = len;
    if(r)
    {
        st_curl_ctx* ctx = curl_ctx_acquire(TRUE);
        ctx->appoint_status_len = len;
        ctx->appoint_status_content = (char*)malloc(len+1);
        bzero(ctx->appoint_status_content,len+1);
        ctx = curl_ctx_release(ctx);
    }

    LOG("headr: %s",ptr);
    LOG("Content-Length: %ld",len);

    return size * nmemb;
}


size_t appoint_status_save(void *buffer, size_t size, size_t nmemb, void *userp)
{
    commons_println("get data size:%d,nmemb:%d",size,nmemb);

    st_curl_ctx* ctx = curl_ctx_acquire(TRUE);
    strcat(ctx->appoint_status_content,buffer);
    ctx = curl_ctx_release(ctx);

    FILE *fp = (FILE *)userp;
    size_t return_size = fwrite(buffer, size, nmemb, fp);
    return return_size;
}

static size_t get_book_model_length_func(void *ptr, size_t size, size_t nmemb,
                                   void *stream)
{
    int r;
    long len = 0;

    r = sscanf(ptr, "Content-Length: %ld\n", &len);
    //if(r)
    // *((long *) stream) = len;
    if(r)
    {
        st_curl_ctx* ctx = curl_ctx_acquire(TRUE);
        ctx->book_model = (char*)malloc(len+1);
        bzero(ctx->book_model,len+1);
        ctx = curl_ctx_release(ctx);
    }

    LOG("headr: %s",ptr);
    LOG("Content-Length: %ld",len);

    return size * nmemb;
}


size_t book_model_callback(void *buffer, size_t size, size_t nmemb, void *userp)
{
    commons_println("get data size:%d,nmemb:%d",size,nmemb);

    st_curl_ctx* ctx = curl_ctx_acquire(TRUE);
    strcat(ctx->book_model,buffer);
    ctx = curl_ctx_release(ctx);

    FILE *fp = (FILE *)userp;
    size_t return_size = fwrite(buffer, size, nmemb, fp);
    return return_size;
}

int book_model_parser(void)
{
    cJSON* root = NULL;
    cJSON* person_root = NULL;
    cJSON* item = NULL;
    cJSON* subitem = NULL;
    cJSON* partitem = NULL;
    cJSON* arry = NULL;
    int i;
    int num;
    int ret = -1;
    st_curl_ctx* ctx = curl_ctx_acquire(FALSE);
    if(!ctx->book_model) LOG("book_model is null");
    root = cJSON_Parse(ctx->book_model);
    ctx = curl_ctx_release(ctx);
    if(!root)
    {
       LOG("failed to parse JSON,%s",cJSON_GetErrorPtr());
       return -1;
    }
    //LOG("%s",cJSON_Print(root));

    item = cJSON_GetObjectItem(root,"ReturnInfo");
    if(!item)
    {
        LOG("failed to get ReturnInfo");
        cJSON_Delete(root);
        return -1;
    }

    subitem = cJSON_GetObjectItem(item,"Result");
    if(!subitem)
    {
        LOG("Result is null,%s",cJSON_GetErrorPtr());
    }
    else
    {
        LOG("Result:%d",subitem->valueint);
    }
    if(subitem->valueint != 0)
    {
        LOG("ReturnInfo,%s",cJSON_Print(item));
        return -1;
    }
    
    item = cJSON_GetObjectItem(root,"ChannelTypeId");
    if(!item)
    {
        LOG("failed to get ChannelTypeId");
        cJSON_Delete(root);
        return -1;
    }
    LOG("ChannelTypeId:%d",item->valueint);
    ctx = curl_ctx_acquire(TRUE);
    ctx->channel_type_id = item->valueint;
    ctx = curl_ctx_release(ctx);

    item = cJSON_GetObjectItem(root,"RsvModel");
    if(!item)
    {
        LOG("failed to get RsvModel");
        cJSON_Delete(root);
        return -1;
    }
    LOG("RsvModel:%d",item->valueint);
    ctx = curl_ctx_acquire(TRUE);
    ctx->rsv_model = item->valueint;
    ctx->visit_type = 1;
    ctx->pay_channel = 2;
    ctx = curl_ctx_release(ctx);
    

    item = cJSON_GetObjectItem(root,"PatientCardJsons");
    if(!item) goto err;
    //LOG("PatientCardJsons %s",item->valuestring);
    
    person_root = cJSON_Parse(item->valuestring);
    if(!person_root) goto err;
    //LOG("PatientCard:%s",cJSON_Print(person_root));

    num = cJSON_GetArraySize(person_root);
    LOG("PatientCardJsons size:%d",num);
    ctx = curl_ctx_acquire(TRUE);
    ctx->person_num = 0;
    bzero(ctx->person_info,sizeof(st_person_info) * 10);
    ctx = curl_ctx_release(ctx);
    for(i = 0;i < num;i ++)
    {
        item = cJSON_GetArrayItem(person_root,i);

        subitem = cJSON_GetObjectItem(item,"IdNumber");
        if(!subitem)
        {
            LOG("IdNumber is null");
            continue;
        }
        ctx = curl_ctx_acquire(TRUE);
        strcpy(ctx->person_info[ctx->person_num].IdNumber,subitem->valuestring);
        LOG("IdNumber is %s",ctx->person_info[ctx->person_num].IdNumber);
        ctx = curl_ctx_release(ctx);

        subitem = cJSON_GetObjectItem(item,"PersonId");
        if(!subitem)
        {
            LOG("PersonId is null");
            continue;
        }
        ctx = curl_ctx_acquire(TRUE);
        strcpy(ctx->person_info[ctx->person_num].PersonId,subitem->valuestring);
        LOG("PersonId is %s",ctx->person_info[ctx->person_num].PersonId);
        ctx = curl_ctx_release(ctx);

        ctx = curl_ctx_acquire(TRUE);
        ctx->person_num ++;
        ctx = curl_ctx_release(ctx);
    }


    item = cJSON_GetObjectItem(root,"TimePartList");
    if(!item) goto err;

    num = cJSON_GetArraySize(item);
    LOG("TimePartList size:%d",num);
 
    ctx = curl_ctx_acquire(TRUE);
    ctx->time_part_num = 0;
    bzero(ctx->time_part,sizeof(st_time_part) * 10);
    ctx = curl_ctx_release(ctx);
    for(i = 0;i < num;i ++)
    {
        subitem = cJSON_GetArrayItem(item,i);
        LOG("TimePart: %s",cJSON_Print(subitem));
        partitem = cJSON_GetObjectItem(subitem,"AppointmentLessCount");
        if(!partitem) goto err;
        if(partitem->valueint == 0) continue;

        partitem = cJSON_GetObjectItem(subitem,"TimePart");
        if(!partitem) goto err;

        ctx = curl_ctx_acquire(TRUE);
        strcpy(ctx->time_part[ctx->time_part_num].TimePart,partitem->valuestring);
        
        partitem = cJSON_GetObjectItem(subitem,"TimePartDate");
        strcpy(ctx->time_part[ctx->time_part_num].TimePartDate,partitem->valuestring);

        partitem = cJSON_GetObjectItem(subitem,"ScheduleItemCode");
        strcpy(ctx->time_part[ctx->time_part_num].ScheduleItemCode,partitem->valuestring);

        ctx->time_part_num++;
        ctx = curl_ctx_release(ctx);
        
        ret = 0;
    }

err:
    cJSON_Delete(root);
    if(person_root) cJSON_Delete(person_root);
    return ret;
}

int appoint_status_parser(void)
{
    cJSON* root = NULL;
    cJSON* item = NULL;
    cJSON* subitem = NULL;
    cJSON* arry = NULL;
    int i;
    int num;
    st_curl_ctx* ctx = curl_ctx_acquire(FALSE);
    root = cJSON_Parse(ctx->appoint_status_content);
    if(!ctx->appoint_status_content) LOG("appoint_status_content is null");
    ctx = curl_ctx_release(ctx);
    if(!root)
    {
       LOG("failed to parse JSON,%s",cJSON_GetErrorPtr());
       return -1;
    }
    LOG("appoint_status_content,%s",cJSON_Print(root));

    arry = cJSON_GetObjectItem(root,"RegistModelList");
    if(!arry)
    {
        LOG("failed to get RegistModelList");
        cJSON_Delete(root);
        return -1;
    }
    //LOG("arry:%s",cJSON_Print(arry));
    num = cJSON_GetArraySize(arry);
    LOG("array size:%d",num);
    for(i = 0; i < num;i ++)
    {
        item = cJSON_GetArrayItem(arry,i);
        subitem = cJSON_GetObjectItem(item,"Status");
        if(subitem->valueint == 1)
        {
            LOG("%s",cJSON_Print(item));
            ctx = curl_ctx_acquire(TRUE);
            subitem = cJSON_GetObjectItem(item,"ClinicLabelId");
            if(!subitem) continue;
            LOG("ClinicLabelId:%s",subitem->valuestring);
            ctx->clinic_label_id = (char*)malloc(strlen(subitem->valuestring)+1);
            bzero(ctx->clinic_label_id,strlen(subitem->valuestring)+1);
            strcpy(ctx->clinic_label_id,subitem->valuestring);

            subitem = cJSON_GetObjectItem(item,"TimePartType");
            if(!subitem) continue;
            LOG("TimePartType:%d",subitem->valueint);
            ctx->time_part_type = subitem->valueint;

            subitem = cJSON_GetObjectItem(item,"ScheduleItemCode");
            if(!subitem) continue;
            LOG("ScheduleItemCode:%s",subitem->valuestring);
            ctx->schedule_item_code = (char*)malloc(strlen(subitem->valuestring)+1);
            bzero(ctx->schedule_item_code,strlen(subitem->valuestring)+1);
            strcpy(ctx->schedule_item_code,subitem->valuestring);

            subitem = cJSON_GetObjectItem(item,"HisStartTime");
            if(!subitem) continue;
            LOG("HisStartTime:%s",subitem->valuestring);
            ctx->his_start_time = (char*)malloc(strlen(subitem->valuestring)+1);
            bzero(ctx->his_start_time,strlen(subitem->valuestring)+1);
            strcpy(ctx->his_start_time,subitem->valuestring);

            subitem = cJSON_GetObjectItem(item,"HisEndTime");
            if(!subitem) continue;
            LOG("HisEndTime:%s",subitem->valuestring);
            ctx->his_end_time = (char*)malloc(strlen(subitem->valuestring)+1);
            bzero(ctx->his_end_time,strlen(subitem->valuestring)+1);
            strcpy(ctx->his_end_time,subitem->valuestring);
            
            subitem = cJSON_GetObjectItem(item,"ClinicDate");
            if(!subitem) continue;
            LOG("ClinicDate:%s",subitem->valuestring);
            ctx->clinic_date = (char*)malloc(strlen(subitem->valuestring)+1);
            bzero(ctx->clinic_date,strlen(subitem->valuestring)+1);
            strcpy(ctx->clinic_date,subitem->valuestring);

            subitem = cJSON_GetObjectItem(item,"RQhisCount");
            if(!subitem) continue;
            LOG("RQhisCount:%s",subitem->valuestring);
            ctx->rQhis_count = (char*)malloc(strlen(subitem->valuestring)+1);
            bzero(ctx->rQhis_count,strlen(subitem->valuestring)+1);
            strcpy(ctx->rQhis_count,subitem->valuestring);
            
            ctx = curl_ctx_release(ctx);
            cJSON_Delete(root);
            return 0;
        }
    }
    cJSON_Delete(root);
    return -1;
}


int code_filter(char* src)
{
    char buffer[64] = {0};
    
    int i;
    for(i = 0;i < strlen(src);i ++)
    {
        if(*(src + i) >= '0' && *(src + i) <= '9')
            strncat(buffer,(src + i),1);
    }
    strcpy(src,buffer);
}

int get_verify_code(CURL *easy_handle)
{
    FILE* pic_fp = NULL;
    CURLcode curl_ret;
    pic_fp = fopen(PATH_VER_CODE,"ab+");
    curl_easy_setopt(easy_handle,CURLOPT_URL,URL_VERFY_CODE);
    curl_easy_setopt(easy_handle, CURLOPT_WRITEFUNCTION, write_data);
    curl_easy_setopt(easy_handle, CURLOPT_WRITEDATA, pic_fp);
    curl_ret = curl_easy_perform(easy_handle);
    if(curl_ret != CURLE_OK)
    {
        LOG("perform error:%d",curl_ret);
        fclose(pic_fp);
        pic_fp = NULL;
        return -1;
    }
    printf("download verfy code succ\r\n");
    fclose(pic_fp);
    
    pic_fp = NULL;
    return 0;
}

int post_pwd(CURL *easy_handle,char* verify_code)
{
    CURLcode curl_ret;
    char post_data[1024] = {0};
    long res_code;
    char* url = NULL;
    
    sprintf(post_data,"LoginName=%s&LoginPwd=%s&LoginType=0&VerCode=%s&",URL_PWD_USRNAME,URL_PWD_PWD,verify_code);
    printf("post_data:[%s]\r\n",post_data);
    curl_easy_setopt(easy_handle,CURLOPT_URL,URL_POST_PWD);
    curl_easy_setopt(easy_handle,CURLOPT_POSTFIELDS, post_data);
    curl_easy_setopt(easy_handle,CURLOPT_POSTFIELDSIZE,sizeof(post_data));
    curl_easy_setopt(easy_handle, CURLOPT_WRITEFUNCTION, write_data);
    curl_easy_setopt(easy_handle, CURLOPT_WRITEDATA, stdout);

    curl_easy_setopt(easy_handle, CURLOPT_FOLLOWLOCATION, 1);
    curl_ret = curl_easy_getinfo(easy_handle,CURLINFO_REDIRECT_URL,&url);
    LOG("curl_easy_getinfo:%d",curl_ret);
    
    curl_ret = curl_easy_perform(easy_handle);
    if(curl_ret != CURLE_OK)
    {
        LOG("perform error:%d",curl_ret);
        return -1;
    }

    curl_ret = curl_easy_getinfo(easy_handle,CURLINFO_RESPONSE_CODE,&res_code);
    LOG("curl_easy_getinfo:%d\r\n",curl_ret);
    LOG("curl_easy_getinfo:%d\r\n",res_code);

    curl_easy_setopt(easy_handle, CURLOPT_FOLLOWLOCATION, 1);
    curl_ret = curl_easy_getinfo(easy_handle,CURLINFO_REDIRECT_URL,&url);
    LOG("curl_easy_getinfo:%d",curl_ret);
    
    if(url)
        LOG("url:%s",url);
    
    return 0;
}

int enter_login_interface(CURL *easy_handle)
{
    FILE *fp = NULL;
    CURLcode curl_ret;

    fp = fopen(PATH_LOGIN_INTERFACE,"ab+");
    
    curl_easy_setopt(easy_handle,CURLOPT_URL,URL_LOGIN);
    curl_easy_setopt(easy_handle, CURLOPT_WRITEFUNCTION, write_data);
    curl_easy_setopt(easy_handle, CURLOPT_WRITEDATA, fp);
    curl_ret = curl_easy_perform(easy_handle);
    if(curl_ret != CURLE_OK)
    {
        LOG("perform error:%d",curl_ret);
    }
    fclose(fp);
}

int enter_main_index(CURL *easy_handle)
{    
    FILE *fp = NULL;
    CURLcode curl_ret;

    fp = fopen(PATH_MAIN_INTERFACE,"ab+");

    curl_easy_reset(easy_handle);
    curl_easy_setopt(easy_handle,CURLOPT_URL,URL_MAIN);
    curl_easy_setopt(easy_handle, CURLOPT_WRITEFUNCTION, write_data);
    curl_easy_setopt(easy_handle, CURLOPT_WRITEDATA, fp);
    curl_ret = curl_easy_perform(easy_handle);
    if(curl_ret != CURLE_OK)
    {
        LOG("perform error:%d",curl_ret);
    }
    fclose(fp);

}

int enter_doctor_interface(CURL *easy_handle,char* doctor_id)
{
    FILE *fp = NULL;
    s32 ret;
    fp = fopen(PATH_DOCTOR_INTERFACE,"ab+");

    CURLcode curl_ret;
    char post_data[1024] = {0};
    long res_code;
    
    sprintf(post_data,"DoctorId=%s&registType=1&",doctor_id);
    printf("post_data:[%s]\r\n",post_data);
    curl_easy_setopt(easy_handle,CURLOPT_URL,URL_DOCTOR_MAIN);
    curl_easy_setopt(easy_handle,CURLOPT_POSTFIELDS, post_data);
    curl_easy_setopt(easy_handle,CURLOPT_POSTFIELDSIZE,sizeof(post_data));
    curl_easy_setopt(easy_handle, CURLOPT_WRITEFUNCTION, appoint_status_save);
    curl_easy_setopt(easy_handle, CURLOPT_WRITEDATA, fp);
    curl_easy_setopt(easy_handle, CURLOPT_HEADERFUNCTION, get_content_length_func);
    
    curl_ret = curl_easy_perform(easy_handle);
    if(curl_ret != CURLE_OK)
    {
        LOG("perform error:%d",curl_ret);
        fclose(fp);
        return -1;
    }

    curl_ret = curl_easy_getinfo(easy_handle,CURLINFO_RESPONSE_CODE,&res_code);
    LOG("curl_easy_getinfo:%d\r\n",curl_ret);
    LOG("CURLINFO_RESPONSE_CODE:%d\r\n",res_code);

    ret = appoint_status_parser();
    if(ret < 0)
    {
        st_curl_ctx* ctx = curl_ctx_acquire(TRUE);
        if(ctx->appoint_status_content) free(ctx->appoint_status_content);
        if(ctx->clinic_label_id) free(ctx->clinic_label_id);
        ctx->appoint_status_content = NULL;
        ctx->clinic_label_id = NULL;
        ctx = curl_ctx_release(ctx);
    }
    
    fclose(fp);
    return ret;
}


int get_book_model(CURL *easy_handle)
{
    CURLcode curl_ret;
    char post_data[1024] = {0};
    long res_code;
    char* url = NULL;

    st_curl_ctx* ctx = curl_ctx_acquire(FALSE);
    
    sprintf(post_data,
        "clinicLabelId=%s \
        &timePartType=%d \
        &clinicDate=%s \
        &ScheduleItemCodeIn=%s \
        &HisStartTime=%s \
        &HisEndTime=%s \
        &RQHisContIn=%s \
        &",
        ctx->clinic_label_id,
        ctx->time_part_type,
        ctx->clinic_date,
        ctx->schedule_item_code,
        ctx->his_start_time,
        ctx->his_end_time,
        ctx->rQhis_count);
    ctx = curl_ctx_release(ctx);
    printf("post_data:[%s]\r\n",post_data);
    curl_easy_reset(easy_handle);
    curl_easy_setopt(easy_handle,CURLOPT_URL,URL_BOOK_MODEL);
    curl_easy_setopt(easy_handle,CURLOPT_POSTFIELDS, post_data);
    curl_easy_setopt(easy_handle,CURLOPT_POSTFIELDSIZE,sizeof(post_data));
    curl_easy_setopt(easy_handle, CURLOPT_WRITEFUNCTION, book_model_callback);
    curl_easy_setopt(easy_handle, CURLOPT_WRITEDATA, stdout);
    curl_easy_setopt(easy_handle, CURLOPT_HEADERFUNCTION, get_book_model_length_func);

    curl_easy_setopt(easy_handle, CURLOPT_FOLLOWLOCATION, 1);
    curl_ret = curl_easy_getinfo(easy_handle,CURLINFO_REDIRECT_URL,&url);
    LOG("curl_easy_getinfo:%d",curl_ret);
    
    curl_ret = curl_easy_perform(easy_handle);
    if(curl_ret != CURLE_OK)
    {
        LOG("perform error:%d",curl_ret);
        return -1;
    }

    curl_ret = curl_easy_getinfo(easy_handle,CURLINFO_RESPONSE_CODE,&res_code);
    LOG("curl_easy_getinfo:%d\r\n",curl_ret);
    LOG("curl_easy_getinfo:%d\r\n",res_code);

    book_model_parser();
    
    return 0;
}


int book_ticket_model(CURL *easy_handle)
{
    CURLcode curl_ret;
    char post_data[1024] = {0};
    long res_code;
    char* url = NULL;
    int i,j;
    
    st_curl_ctx* ctx = curl_ctx_acquire(FALSE);

    for(i = 0;i < ctx->person_num;i ++)
    {
        if(0 == strcmp(ctx->person_info[i].IdNumber,SELECT_ID))
        {
            LOG("find,%s!",ctx->person_info[i].IdNumber);
            break;
        }
    }

    for(j = 0;j < ctx->time_part_num;j ++)
    {
        if(0 == strcmp(ctx->time_part[j].TimePart,PREFER_TIME))
        {
            LOG("find,%s!",ctx->time_part[j].TimePart);
            break;
        }
    }
    
    sprintf(post_data,
        "param=orderAPI\
&hospitalId=61010017\
&patientId=%s\
&clinicLabelId=%s\
&clinicDate=%s \
&timePartType=%d\
&timePart=\
&channcelType=%d\
&rsvmodel=%d\
&returnVisitId=%d\
&symptom=\
&cardnum=%s\
&TimePartStartTime=%s\
&TimePartEndTime=%s\
&ScheduleItemCode=%s\
&payChanel=%d\
",
        ctx->person_info[i].PersonId,
        ctx->clinic_label_id,
        ctx->clinic_date,
        ctx->time_part_type,
        ctx->channel_type_id,
        ctx->rsv_model,
        ctx->visit_type,
        ctx->person_info[i].IdNumber,
        ctx->time_part[0].TimePart,
        ctx->time_part[0].TimePart,
        ctx->schedule_item_code,
        ctx->pay_channel
        );
    ctx = curl_ctx_release(ctx);
    printf("post_data:[%s]\r\n",post_data);
#if 0
    curl_easy_reset(easy_handle);
    curl_easy_setopt(easy_handle,CURLOPT_URL,URL_BOOK_TICKET);
    curl_easy_setopt(easy_handle,CURLOPT_POSTFIELDS, post_data);
    curl_easy_setopt(easy_handle,CURLOPT_POSTFIELDSIZE,sizeof(post_data));
    curl_easy_setopt(easy_handle, CURLOPT_WRITEFUNCTION, write_data);
    curl_easy_setopt(easy_handle, CURLOPT_WRITEDATA, stdout);
    //curl_easy_setopt(easy_handle, CURLOPT_HEADERFUNCTION, get_book_model_length_func);

    curl_easy_setopt(easy_handle, CURLOPT_FOLLOWLOCATION, 1);
    
    curl_ret = curl_easy_perform(easy_handle);
    if(curl_ret != CURLE_OK)
    {
        LOG("perform error:%d",curl_ret);
        return -1;
    }

    curl_ret = curl_easy_getinfo(easy_handle,CURLINFO_RESPONSE_CODE,&res_code);
    LOG("curl_easy_getinfo:%d\r\n",curl_ret);
    LOG("curl_easy_getinfo:%d\r\n",res_code);
#endif
    
    return 0;
}


void open_webservice(void)
{
    CURL *easy_handle = curl_easy_init();
    CURLcode curl_ret;
    FILE* code_fp = NULL;
    char code_data[128] = {0};    
    struct curl_slist *headers = NULL;
    s32 ret;
    
    if(easy_handle == NULL)
    {
        LOG("null pointer");
        return ;
    }

    unlink(PATH_MAIN_INTERFACE);
    unlink(PATH_VER_CODE);
    unlink(PATH_LOGIN_INTERFACE);

    headers = curl_slist_append(headers, "Accept: Agent-007");
    curl_easy_setopt(easy_handle, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(easy_handle, CURLOPT_COOKIEFILE, "./cookie.txt"); // 指定cookie文件
    
#if 1
    printf("enter main interface\r\n");
    enter_login_interface(easy_handle);

    get_verify_code(easy_handle);
    
#if 0
    unlink("./result.txt");
    system("tesseract verfycode.jpg result");
    sleep(2);


    code_fp = fopen("result.txt","r");
    if(NULL == code_fp) goto end;
    
    fread(code_data,sizeof(code_data),1,code_fp);
    printf("code:[%s]\r\n",code_data);
    code_filter(code_data);
    printf("code:[%s]\r\n",code_data);
#else
    printf("pls input VerCode:");
    scanf("%s",code_data);
    printf("code:[%s]\r\n",code_data);

#endif
    post_pwd(easy_handle,code_data);


    enter_main_index(easy_handle);
#endif

    while(1)
    {
        ret = enter_doctor_interface(easy_handle,DOCTOR_ID_TEST);
        if(ret != 0)
        {
            LOG("try again....");
            sleep(1);
        }else
        {
            break;
        }
    }

    get_book_model(easy_handle);

    book_ticket_model(easy_handle);
    
    commons_println("login finish");

    //curl_easy_setopt(easy_handle,CURLOPT_URL,URL_LOGIN);
    //curl_easy_setopt(easy_handle, CURLOPT_WRITEFUNCTION, write_data);
    //curl_easy_setopt(easy_handle, CURLOPT_WRITEDATA, fp);
    //curl_ret = curl_easy_perform(easy_handle);

end:
    if(code_fp) fclose(code_fp);
    curl_easy_cleanup(easy_handle);
    curl_global_cleanup();
}




