# BlueScript Documentation
##### Author: May Draskovics
##### Date: 10/5/2021
 BlueScript can be written in two froms the actual [Script form](#bluescript-source) later parsed to the [Compiled form](#bluescript-compiled).

### BlueScript Source

#### Hello world in Blue Script
To get started with BlueScript it's best to know how it actually works,
so the most basic program you can write in bluescript is as follows
```js
// all bluescript scripts must start with a main function
function main()
{
    // out puts to console
    Print "Hello world"
}
```

The _function_ keyword is used as a marker for the parser to remember it's location in the program,
later this will be turned into a label with the function name.

The _Print_ keyword is the parsers key to make a console out statement and to print to console for 
the runtime.

#### Variables
In BlueScript there is no static types, all types are dynamic and will be dynamically allocated at runtime.
That does mean that handling variables are a bit harder, and correct typing should be kept in mind.

There is no keyword for declairing variables, so the way it's done it with an '=' sign between the variable
name and the value.

```js
function main()
{
    // declair the variables
    stringVar = "hello world"
    numericVar = 10
    boolVar = true

    // print the variable
    Print stringVar
    Print numericVar
    Print boolVar
}
```

There are 6 main types in BlueScript
1. String
    Generic Strings of characters (contains a char array)
2. Char
   A single character of a string
3. Numeric
    All numbers (floats or ints)
4. Bool
    Variables that are either True or False
5. Bytes
    A string of bits
6. Null
   A type with value of null

#### Loops
Currently in BlueScript there is only one type of loop the _while_ loop,later on I will be adding the _for_ loop 
but that isn't a top priority.
The while loop in BlueScript does one thing, check if an expression is true, if it is then iterate else continue.
```js
function main()
{
    // create something to loop with
    i = 0
    // create the while loop with expression
    while i != 10
    {
        // print the current index
        Print i
        // increment by one
        i += 1
    }
}
```

#### Basic Math
In BlueScript there are 4 core operations
- Addition
- Subtraction
- Multiplication
- Division

With The BlueScript math library there are going to be more but thats for a different section.
```js
function main()
{
    // math inside variable declarations
    div = 10 / 2 // evaluates to 5.0
    mul = 10 * 2 // evaulates to 20
    sub = 2 - 4  // evaulates to -2
    add = 3 + 5  // evaluates to 8

    // math in variable extensions
    // where we are just changing
    // the value of a variable
    div /= 2  
    mul *= 6
    sub -= 3
    add += 1
}
```


#### Functions

#### Includes


#### Including other BlueScript scripts (.bs)

#### Using pre-compiled BlueScript files (.cbs)

#### Arrays

#### Keywords

| key word | purpose                                   |
|----------|-------------------------------------------|
| function | a marker of where a function starts       |
| return   | return from a function                    |
| if       | check if an expression is true else pass  |
| else     | do something if the previous if was false |
| while    | do something while true                   |
| Print    | print to console                          |
| input    | take command line input                   |
| free     | free unused memory                        |
| true     | set a true value                          |
| false    | set a false value                         |
| null     | set a null value                          |

### BlueScript Compiled
True BlueScript that actually gets run is not the same as what the parser
reads. The parser reads
```js
function foo(bar)
{
    bar += 1
    return bar
}
```
but the actual runtime sees
```
label foo
    pop foo.bar
    add foo.bar,1
    push %foo.bar
    free %foo.bar
    ret
```
