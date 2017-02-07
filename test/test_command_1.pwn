#include <a_samp>

#include "../samp-redis.inc"


public OnFilterScriptInit()
{
	print("\n\n---\nRedis for SA:MP - test_command_1 begin");
	new Redis:context;
	new ret;

	print("Connecting to Redis at localhost:6379...");
	context = Redis_Connect("localhost", 6379, 1);


	print("Sending command SET test \"hello world!\"");
	ret = Redis_Command(context, "SET test \"hello world!\"");
	printf("returned: %d", ret);


	ret = Redis_Disconnect(context);

	print("\n\n---\nRedis for SA:MP - test_command_1 end\n\n");
}
