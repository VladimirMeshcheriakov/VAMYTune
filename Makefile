# Makefile

CPPFLAGS = -MMD
CC = gcc
CFLAGS =  `pkg-config --cflags gtk+-3.0` -Wall -Wextra  -ggdb3 -O3 -g -fsanitize=address -fsanitize=undefined -fno-sanitize-recover=all -fsanitize=float-divide-by-zero -fsanitize=float-cast-overflow -fno-sanitize=null -fno-sanitize=alignment
LDFLAGS =
LDLIBS = -lm -lSDL2 -lasound `pkg-config --libs gtk+-3.0`

SRC = dynamic_array/dynamic_array.c \
visualiser/node/node.c \
little_endian/little_endian.c \
time_stamp/time_stamp.c \
adsr/adsr.c \
keyboard_keys/keys.c \
userdata/userdata.c \
header_wav/header_wav.c \
wav_management/wav_management.c \
wav_interface/wav_interface.c \
FFT/complex_numbers/complex_number.c \
FFT/dft.c \
note_state_utils/note_state_utils.c \
basic_signals/basic_signals.c \
signals/signals.c \
visualiser/final_signal_production/final_signal_production.c \
visualiser/midi_record_vis/midi_record_vis.c \
midi_interface_management/midi_management.c \
visualiser/filter_visualiser/filter_vis.c \
visualiser/signal_creator/signal_creator_utils/signal_creator_utils.c \
visualiser/signal_creator/signal_creator_node_adder/list_node_adder.c \
visualiser/piano_widget/piano_widget.c \
visualiser/midi_device_chooser/midi_device_chooser.c \
visualiser/load_save_triton/load_save_triton.c \
visualiser/last_session/last_session.c \
visualiser/vis.c \
sdl_call_func/sdl_call_func.c \
audio_callback/audio_callback.c 
OBJ = ${SRC:.c=.o}
DEP = ${SRC:.c=.d}

all: main

main: dynamic_array/dynamic_array.o \
visualiser/node/node.o \
little_endian/little_endian.o \
time_stamp/time_stamp.o \
adsr/adsr.o \
keyboard_keys/keys.o \
userdata/userdata.o \
header_wav/header_wav.o \
wav_management/wav_management.o \
wav_interface/wav_interface.o \
FFT/complex_numbers/complex_number.o \
FFT/dft.o \
note_state_utils/note_state_utils.o \
basic_signals/basic_signals.o \
signals/signals.o \
visualiser/final_signal_production/final_signal_production.o \
visualiser/midi_record_vis/midi_record_vis.o \
midi_interface_management/midi_management.o \
visualiser/filter_visualiser/filter_vis.o \
visualiser/signal_creator/signal_creator_utils/signal_creator_utils.o \
visualiser/signal_creator/signal_creator_node_adder/list_node_adder.o \
visualiser/piano_widget/piano_widget.o \
visualiser/midi_device_chooser/midi_device_chooser.o \
visualiser/load_save_triton/load_save_triton.o \
visualiser/last_session/last_session.o \
visualiser/vis.o \
sdl_call_func/sdl_call_func.o \
audio_callback/audio_callback.o \

-include ${DEP}


.PHONY: clean
clean:
	${RM} ${OBJ}
	${RM} ${DEP}
	${RM} dynamic_array/dynamic_array \
	little_endian/little_endian \
	visualiser/node/node \
	time_stamp/time_stamp \
	adsr/adsr \
	keyboard_keys/keys \
	userdata/userdata \
	header_wav/header_wav \
	wav_management/wav_management \
	wav_interface/wav_interface \
	FFT/complex_numbers/complex_number \
	FFT/dft \
	note_state_utils/note_state_utils \
	basic_signals/basic_signals \
	signals/signals \
	visualiser/final_signal_production/final_signal_production \
	visualiser/midi_record_vis/midi_record_vis \
	midi_interface_management/midi_management \
	visualiser/filter_visualiser/filter_vis \
	visualiser/signal_creator/signal_creator_utils/signal_creator_utils \
	visualiser/signal_creator/signal_creator_node_adder/list_node_adder \
	visualiser/piano_widget/piano_widget \
	visualiser/midi_device_chooser/midi_device_chooser \
	visualiser/load_save_triton/load_save_triton \
	visualiser/last_session/last_session \
	visualiser/vis \
	sdl_call_func/sdl_call_func \
	audio_callback/audio_callback \
	main
# END