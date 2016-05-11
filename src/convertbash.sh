#create interface include file
dbus-binding-tool --mode=glib-server --prefix=dbus_module dbus_interface.xml>dbus_interface.h
#create request include file
dbus-binding-tool --mode=glib-client --prefix=dbus_module dbus_interface.xml>dbus_request.h