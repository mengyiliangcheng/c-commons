#include <curl/curl.h>
#include <stdio.h>
#include <stdlib.h>
#include "osadapter.h"
#include "commons_log.h"

#define LOG(...) COMMONS_LOG("CURL",__VA_ARGS__);

#define URL_LOGIN  "http://2yuanyy.com/Personal/Login"
#define URL_MAIN   "http://2yuanyy.com/Home/Index"
#define URL_VERFY_CODE "http://2yuanyy.com/verify_Image.ashx"
#define URL_PYC    "http://2yuanyy.com/Passport/LoginMethod?LoginName=340881199308270615&LoginPwd=pp265358&LoginType=0&VerCode=1742"
#define URL_POST_PWD  "http://2yuanyy.com/Passport/LoginMethod"

#define URL_PWD_USRNAME "340881199308270615"
#define URL_PWD_PWD     "pp265358"

#define PATH_MAIN_INTERFACE "./jiaoda.html"
#define PATH_VER_CODE "./verfycode.jpg"
#define PATH_LOGIN_INTERFACE "./login.html"

size_t write_data(void *buffer, size_t size, size_t nmemb, void *userp)
{
    commons_println("get data size:%d,nmemb:%d",size,nmemb);

    FILE *fp = (FILE *)userp;
    size_t return_size = fwrite(buffer, size, nmemb, fp);
    return return_size;
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

void open_webservice(void)
{
    CURL *easy_handle = curl_easy_init();
    CURLcode curl_ret;
    FILE* code_fp = NULL;
    char code_data[128] = {0};    
    struct curl_slist *headers = NULL;

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
    curl_easy_setopt(easy_handle, CURLOPT_COOKIEFILE, "./cookie.txt"); // ???¡§cookie???t
    

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




