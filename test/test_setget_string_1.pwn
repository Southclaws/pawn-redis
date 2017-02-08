#include <a_samp>

#include "../samp-redis.inc"


public OnFilterScriptInit()
{
	print("\n\n---\nRedis for SA:MP - test_getset_string_1 begin");
	new Redis:context;
	new ret;

	print("Connecting to Redis at localhost:6379...");
	context = Redis_Connect("localhost", 6379, 1);


	print("Setting key test");
	ret = Redis_SetString(context, "test", "hello world!");
	printf("returned: %d", ret);


	print("Getting key test");
	new result[128];
	ret = Redis_GetString(context, "test", result, 128);
	printf("returned: %d result: '%s'", ret, result);


	ret = Redis_Disconnect(context);

	print("\n\n---\nRedis for SA:MP - test_getset_string_1 end\n\n");
}
