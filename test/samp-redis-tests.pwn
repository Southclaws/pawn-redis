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


/*
	Set a string key then get it and compare the value.
*/
TestInit:SetThenGetInt()
{
	context = Redis_Connect("localhost", 6379, 1);
}

Test:SetThenGetInt()
{
	new ret = Redis_SetInt(context, "test", 42);
	printf("ret: %d", ret);
	ASSERT(ret == 0);

	new want = 42;
	new got;

	ret = Redis_GetInt(context, "test", got);
	printf("ret: %d", ret);
	ASSERT(ret == 0);

	printf("want: '%d' got: '%d'", want, got);
	ASSERT(want == got);
}

TestClose:SetThenGetInt()
{
	Redis_Disconnect(context);
}


/*
	Set a string key then get it and compare the value.
*/
TestInit:SetThenGetFloat()
{
	context = Redis_Connect("localhost", 6379, 1);
}

Test:SetThenGetFloat()
{
	new ret = Redis_SetFloat(context, "test", 77.653);
	printf("ret: %d", ret);
	ASSERT(ret == 0);

	new Float:want = 77.653;
	new Float:got;

	ret = Redis_GetFloat(context, "test", got);
	printf("ret: %d", ret);
	ASSERT(ret == 0);

	printf("want: '%f' got: '%f'", want, got);
	ASSERT(want == got);
}

TestClose:SetThenGetFloat()
{
	Redis_Disconnect(context);
}


/*
	Subscribe to a 
*/
TestInit:Subscribe()
{
	context = Redis_Connect("localhost", 6379, 1);
}

Test:Subscribe()
{
	new ret = Redis_Subscribe(context, "samp.test.1", "Recieve");
	ASSERT(ret == 0);
}

TestClose:Subscribe()
{
	Redis_Disconnect(context);
}

forward Recieve(data[]);
public Recieve(data[])
{
	printf("Recieve called with data: '%s'", data);
}