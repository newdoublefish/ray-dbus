#ifndef DBUS_XK_H
#define DBUS_XK_H
#include <stdio.h> 
#include <stdlib.h> 
#include <dbus/dbus-glib.h>

#define MAX_RAY_DBUS_CONN_NAME 30
#define MAX_RAY_DBUS_OBJ_NAME 30
typedef struct
{
	char connName[MAX_RAY_DBUS_CONN_NAME];
	char objectName[MAX_RAY_DBUS_OBJ_NAME];
	GMainLoop *loop;
	void *obj;
	void *onRequest;
	void *onCall;
}tRayDbusServer;

typedef void (*ON_REQUEST_FUNC)(const gint inCnt, const GArray* inArray, GArray** outArray, gint* outCnt);
typedef void (*ON_CALL_FUNC)(const char * inStr, char ** outStr);

void rayDbusInit(void);
gboolean registerDbusServer(tRayDbusServer *t);
void rayDbusBroadCast(char *connName,char *message);
void releaseRayDbus(char *connName);
void rayDbusBroadCast(char *connName,char *message);
#endif
