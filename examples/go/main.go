package main

import (
	"log"

	"github.com/go-redis/redis"
)

func main() {
	client := redis.NewClient(&redis.Options{
		Addr: "localhost:6379",
	})

	for {
		reply, err := client.BLPop(0, "OnPlayerText").Result()
		if err != nil {
			log.Printf("failed to pop message off channel: %v\n", err)
			continue
		}

		if len(reply) < 2 {
			log.Printf("redis reply empty\n")
			continue
		}

		log.Println(reply[1])
	}
}
