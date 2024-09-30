# Visu

3D visualization using force directed graph.

# Dependencies

```
libjsoncpp-dev
libglm-dev
libgl-dev
libglfw3-dev
libglu1-mesa-dev
make
```

# Building

```
make
make run
```

# Configuration

there is currently no configuration options
it opens port `9002` for websocket access and accept json commands.


- to create a new node

```
{ "command": "add", "name": "Name of the node", "color": "optional HEX color" }
```

- to connect two nodes

```
{ "command": "connect", "source": "source node", "target": "target node" }
```

## TODO

- [x] Add instance rendering
- [ ] Add bloom effect

