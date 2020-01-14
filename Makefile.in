# N-Mail Makefile

TARGET    =nmail
OBJS      =main.o ui_main.o ui_model.o imap_client.o imap_folder.o imap_message.o address_list.o
CFLAGS    =-Wall -g
CXX       =g++
INCLUDES  =-I./ 
INCLUDES += $(shell pkg-config --cflags webkit2gtk-4.0 gtkmm-3.0 libcurl icu-uc icu-io icu-i18n libssl libcrypto)
LIBS      = $(shell pkg-config --libs webkit2gtk-4.0 gtkmm-3.0 libcurl icu-uc icu-io icu-i18n libssl libcrypto)

$(TARGET): ${OBJS}
	${CXX} -o $@ ${OBJS} ${LIBS}

%.o: %.cpp
	${CXX} -o $@ -c ${CFLAGS} ${INCLUDES} $<

clean:
	rm -f $(TARGET) $(OBJS) core *.core
