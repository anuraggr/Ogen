A dummy compiler for a dummy language.

Generates asm only for x86_64 for Linux.

- **Target Architecture**: x86_64
- **Operating System**: Linux
- **Assembler**: NASM
- **Linker**: LD


## Example Code Snippets

```
let x = 10;

for(let i = 0; i<x; i=i+1){
    if(i==10){
        exit(i);
    }
}

exit(x);
```

```
let x = 10;

while(x>=1){
    x = x-1;
}

exit(x);
```
```
let variableName be 2;

if(variableName==10){
    exit(variableName);
}
elif(variableName==9){
    exit(variableName);
}
else{
    exit(1);
}
```
```
let var1 = 10;
let var2 = 20;

if(var1+var2==10){
    exit(99);
}
#Exits with 0 because condition not true.
#This is for comments.
```

```
let var1 = 10;
let var2 = 20;

print(var2+var1); #print stmt

exit(10);
```

```
print(var2+var1+1);

fun add(x, y) {
   return x + y;
}

print(add(100, add(10,1)));     #prints 111
exit(69);
```



## Keywords:
- **exit**
- **let**
- **be**
- **if**
- **elif**
- **else**
- **while**
- **for**
- **print**
- **func**