# nmail
N-Mail sources

## how to make
I've just started this making and there's no Makefile in it yet.  
You can make with;  

>> g++ -c -o ui_main.o ui_main.cpp -I./ `pkg-config --cflags --libs gtkmm-3.0 libcurl`
>> g++ -o nmail main.o ui_main.o ui_handler.o imap_client.o `pkg-config --libs gtkmm-3.0 libcurl`

You compile each .cpp to .o like above first line.
 
