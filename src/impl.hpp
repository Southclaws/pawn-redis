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


namespace Redisamp
{

int Connect(string hostname, int port, int timeout);
int Disconnect(int context);
int Command(string command);
int Subscribe(int context, string channel, string callback);
int Publish(int context, string channel, string data);

void amx_tick(AMX* amx);

extern int context_count;
extern std::map<int, redisContext*> contexts;

}



#endif
