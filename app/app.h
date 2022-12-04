#ifndef APP_H_
#define APP_H_

#define APP_MENU_SIZE 11

#include "./app_def.h"
#include "./app_project.h"
#include "./app_tracks.h"
#include "./app_view_instrument.h"
#include "./app_view_pattern.h"
#include "./app_view_project.h"
#include "./app_view_projectEditName.h"
// #include "./app_view_track.h"
// #include "./app_view_trackDelay.h"
#include "./app_view_menu.h"
#include "./app_view_trackSequencer.h"
#include <app_core_display.h>
#include <app_core_renderer.h>
#include <zic_seq_tempo.h>

class App {

protected:
    App(App_Display* _display)
        : menuView(&menu[0])
    {
        App::display = _display;
        menuView.initMenu();
    }

public:
    App_Tracks tracks;
    Zic_Seq_Tempo<> tempo;
    App_Project project;

    Zic_Seq_Pattern patterns[PATTERN_COUNT];

    static App_Display* display;
    UiKeys keys;

    App_View_Menu menuView;

    bool rendered = false;

    Menu menu[APP_MENU_SIZE] = {
        (Menu) { 10, 1, "Grid: Pattern sequencer", "Pattern", App_View_TrackSequencer::getInstance(&tracks, &patterns[0]) },
        (Menu) { 11, 1, "Grid: Instrument seq.", "Instr.", NULL },
        (Menu) { 12, 1, "Grid: Effect sequencer", "IFX", NULL },
        (Menu) { 13, 1, "Grid: Volume & Master FX", "VOL+MFX", NULL },
        (Menu) { 20, 2, "Edit: Pattern", "Pattern", App_View_Pattern::getInstance(&patterns[0]) },
        (Menu) { 21, 2, "Edit: Instrument", "Instr.", App_View_Instrument::getInstance(&tracks) },
        (Menu) { 22, 2, "Edit: Effect", "IFX", NULL },
        (Menu) { 30, 3, "Scatter effect", "Scatter", NULL },
        (Menu) { 31, 3, "Master filter & effect", "MF+MFX", NULL },
        (Menu) { 40, 4, "Project", "Project", App_View_Project::getInstance(&tempo, &tracks, &project, &menuView) }, // Select project
        (Menu) { 41, 4, "Edit project name", "Name", App_View_ProjectEditName::getInstance(&project, &menuView) }, // Select project
    };

    static App* instance;

    static App* getInstance(App_Display* _display)
    {
        if (!instance) {
            instance = new App(_display);
        }
        return instance;
    }

    void sample(float* buf, int len)
    {
        if (tempo.next(SDL_GetTicks64()))
        // if (tempo.next())
        {
            tracks.next();
            if (menuView.getView()->renderOn(EVENT_VIEW_ON_TEMPO)) {
                render();
            }
        }
        tracks.sample(buf, len);
    }

    void render()
    {
        if (App::display->ready()) {
            // TODO find a better place way to reset cursor
            App::display->reset();
            menuView.getView()->render(App::display);
            App::display->drawText();
            rendered = true;
        }
    }

    void handleUi(uint8_t keysBin)
    {
        keys.Up = (keysBin >> UI_KEY_UP) & 1;
        keys.Down = (keysBin >> UI_KEY_DOWN) & 1;
        keys.Left = (keysBin >> UI_KEY_LEFT) & 1;
        keys.Right = (keysBin >> UI_KEY_RIGHT) & 1;
        keys.Edit = (keysBin >> UI_KEY_EDIT) & 1;
        keys.Menu = (keysBin >> UI_KEY_MENU) & 1;
        // SDL_Log("%d%d%d%d%d%d\n", keys.Up, keys.Down, keys.Left, keys.Right, keys.A, keys.Y);

        if (keys.Menu && keys.Edit) {
            tracks.togglePlay();
        } else if (menuView.update(&keys, App::display) != VIEW_NONE) {
            render();
        } else if (menuView.getView()->update(&keys, App::display) != VIEW_NONE) {
            render();
        }
    }

    void quit()
    {
        App_Renderer renderer;
        for (uint8_t i; i < APP_MENU_SIZE; i++) {
            if (menu[i].view) {
                renderer.reset();
                renderer.startRow = 0;
                // TODO need to use different way to save project
                // menu[i].view->snapshot(&renderer);
            }
        }
    }

    void start()
    {
        for (uint8_t i; i < APP_MENU_SIZE; i++) {
            if (menu[i].view) {
                menu[i].view->loadSnapshot();
            }
        }
    }
};

App_Display* App::display = NULL;
App* App::instance = NULL;

#endif