#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#define TEXLEN 12

#define EV_IDLE 0
#define EV_LOOK 1

#define STATE_IDLE    0
#define STATE_LOOK    1

SDL_Window   *window   = NULL;
SDL_Renderer *renderer = NULL;
SDL_Event     event;
SDL_Texture  *framesTex[TEXLEN];

void frame(int);
char* catstr(char*, char*);
Uint32 idleTick(Uint32, void*);

int main(int argc, char *argv[]) {
  int mouseX = 0, mouseY = 0,
      winX   = 0, winY   = 0;
  
  // Initialize SDL
  
  if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0) {
    printf("cant make window\n");
    goto error;
  }
  
  window = SDL_CreateWindow("",
    SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
    118, 223, SDL_WINDOW_SHOWN | SDL_WINDOW_ALWAYS_ON_TOP
  );
  if(window == NULL) {
    printf("%s\n", SDL_GetError());
    goto error;
  }
  
  renderer = SDL_CreateRenderer(
    window,
    -1, 0
  );
  if(renderer == NULL) {
    printf("%s\n", SDL_GetError());
    goto error;
  }
  
  SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
  
  // Load assets
  
  char *genericResPath = "img/";
  char *unixResPath    = "/usr/share/icons/desktop-raine/";
  char *resPath;
  char iconPath[128]   = {0};
  
  if(argc > 1) {
    resPath = argv[1];
  } else if(access(genericResPath, F_OK) == 0) {
    resPath = genericResPath;
  } else if(access(unixResPath, F_OK) == 0) {
    resPath = genericResPath;
  } else {
    SDL_ShowSimpleMessageBox(
      SDL_MESSAGEBOX_ERROR,
      "Could not get images",
      "Could not get images. Make sure program is installed.",
      window
    );
    goto error;
  }
  
  catstr(catstr(iconPath, resPath), "icon.png");
  SDL_Surface *icon = IMG_Load(iconPath);
  SDL_SetWindowIcon(window, icon);
  
  char *framesName[TEXLEN] = {
    "blank",
    "idle_2",
    "idle_1",
    "blink",
    "look_n",
    "look_ne",
    "look_e",
    "look_se",
    "look_s",
    "look_sw",
    "look_w",
    "look_nw"
  };
  
  SDL_Surface *framesImg[TEXLEN];
  char resPaths[TEXLEN][128] = {0};
  for(int i = 0; i < TEXLEN; i++) {
    catstr(
      catstr(
        catstr(
          resPaths[i],
          resPath),
        framesName[i]),
      ".png"
    );
    framesImg[i] = IMG_Load(resPaths[i]);
    framesTex[i] = SDL_CreateTextureFromSurface(
      renderer, framesImg[i]
    );
  }
  
  // Loop
  
  int idleFrame    = 0;
  int blinkCounter = 0;
  int lookCounter  = 0;
  int state        = STATE_IDLE;
  frame(1);
  SDL_AddTimer(500, idleTick, NULL);
  
  while(SDL_WaitEvent(&event)) {
    SDL_PumpEvents();
    SDL_GetMouseState(&mouseX, &mouseY);
    SDL_GetWindowPosition(window, &winX, &winY);
    
    switch (event.type) {
      case SDL_QUIT:
        goto exit;
      
      case SDL_USEREVENT:
        switch(event.user.code) {
          case EV_IDLE:
            blinkCounter++;
            if(lookCounter > 0) lookCounter--;
            if(blinkCounter > 6 && idleFrame) {
              idleFrame = 2;
              blinkCounter = 0;
            }
            if(state == STATE_IDLE) {
              frame(1 + idleFrame);
              idleFrame = !idleFrame;
            }
            break;
          
          case EV_LOOK:
            lookCounter = 5;
            break;
        }
        break;
    }
  }
  
  exit:
  for(int i = 0; i < TEXLEN; i++) {
    SDL_DestroyTexture(framesTex[i]);
    SDL_FreeSurface(framesImg[i]);
  }
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
  return 0;
  
  error:
  return 1;
}

/*
  frame
  Draws the specified frame to screen
*/
void frame(int frame) {
  SDL_RenderCopy(renderer, framesTex[frame], NULL, NULL);
  SDL_RenderPresent(renderer);
}

Uint32 idleTick(Uint32 interval, void *param) {
  SDL_Event event;
  SDL_UserEvent userevent;
  
  userevent.type = SDL_USEREVENT;
  userevent.code = 0;

  event.type = SDL_USEREVENT;
  event.user = userevent;

  SDL_PushEvent(&event);
  return(interval);
}

/*
  catstr
  Adds the second char* to the first one, starting at wherever
  the first pointer points. Returns the pointer to the index it
  left off on so it can be daisy chained.
*/
char* catstr(char *paper, char *pen) {
  while(*pen) {
    *paper = *pen;
    paper++;
    pen++;
  }
  return paper;
}
