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
		This is a special header file that declares the natives to export to the
		SA:MP server. These function declarations are a little different to
		regular C++ library exports. Their return type must be "cell" which
		refers to the byte-width of all Pawn variables. This set of declarations
		also maps to the "native_list" array in "main.cpp". The functions here
		will mean nothing without that "native_list" filled in as that's the
		part that actually tells SA:MP the function addresses of these functions
		so it can call them.


==============================================================================*/

#ifndef PAWN_REDIS_NATIVES_H
#define PAWN_REDIS_NATIVES_H

#include <string>
#include <vector>

using std::string;
using std::vector;

#include <amx/amx2.h>

#include "common.hpp"

namespace Natives {

cell Connect(AMX* amx, cell* params);
cell Disconnect(AMX* amx, cell* params);

cell Command(AMX* amx, cell* params);
cell Exists(AMX* amx, cell* params);
cell SetString(AMX* amx, cell* params);
cell GetString(AMX* amx, cell* params);
cell SetInt(AMX* amx, cell* params);
cell GetInt(AMX* amx, cell* params);
cell SetFloat(AMX* amx, cell* params);
cell GetFloat(AMX* amx, cell* params);
cell SetHashValue(AMX* amx, cell* params);
cell GetHashValue(AMX* amx, cell* params);

cell Subscribe(AMX* amx, cell* params);
cell Publish(AMX* amx, cell* params);
};

#endif
