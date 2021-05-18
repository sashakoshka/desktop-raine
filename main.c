#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#define TEXLEN 3


SDL_Window   *window   = NULL;
SDL_Renderer *renderer = NULL;
SDL_Event     event;
SDL_Texture  *framesTex[TEXLEN];

void frame(int);

int main(int argc, char *argv[]) {
  int mouseX = 0, mouseY = 0;
  
  // Initialize SDL
  
  if(SDL_Init(SDL_INIT_VIDEO) < 0) {
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
  
  char *unixResPath[TEXLEN] = {
    "/usr/share/icons/desktop-raine/0.png",
    "/usr/share/icons/desktop-raine/1.png",
    "/usr/share/icons/desktop-raine/2.png",
  };
  
  char *genericResPath[TEXLEN] = {
    "img/0.png",
    "img/1.png",
    "img/2.png",
  };
  
  SDL_Surface *icon;
  SDL_Surface *framesImg[TEXLEN];
  
  if(access(genericResPath[0], F_OK) == 0) {
    icon = IMG_Load("img/icon.png");
    for(int i = 0; i < TEXLEN; i++)
      framesImg[i] = IMG_Load(genericResPath[i]);
  } else if(access(unixResPath[0], F_OK) == 0) {
    icon = IMG_Load("/usr/share/icons/desktop-raine/icon.png");
    for(int i = 0; i < TEXLEN; i++)
      framesImg[i] = IMG_Load(unixResPath[i]);
  } else {
    SDL_ShowSimpleMessageBox(
      SDL_MESSAGEBOX_ERROR,
      "Could not get images",
      "Could not get images. Make sure program is installed.",
      window
    );
    goto error;
  }
  
  SDL_SetWindowIcon(window, icon);
  
  for(int i = 0; i < TEXLEN; i++)
    framesTex[i] = SDL_CreateTextureFromSurface(
      renderer, framesImg[i]
    );
  
  // Loop
  
  while(SDL_WaitEvent(&event)) {
    SDL_PumpEvents();
    SDL_GetMouseState(&mouseX, &mouseY);
  
    frame(1);
    
    switch (event.type) {
      case SDL_QUIT:
        goto exit;
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
