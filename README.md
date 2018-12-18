# Redis for SA:MP

A powerful Redis plugin granting access to a fast in-memory database and
asynchronous message channels between other programs.

It's open source and available [here](https://github.com/Southclaws/pawn-redis)

## What Redis Is

> Redis is an open source (BSD licensed), in-memory data structure store, used
> as a database, cache and message broker. It supports data structures such as
> strings, hashes, lists, sets, sorted sets with range queries, bitmaps,
> hyperloglogs and geospatial indexes with radius queries.

(from the [Redis official site](https://redis.io))

In SA:MP terms: Redis is can be the GVar plugin on steroids. It can be a place
to store large amounts of information and free up heap space (and cut compile
times!)

You can also use Redis to delegate slower tasks to other programs through the
power of pubsub message queues!

## What Redis Is Not

Redis is not a permanent database. It exists entirely in memory which makes it
very fast to read from and write to but it is not a replacement for something
like MySQL to store data for a _very long period of time_.

## Installation

Simply install to your project:

```bash
sampctl package install Southclaws/pawn-redis
```

Include in your code and begin using the library:

```pawn
#include <redis>
```
