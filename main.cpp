#include "RenderContext.h"
#include "LayoutManager.h"
#include "Sprite.h"
#include "Text.h"
//#include "Console.h"
#include "Utils/Utils.h"

int main() {
    try {
        std::string mediaPath = UTILS::GetMediaFolder();
        std::string fontPath = mediaPath + "Fonts/Calibri.ttf";

        RenderContextSetup setup;
        setup.Width = 640;
        setup.Height = 480;
        RenderContext renderContext(setup);
        //LayoutManager layout(&renderContext);

        //Text text(&renderContext, fontPath, &layout);
        Sprite sprite0(&renderContext);
        Sprite sprite1(&renderContext);
        Sprite sprite2(&renderContext);
        Sprite sprite3(&renderContext);
        
        sprite0.SetSize(0.4f, 0.3f);
        sprite1.SetSize(0.4f, 0.3f);
        sprite2.SetSize(0.4f, 0.3f);
        sprite3.SetSize(0.4f, 0.3f);
        
        sprite0.SetPosition(0.0f, 0.0f);
        sprite1.SetPosition(0.6f, 0.0f);
        sprite2.SetPosition(0.0f, 0.7f);
        sprite3.SetPosition(0.6f, 0.7f);

        sprite0.SetTexture(mediaPath + "Textures/Smile.png");
        sprite1.SetTexture(mediaPath + "Textures/Smile.png");
        sprite2.SetTexture(mediaPath + "Textures/Smile.png");
        sprite3.SetTexture(mediaPath + "Textures/Smile.png");
        //Console console(&renderContext);

        //text << Text::set_size(48) << Text::set_pos(0.0, 0.5) << "Font rendering\nis guano!";
        float opacity = 1.0f;
        bool direction = true;
        while (renderContext.ProcessMessage()) {
            renderContext.Clear();
            if (direction) {
                opacity -= 0.01f;
                sprite0.SetPosition(sprite0.GetX() + 0.01f, sprite0.GetY() + 0.01f);
                sprite3.SetPosition(sprite3.GetX() - 0.01f, sprite3.GetY() - 0.01f);
                if (opacity <= 0.0f) {
                    direction = false;
                    sprite2.SetParent(NULL);
                    sprite1.SetParent(&sprite2);
                    sprite0.SetParent(&sprite2);
                    sprite3.SetParent(&sprite2);
                }
            }
            else {
                opacity += 0.01f;
                sprite0.SetPosition(sprite0.GetX() - 0.01f, sprite0.GetY() - 0.01f);
                sprite3.SetPosition(sprite3.GetX() + 0.01f, sprite3.GetY() + 0.01f);
                if (opacity >= 1.0f) {
                    direction = true;
                    sprite1.SetParent(NULL);
                    sprite2.SetParent(&sprite1);
                    sprite0.SetParent(NULL);
                    sprite3.SetParent(NULL);
                }
            }
            sprite0.SetOpacity(opacity);
            sprite3.SetOpacity(opacity);

            sprite0.Render();
            sprite1.Render();
            sprite2.Render();
            sprite3.Render();
            
            //console.Render();
            //text.Render();
            renderContext.Present();
        }
        return 0;
    }
    catch (std::exception& ex) {
        ::MessageBoxA(NULL, ex.what(), NULL, NULL); 
    }
}