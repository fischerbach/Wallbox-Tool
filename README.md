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


## Links
 * [wallbox](https://github.com/cliviu74/wallbox) - Python Module interface for Wallbox EV chargers api
 * [Wallbox : Get The Most Of It (with API)](https://www.lets-talk-about.tech/2022/08/wallbox-get-most-of-it-with-api.html)