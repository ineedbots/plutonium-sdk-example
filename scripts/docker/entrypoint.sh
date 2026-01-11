#!/bin/sh

echo "Updating Plutonium files..."
./plutonium-updater -d ./plutonium/
cd plutonium/

GAME_PATH="/game"
if [[ ! -d $GAME_PATH ]]; then
    echo "Make sure to mount your game files to $GAME_PATH"
    exit 1
fi

if [[ -z "$PLUTO_GAME" ]]; then
    echo "Make sure to set the PLUTO_GAME environment variable (iw5mp, t5sp, t6zm, etc.)"
    exit 1
fi

PREARGS="-dedicated"
if [[ -n "$PLUTO_KEY" ]]; then
    PREARGS="$PREARGS -key $PLUTO_KEY"
else
    echo "PLUTO_KEY environment variable not set."
fi
if [[ -n "$PLUTO_FLAGS" ]]; then
    PREARGS="$PREARGS $PLUTO_FLAGS"
else
    echo "PLUTO_FLAGS environment variable not set."
fi

GAME_PORT="28960"
if [[ -n "$PLUTO_PORT"]]; then
    GAME_PORT="$PLUTO_PORT"
else
    echo "PLUTO_PORT environment variable not set."
fi

ARGS="+set net_port ${PLUTO_PORT:-28960}"
if [[ -n "$PLUTO_CONFIG" ]]; then
    if [[ "$PLUTO_GAME" == iw5* ]]; then
        ARGS="$ARGS +set sv_config $PLUTO_CONFIG"
    else
        ARGS="$ARGS +exec $PLUTO_CONFIG"
    fi
else
    echo "PLUTO_CONFIG environment variable not set."
fi
if [[ -n "$PLUTO_ARGS" ]]; then
    ARGS="$ARGS $PLUTO_ARGS"
else
    echo "PLUTO_ARGS environment variable not set."
fi

POSTARGS="+map_rotate"
if [[ "$#" -gt 0 ]]; then
    POSTARGS="$POSTARGS $@"
else
    echo "No additional arguments provided."
fi

echo "Executing:"
echo "wine bin/plutonium-bootstrapper-win32.exe $PLUTO_GAME $GAME_PATH $PREARGS $ARGS $POSTARGS"
wine bin/plutonium-bootstrapper-win32.exe $PLUTO_GAME $GAME_PATH $PREARGS $ARGS $POSTARGS
