# A mousekey for analog keyboard

## Platform
MacOS

## Requires
Wooting analog SDK
https://github.com/WootingKb/wooting-analog-sdk

## Compile
```
clang++ main.cpp -std=c++17 -O2 -lwooting_analog_sdk -framework ApplicationServices -o main.out -Wall
```

## Some feature
- numpad 8/4/5/6 -> move cursor around
- numpad 9/3 -> scroll wheel
- numpad 0 -> left click
- numpad + -> right click
- numpad 7 -> middle click
- NumLock -> quit the app
