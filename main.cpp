#include "RenderContext.h"
#include "Sprite.h"
#include "Font.h"
#include "Console.h"

int main() {
    try {
	    RenderContextSetup setup;
	    setup.Width = 640;
	    setup.Height = 480;
	    RenderContext renderContext(setup);
		Font font(&renderContext);
		Sprite sprite(&renderContext);
		Console console(&renderContext);
	    while (renderContext.ProcessMessage()) {
			renderContext.Clear();
            renderContext.Device->BeginScene();
            sprite.Render();
			console.Render();
            renderContext.Device->EndScene();
			renderContext.Present();
	    }
    	return 0;
	}
    catch (std::exception& ex) {
        ::MessageBoxA(NULL, ex.what(), NULL, NULL); 
    }
}