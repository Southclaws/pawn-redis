#include <a_samp>
#define RUN_TESTS
#include <YSI\y_testing>

#include "../samp-redis.inc"

new Redis:context;


Test:ConnectDisconnect()
{
	context = Redis_Connect("localhost", 6379, 1);
	printf("context: %d", _:context);
	ASSERT(context == Redis:0);

	new ret = Redis_Disconnect(context);
	printf("ret: %d", ret);
	ASSERT(ret == 0);
}

Test:DisconnectInvalid()
{
	new ret = Redis_Disconnect(Redis:2835672);
	printf("ret: %d", ret);
	ASSERT(ret == REDIS_ERROR_CONTEXT_INVALID_ID);
}


/*
	Simple ping
*/
TestInit:Ping()
{
	context = Redis_Connect("localhost", 6379, 1);
}

Test:Ping()
{
	new ret = Redis_Command(context, "PING");
	printf("ret: %d", ret);
	ASSERT(ret == 0);
}

TestClose:Ping()
{
	Redis_Disconnect(context);
}


/*
	Set a string key then get it and compare the value.
*/
TestInit:SetThenGetString()
{
	context = Redis_Connect("localhost", 6379, 1);
}

Test:SetThenGetString()
{
	new ret = Redis_SetString(context, "test", "hello world!");
	printf("ret: %d", ret);
	ASSERT(ret == 0);

	new want[13] = {"hello world!"};
	new got[13];

	ret = Redis_GetString(context, "test", got, 13);
	printf("ret: %d", ret);
	ASSERT(ret == 0);

	printf("want: '%s' got: '%s'", want, got);
	ASSERT(strcmp(want, got) == 0);
}

TestClose:SetThenGetString()
{
	Redis_Disconnect(context);
}
