# Forthuna

![Tests](https://ci.ansocket.ru/api/badges/1/status.svg)

Today people invented many programming languages for various purpouses. Some languages have become very popular, some have forgotten in the deep of history. Here we have not the most popular language, but, of course, with specific charm and charisma. That's *Forth* 

### Another ordinary implementation of Forth?
Yep, there is. Main idea is to using Forth REPL as a CLI interface for embedded targets like microcontrollers. No excessive hardware dependencies, no complex porting.   

### What's in there?
- Based on C language (GCC)
- No malloc/free required.
- Just needs to implement only input and output function - easy as nothing
- Core functions keep in ROM memory, so RAM usage should be minimal.
- Lots of fun :)

### Why project named "forthuna"?
It's a combination of words *Forth* (obviously) and name of the Ancient Rome's goddess of luck and fate - *Fortūna*. I think, good luck - is a necessary requirement for memory management in Forth ^^ 

### Building
REPL: `cmake -S . -B build -DEXAMPLE_REPL=ON && cmake --build build`
### Using
```
Forthuna REPL. Enjoy!
S" Hello world! " TYPE
Hello world! OK
```