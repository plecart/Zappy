CC			=	gcc -g3

TOOLS__DIR	=	tools/srcs/
SERVER_DIR	=	server/srcs/
CLIENT_DIR	=	client/srcs/


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
				$(TOOLS__SRCS)
CLIENT_SRCS	=	$(CLIENT_DIR)main_client.c \
				$(CLIENT_DIR)flags_client.c \
				$(CLIENT_DIR)client.c \
				$(TOOLS__SRCS)

SERVER_OBJS	=	$(SERVER_SRCS:.c=.o)
CLIENT_OBJS =	$(CLIENT_SRCS:.c=.o)

SERVER_NAME	=	zappy_server
CLIENT_NAME	=	zappy_client

RM			=	rm -rf
CFLAGS		+=  -Wall -Wextra

all: server client

$(SERVER_NAME): $(SERVER_OBJS)
	$(CC) -o $(SERVER_NAME) $(SERVER_OBJS)

$(CLIENT_NAME): $(CLIENT_OBJS)
	$(CC) -o $(CLIENT_NAME) $(CLIENT_OBJS)

server: $(SERVER_NAME)

client: $(CLIENT_NAME)

clean:
	$(RM) $(SERVER_OBJS)
	$(RM) $(CLIENT_OBJS)

fclean: clean
	$(RM) $(SERVER_NAME)
	$(RM) $(CLIENT_NAME)

re: fclean all

.PHONY: server client all clean fclean re 