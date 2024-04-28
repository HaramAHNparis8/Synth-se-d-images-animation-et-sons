SHELL = /bin/sh

# 컴파일러 설정
CC = gcc

# SDL2 컴파일 플래그 및 링크 플래그
SDL_CFLAGS = $(shell sdl2-config --cflags)
SDL_LDFLAGS = $(shell sdl2-config --libs)

# 추가 헤더 파일 위치
CPPFLAGS = -I. -I/opt/local/include -I/usr/local/include $(SDL_CFLAGS)

# 컴파일 옵션
CFLAGS = -Wall -O3

# 링크 옵션
LDFLAGS = -L/opt/local/lib -lSDL2_image -lSDL2_mixer -lSDL2_ttf -lassimp -lGL4Dummies $(SDL_LDFLAGS)
ifeq ($(shell uname),Darwin)
    LDFLAGS += -framework OpenGL
else
    LDFLAGS += -lGL
endif

# 소스 파일
SOURCES = window.c assimp.c credit.c
# 오브젝트 파일
OBJ = $(SOURCES:.c=.o)
# 최종 실행 파일 이름
PROGNAME = texte_ttf

# 빌드 대상
all: $(PROGNAME)

# 링크 규칙
$(PROGNAME): $(OBJ)
	$(CC) $(OBJ) -o $@ $(LDFLAGS)

# 컴파일 규칙
%.o: %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

# 클린 규칙
clean:
	rm -f $(OBJ) $(PROGNAME)

.PHONY: all clean
