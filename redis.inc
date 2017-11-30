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


==============================================================================*/


#define REDIS_ERROR_CONNECT_GENERIC			(-1)
#define REDIS_ERROR_CONNECT_FAIL			(-2)
#define REDIS_ERROR_CONNECT_AUTH			(-3)
#define REDIS_ERROR_CONTEXT_INVALID_ID		(10)
#define REDIS_ERROR_CONTEXT_MISSING_POINTER	(20)
#define REDIS_ERROR_COMMAND_BAD_REPLY		(30)
#define REDIS_ERROR_COMMAND_NO_REPLY		(40)
#define REDIS_ERROR_SUBSCRIBE_THREAD_ERROR	(50)
#define REDIS_ERROR_UNEXPECTED_RESULT_TYPE	(60)

#define REDIS_REPLY_STRING					(1)
#define REDIS_REPLY_ARRAY					(2)
#define REDIS_REPLY_INTEGER					(3)
#define REDIS_REPLY_NIL						(4)
#define REDIS_REPLY_STATUS					(5)
#define REDIS_REPLY_ERROR					(6)

native Redis:Redis_Connect(host[], port, auth[]);
native Redis_Disconnect(Redis:context);

native Redis_Command(Redis:context, command[]);
native Redis_Exists(Redis:context, key[]);
native Redis_SetString(Redis:context, key[], value[]);
native Redis_GetString(Redis:context, key[], value[], len = sizeof(value));
native Redis_SetInt(Redis:context, key[], value);
native Redis_GetInt(Redis:context, key[], &value);
native Redis_SetFloat(Redis:context, key[], Float:value);
native Redis_GetFloat(Redis:context, key[], &Float:value);

native Redis_SetHashValue(Redis:context, key[], inner_key[], value[]);
native Redis_GetHashValue(Redis:context, key[], inner_key[], value[], len = sizeof(value));
native Redis_SetHashValues(Redis:context, key[], ...);
native Redis_GetHashValues(Redis:context, key[], values[][]);

native Redis_BindMessage(Redis:context, channel[], callback[]);
native Redis_SendMessage(Redis:context, channel[], data[]);
