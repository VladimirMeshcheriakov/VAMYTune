# TRITON
## Sound Synthesis application

This application will allow to create sounds out of basic soundwaves

### Installation

Download the source code and compile it using make

```$ make```

### Usage

#### Tool bar

![Tool bar](https://cdn.discordapp.com/attachments/509756655215443969/975420549792665630/barre_doutils.png?size=1024)



##### Connect a Midi device

You can connect a device using the **Choose midi device** button.
A window will pop up.
Select your device in the list (if no midi device is connected to your computer, the list will be empty)


If you don't have a midi device, you will have to use the visual keyboard.


##### Save your work

To save your sound, press the **save file** button in the toolbar.
A window will pop up asking you the name for your save.
The file will be saved in the current directory.



##### Load your work

To load a signal, **press the top left button**.
Select the file you want to open.
The file must be named " ~~name~~ *.triton* ".

***WARNING***

Loading a file will not overwrite your current progress so you can combine multiple complex signals.



#### Tab bar

##### Record

From this tab you can record a song and then play it.

You can also record will playing a song if you want to include multiple voices in your song.

![record tab](https://cdn.discordapp.com/attachments/509756655215443969/975422695078506596/record_interface.png?size=1024)

##### Midi

From this tab, you can record a midi song.

Recording this way will let you see what you've played.

You can then change the sound used if needed.

![Midi record](https://cdn.discordapp.com/attachments/509756655215443969/975421698453147708/midid_record.png?size=1024)


##### Signal Creation


This tab let you create a sound from basic soundwaves (sine / square / triangle / saw).

Select a signal on top of the tab to add it to the list.

Scroll through the list to select the parameters for each signal (amplitude / frequency / components / shift).

For each signal, a visaliser will allow you to see the look of the sound.

The visualiser on top of the tab is the sum of all the signals you added.

Use the scroller on the left to zoom in / out.

![signal creation tab](https://cdn.discordapp.com/attachments/509756655215443969/975423394042494996/crea.png?size=1024)


##### Filter

The filter tab let you remove some frequencies of your signal using filters (low pass / high pass / band pass / band cut)

Select the filters on the left.

The result can be seen on the right.


![filter tab](https://cdn.discordapp.com/attachments/509756655215443969/975423543045140490/filter_in_application.png?size=1024)

##### ADSR

The ADSR tab will allow you to edit the attack, decay, sustain and release parameters.

The sliders on top of the tab are the velocities at the start of each phases

The parameters at the bottom of the tab are the duration of each phases (the widget might be flattened right under the visualizer)

![ADSR](https://cdn.discordapp.com/attachments/509756655215443969/975420085294493766/adsr_schema.png?size=1024)


### Contact


This project is far from done, and has a lot of bugs and unimplemented functionalities, however any ideas and reports will be welcome
Please contact me on discord: https://discordapp.com/users/759453185441529867/
