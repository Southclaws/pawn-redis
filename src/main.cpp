/*==============================================================================


	Redis for SA:MP

		Copyright (C) 2016 Barnaby "Southclaws" Keene

		This program is free software: you can redistribute it and/or modify it
		under the terms of the GNU General Public License as published by the
		Free Software Foundation, either version 3 of the License, or (at your
		option) any later version.

		This program is distributed in the hope that it will be useful, but
		WITHOUT ANY WARRANTY; without even the implied warranty of
		MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
		See the GNU General Public License for more details.

		You should have received a copy of the GNU General Public License along
		with this program.  If not, see <http://www.gnu.org/licenses/>.

	Note:
		This is the main source code file which handles all the communication
		with the SA:MP server and AMX instances. Pretty standard in all SA:MP
		plugins.


==============================================================================*/

#include <set>

using std::set;

#include <amx/amx.h>
#include <plugincommon.h>

#include "common.hpp"
#include "impl.hpp"
#include "natives.hpp"

logprintf_t logprintf;

/*==============================================================================

	Load/Unload and AMX management

==============================================================================*/

set<AMX*> amx_list;

extern "C" const AMX_NATIVE_INFO native_list[] = {
    { "Redis_Connect", Natives::Connect },
    { "Redis_Disconnect", Natives::Disconnect },

    { "Redis_Command", Natives::Command },
    { "Redis_Exists", Natives::Exists },
    { "Redis_SetString", Natives::SetString },
    { "Redis_GetString", Natives::GetString },
    { "Redis_SetInt", Natives::SetInt },
    { "Redis_GetInt", Natives::GetInt },
    { "Redis_SetFloat", Natives::SetFloat },
    { "Redis_GetFloat", Natives::GetFloat },

    { "Redis_SetHString", Natives::SetHString },
    { "Redis_SetHInt", Natives::SetHInt },
    { "Redis_GetHString", Natives::GetHString },
    { "Redis_HExists", Natives::HExists },
    { "Redis_HDel", Natives::HDel },
    { "Redis_HIncrBy", Natives::HIncrBy },
    { "Redis_HIncrByFloat", Natives::HIncrByFloat },
    { "Redis_GetHInt", Natives::GetHInt },

    { "Redis_Subscribe", Natives::Subscribe },
    { "Redis_Unsubscribe", Natives::Unsubscribe },
    { "Redis_Publish", Natives::Publish },

    { NULL, NULL }
};

PLUGIN_EXPORT bool PLUGIN_CALL Load(void** ppData)
{
    pAMXFunctions = ppData[PLUGIN_DATA_AMX_EXPORTS];
    logprintf = (logprintf_t)ppData[PLUGIN_DATA_LOGPRINTF];

    logprintf("\n");
    logprintf("SA:MP Redis - Redis for SA:MP by Southclaws");
    logprintf("\n");

    return true;
}

PLUGIN_EXPORT void PLUGIN_CALL Unload()
{
    logprintf("SA:MP Redis unloaded.");
}

PLUGIN_EXPORT void PLUGIN_CALL ProcessTick()
{
    Impl::amx_tick();
}

PLUGIN_EXPORT int PLUGIN_CALL AmxLoad(AMX* amx)
{
    amx_list.insert(amx);
    return amx_Register(amx, native_list, -1);
}

PLUGIN_EXPORT int PLUGIN_CALL AmxUnload(AMX* amx)
{
    amx_list.erase(amx);
    return AMX_ERR_NONE;
}

PLUGIN_EXPORT unsigned int PLUGIN_CALL Supports()
{
    return SUPPORTS_VERSION | SUPPORTS_AMX_NATIVES | SUPPORTS_PROCESS_TICK;
}
