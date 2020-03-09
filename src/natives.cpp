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
		This file contains the actual definitions of the functions exported to
		the SA:MP server. When those exported functions are called from a Pawn
		script (AMX instance actually) this is the code that will run. I've done
		what most logical programmers would do and separated my actual
		implementation code into another module so all these functions do is
		process the input and call the implementation functions with the
		sanitised and processed input.

		Talking about input, parameters come in from Pawn as a simple pointer
		which must be "decoded" into the original intended parameters and their
		types (as much as Pawn has "types"...) which isn't much different to
		extracting parameters from **argv if you're familiar with that.

		Luckily, the SDK has provided some helper functions to extract the data
		from parameters. It's important the "native" declarations in Pawn match
		the parameter extraction perfectly, if you specify too little parameters
		in a "native" declaration, the code here will extract garbage data and
		potentially crash horribly. If you try to send too many parameters, it
		should be freed fine and not cause issue but is very pointless!


==============================================================================*/

#include "natives.hpp"
#include "impl.hpp"

cell Natives::Connect(AMX* amx, cell* params)
{
    string hostname = amx_GetCppString(amx, params[1]);
    int port = params[2];
    string auth = amx_GetCppString(amx, params[3]);
    cell* addr;
    amx_GetAddr(amx, params[4], &addr);

    int ret = Impl::Connect(hostname, port, auth, *addr);

    return ret;
}

cell Natives::Disconnect(AMX* amx, cell* params)
{
    int context_id = params[1];

    return Impl::Disconnect(context_id);
}

cell Natives::Command(AMX* amx, cell* params)
{
    int context_id = params[1];
    string command = amx_GetCppString(amx, params[2]);

    return Impl::Command(context_id, command);
}

cell Natives::Exists(AMX* amx, cell* params)
{
    int context_id = params[1];
    string key = amx_GetCppString(amx, params[2]);

    return Impl::Exists(context_id, key);
}

cell Natives::SetString(AMX* amx, cell* params)
{
    int context_id = params[1];
    string key = amx_GetCppString(amx, params[2]);
    string value = amx_GetCppString(amx, params[3]);

    return Impl::SetString(context_id, key, value);
}

cell Natives::GetString(AMX* amx, cell* params)
{
    int context_id = params[1];
    string key = amx_GetCppString(amx, params[2]);
    string value;
    int ret;

    ret = Impl::GetString(context_id, key, value);
    amx_SetCppString(amx, params[3], value, params[4]);

    return ret;
}

cell Natives::SetInt(AMX* amx, cell* params)
{
    int context_id = params[1];
    string key = amx_GetCppString(amx, params[2]);
    int value = params[3];

    return Impl::SetInt(context_id, key, value);
}

cell Natives::GetInt(AMX* amx, cell* params)
{
    int context_id = params[1];
    string key = amx_GetCppString(amx, params[2]);
    int value;
    int ret;

    ret = Impl::GetInt(context_id, key, value);

    cell* address;
    amx_GetAddr(amx, params[3], &address);
    *address = value;

    return ret;
}

cell Natives::SetFloat(AMX* amx, cell* params)
{
    int context_id = params[1];
    string key = amx_GetCppString(amx, params[2]);
    float value = *(float*)&params[3]; // weird float conversion

    return Impl::SetFloat(context_id, key, value);
}

cell Natives::GetFloat(AMX* amx, cell* params)
{
    int context_id = params[1];
    string key = amx_GetCppString(amx, params[2]);
    float value;
    int ret;

    ret = Impl::GetFloat(context_id, key, value);

    cell* address;
    amx_GetAddr(amx, params[3], &address);
    *address = amx_ftoc(value);

    return ret;
}

cell Natives::Subscribe(AMX* amx, cell* params)
{
    string host = amx_GetCppString(amx, params[1]);
    int port = params[2];
    string auth = amx_GetCppString(amx, params[3]);
    string channel = amx_GetCppString(amx, params[4]);
    string callback = amx_GetCppString(amx, params[5]);

	cell* addr;
	amx_GetAddr(amx, params[6], &addr);
    return Impl::Subscribe(host, port, auth, channel, callback, *addr);
}

cell Natives::Unsubscribe(AMX* amx, cell* params)
{
    return Impl::Unsubscribe(params[1]);
}

cell Natives::Publish(AMX* amx, cell* params)
{
    int pubsub_id = params[1];
    string channel = amx_GetCppString(amx, params[2]);
    string message = amx_GetCppString(amx, params[3]);

    return Impl::Publish(pubsub_id, channel, message);
}
