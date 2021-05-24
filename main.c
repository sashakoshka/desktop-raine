#include <SDL2/SDL.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>

#define TEXLEN 22

#define EV_IDLE 0
#define EV_LOOK 1

#define STATE_IDLE 0
#define STATE_LOOK 1
#define STATE_WALK 2
#define STATE_BOOP 3

SDL_Window   *window   = NULL;
SDL_Renderer *renderer = NULL;
SDL_Surface  *surface  = NULL;
SDL_Event     event;
SDL_Texture  *framesTex[TEXLEN];
SDL_Surface  *framesImg[TEXLEN];
SDL_Rect      winRect = {0, 0, 118, 223};

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
  
  window = SDL_CreateWindow("Desktop Raine",
    SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
    118, 223, SDL_WINDOW_SHOWN | SDL_WINDOW_ALWAYS_ON_TOP
  );
  if(window == NULL) {
    printf("%s\n", SDL_GetError());
    goto error;
  }
  SDL_SetWindowBordered(window, SDL_FALSE);
  
  renderer = SDL_CreateRenderer(
    window,
    -1, 0
  );
  if(renderer == NULL) {
    printf("%s\n", SDL_GetError());
    goto error;
  }
  
  surface = SDL_GetWindowSurface(window);
  
  //SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
  
  // Load assets
  
  char *genericResPath = "img/";
  char *unixResPath = "/usr/share/icons/desktop-raine/";
  char *winResPath = "C:\\Program Files\\Desktop Raine\\img\\";
  char *resPath;
  char iconPath[128]   = {0};
  
  if(argc > 1) {
    resPath = argv[1];
  } else if(access(genericResPath, F_OK) == 0) {
    resPath = genericResPath;
  } else if(access(unixResPath, F_OK) == 0) {
    resPath = unixResPath;
  } else if(access(winResPath, F_OK) == 0) {
    resPath = winResPath;
  } else {
    goto res_error;
  }
  
  catstr(catstr(iconPath, resPath), "icon.bmp");
  SDL_Surface *icon = SDL_LoadBMP(iconPath);
  SDL_SetWindowIcon(window, icon);
  
  char *framesName[TEXLEN] = {
    "blank",    // nothing
    
    "idle_2",   // ide animation
    "idle_1",
    "blink",
    
    "look_nw",  // north lookaround
    "look_n",
    "look_ne",
    
    "look_w",   // middle lookaround
    "idle_1",
    "look_e",
    
    "look_sw",  // south lookaround
    "look_s",
    "look_se",
    
    "walk_l_1", // left walk
    "walk_l_2",
    "walk_l_3",
    "walk_l_4",
    
    "walk_r_1", // right walk
    "walk_r_2",
    "walk_r_3",
    "walk_r_4",
    
    "boop"      // click
  };
  
  char resPaths[TEXLEN][128] = {0};
  for(int i = 0; i < TEXLEN; i++) {
    catstr(
      catstr(
        catstr(
          resPaths[i],
          resPath),
        framesName[i]),
      ".bmp"
    );
    printf("loading %s\n", resPaths[i]);
    framesImg[i] = SDL_LoadBMP(resPaths[i]);
    if(framesImg[i] == NULL) goto res_error;
    SDL_SetColorKey(
      framesImg[i], SDL_TRUE,
      SDL_MapRGB( framesImg[i]->format, 0xFF, 0, 0xFF )
    );
    framesTex[i] = SDL_CreateTextureFromSurface(
      renderer, framesImg[i]
    );
  }
  
  time_t t;
  srand((unsigned)time(&t));
  
  // Loop
  
  int idleFrame    = 0;
  int blinkCounter = 0;
  int lookCounter  = 0;
  int walkFrame    = 0;
  int       walkVX = 16,
            walkVY = 0;
  int walkCounter  = 0;
  int state        = STATE_IDLE;
  frame(1);
  
  // Fire EV_IDLE every half second
  SDL_AddTimer(500, idleTick, NULL);
  
  while(SDL_WaitEvent(&event)) {
    rand();
    SDL_PumpEvents();
    SDL_GetMouseState(&mouseX, &mouseY);
    SDL_GetWindowPosition(window, &winX, &winY);
    
    switch (event.type) {
      case SDL_QUIT:
        goto exit;
      
      case SDL_MOUSEBUTTONDOWN:
        rand();
        switch(event.button.button) {
          case SDL_BUTTON_LEFT:
            if(mouseX > 48 && mouseX < 84 && mouseY < 80) {
              lookCounter = 1;
              state = STATE_BOOP;
              frame(21);
            }
            break;
          case SDL_BUTTON_RIGHT:
            
            break;
        }
        break;
      
      case SDL_MOUSEMOTION:
        rand();
        if(state <= STATE_LOOK) {
          lookCounter = 1;
          state = STATE_LOOK;
          int lookDirection = 0;
          lookDirection += mouseY > 32;
          lookDirection += mouseY > 44;
          lookDirection *= 3;
          lookDirection += 4;
          lookDirection += mouseX > 48;
          lookDirection += mouseX > 84;
          frame(lookDirection);
        }
        break;
      
      case SDL_USEREVENT:
        switch(event.user.code) {
          case EV_IDLE:
            switch(state) {
              case STATE_IDLE:
                // Currently idling - standing and blinking
                blinkCounter--;
                if(blinkCounter < 0 && idleFrame) {
                  idleFrame = 2;
                  blinkCounter = 6;
                }
                if(state == STATE_IDLE) {
                  frame(1 + idleFrame);
                  idleFrame = !idleFrame;
                }
                // Decide to walk every once in a while
                if(rand() % 24 == 0) {
                  walkCounter = rand() % 32;
                  walkVX = (rand() % 2) * 32 - 16;
                  walkVY = (rand() % 32) - 16;
                  state = STATE_WALK;
                }
                break;
              
              case STATE_BOOP:
              case STATE_LOOK:
                if(lookCounter > 0)
                  lookCounter--;
                else {
                  /* Lose interest in mouse, blink and return to
                  idle state */
                  state = STATE_IDLE;
                  blinkCounter = 0;
                  idleFrame = 1;
                }
                break;
              
              case STATE_WALK:
                // Walk around the screen
                SDL_SetWindowPosition(
                  window,
                  winX + walkVX,
                  winY + walkVY
                );
                frame(13 + 4 * (walkVX > 0) + walkFrame);
                walkFrame++;
                walkFrame %= 4;
                if(rand() % 8 == 0) {
                  walkVX = (rand() % 2) * 32 - 16;
                  walkVY = (rand() % 32) - 16;
                }
                if(walkCounter > 0)
                  walkCounter--;
                else
                  state = STATE_IDLE;
                break;
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
  
  res_error:
  SDL_ShowSimpleMessageBox(
    SDL_MESSAGEBOX_ERROR,
    "Could not get images",
    "Could not get images. Make sure program is installed.",
    window
  );
  
  error:
  return 1;
}

/*
  frame
  Draws the specified frame to screen
*/
void frame(int frame) {
  SDL_SetRenderDrawColor(renderer, 80, 120, 180, 0);
  SDL_RenderClear(renderer);
  SDL_RenderCopy(renderer, framesTex[frame], NULL, NULL);
  SDL_RenderPresent(renderer);
}

/*
  idleTick
  Callback to fire an EV_IDLE code.
*/
Uint32 idleTick(Uint32 interval, void *param) {
  SDL_Event event;
  SDL_UserEvent userevent;
  
  userevent.type = SDL_USEREVENT;
  userevent.code = EV_IDLE;

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
