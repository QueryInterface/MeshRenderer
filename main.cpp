#include "RenderContext.h"
#include "LayoutManager.h"
#include "Sprite.h"
#include "Font.h"
#include "Console.h"
#include "Utils/Utils.h"

int main() {
    try {
        std::string mediaPath = UTILS::GetMediaFolder();
        std::string fontPath = mediaPath + "Fonts/Calibri.ttf";

        RenderContextSetup setup;
        setup.Width = 640;
        setup.Height = 480;
        RenderContext renderContext(setup);
        LayoutManager layout(&renderContext);

        Font font(&renderContext, fontPath, &layout);
        Sprite sprite(&renderContext);
        Console console(&renderContext);

        font << Font::set_size(48) << Font::set_pos(0.0, 0.5) << "Font rendering\nis guano!";

        while (renderContext.ProcessMessage()) {
            renderContext.Clear();
            sprite.Render();
            console.Render();
            font.Render();
            renderContext.Present();
        }
        return 0;
    }
    catch (std::exception& ex) {
        ::MessageBoxA(NULL, ex.what(), NULL, NULL); 
    }
}