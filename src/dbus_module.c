#include <dbus/dbus-glib.h>
#include <stdio.h>
#include <stdlib.h>
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

gboolean
dbus_module_emit_send_signal (DbusModule *obj,char *message)
{
  printf("-----sendSignal-----");
  g_signal_emit (obj, signals[SEND_SIGNAL], 0,message);
  return TRUE;
}



gboolean dbus_module_request(DbusModule *obj, const gint IN_arg0, const GArray* IN_arg1, GArray** OUT_arg2, gint* OUT_arg3, GError **error)
{
  gint temp=0;
  printf("%d\n",IN_arg0);
  * OUT_arg2=g_array_new(FALSE, FALSE, sizeof(char));
  for(temp=0;temp<IN_arg0;temp++)
  {
	printf("%d\n", g_array_index(IN_arg1, char,temp));
	g_array_append_val(* OUT_arg2,temp);
  }
  * OUT_arg3=8;
  dbus_module_emit_send_signal(obj,"signal dbus_module_request");
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


int main(int argc,char **argv)
{
  DBusGConnection *bus;
  DBusGProxy *bus_proxy;
  GError *error = NULL;
  DbusModule *obj;
  GMainLoop *mainloop;
  guint request_name_result;
  g_type_init ();
  dbus_g_object_type_install_info (DBUS_TYPE_MODULE, &dbus_glib_dbus_module_object_info);
  mainloop = g_main_loop_new (NULL, FALSE);
  bus = dbus_g_bus_get (DBUS_BUS_SESSION, &error);
  if (!bus)
    lose_gerror ("Couldn't connect to session bus", error);  
 
  bus_proxy = dbus_g_proxy_new_for_name (bus, "org.freedesktop.DBus",
					 "/org/freedesktop/DBus",
					 "org.freedesktop.DBus");

  if (!dbus_g_proxy_call (bus_proxy, "RequestName", &error,
			  G_TYPE_STRING, "com.xk.service",
			  G_TYPE_UINT, 0,
			  G_TYPE_INVALID,
			  G_TYPE_UINT, &request_name_result,
			  G_TYPE_INVALID))
    lose_gerror ("Failed to acquire com.xk.service", error); 
	obj=g_object_new(DBUS_TYPE_MODULE,NULL);
	dbus_g_connection_register_g_object (bus, "/DbusModule", G_OBJECT (obj));
	printf ("service running\n");
	g_main_loop_run (mainloop);
	exit (0);
}
