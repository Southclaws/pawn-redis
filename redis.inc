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

#if defined _inc_redis
    #undef _inc_redis
#endif

#if defined _redis_included
    #endinput
#endif
#define _redis_included

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

native Redis_Connect(const host[], port, const auth[], &Redis:client);
native Redis_Disconnect(Redis:client);

native Redis_Command(Redis:client, const command[]);
native Redis_Exists(Redis:client, const key[]);
native Redis_SetString(Redis:client, const key[], value[]);
native Redis_GetString(Redis:client, const key[], value[], len = sizeof(value));
native Redis_SetInt(Redis:client, const key[], value);
native Redis_GetInt(Redis:client, const key[], &value);
native Redis_SetFloat(Redis:client, const key[], Float:value);
native Redis_GetFloat(Redis:client, const key[], &Float:value);

native Redis_SetHString(Redis:client, const key[], const field[], value[]);
native Redis_SetHInt(Redis:client, const key[], const field[], value);
native Redis_GetHString(Redis:client, const key[], const field[], value[], len = sizeof(value));
native Redis_GetHInt(Redis:client, const key[], const field[]);
native Redis_HExists(Redis:client, const key[], const field[]);
native Redis_HDel(Redis:client, const key[], const field[]);
native Redis_HIncrBy(Redis:client, const key[], const field[], incr);
native Redis_HIncrByFloat(Redis:client, const key[], const field[], Float:incr);

native Redis_Subscribe(const host[], port, const auth[], const channel[], const callback[], &PubSub:client);
native Redis_Unsubscribe(PubSub:client);
native Redis_Publish(Redis:client, const channel[], const data[]);
