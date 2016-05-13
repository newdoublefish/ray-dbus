#include "dbus_xk.h"

void onRequest(const gint IN_arg0, const GArray* IN_arg1, GArray** OUT_arg2, gint* OUT_arg3)
{
	printf("------onRequest----\n");
  	gint temp=0;
  	printf("%d\n",IN_arg0);
  	* OUT_arg2=g_array_new(FALSE, FALSE, sizeof(char));
  	for(temp=0;temp<IN_arg0;temp++)
  	{
		printf("%d\n", g_array_index(IN_arg1, char,temp));
		g_array_append_val(* OUT_arg2,temp);
  	}
  	* OUT_arg3=8;	
	//rayDbusBroadCast("com.xk.service","hello world\n");
	//releaseRayDbus("com.xk.service");
}

void onCall(const char * inStr, char ** outStr)
{
	//printf("-----onCall-------\n");
	printf("---------------:%s\n",inStr);
	char *buffer=(char *)malloc(10);
	sprintf(buffer,"%s","oncall");
	outStr=&buffer;
}

void onReceiveSignal(const char * inStr)
{
	printf("onReceiveSignal::%s\n",inStr);
}


void Usage()
{
	printf("-------------rayDbusUseage------------\n");
	printf("1 create server\n");
	printf("2 register signal receiver\n");
	printf("3 request\n");
	printf("4 sendSignal\n");
	printf("5 call\n");
	printf("--------------------------------------\n");
}
int state=0;
static char serverConnName[20]={0};
void process(char *str)
{
	/*if(strcmp(str,"cancel")==0)
	{
		Usage();
		state=0;
		return;
	}
	if(strcmp(str,"help")==0)
	{
		Usage();
	}else*/
	{
		if(state==0)
		{
			printf("inputCommand:%s\n",str);
			if(atoi(str)==1)
			{
				printf("input Server Name:\n");
				state=1;
			}else if(atoi(str)==2)
			{
				printf("register signal receiver:\n");
				state=2;
			}else if(atoi(str)==3)
			{
				state=3;
			}else if(atoi(str)==4)
			{
				state=4;
			}else if(atoi(str)==5)
			{
				state=5;
			}else{
				printf("invalid Command\n");
				state=0;
			}
		}else if(state==1)
		{
			
			str[strlen(str)-1]=0;
			registerDbusServer(str,"/DbusModule",onRequest,onCall);
			sprintf(serverConnName,"%s",str);
			state=0;
		}else if(state==2)
		{
			str[strlen(str)-1]='\0';
			rayDbusRegisterSignalReceiver(str,onReceiveSignal);
			state=0;
		}else if(state==3)
		{
			printf("request:%s\n",str);
			state=0;
		}else if(state==4)
		{
			printf("sendSignal:%s\n",str);
			rayDbusBroadCast(serverConnName,str);
			state=0;
		}else if(state==5)
		{
			char *out;
			str[strlen(str)-1]='\0';
			rayDbusCall(str,"zaza",&out);
			printf("%s\n",out);
			free(out);
			state=0;
		}
	}
}

static gboolean channel_cb(GIOChannel *source, GIOCondition condition, gpointer data)
{
	gchar* str;    
	gsize len;
	g_io_channel_read_line(source, &str, &len, NULL, NULL); 
	//printf("%s",str);
	process(str);
	g_free(str); 
	return TRUE;
}

int main(int argc,char *argv[])
{
	guint source;
	GIOChannel *chan;
	GMainLoop*	mainLoop;
	rayDbusInit();
	mainLoop = g_main_loop_new (NULL, FALSE);
	if (!g_thread_supported ()) g_thread_init (NULL);
	chan = g_io_channel_unix_new(1);
	source = g_io_add_watch(chan, G_IO_IN, channel_cb, NULL);
	g_io_channel_unref(chan); 
	Usage();
	g_main_loop_run (mainLoop);
	return 0;
}

