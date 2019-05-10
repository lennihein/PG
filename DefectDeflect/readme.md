## readme

The debugger was tested on a fresh Ubuntu 18.10 

### setup

Go to directory /DefectDeflect and run the following console commands:

    chmod +x setup.sh
    sudo ./setup.sh

### compile Debugger Server

    make compile

### compile CLI

    chmod +x cli.sh
    sudo ./cli.sh

### execute CLI

In directory /DefectDeflect run the following command to start debugging <TARGET>:

    ./cli <TARGET>

### utilise 

Once the cli is exectued it will run the debugger.
Messages sent to the cli will be redirected to the debugger and then executed.

When everything is set up a choice of different commands are shown.
*For example if you want to poke in register RAX type:
'poke_reg' and then '80' for Register RAX. Next input will be the data you want to overwrite it with.
*The data input has to be decimal numbers (exception is 'inject_instructions' payload)
*addresses are also refered to with decimal numbers.
*The input is not case sensitive

Output of the Tracee will be found in `log.log`, alongside some Log from the Debugging Server
