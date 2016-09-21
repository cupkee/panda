# panda

## quick look
panda language:
````
var a = 0, b = 4;             // define variable a, b, initialized as a number
var c = "hello world";        // define variable c, initialized as a string
var d = [1, 2, 3];            // define variable d, initialized as a array
var e = {a: 1, b: 2, c: 3};   // define variable e, initialized as a dictionary

def add(a, b) {               // define function add
  return a + b;
}

def inc_a() a += 1;           // one line function, closure
def dec_a() a -= 1;

while (a < 10) {
    d.push(add(a, b))         // append element for array

    inc_a()                   // ';' is optional
}

````

test:
````
$ make test
````

biuld your program with panda (example):
````
$ make example
````
* repl
  a interactive interpreter of panda language
````
$ ./repl
$ ./panda/panda -c input.pd   // compile input.pd, output name is: input.pdc
$ ./panda/panda input.pdc     // interpreter mode, execute compiled file
````

* compile
  a compiler，that translate panda script file to panda binary image
````
$ ./compile input             // output: input.pdc
````

* dump
  a tool to dump infomation of panda binary image
````
$ ./dump input.pdc
````

* panda
  a interpreter panda script or binary image
````
$ ./panda input.pd            // script file
$ ./panda input.pdc           // image file
````

