#ifndef APP_AUDIO_TRACK_H_
#define APP_AUDIO_TRACK_H_

#include "./app_def.h"
#include "./app_instrument.h"

#include <PdBase.hpp>
#include <PdObject.h>
#include <app_core_file.h>
#include <wavetables/wavetable_Bank.h>
#include <zic_effect_delay.h>
#include <zic_seq_loopMaster.h>

#define APP_TRACK_STATE_SIZE 8

class App_Audio_TrackState {
protected:
    void setEmptyPatch()
    {
        strcpy(patchFilename, "--\0");
    }

    bool isPatchEmpty()
    {
        return patchFilename[0] == '-' && patchFilename[1] == '-';
    }

public:
    char patchFilename[40];

    App_Audio_TrackState()
    {
        strcpy(patchFilename, "--\0");
    }

    void setNextPatch(int8_t direction)
    {
        if (isPatchEmpty()) {
            if (direction == 1 && !firstFile(patchFilename, "instruments")) {
                setEmptyPatch();
            }
        } else if (!nextFile(patchFilename, "instruments", patchFilename, direction) && direction == -1) {
            setEmptyPatch();
        }
    }
};

class App_Audio_Track {
protected:
    Zic_Seq_Step* stepOff[INSTRUMENT_COUNT];
    const float tickDivider = 1.0f / (256.0f * APP_CHANNELS);
    PdObject pdObject;

public:
    uint8_t id = 0;

    Zic_Seq_PatternComponent components[APP_TRACK_STATE_SIZE];
    App_Audio_TrackState state[APP_TRACK_STATE_SIZE];

    Zic_Seq_LoopMaster looper;
    pd::PdBase pd;
    pd::Patch patch;

    App_Audio_Track(uint8_t _id = 0)
        : pdObject(_id)
        , looper(&components[0], APP_TRACK_STATE_SIZE)
    {
        id = _id;
        loadState();
        if (!pd.init(0, APP_CHANNELS, SAMPLE_RATE)) {
            APP_LOG("Could not init pd\n");
        }
        pd.computeAudio(true);
        patch = pd.openPatch("main.pd", "instruments/02_synth");
        pd.setReceiver(&pdObject);
        pd.setMidiReceiver(&pdObject);
        // pd.sendControlChange(1, 1, 10);
    }

    ~App_Audio_Track()
    {
        pd.closePatch(patch);
        pd.clear();
    }

    void next()
    {
        looper.next();

        // TODO need to handle slide
        for (uint8_t i = 0; i < INSTRUMENT_COUNT; i++) {
            if (stepOff[i] && !stepOff[i]->slide) {
                // printf("note off %d\n", stepOff[i]->note);
                pd.sendNoteOn(1, stepOff[i]->note, 0);
                stepOff[i] = NULL;
            }
            if (looper.state.playing && looper.stepOn != 255) {
                Zic_Seq_Step* step = &looper.state.pattern->steps[i][looper.stepOn];
                if (step->note > 0) {
                    // printf("note on %d (%d)\n", step->note, step->velocity);
                    pd.sendNoteOn(1, step->note, step->velocity);
                    stepOff[i] = step;
                }
            }
        }
    }

    void sample(float* buf, int len)
    {
        int ticks = len * tickDivider;
        pd.processFloat(ticks, NULL, buf);
    }

    void cc(uint8_t num, uint8_t val)
    {
        pd.sendControlChange(1, num, val);
    }

    void loadState()
    {
        // for (uint8_t i = 0; i < APP_TRACK_STATE_SIZE; i++) {
        //     strcpy(state[i].patchFilename, "--\0");
        // }
    }
};

#endif