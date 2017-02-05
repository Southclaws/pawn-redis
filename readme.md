# Redis for SA:MP

Work in progress

Roadmap:

- multiple db connection contexts
- Pub/Sub with callback mechanism
- threadsafe
- Key/Value set/get

Rough outline of desired v0.1 API:

- `Redis_Connect(host[], port, timeout) > ctx` connect to redis server
- `Redis_Close(ctx)` disconnect context from redis server
- `Redis_Command(ctx, command[], ...)` issue any Redis command (except SUBSCRIBE and PUBLISH)
- `Redis_Subscribe(ctx, channel[], callback[])` waits for messages on channel and calls Pawn callback when message is received.
- `Redis_Publish(ctx, channel[])` publish a message on channel
