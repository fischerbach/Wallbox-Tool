# Wallbox Tool
Tool for using API of Wallbox EV charger

The project was set up to learn C, so I don't know yet in which direction it will develop. 

## Build
```
clang wallbox.c -o wallbox `curl-config --libs` `pkg-config --cflags --libs json-c`
```

## Usage
```
WALLBOX_USERPWD="email:password" ./wallbox
```


## Other projects for Wallbox API
 * [wallbox](https://github.com/cliviu74/wallbox) - Python Module interface for Wallbox EV chargers api