#ifndef APP_VIEW_GRID_INSTRUMENT_H_
#define APP_VIEW_GRID_INSTRUMENT_H_

#include "./app_tracks.h"
#include <app_core_renderer.h>
#include <app_core_util.h>
#include <app_core_view_table.h>

#include "./app_view_grid.h"

class App_View_GridInstrumentField : public App_View_GridField {
protected:
    Zic_Seq_PatternComponent newComponent;
    bool editing = false;

public:
    App_View_GridInstrumentField(App_Tracks* _tracks, char* _description)
        : App_View_GridField(_tracks, _description)
    {
    }

    Zic_Seq_PatternComponent* getComponent(uint8_t row, uint8_t col, bool isSelected = true)
    {
        if (isSelected && editing) {
            return &newComponent;
        }
        App_Audio_Track* track = tracks->tracks[uint8_t(col / 3) % TRACK_COUNT];
        return &track->components[(row) % APP_TRACK_STATE_SIZE];
    }

    void selectCol0(App_Renderer* renderer, uint8_t row, uint8_t col)
    {
        Zic_Seq_PatternComponent* component = getComponent(row, col);
        if (component->pattern == NULL) {
            sprintf(description, "Pattern: to be selected");
        } else {
            sprintf(description, "Pattern: %02X (%d steps)", component->pattern->id + 1, component->pattern->stepCount);
        }
    }

    void selectCol1(App_Renderer* renderer, uint8_t row, uint8_t col)
    {
        Zic_Seq_PatternComponent* component = getComponent(row, col);
        sprintf(description, "Detune: %d semi tone", component->detune);
    }

    void selectCol2(App_Renderer* renderer, uint8_t row, uint8_t col)
    {
        Zic_Seq_PatternComponent* component = getComponent(row, col);
        sprintf(description, "Condition: %s %s",
            SEQ_CONDITIONS_NAMES[component->condition],
            SEQ_CONDITIONS_FULLNAMES[component->condition]);
    }

    void renderCol0(App_Renderer* renderer, uint8_t row, uint8_t col, bool isSelected)
    {
        App_Audio_Track* track = tracks->tracks[uint8_t(col / 3) % TRACK_COUNT];
        Zic_Seq_PatternComponent* component = getComponent(row, col, isSelected);
        renderer->useColor(row + 1, col / 3 * 7, track->looper.isComponentPlaying(row) ? COLOR_PLAY : COLOR_MARKER);
        strcat(renderer->text,
            track->looper.isComponentPlaying(row) ? ">"
                                                  : (track->looper.isCurrentComponent(row) ? "*" : " "));
        if (component->pattern == NULL) {
            strcat(renderer->text, "--");
        } else {
            sprintf(renderer->text + strlen(renderer->text), "%02X", component->pattern->id + 1);
        }
    }

    void renderCol1(App_Renderer* renderer, uint8_t row, uint8_t col, bool isSelected)
    {
        Zic_Seq_PatternComponent* component = getComponent(row, col, isSelected);
        renderer->useColor(row + 1, col / 3 * 7 + 3, COLOR_LIGHT, 4);
        if (component->detune < 0) {
            sprintf(renderer->text + strlen(renderer->text), "-%c", alphanum[-component->detune]);
        } else {
            sprintf(renderer->text + strlen(renderer->text), "+%c", alphanum[component->detune]);
        }
    }

    void renderCol2(App_Renderer* renderer, uint8_t row, uint8_t col, bool isSelected)
    {
        Zic_Seq_PatternComponent* component = getComponent(row, col, isSelected);
        strcat(renderer->text, SEQ_CONDITIONS_NAMES[component->condition]);
    }

    void updateStart(uint8_t row, uint8_t col)
    {
        newComponent.set(getComponent(row, col));
        editing = true;
    }

    void updateEnd(uint8_t row, uint8_t col)
    {
        editing = false;
        getComponent(row, col)->set(&newComponent);
    }

    uint8_t updateCol0(UiKeys* keys, App_Renderer* renderer, uint8_t row, uint8_t col)
    {
        int8_t direction = getDirection(keys, 16);
        int16_t id = newComponent.pattern == NULL ? -1 : newComponent.pattern->id;
        id = (id + direction) % PATTERN_COUNT;
        newComponent.pattern = NULL;
        return VIEW_CHANGED;
    }
    uint8_t updateCol1(UiKeys* keys, App_Renderer* renderer, uint8_t row, uint8_t col)
    {
        int8_t direction = getDirection(keys, 12);
        newComponent.setDetune(newComponent.detune + direction);
        return VIEW_CHANGED;
    }
    uint8_t updateCol2(UiKeys* keys, App_Renderer* renderer, uint8_t row, uint8_t col)
    {
        int8_t direction = getDirection(keys, 1);
        newComponent.setCondition(newComponent.condition + direction);
        return VIEW_CHANGED;
    }

    uint8_t getDirection(UiKeys* keys, uint8_t bigStep)
    {
        if (keys->Right) {
            return 1;
        } else if (keys->Left) {
            return -1;
        } else if (keys->Up) {
            return bigStep;
        } else if (keys->Down) {
            return -bigStep;
        }
        return 0;
    }
};

class App_View_GridInstrument : public App_View_Grid {
protected:
    App_View_GridInstrumentField field;
    App_Tracks* tracks;

    App_View_GridInstrument(App_Tracks* _tracks)
        : App_View_Grid(&field)
        , field(_tracks, description)
        , tracks(_tracks)
    {
        initSelection();
    }

public:
    static App_View_GridInstrument* instance;

    static App_View_GridInstrument* getInstance(App_Tracks* _tracks)
    {
        if (!instance) {
            instance = new App_View_GridInstrument(_tracks);
        }
        return instance;
    }
};

App_View_GridInstrument* App_View_GridInstrument::instance = NULL;

#endif