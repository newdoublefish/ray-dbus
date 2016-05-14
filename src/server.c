#include "dbus_xk.h"

void onCall(const char * inStr, char ** outStr)
{
	printf("onCall:%s\n",inStr);
	* outStr=g_strdup ("call success from server");
	rayDbusBroadCast("com.demo.service","broadCaseFromServer!!!");
}

int main(int argc,char *argv[])
{
	GMainLoop*	mainLoop;
	rayDbusInit();
	mainLoop = g_main_loop_new (NULL, FALSE);
	if (!g_thread_supported ()) g_thread_init (NULL);
	registerDbusServer("com.demo.service","/DbusModule",NULL,onCall);
	g_main_loop_run (mainLoop);
	return 0;
}
