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

### Why the project named "forthuna"?
It's a combination of words *Forth* (obviously) and the name of the Ancient Rome's goddess of luck and fate - *Fortūna*. I think, good luck - is a necessary requirement for memory management in Forth ^^ 

### Building
REPL: `cmake -S . -B build -DEXAMPLE_REPL=ON && cmake --build build`
### Using
```
Forthuna REPL. Enjoy!
S" Hello world! " TYPE
Hello world! OK
WORDS
FORTH MARKER ACCEPT KEY PARSE-NAME PARSE EVALUATE POSTPONE LITERAL ] [ ( DEPTH WORDS VARIABLE CONSTANT RECURSE REPEAT WHILE UNTIL BEGIN J I UNLOOP LEAVE +LOOP LOOP DO ELSE THEN IF IMMEDIATE HEX DECIMAL BASE CURRENT . SIGN #> #S # <# HLD PAD HERE FALSE TRUE TYPE BL SPACE CR EMIT ." ." S" S" CHARS CHAR+ [CHAR] CHAR C, C! C@ +! @ ! ; : 1- 1+ 0= 0< OR AND = > < SWAP DROP ?DUP DUP ABS NEGATE INVERT MAX MIN / MOD RSHIFT LSHIFT OVER XOR * - + EXECUTE ' , MOVE ALLOT CELL+ CELLS ALIGNED ALIGN DOES> DOES> FIND CREATE EXIT  OK
```

### Memory Usage
Builded by arm-none-eabi-gcc compiler for arm32 cortex-m3  (commit 5dfb880a9d). Also the lib is required a `forth_t` struct for saving context of the vm so plus to bss 80bytes.   
`-O0`
```
   text    data     bss     dec     hex filename
   1344       0       0    1344     540 forth.c.obj (ex libforthuna.a)
  14798       4       0   14802    39d2 forth_core.c.obj (ex libforthuna.a)
```
`-Os`
```
    735       0       0     735     2df forth.c.obj (ex libforthuna.a)
   8223       4       0    8227    2023 forth_core.c.obj (ex libforthuna.a)
```