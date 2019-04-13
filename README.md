compile the project using ./make.sh
if you want to create a c/cpp/shell project,you can use ./make.sh -p option.
-p means project,you can choose to create c or cpp or shell project by [-c|-cpp|-sh] option
for example:
    you want to create a c project named "hello",so you just need to run ./make.sh -p -c hello
    then you will get a c project including:compiler.sh(compile script),makefile,main.c
    you can run the project using the command ./compiler.sh
