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
#ifndef HEXCHAT_IDLE_HEXCHATIDLEPLUGIN_H
#define HEXCHAT_IDLE_HEXCHATIDLEPLUGIN_H

#include <gio/gio.h>
#include <hexchat-plugin.h>
#include <string>

#define PLUGIN_NAME "Idle"
#define PLUGIN_DESCRIPTION "Provides a simple plugin that sets your away status based on your idle time."
#define PLUGIN_VERSION "0.1"

// 5 Minutes
#define IDLE_TIME 300000

class HexchatIdlePlugin {
public:
        HexchatIdlePlugin (hexchat_plugin* plugin);

        void Initialize ();
        void Shutdown ();

        void GetPluginInformation (char**, char**, char**);

        void DBusCallback (GDBusConnection *connection, const gchar *sender_name, const gchar *object_path, const gchar *interface_name, const gchar *signal_name, GVariant *parameters, gpointer user_data);
private:
        char* plugin_name = PLUGIN_NAME;
        char* plugin_description = PLUGIN_DESCRIPTION;
        char* plugin_version = PLUGIN_VERSION;

        hexchat_plugin* plugin = nullptr;

        GDBusConnection* dbus_connection = nullptr;
        GDBusProxy* dbus_proxy = nullptr;

        guint signal_handler = 0;
        guint watch_active = 0;
        guint watch_idle = 0;

        bool state = false;

        bool ConnectDBus ();
        void DisconnectDBus ();
        bool RegisterDBusHooks ();
        void UnregisterDBusHooks ();

        void RegisterActivityHook ();
        void UnregisterActivityHook ();

        void SetState (bool);

        void Print (const char*);
};


#endif //HEXCHAT_IDLE_HEXCHATIDLEPLUGIN_H
