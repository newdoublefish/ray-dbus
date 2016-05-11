/*  Test for invoke method asynchronously */ 
#include <stdio.h> 
#include <stdlib.h> 
#include <dbus/dbus-glib.h>
#include "dbus_request.h"

static void send_signal_callback(DBusGProxy *proxy,const char *string,gpointer user_data)
{
	printf("receive hello signal %s/n",string);
}

static gboolean emit_signal(gpointer arg)
{  	
	GError * error = NULL; 
	char *out_arg1;
	DBusGProxy *proxy = arg;    
	com_xk_interface_call (proxy,"hello world",&out_arg1,&error);
	printf("--------------%s\n",out_arg1);
	free(out_arg1); 
	return TRUE;
}


int main( int argc , char ** argv) 
{ 
    GError * error = NULL; 
    DBusGConnection * connection; 
    DBusGProxy    * proxy;
    char ***outArg;
	char *out_arg1;
	GArray *gInputArray;
	GArray *gOutputArray=NULL;
	gint outputArg=0;
	gint inputArg=8;
	GMainLoop *mainloop;
    
    g_type_init(); 
    //main_loop = g_main_loop_new(NULL,TRUE);
    mainloop = g_main_loop_new (NULL, FALSE);
    connection = dbus_g_bus_get (DBUS_BUS_SESSION, & error); 
    if(connection == NULL){ 
        g_printerr ("Failed to open connection to bus : %s/n", 
                error->message); 
        g_error_free( error); 
        exit( 1 ); 
    }
    /* Create a proxy object for the 'bus driver' named org.freedesktop.DBus */ 
    proxy = dbus_g_proxy_new_for_name (connection,"com.xk.service","/DbusModule","com.xk.interface"); 

    /*com_xk_interface_request(proxy,"helloworld",outArg,&error);	
	printf("%s,%s\n",(*outArg)[0],(*outArg)[1]);
	*/

	dbus_g_proxy_add_signal(proxy,"SendSignal",G_TYPE_STRING,G_TYPE_INVALID);
	dbus_g_proxy_connect_signal(proxy,"SendSignal",G_CALLBACK(send_signal_callback),NULL,NULL);



	gInputArray=g_array_new(FALSE, FALSE, sizeof(char));
	gchar index=0;
	for(;inputArg>0;inputArg--)
	{
		g_array_append_val(gInputArray,index);
		index++;
	}

	com_xk_interface_request (proxy,8,gInputArray,&gOutputArray,&outputArg,&error);

	
	//printf("---end---\n");
	
	int i=0;
	for(i=0;i<5;i++)
	{
	  emit_signal(proxy);
	}
	g_timeout_add (2000, emit_signal, proxy);
	g_main_loop_run (mainloop);
	return 0; 
}
