#ifndef APP_VIEW_TRACK_SEQUENCER_H_
#define APP_VIEW_TRACK_SEQUENCER_H_

#include "./app_display.h"
#include "./app_tracks.h"
#include "./app_view_table.h"

#define VIEW_TRACK_SEQUENCER_ROW 5
#define VIEW_TRACK_SEQUENCER_COL TRACK_COUNT * 3

class App_View_TrackSequencerHeader : public App_View_TableField {
public:
    void render(App_Display* display, uint8_t row, uint8_t col, uint8_t selectedRow, uint8_t selectedCol)
    {
        // sprintf(display->text + strlen(display->text), "%cTR%d", tracks->trackId == trackId ? '*' : ' ', trackId + 1);
        sprintf(display->text + strlen(display->text), " TRACK%d", col + 1);
    }
};

class App_View_TrackSequencerPat : public App_View_TableField {
protected:
    App_Tracks* tracks;

public:
    App_View_TrackSequencerPat(App_Tracks* _tracks)
        : tracks(_tracks)
    {
    }
    void render(App_Display* display, uint8_t row, uint8_t col, uint8_t selectedRow, uint8_t selectedCol)
    {
        App_Audio_Track* track = tracks->tracks[uint8_t(col / 3) % TRACK_COUNT];
        Zic_Seq_PatternComponent* component = &track->components[(row - 1) % PATTERN_COMPONENT_COUNT];

        if (selectedRow == row && selectedCol == col) {
            display->setCursor(2, col % 3 == 0 ? 1 : 0);
        }
        if (col % 3 == 0) {
            if (component->pattern == NULL) {
                strcat(display->text, " --");
            } else {
                sprintf(display->text + strlen(display->text), "%02X", component->pattern->id + 1);
            }
        } else if (col % 3 == 1) {
            if (component->detune < 0) {
                sprintf(display->text + strlen(display->text), "-%c", '0' - component->detune);
            } else {
                sprintf(display->text + strlen(display->text), "+%c", '0' + component->detune);
            }
        } else {
            strcat(display->text, SEQ_CONDITIONS_NAMES[component->condition]);
        }
    }

    bool isSelectable(uint8_t row, uint8_t col) override
    {
        return true;
    }
};

class App_View_TrackSequencer : public App_View_Table {
protected:
    App_View_TrackSequencerHeader header;
    App_View_TrackSequencerPat patField;

    App_View_TableField* fields[VIEW_TRACK_SEQUENCER_ROW * VIEW_TRACK_SEQUENCER_COL] = {
        // clang-format off
        &header, &header, &header, &header, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
        &patField, &patField, &patField, &patField, &patField, &patField, &patField, &patField, &patField, &patField, &patField, &patField,
        &patField, &patField, &patField, &patField, &patField, &patField, &patField, &patField, &patField, &patField, &patField, &patField,
        &patField, &patField, &patField, &patField, &patField, &patField, &patField, &patField, &patField, &patField, &patField, &patField,
        &patField, &patField, &patField, &patField, &patField, &patField, &patField, &patField, &patField, &patField, &patField, &patField,
        // clang-format on
    };

public:
    App_View_TrackSequencer(App_Tracks* _tracks)
        : App_View_Table(fields, VIEW_TRACK_SEQUENCER_ROW, VIEW_TRACK_SEQUENCER_COL)
        , patField(_tracks)
    {
        initSelection();
    }

    void initDisplay(App_Display* display)
    {
        display->useColoredHeader();
        App_View_Table::initDisplay(display);
    }
};

#endif