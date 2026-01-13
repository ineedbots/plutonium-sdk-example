# plutonium_plugin_base
A plugin that has no code that hopefully compiles and the game will load it to do nothing. Stability not guaranteed.

Requires Git (https://git-scm.com/), Premake5 (https://premake.github.io/), and MSVC 2022 (https://visualstudio.microsoft.com/vs/features/cplusplus/) to build.

# Installation
Move the `plutonium_plugin_base.dll` to `%LOCALAPPDATA%\Plutonium\plugins\`, the plugin will be loaded when you start up a dedicated server for Plutonium.

# Features

Nothing.

# Docker Image

When you run the image, you need to bind your game folder (root of your game's files) to `/game`.

The image uses `/app/plutonium` as a volume for the plutonium install files (these will be automatically downloaded and/or updated).

You need to provide `PLUTO_GAME` environment variable, this is one of `iw5mp`, `t4sp`, `t6zm`, etc.

You can provide `PLUTO_KEY` environment variable, this is your dedicated server key.

You can provide `PLUTO_CONFIG` environment variable, this is the config file to execute when the dedicated server starts (like dedicated.cfg).

`PLUTO_FLAGS` environment variable is optional for including things like `-lan -raw-console`.

`PLUTO_ARGS` environment variable is optional for including things like `+set developer 2 +set developer_script 1`.

`PLUTO_PORT` environment variable is optional, defaults to `28960`. This is the UDP port of the game, image defaults `28960` to be exposed.

And any additional args will be appended to the bootstrapper args.

Heres an example of building and running the image.
```bash
docker build --tag plutonium_plugin_base .
docker run --mount type=bind,src="C:\\path\\to\\your\\BO2",dst=/game \
  --mount type=bind,src="C:\\path\\to\\your\\Plutonium",dst=/app/plutonium \
  --env PLUTO_GAME=t6mp \
  --env PLUTO_FLAGS=-lan \
  --env PLUTO_CONFIG=docker.cfg \
  -p 28960:28960/udp
  -it --rm plutonium_plugin_base
```


# Credits
- momo5502 (https://github.com/momo5502)
- alice (https://github.com/alicealys/t6-gsc-utils)
- mxve (https://github.com/mxve/plutonium-updater.rs)
- Plutonium Team (https://github.com/plutoniummod/plutonium-sdk)
- TsudaKageyu (https://github.com/TsudaKageyu/minhook)
