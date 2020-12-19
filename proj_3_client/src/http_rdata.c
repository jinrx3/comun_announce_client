#include "http_rdata.h"
int *lcd_p;
font *f;

void http_rdata(char *data)
{
	int sockfd = tcp_connect("121.32.228.42","80");
	//58.222.18.2/47.107.155.132
	//重点！！定制HTTP 请求协议  
	//https:// api.qingyunke.com /api.php?key=free&appid=0&msg=广州天气 
	//http: //t.weather.itboy.net/api/weather/city/101280101
	//https:// v1.alapi.cn/api/new/ toutiao
	
	char  http[4096]={0}; 
	sprintf(http,"GET /api/new/%s HTTP/1.1\r\nHost:v1.alapi.cn\r\n\r\n",data);

	//发送数据给服务器 
	write(sockfd,http,strlen(http));
	
	//获取服务器的信息 
	char  buf[4096*5]={0}; 
	int size=read(sockfd,buf,4096*5);
	
	printf("size=%d head=%s\n",size,buf);
	//指向文件的末尾 
	char *end =  strstr(buf,"{"); 
	
	cJSON* cjson_new = NULL;
    cJSON* cjson_title = NULL;
    cJSON* cjson_digest = NULL;
    cJSON* cjson_source = NULL;
    cJSON* cjson_time = NULL;
	cJSON* cjson_data = NULL;
	int    data_array_size = 0, i = 0;
    cJSON* cjson_data_item = NULL;
	
	//把数据转换成 JSON 对象  
	cjson_new=cJSON_Parse(end);
	if(cjson_new == NULL)
    {
        printf("parse fail.\n");
        return ;
    }
	
	cjson_data = cJSON_GetObjectItem(cjson_new, "data");
    //data_array_size = cJSON_GetArraySize(cjson_data);
	
	//0为数组索引
	cjson_data_item = cJSON_GetArrayItem(cjson_data, 0);
	//printf("%s,", cjson_data_item->valuestring);
	
	//标题
	cjson_title=cJSON_GetObjectItem(cjson_data_item,"title");
	//摘要
	cjson_digest=cJSON_GetObjectItem(cjson_data_item,"digest");
	//来源
	cjson_source=cJSON_GetObjectItem(cjson_data_item,"source");
	//时间
	cjson_time=cJSON_GetObjectItem(cjson_data_item,"time");

    char word_new[256];
	sprintf(word_new," 标题：%s\n 摘要：%s\n 来源：%s\n 时间：%s\n ",cjson_title->valuestring,cjson_digest->valuestring,cjson_source->valuestring,cjson_time->valuestring);
	
	//字体大小的设置
	fontSetSize(f,32);
	
	//创建一个画板（点阵图）
	bitmap *bm = createBitmapWithInit(800,480,4,getColor(0,255,255,255));
	
	//将字体写到点阵图上
	fontPrint(f,bm,100,10,word_new,getColor(0,255,0,0),630);
	//printf("size =%d  buf=%s\n",size,end);
	
	//把字体框输出到LCD屏幕上
	show_font_to_lcd(lcd_p,0,0,bm);
	
	//关闭画板
	destroyBitmap(bm);
	
}

void http_rdata1(char *data,int num)
{



	int sockfd = tcp_connect("58.222.18.2","80");
	//58.222.18.2/47.107.155.132
	//重点！！定制HTTP 请求协议  
	//https:// api.qingyunke.com /api.php?key=free&appid=0&msg=广州天气 
	//http: //t.weather.itboy.net/api/weather/city/101280101
	//https:// v1.alapi.cn/api/new/ toutiao
	char  http[4096]={0}; 
	sprintf(http,"GET /api/weather/city/%s HTTP/1.1\r\nHost:t.weather.itboy.net\r\n\r\n",data);
LABLE:
{
	
	//发送数据给服务器 
	write(sockfd,http,strlen(http));
	
	
	//获取服务器的信息 
	char  buf[4096*5]={0}; 
	int size=read(sockfd,buf,4096*5);
	
	//printf("size=%d head=%s\n",size,buf);
	//跳过头数据
	char *end =  strstr(buf,"\n{"); 
	printf("size=%d end=%s\n",size,end);
	
	//把数据转换成 JSON 对象  
	cJSON *obj=cJSON_Parse(end);
	if(obj == NULL)
    {
        //printf("parse fail.\n");
       // return ;
	   goto LABLE;
    }

	
	int i=0;
	//字体大小的设置
	fontSetSize(f,32);
		
	//创建一个画板（点阵图）
	bitmap *bm = createBitmapWithInit(800,480,4,getColor(0,255,255,255));
	
	for(i=0;i<2;i++)
	{
		char date[100];//日期
		char time[100];//当前时间
		char city[100];//城市
		char week[100];//星期
		char weather[100];//天气
		char low[100];//低温
		char high[100];//高温
		char notice[100];//建议
		
		cJSON *new_obj=cJSON_GetObjectItem(obj,"cityInfo");
		cJSON *value=cJSON_GetObjectItem(new_obj,"city"); 	
		strcpy(city,cJSON_Print(value));
		cJSON *obj_1=cJSON_GetObjectItem(obj,"data");
		cJSON *obj_arry=cJSON_GetObjectItem(obj_1,"forecast"); 
		cJSON *obj_arry_0=cJSON_GetArrayItem(obj_arry,num);	

		//日期
		value=cJSON_GetObjectItem(obj_arry_0,"ymd"); 
		strcpy(date,cJSON_Print(value));
		//时间
		value=cJSON_GetObjectItem(obj,"time"); 
		strcpy(time,cJSON_Print(value));
		//星期
		value=cJSON_GetObjectItem(obj_arry_0,"week"); 
		strcpy(week,cJSON_Print(value));
		//天气
		value=cJSON_GetObjectItem(obj_arry_0,"type"); 
		strcpy(weather,cJSON_Print(value));
		//低温
		value=cJSON_GetObjectItem(obj_arry_0,"low"); 
		strcpy(low,cJSON_Print(value));
		//高温
		value=cJSON_GetObjectItem(obj_arry_0,"high"); 
		strcpy(high,cJSON_Print(value));
		//建议
		value=cJSON_GetObjectItem(obj_arry_0,"notice"); 
		strcpy(notice,cJSON_Print(value));
		char word_weather[256];
		sprintf(word_weather," 天气更新时间：%s\n 日期：%s\n 城市：%s\n 星期：%s\n 天气:%s\n %s %s\n %s\n",time,date,city,week,weather,low,high,notice);
	
		
		//将字体写到点阵图上
		fontPrint(f,bm,100,10+i*240,word_weather,getColor(rand()%255,rand()%255,rand()%255,rand()%255),630);
		//printf("size =%d  buf=%s\n",size,end);
		
		num++;
		
	}
	//把字体框输出到LCD屏幕上
	show_font_to_lcd(lcd_p,0,0,bm);
	//关闭画板
		destroyBitmap(bm);
}	
}