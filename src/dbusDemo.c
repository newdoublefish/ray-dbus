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
	releaseRayDbus("com.xk.service");
}

void onCall(const char * IN_arg0, char ** OUT_arg1)
{
	printf("-----onCall-------\n");
}

int main(int argc,char *argv[])
{
	GMainLoop*	mainLoop;
	rayDbusInit();
	mainLoop = g_main_loop_new (NULL, FALSE);
	if (!g_thread_supported ()) g_thread_init (NULL);
    registerDbusServer("com.xk.service","/DbusModule",onRequest,onCall);
	registerDbusServer("com.zz.service","/Hello",onRequest,onCall);
	g_main_loop_run (mainLoop);
	return 0;
}

