# panda

## quick look
test:
````
$ make test
````

biuld your panda:
````
$ make panda
````

run panda:
````
$ ./panda/panda               // interactive mode
$ ./panda/panda input.pd      // interpreter mode, execute string file
$ ./panda/panda -c input.pd   // compile input.pd, output name is: input.pdc
$ ./panda/panda input.pdc     // interpreter mode, execute compiled file
````

panda language:
````
var a;                        // define variable a
var b = 1;                    // define variable b, initialized as a number
var c = "hello world";        // define variable c, initialized as a string
var d = [1, 2, 3];            // define variable d, initialized as a array
var e = {a: 1, b: 2, c: 3};   // define variable e, initialized as a dictionary

def add(a, b) {               // define function add
  return a + b;
}

print(a, b, c);               // call native function "print"
print(add(1, b));
print(add(c, ", bye"));

````

