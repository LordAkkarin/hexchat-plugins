/**
 * Copyright 2015 Johannes Donath <johannesd@torchmind.com>
 * and other copyright owners as documented in the project's IP log.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <gio/gio.h>
#include <hexchat-plugin.h>
#include "HexchatIdlePlugin.h"

HexchatIdlePlugin::HexchatIdlePlugin (hexchat_plugin* plugin) {
        this->plugin = plugin;
}

void HexchatIdlePlugin::Initialize () {
        hexchat_printf (this->plugin, "Loading %s v%s ...", this->plugin_name, this->plugin_version);

        if (!this->ConnectDBus ()) {
                this->Print ("Could not connect to DBus.");
                this->DisconnectDBus ();
                return;
        }

        // TODO: Debug
        this->Print ("Connected DBus!");

        if (!this->RegisterDBusHooks ()) {
                this->Print ("Could not hook DBus signals.");
                this->UnregisterDBusHooks ();
                return;
        }

        hexchat_printf (this->plugin, "Successfully initialized %s v%s.", this->plugin_name, this->plugin_version);
}

void HexchatIdlePlugin::Shutdown () {
        this->UnregisterDBusHooks ();
        this->DisconnectDBus ();
}

void HexchatIdlePlugin::GetPluginInformation (char** name, char** description, char** version) {
        *name = this->plugin_name;
        *description = this->plugin_description;
        *version = this->plugin_version;
}

bool HexchatIdlePlugin::ConnectDBus () {
        GError* error = nullptr;

        this->dbus_connection = g_bus_get_sync (G_BUS_TYPE_SESSION, NULL, &error);
        if (error != nullptr && error != NULL) {
                hexchat_printf (this->plugin, "Failed to connect DBus: %s", error->message);
                return false;
        }

        this->dbus_proxy = g_dbus_proxy_new_sync (this->dbus_connection, G_DBUS_PROXY_FLAGS_NONE, NULL, "org.gnome.Mutter.IdleMonitor", "/org/gnome/Mutter/IdleMonitor/Core", "org.gnome.Mutter.IdleMonitor", NULL, &error);
        if (error != nullptr && error != NULL) {
                hexchat_printf (this->plugin, "Failed to create DBus proxy: %s", error->message);
                return false;
        }

        return true;
}


void HexchatIdlePlugin::DisconnectDBus () {
        g_object_unref (this->dbus_proxy);
        g_object_unref (this->dbus_connection);

        hexchat_printf (this->plugin, "Successfully unloaded %s v%s.", this->plugin_name, this->plugin_version);
}

void DBusCallbackHelper (GDBusConnection *connection, const gchar *sender_name, const gchar *object_path, const gchar *interface_name, const gchar *signal_name, GVariant *parameters, gpointer user_data) {
        HexchatIdlePlugin* plugin = ((HexchatIdlePlugin*) user_data);
        plugin->DBusCallback (connection, sender_name, object_path, interface_name, signal_name, parameters, user_data);
}

bool HexchatIdlePlugin::RegisterDBusHooks () {
        GError* error = nullptr;

        GVariant* callResult = g_dbus_proxy_call_sync (this->dbus_proxy, "AddIdleWatch", g_variant_new ("(t)", IDLE_TIME), G_DBUS_CALL_FLAGS_NONE, 10000, NULL, &error);
        if (error != nullptr && error != NULL) {
                hexchat_printf (this->plugin, "Failed to register (idle) signal handler: %s", error->message);
                return false;
        }
        g_variant_get (callResult, "(u)", &this->watch_idle);
        g_variant_unref (callResult);

        this->RegisterActivityHook ();

        this->signal_handler = g_dbus_connection_signal_subscribe (this->dbus_connection, NULL, "org.gnome.Mutter.IdleMonitor", "WatchFired", "/org/gnome/Mutter/IdleMonitor/Core", NULL, G_DBUS_SIGNAL_FLAGS_NONE, DBusCallbackHelper, this, NULL);
        if (this->signal_handler == 0) {
                this->Print ("Failed to register signal handler.");
                return false;
        }

        return true;
}

void HexchatIdlePlugin::UnregisterDBusHooks () {
        GError* error = nullptr;

        if (this->signal_handler != 0) {
                g_dbus_connection_signal_unsubscribe (this->dbus_connection, this->signal_handler);
        }

        if (this->watch_idle != 0) {
                GVariant* result = g_dbus_proxy_call_sync (this->dbus_proxy, "RemoveWatch", g_variant_new ("(u)", this->watch_idle), G_DBUS_CALL_FLAGS_NONE, 10000, NULL, &error);
                if (error != nullptr && error != NULL) {
                        this->Print ("Could not properly unregister idle watcher.");
                }

                g_variant_unref (result);
                this->watch_idle = 0;
        }

        this->UnregisterActivityHook ();
}


void HexchatIdlePlugin::DBusCallback (GDBusConnection *connection, const gchar *sender_name, const gchar *object_path, const gchar *interface_name, const gchar *signal_name, GVariant *parameters, gpointer user_data) {
        guint handlerID;
        g_variant_get (parameters, "(u)", &handlerID);

        if (handlerID == this->watch_active) {
                this->SetState (true);
                this->UnregisterActivityHook ();
        } else if (handlerID == this->watch_idle) {
                this->SetState (false);
                this->RegisterActivityHook ();
        }

        g_variant_unref (parameters);
}


void HexchatIdlePlugin::RegisterActivityHook () {
        GError* error = nullptr;
        GVariant* callResult = g_dbus_proxy_call_sync (this->dbus_proxy, "AddUserActiveWatch", NULL, G_DBUS_CALL_FLAGS_NONE, 10000, NULL, &error);

        if (error != nullptr && error != NULL) {
                hexchat_printf (this->plugin, "Failed to register (active) signal handler: %s", error->message);
        } else {
                g_variant_get (callResult, "(u)", &this->watch_active);
                g_variant_unref (callResult);
        }
}

void HexchatIdlePlugin::UnregisterActivityHook () {
        if (this->watch_active != 0) {
                GError* error = nullptr;

                GVariant* result = g_dbus_proxy_call_sync (this->dbus_proxy, "RemoveWatch", g_variant_new ("(u)", this->watch_active), G_DBUS_CALL_FLAGS_NONE, 10000, NULL, &error);
                if (error != nullptr && error != NULL) {
                        this->Print ("Could not properly unregister active watcher.");
                }

                g_variant_unref (result);
                this->watch_active = 0;
        }
}

void HexchatIdlePlugin::SetState (bool state) {
        bool oldState = this->state;
        this->state = state;

        if (oldState != state) {
                if (state) {
                        hexchat_command (this->plugin, "ALLSERV BACK");
                } else {
                        hexchat_command (this->plugin, "ALLSERV AWAY");
                        this->Print ("You have been marked away due to inactivity.");
                }
        }
}

void HexchatIdlePlugin::Print (const char *message) {
        hexchat_print (this->plugin, message);
}
