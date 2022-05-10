# VAMYTune
Sound Synthesis application

This application will allow to create sounds out of basic soundwaves (sine,square,triangle,saw).

Compile it with make

To connect yout midi keyboard please launch ./main -l and you will see the list of midi keyboards
Remember the port on which your keyboard is and close the app.
Now launch it with ./main -p 'the port of your midi-keyboard'

To create a signal, go to the "Signal Creator" tab, and choose one of 5 basic signals than click on it.
This will add this basic signal to the creator panel. Now you can play arround with parameters of the signal,
To set the desired curve. Let us describe the parameters:
    -   amplitude : This is the amplitude of this basic signal
    - frequency : This is the relative frequency in regards to the played frequency
        For instance if playing an A (440Hz) and the relative fequency is set to 0.5, the result will sound like 0.5 * 440 = 220Hz
    - phase : This is the phase of a signal
    - components : Complex parameter that represents the number of sinusoidal waves to sum in a saw-composite signal
    - shift : Shifts the square wave
You can also load signals from files with names .triton, and save the created signals to those files

To create and modify the ADSR of a signal go to the ADSR tab

To play around with the filters go to the filter tab, and really have fun! 


The Record tab is not yet working

NOTE:

This project is far from done, and has a lot of bugs and unimplemented functionalities, however any ideas and reports will be welcome
Please contact me on discord: https://discordapp.com/users/759453185441529867/




