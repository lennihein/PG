## readme

The debugger was tested on a fresh Ubuntu 18.10 

### setup

Go to directory /DefectDeflect and run the following console commands:

    chmod +x setup.sh
    sudo ./setup.sh

### execute

In directory /DefectDeflect run the following command:

    ./cli.sh

### utilize 

Once the cli is exectued it will run the debugger.
Messages sent to the cli will be redirected to the debugger and then executed.

When everything is set up a choice of different commands are shown.
*For example if you want to poke in register RAX type:
'poke_reg' and then '80' for Register RAX. Next input will be the data you want to overwrite it with.
*The data input has to be decimal numbers (exception is 'inject_instructions' payload)
*addresses are also refered to with decimal numbers.
*The input is not case sensitive.