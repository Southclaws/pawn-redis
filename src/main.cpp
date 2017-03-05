/*==============================================================================


	Redis for SA:MP

		Copyright (C) 2016 Barnaby "Southclaw" Keene

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

#include "hiredis/hiredis.h"
#include <sdk.hpp>

#include "main.hpp"
#include "natives.hpp"
#include "impl.hpp"


/*==============================================================================

	Load/Unload and AMX management

==============================================================================*/


extern void	*pAMXFunctions;
logprintf_t logprintf_fp;

/*
	Note:
	This list contains all the AMX instances the plugin has registered. When you
	load the server, the gamemode counts as an AMX instance and is always
	registered. When you load a filterscript, it will be registered and when you
	unload a filterscript, it will be un-registered. This list represents all
	registered AMX instances.
*/
set<AMX*> amx_list;


PLUGIN_EXPORT bool PLUGIN_CALL Load(void **ppData) 
{
	pAMXFunctions = ppData[PLUGIN_DATA_AMX_EXPORTS];
	logprintf_fp = (logprintf_t)ppData[PLUGIN_DATA_LOGPRINTF];

	logprintf("\n");
	logprintf("SA:MP Redis - Redis for SA:MP by Southclaw");
	logprintf("\n");

	return true;
}

PLUGIN_EXPORT void PLUGIN_CALL Unload() 
{
	logprintf("SA:MP Redis unloaded.");
}

/*
	Note:
	The ProcessTick function is called from the SA:MP server every time it
	completes (or starts, I forgot) an internal cycle of the main loop. So in
	this plugin, we loop over all the AMX instances and call amx_tick for each.
	There may be a better way to do this which I will experiment with in future.
*/
PLUGIN_EXPORT void PLUGIN_CALL ProcessTick()
{
	for(AMX* i : amx_list)
	{
		Redisamp::amx_tick(i);
	}
}

void logprintf(const char* message, ...)
{
	unsigned int len = 256;
	char* result = new char[len];

	va_list args;
	va_start(args, message);

	vsnprintf(result, len, message, args);
	logprintf_fp(result);

	va_end(args);
	delete result;
}


/*==============================================================================

	Export stuff

	Note:
	I separated this from the above code because it's largely untouched once the
	plugin project framework is set up. This area just does the uninteresting
	portions of communicating with the SA:MP server such as telling it what
	functions are available in the plugin and initialises AMX instances.
	If you're wondering what an AMX instance is, it's any lone compiled Pawn
	script running, which includes gamemodes and filterscripts. The maximum I
	believe is 17 AMX instances: 1 gamemode and 16 filterscripts. Plugins need
	to know about filterscripts as well as the main gamemode since all AMX
	instances might want to interact with a plugin.

==============================================================================*/


extern "C" const AMX_NATIVE_INFO native_list[] = 
{
	{"Redis_Connect", Native::Connect},
	{"Redis_Disconnect", Native::Disconnect},

	{"Redis_Command", Native::Command},
	{"Redis_SetString", Native::SetString},
	{"Redis_GetString", Native::GetString},
	{"Redis_SetInt", Native::SetInt},
	{"Redis_GetInt", Native::GetInt},
	{"Redis_SetFloat", Native::SetFloat},
	{"Redis_GetFloat", Native::GetFloat},
	{"Redis_SetHashValue", Native::SetHashValue},
	{"Redis_GetHashValue", Native::GetHashValue},
	{"Redis_SetHashValues", Native::SetHashValues},
	{"Redis_GetHashValues", Native::GetHashValues},

	{"Redis_BindMessage", Native::BindMessage},
	{"Redis_SendMessage", Native::SendMessage},

	{NULL, NULL}
};

PLUGIN_EXPORT int PLUGIN_CALL AmxLoad(AMX *amx) 
{
	amx_list.insert(amx);
	logprintf("Loaded AMX %d", (int)amx);
	return amx_Register(amx, native_list, -1);
}

PLUGIN_EXPORT int PLUGIN_CALL AmxUnload(AMX *amx) 
{
	amx_list.erase(amx);
	logprintf("Unloaded AMX %d", (int)amx);
	return AMX_ERR_NONE;
}

/*
	Note:
	This function tells the SA:MP server what the plugin does. As far as I've
	seen, SUPPORTS_VERSION and SUPPORTS_AMX_NATIVES are default standard in all
	plugins and are required to work properly. AMX_NATIVES generally refers to
	usage of AMX functions to talk *back* to AMX instances. Without this, the
	plugin could be called into but never send information back to AMX other
	than a single cell via a function return.

	The important one here is PROCESS_TICK, this tells the SA:MP server that it
	should call the ProcessTick function in this plugin. If we weren't using
	ProcessTick, this could be disabled to save a tiny bit of time while running
	the main SA:MP server loop.
*/
PLUGIN_EXPORT unsigned int PLUGIN_CALL Supports()
{
	return SUPPORTS_VERSION | SUPPORTS_AMX_NATIVES | SUPPORTS_PROCESS_TICK; 
}
