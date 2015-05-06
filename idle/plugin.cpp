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
#include <hexchat-plugin.h>
#include "HexchatIdlePlugin.h"

static HexchatIdlePlugin* m_plugin;

extern "C" void hexchat_plugin_get_info (char **name, char **desc, char **version, void **reserved) {
        m_plugin->GetPluginInformation (name, desc, version);
}

extern "C" int hexchat_plugin_init (hexchat_plugin *plugin_handle, char **plugin_name, char **plugin_desc, char **plugin_version, char *arg) {
        m_plugin = new HexchatIdlePlugin (plugin_handle);
        m_plugin->GetPluginInformation (plugin_name, plugin_desc, plugin_version);

        m_plugin->Initialize ();

        return 1;
}

extern "C" int hexchat_plugin_deinit () {
        m_plugin->Shutdown ();
        free (m_plugin);

        return 1;
}
