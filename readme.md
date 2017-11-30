# Redis for SA:MP

A powerful Redis plugin granting access to a fast in-memory database and
asynchronous message channels between other programs.

[If you have no idea what Redis is and why it's so powerful with SA:MP, **click here!**](docs/readme.md)

It's open source and available [here](https://github.com/Southclawss/samp-redis)

![allaboardtheredismobile.png](allaboardtheredismobile.png)

## What Redis Is

> Redis is an open source (BSD licensed), in-memory data structure store, used
> as a database, cache and message broker. It supports data structures such as
> strings, hashes, lists, sets, sorted sets with range queries, bitmaps,
> hyperloglogs and geospatial indexes with radius queries.

(from the [Redis official site](https://redis.io))

In SA:MP terms: Redis is can be the GVar plugin on steroids. It can be threaded
MySQL. It can be a place to store large amounts of information and free up heap
space (and cut compile times!)

You can also use Redis to delegate slower tasks to other programs through the
power of message queues!

## What Redis Is Not

Redis is not a permanent database. It exists entirely in memory which makes it
very fast to read from and write to but it is not a replacement for something
like MySQL to store data for a _very long period of time_.

## Building

I dev on Ubuntu 64 bit and I probably won't have time to maintain other versions
so I'll only provide details for my platform, If you're on a 64 bit system,
you'll need 32 bit libc stuff: `sudo apt install libc6-dev-i386` You'll also
need `g++multilib` but aside from that, I don't think anything else is required
(though it probably is... c++ is fun like that)

Once you've got all that sorted, just run `make` and you should see a `redis.so`
file appear!

If you're on a non-Linux platform and you have Docker available, you can run
`make run-builder` and that should work 99.9% of the time!

## Development

"Redisamp" is currently in development and is in version 0.x since it's not been
fully tested in a production/public server environment.

There are tests in the repository however and everything seems to be running
fine so feel free to try it out and give feedback.

My roadmap for development is:

* Array support (arrays of ints, arrays of floats, arrays of strings/arrays!)
* Optimisation with c-strings instead of c++ strings, for now I stuck with the
  safer option and decided to do two levels of conversion (amx (c strings) ->
  implementation (c++ strings) > redis (c strings))
* More tests! I need to add more failure states and test corner cases to ensure
  stability.

## API

* Redis:Redis_Connect(host[], port); // Connect to a Redis server
* Redis_Disconnect(Redis:context); // Disconnect, do this on OnGameModeExit
* Redis_Command(Redis:context, command[]); // Send a Redis command
* Redis_SetString(Redis:context, key[], value[]); // Set a string key
* Redis_GetString(Redis:context, key[], value[], len); // Retrieve a string
* Redis_SetInt(Redis:context, key[], value); // Set a integer key
* Redis_GetInt(Redis:context, key[], &value); // Retrieve a integer
* Redis_SetFloat(Redis:context, key[], Float:value); // Set a floating point key
* Redis_GetFloat(Redis:context, key[], &Float:value); // Retrieve a floating
  point
* Redis_BindMessage(Redis:context, channel[], callback[]); // Bind a message
  channel to a Pawn callback
* Redis_SendMessage(Redis:context, channel[], data[]); // Send a message to a
  message channel

Check out the test script for examples of all these functions.

## Examples

You might be still wondering why this is even useful. Here are some possible use
cases:

* Database: much like the threaded MySQL plugin but you can use more modern
  technologies and nicer languages to handle your data
* Chat: connecting to Discord will be much easier. I know maddinat0r is working
  on a Discord plugin in C++ but it would be quicker to just use the existing
  Discord.py Python library to write the plugin in Python and use the Redis
  plugin to send chat messages or commands to and from the SA:MP server.
* Large data store: In my Scavenge and Survive gamemode, there can be tens of
  thousands of items in the world and a lot of these items have data associated
  with them such as integers and strings. Redis could be useful in situations
  like this to store data outside of the SA:MP context, cut down compile times
  and increase development productivity.
* Logging: Redis has been used as a log aggregator in the past, if you have a
  lot of log data and you want to do something clever with it (admin panel,
  analysis, etc) you can shove it down a Redis queue.

I'll release some code examples at a later date to showcase the power of this
plugin.
