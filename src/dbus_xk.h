#ifndef DBUS_XK_H
#define DBUS_XK_H
#include <stdio.h> 
#include <stdlib.h> 
#include <dbus/dbus-glib.h>

#define MAX_RAY_DBUS_CONN_NAME 30
#define MAX_RAY_DBUS_OBJ_NAME 30

#define SERVER_TYPE 1
#define SIGNAL_TYPE 2
#define CLIENT_TYPE 3

typedef struct
{
	char connName[MAX_RAY_DBUS_CONN_NAME];
	char objectName[MAX_RAY_DBUS_OBJ_NAME];
	GMainLoop *loop;
	void *obj;
	void *onRequest;
	void *onCall;
	void *onSignal;
	int type;
}tRayDbusServer;

typedef void (*ON_REQUEST_FUNC)(const gint inCnt, const GArray* inArray, GArray** outArray, gint* outCnt);
typedef void (*ON_CALL_FUNC)(const char * inStr, char ** outStr);
typedef void (*ON_SIGNAL_FUNC)(const char *message);

void rayDbusInit(void);
gboolean registerDbusServer(char *connName,char *objName,void *onRequest,void *onCall);
void rayDbusBroadCast(char *connName,char *message);
void releaseRayDbus(char *connName);
void rayDbusBroadCast(char *connName,char *message);


void rayDbusRegisterSignalReceiver(char *connName,void *onSignal);
void rayDbusCall(char *connName,char *inpuStr,char **outputStr);


#endif
