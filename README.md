# Forthuna

![Tests](https://ci.ansocket.ru:9777/api/badges/1/status.svg)

Today people invented many programming languages for various purpouses. Some languages have become very popular, some have forgotten in the deep of history. Here we have not the most popular language, but, of course, with specific charm and charisma. That's *Forth* 

### Another ordinary implementation of Forth?
Yep, there is. Main idea is to using Forth REPL as a CLI interface for embedded targets like microcontrollers. No excessive hardware dependencies, no complex porting.   

### What's in there?
- Based on C language (GCC)
- No malloc/free required.
- Just needs to implement only input and output function - easy as nothing
- Core functions keep in ROM memory, so RAM usage should be minimal.
- No Stdlib required!
- Lots of fun :)

### Why the project named "forthuna"?
It's a combination of words *Forth* (obviously) and the name of the Ancient Rome's goddess of luck and fate - *Fortūna*. I think, good luck - is a necessary requirement for memory management in Forth ^^ 

### Building
REPL: `./scripts/build_examples.sh`
### Using
```
Forthuna REPL. Enjoy!
S" Hello world! " TYPE
Hello world! OK
```
### Creating custom word sets
Forth language provides to adding custom sets of words to extend functionality of your application. I suggest you to create custom sets to hardware-depended words instead creating new words in "core" set. Adding new sets are pretty straightforward (examples/custom_set). Feel free to make MR with support your hardware! It's not mandatory (MIT License) but I will be very happy ^^
 
### Memory Usage
Memory usage depends on what platform you're using. Builded for 32bit Arm Cortex-M3 lib will be about 10K FLASH and 100 bytes of RAM with -Os optimization.
