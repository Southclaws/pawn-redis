# Interactive Deathmatch

This example is a very simple SA:MP deathmatch server where users can interact with the arena via a webapp.

It's just a concept here for now, mainly just notes for myself, if I develop it I'll run it as a demo on my dedicated server.

## SA:MP Server

- Basic deathmatch arena
- Buttons to spawn weapons
- Heatmap of death spots - queried frequently

## Backend

- Restful API
- POST: spawn weapon at some position - returns error if limit reached
- GET: request latest heatmap
- websockets? attempt two-way events - chat, deaths, etc
