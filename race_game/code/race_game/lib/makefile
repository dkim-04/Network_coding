OBJS=setup_client.o setup_server.o setup_m_srv.o

snet.a: $(OBJS)
	rm -f snet.a
	ar rc snet.a ${OBJS}
	ranlib snet.a
	rm ${OBJS} 