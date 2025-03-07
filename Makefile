CC			=	gcc -g3

TOOLS__DIR	=	tools_dir/srcs/
SERVER_DIR	=	server_dir/srcs/
CLIENT_DIR	=	client_dir/srcs/


TOOLS__SRCS =	$(TOOLS__DIR)tools.c
SERVER_SRCS	=	$(SERVER_DIR)main_server.c \
				$(SERVER_DIR)flags_server.c \
				$(SERVER_DIR)server.c \
				$(SERVER_DIR)map.c \
				$(SERVER_DIR)player.c \
				$(SERVER_DIR)action.c \
				$(SERVER_DIR)get.c \
				$(SERVER_DIR)action_data.c \
				$(SERVER_DIR)egg.c \
				$(SERVER_DIR)graphic.c \
				$(TOOLS__SRCS)
CLIENT_SRCS	=	$(CLIENT_DIR)main_client.c \
				$(CLIENT_DIR)flags_client.c \
				$(CLIENT_DIR)client.c \
				$(CLIENT_DIR)brain.c \
				$(CLIENT_DIR)response.c \
				$(CLIENT_DIR)get.c \
				$(CLIENT_DIR)slave.c \
				$(TOOLS__SRCS)

SERVER_OBJS	=	$(SERVER_SRCS:.c=.o)
CLIENT_OBJS =	$(CLIENT_SRCS:.c=.o)

SERVER_NAME	=	server
CLIENT_NAME	=	client
GFX_NAME	=	gfx

RM			=	rm -rf
CFLAGS		+=  -Wall -Wextra

all: server client gfx

$(SERVER_NAME): $(SERVER_OBJS)
	$(CC) -o $(SERVER_NAME) $(SERVER_OBJS) 

$(CLIENT_NAME): $(CLIENT_OBJS)
	$(CC) -o $(CLIENT_NAME) $(CLIENT_OBJS) -lm

$(GFX_NAME):
	echo "#!/bin/bash" > $(GFX_NAME)
	echo "cd front && flutter run -d chrome" >> $(GFX_NAME)
	chmod +x $(GFX_NAME)

server: $(SERVER_NAME)

client: $(CLIENT_NAME)

gfx: $(GFX_NAME)

clean:
	$(RM) $(SERVER_OBJS)
	$(RM) $(CLIENT_OBJS)

fclean: clean
	$(RM) $(SERVER_NAME)
	$(RM) $(CLIENT_NAME)
	$(RM) $(GFX_NAME)

re: fclean all

.PHONY: server client gfx all clean fclean re 