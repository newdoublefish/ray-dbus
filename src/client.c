#include "dbus_xk.h"
void onReceiveSignal(const char * inStr)
{
   printf("onReceiveSignal::%s\n",inStr);
}

static gboolean doCall(gpointer arg)
{ 
     char *out; 	
     rayDbusCall("com.demo.service","from client",&out); 
     printf("doCall:%s\n",out);
     free(out);
     return TRUE;
}

int main(int argc,char *argv[])
{

  GMainLoop*	mainLoop;	
  rayDbusInit();
  mainLoop = g_main_loop_new (NULL, FALSE);
  rayDbusRegisterSignalReceiver("com.demo.service",onReceiveSignal);
  g_timeout_add (2000,doCall, NULL);
  g_main_loop_run (mainLoop);
  return 0;
}
