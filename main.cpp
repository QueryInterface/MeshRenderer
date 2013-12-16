#include "RenderContext.h"
#include "LayoutManager.h"
#include "Sprite.h"
#include "Text.h"
//#include "Console.h"
#include "Utils/Utils.h"

#define SPRITE_SIZE 64

int main() {
    try {
        std::string mediaPath = UTILS::GetMediaFolder();
        std::string fontPath = mediaPath + "Fonts/courier.ttf";

        RenderContextSetup setup;
        setup.Width = 640;
        setup.Height = 480;
        RenderContext renderContext(setup);
        //LayoutManager layout(&renderContext);

        // Sprite setup
        Sprite sprite0(&renderContext);
        Sprite sprite1(&renderContext);
        Sprite sprite2(&renderContext);
        Sprite sprite3(&renderContext);
        
        sprite0.SetSize(SPRITE_SIZE, SPRITE_SIZE);
        sprite1.SetSize(SPRITE_SIZE, SPRITE_SIZE);
        sprite2.SetSize(SPRITE_SIZE, SPRITE_SIZE);
        sprite3.SetSize(SPRITE_SIZE, SPRITE_SIZE);
        
        sprite0.SetPosition(0, 0);
        sprite1.SetPosition(renderContext.GetWindowWidth() - SPRITE_SIZE, 0);
        sprite2.SetPosition(0, renderContext.GetWindowHeight() - SPRITE_SIZE);
        sprite3.SetPosition(renderContext.GetWindowWidth() - SPRITE_SIZE, renderContext.GetWindowHeight() - SPRITE_SIZE);

        sprite0.SetTexture(mediaPath + "Textures/Smile.png");
        sprite1.SetTexture(mediaPath + "Textures/Smile.png");
        sprite2.SetTexture(mediaPath + "Textures/Smile.png");
        sprite3.SetTexture(mediaPath + "Textures/Smile.png");
        //Console console(&renderContext);
        // Text setup
        Text text(&renderContext, fontPath);
        text << Text::set_size(32) << "Font rendering is guano!";
        float opacity = 1.0f;
        bool direction = true;
        while (renderContext.ProcessMessage()) {
            renderContext.Clear();
            if (direction) {
                opacity -= 0.01f;
                sprite0.SetPosition(sprite0.GetX() + 1, sprite0.GetY() + 1);
                sprite3.SetPosition(sprite3.GetX() - 1, sprite3.GetY() - 1);
                if (opacity <= 0.0f) {
                    direction = false;
                }
            }
            else {
                opacity += 0.01f;
                sprite0.SetPosition(sprite0.GetX() - 1, sprite0.GetY() - 1);
                sprite3.SetPosition(sprite3.GetX() + 1, sprite3.GetY() + 1);
                if (opacity >= 1.0f) {
                    direction = true;
                }
            }
            sprite0.SetOpacity(opacity);
            sprite3.SetOpacity(opacity);

            sprite0.Render();
            sprite1.Render();
            sprite2.Render();
            sprite3.Render();
            
            //console.Render();
            text.Render();
            renderContext.Present();
        }
        return 0;
    }
    catch (std::exception& ex) {
        ::MessageBoxA(NULL, ex.what(), NULL, NULL); 
    }
}