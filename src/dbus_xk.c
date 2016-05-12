#include <stdio.h> 
#include <stdlib.h> 
#include <dbus/dbus-glib.h>
#include <dbus/dbus.h>
#include "dbus_xk.h"

GSList *gsList=NULL;

enum
{
	SEND_SIGNAL,
	LAST_SIGNAL
};

static guint signals[LAST_SIGNAL]={0};

static void lose (const char *fmt, ...) G_GNUC_NORETURN G_GNUC_PRINTF (1, 2);
static void lose_gerror (const char *prefix, GError *error) G_GNUC_NORETURN;
static void lose (const char *str, ...)
{
  va_list args;

  va_start (args, str);

  vfprintf (stderr, str, args);
  fputc ('\n', stderr);

  va_end (args);

  exit (1);
}
static void lose_gerror (const char *prefix, GError *error) 
{
  lose ("%s: %s", prefix, error->message);
}
//-------------------------------
#define  _DBUS_POINTER_UNSHIFT(p) ((void*) (((char*)p) - sizeof (void*)))

#define DBUS_CONNECTION_FROM_G_CONNECTION(x)    ((DBusConnection*) _DBUS_POINTER_UNSHIFT(x))


void dbus_g_connection_close(DBusGConnection *  connection )

{

return dbus_connection_close(DBUS_CONNECTION_FROM_G_CONNECTION(connection));

}   


//


typedef struct DbusModule DbusModule;
typedef struct DbusModuleClass DbusModuleClass;

GType Dbus_Module_get_type (void);

struct DbusModule
{
  GObject parent;
};

struct DbusModuleClass
{
  GObjectClass parent;
};

#define DBUS_TYPE_MODULE              (dbus_module_get_type ())
#define DBUS_MODULE(object)           (G_TYPE_CHECK_INSTANCE_CAST ((object), DBUS_TYPE_MODULE, DbusModule))
#define DBUS_MODULE_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST ((klass), DBUS_TYPE_MODULE, DbusModuleClass))
#define DBUS_IS_MODULE(object)        (G_TYPE_CHECK_INSTANCE_TYPE ((object), DBUS_TYPE_MODULE))
#define DBUS_IS_MODULE_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), DBUS_TYPE_MODULE))
#define DBUS_MODULE_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS ((obj), DBUS_TYPE_MODULE, DbusModuleClass))

G_DEFINE_TYPE(DbusModule, dbus_module, G_TYPE_OBJECT)






gboolean dbus_module_request(DbusModule *obj, const gint IN_arg0, const GArray* IN_arg1, GArray** OUT_arg2, gint* OUT_arg3, GError **error);
gboolean dbus_module_call(DbusModule *obj, const char * IN_arg0, char ** OUT_arg1, GError **error);

#include "dbus_interface.h"

static void dbus_module_init (DbusModule *obj)
{
}

static void dbus_module_class_init (DbusModuleClass *klass)
{
  signals[SEND_SIGNAL] =
	  g_signal_new ("send_signal", 		
	  G_OBJECT_CLASS_TYPE (klass),				  
	  G_SIGNAL_RUN_LAST | G_SIGNAL_DETAILED,				  
	  0,				  
	  NULL, NULL,				   
	  g_cclosure_marshal_VOID__STRING, 				 
	  G_TYPE_NONE, 1, G_TYPE_STRING);

}




tRayDbusServer *getRayDbusServerByObj(DbusModule *obj)
{
    GSList *iterator = NULL;
    for (iterator = gsList; iterator; iterator = iterator->next) {	
		tRayDbusServer *t=(tRayDbusServer*)(iterator->data);
		printf("-------%s\n",t->connName);
		if((DbusModule*)(t->obj)==obj)
		{
			printf("---find----\n");
			return t;
		}
    }
	return NULL;
}

tRayDbusServer *getRayDbusServerByName(char *connName)
{
    GSList *iterator = NULL;
    for (iterator = gsList; iterator; iterator = iterator->next) {	
		tRayDbusServer *t=(tRayDbusServer*)(iterator->data);
		//printf("-------%s\n",t->connName);
		if(strcmp(connName,t->connName)==0)//(DbusModule*)(t->obj)==obj)
		{
			printf("---find----\n");
			return t;
		}
    }
	return NULL;
}

gboolean dbus_module_emit_send_signal (DbusModule *obj,char *message)
{
 	printf("-----sendSignal-----\n");
  	g_signal_emit (obj, signals[SEND_SIGNAL], 0,message);
  	return TRUE;
}

void rayDbusBroadCast(char *connName,char *message)
{
	tRayDbusServer *ts=getRayDbusServerByName(connName);
	printf("-----sendSignal1-----\n");
	dbus_module_emit_send_signal((DbusModule*)(ts->obj),message);
}




gboolean dbus_module_request(DbusModule *obj, const gint IN_arg0, const GArray* IN_arg1, GArray** OUT_arg2, gint* OUT_arg3, GError **error)
{
  //dbus_module_emit_send_signal(obj,"signal dbus_module_request");
  tRayDbusServer *ts=getRayDbusServerByObj(obj);
  (*(ON_REQUEST_FUNC)ts->onRequest)(IN_arg0,IN_arg1,OUT_arg2,OUT_arg3);
  
  
  return TRUE;
}
static int count=0;
gboolean dbus_module_call(DbusModule *obj, const char * IN_arg0, char ** OUT_arg1, GError **error)
{
 	char buffer[200]={0};
	printf("%s\n",IN_arg0);
	*OUT_arg1=g_strdup ("dbus_module_call");
	sprintf(buffer,"dbus_module_call:count:%d",count++);
	dbus_module_emit_send_signal(obj,buffer);
	return TRUE;

}


DBusGConnection *getSessionBus(GMainContext* context)
{
	GError *error = NULL;
	DBusGConnection *sessionBus=NULL;
	if(sessionBus==NULL)
	{
		//sessionBus = dbus_g_bus_get (DBUS_BUS_SESSION, &error);
		sessionBus=dbus_g_bus_get_private(DBUS_BUS_SESSION,context,&error);
		if (!sessionBus)
		{
		  lose_gerror ("Couldn't connect to session bus", error);
		  sessionBus=NULL;
		  g_error_free( error); 
		}
	}
	return sessionBus;
}
GMainContext *context;

void serverThreadCall(gpointer data)
{
	tRayDbusServer *tServer=(tRayDbusServer *)data;
	GError *error = NULL;
	GMainContext* context;
	DBusGProxy *bus_proxy;
	DbusModule *obj;
	guint request_name_result;
	context = g_main_context_new();
	GMainLoop *loop = g_main_loop_new(context,FALSE);
	if(!context)
    {
       return;
    }
	dbus_g_object_type_install_info (DBUS_TYPE_MODULE, &dbus_glib_dbus_module_object_info);
	DBusGConnection *sessionbuss=getSessionBus(context);
	//dbus_connection_setup_with_g_main(sessionbuss, context);
  	bus_proxy = dbus_g_proxy_new_for_name (sessionbuss, "org.freedesktop.DBus",
					 "/org/freedesktop/DBus",
					 "org.freedesktop.DBus");
  	if (!dbus_g_proxy_call (bus_proxy, "RequestName", &error,
			  G_TYPE_STRING,tServer->connName,
			  G_TYPE_UINT, 0,
			  G_TYPE_INVALID,
			  G_TYPE_UINT, &request_name_result,
			  G_TYPE_INVALID))
    lose_gerror ("Failed to acquire com.xk.service", error); 
	obj=g_object_new(DBUS_TYPE_MODULE,NULL);
	dbus_g_connection_register_g_object (sessionbuss,tServer->objectName, G_OBJECT (obj));
	tServer->obj=(void *)obj;
	tServer->loop=loop;
	printf("service running\n");     
    if(!loop)
    {
      return;
    }
	gsList = g_slist_append(gsList,(gpointer)tServer);
	g_main_loop_run(loop);
	printf("-------------quit--------------\n");
	dbus_g_connection_close(sessionbuss);
	dbus_g_connection_unref(sessionbuss);
	g_main_context_unref(context);
}

gboolean registerDbusServer(char *connName,char *objName,void *onRequest,void *onCall)
{
	GError*  error = NULL;
	GThread* thread = NULL;
	tRayDbusServer *t=(tRayDbusServer*)malloc(sizeof(tRayDbusServer));
	sprintf(t->connName,"%s",connName);
	sprintf(t->objectName,"%s",objName);
	t->onRequest=onRequest;
	t->onCall=onCall;
	thread = g_thread_create((GThreadFunc)serverThreadCall, t, FALSE, &error);	
}

void releaseRayDbus(char *connName)
{
	tRayDbusServer *tt=getRayDbusServerByName(connName);
	g_main_loop_quit (tt->loop);	
}


void rayDbusInit()
{
	g_type_init ();
	if (!g_thread_supported ()) g_thread_init (NULL);
	dbus_g_thread_init();	
}

/*
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
	g_main_loop_run (mainLoop);
	return 0;
}*/

