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

void open_webservice(void)
{
    CURL *easy_handle = curl_easy_init();
    CURLcode curl_ret;
    FILE *fp = NULL;
    FILE* code_fp = NULL;
    FILE* pic_fp = NULL;

    if(easy_handle == NULL)
    {
        LOG("null pointer");
        return ;
    }

    fp = fopen("./jiaoda.html","ab+");
    unlink("./verfycode.jpg");
    pic_fp = fopen("./verfycode.jpg","ab+");
    if(!pic_fp) goto end;
    
    curl_easy_setopt(easy_handle,CURLOPT_URL,URL_LOGIN);
    curl_easy_setopt(easy_handle, CURLOPT_WRITEFUNCTION, write_data);
    curl_easy_setopt(easy_handle, CURLOPT_WRITEDATA, fp);
    curl_ret = curl_easy_perform(easy_handle);
    printf("enter main interface\r\n");

    curl_easy_setopt(easy_handle,CURLOPT_URL,URL_VERFY_CODE);
    curl_easy_setopt(easy_handle, CURLOPT_WRITEFUNCTION, write_data);
    curl_easy_setopt(easy_handle, CURLOPT_WRITEDATA, pic_fp);
    curl_ret = curl_easy_perform(easy_handle);
    if(curl_ret != CURLE_OK)
    {
        LOG("perform error:%d",curl_ret);
        fclose(pic_fp);
        pic_fp = NULL;
        goto end;
    }
    printf("download verfy code\r\n");
    fclose(pic_fp);
    pic_fp = NULL;

    unlink("./result.txt");
    system("tesseract verfycode.jpg result");
    sleep(2);

    code_fp = fopen("result.txt","r");
    if(NULL == code_fp) goto end;
    char code_data[128] = {0};
    fread(code_data,sizeof(code_data),1,code_fp);
    printf("code:[%s]\r\n",code_data);
    code_filter(code_data);
    printf("code:[%s]\r\n",code_data);

    char post_data[1024] = {0};
    sprintf(post_data,"LoginName=312321&LoginPwd=123123&LoginType=0&VerCode=%s",code_data);
    printf("post_data:[%s]\r\n",post_data);
    curl_easy_setopt(easy_handle,CURLOPT_URL,URL_POST_PWD);
    curl_easy_setopt(easy_handle,CURLOPT_POSTFIELDS, post_data);
    curl_easy_setopt(easy_handle,CURLOPT_POSTFIELDSIZE,sizeof(post_data));
    curl_easy_setopt(easy_handle, CURLOPT_WRITEFUNCTION, write_data);
    curl_easy_setopt(easy_handle, CURLOPT_WRITEDATA, stdout);
    curl_ret = curl_easy_perform(easy_handle);
    if(curl_ret != CURLE_OK)
    {
        LOG("perform error:%d",curl_ret);
        goto end;
    }
    commons_println("login succ");

    //curl_easy_setopt(easy_handle,CURLOPT_URL,URL_LOGIN);
    //curl_easy_setopt(easy_handle, CURLOPT_WRITEFUNCTION, write_data);
    //curl_easy_setopt(easy_handle, CURLOPT_WRITEDATA, fp);
    //curl_ret = curl_easy_perform(easy_handle);

end:
    if(fp) fclose(fp);
    if(code_fp) fclose(code_fp);
    if(pic_fp) fclose(pic_fp);
    curl_easy_cleanup(easy_handle);
    curl_global_cleanup();
}




