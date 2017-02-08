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
		This header and it's counterpart .cpp are the only two files that don't
		really contain generic SA:MP plugin boilerplate code. See the .cpp for
		implementation details.


==============================================================================*/


#ifndef SAMP_REDIS_IMPL_H
#define SAMP_REDIS_IMPL_H

#include <string>
#include <map>

using std::string;

#include "hiredis/hiredis.h"

#include "main.hpp"


#define REDIS_ERROR_CONNECT_GENERIC			(-1)
#define REDIS_ERROR_CONNECT_FAIL			(-2)
#define REDIS_ERROR_CONTEXT_INVALID_ID		(10)
#define REDIS_ERROR_CONTEXT_MISSING_POINTER	(20)
#define REDIS_ERROR_COMMAND_BAD_REPLY		(30)
#define REDIS_ERROR_COMMAND_NO_REPLY		(40)


namespace Redisamp
{

int Connect(string hostname, int port, int timeout);
int Disconnect(int context_id);

int Command(int context_id, string command);
int SetString(int context_id, string key, string value);
int GetString(int context_id, string key, string& value);

int Subscribe(int context_id, string channel, string callback);
int Publish(int context_id, string channel, string data);

void amx_tick(AMX* amx);
int contextFromId(int context_id, redisContext*& context);

extern int context_count;
extern std::map<int, redisContext*> contexts;

}

#endif
