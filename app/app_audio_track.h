#ifndef APP_AUDIO_TRACK_H_
#define APP_AUDIO_TRACK_H_

#include "./app_def.h"
#include "./app_instrument.h"

#include <PdBase.hpp>
#include <wavetables/wavetable_Bank.h>
#include <zic_effect_delay.h>
#include <zic_seq_loopMaster.h>

#define PATTERN_COMPONENT_COUNT 9

class App_Audio_Track {
protected:
    Zic_Seq_Step* stepOn[INSTRUMENT_COUNT];
    const float tickDivider = 1.0f / (256.0f * APP_CHANNELS);

public:
    uint8_t id = 0;

    Zic_Seq_PatternComponent components[PATTERN_COMPONENT_COUNT];

    Zic_Seq_LoopMaster looper;
    pd::PdBase pd;
    pd::Patch patch;

    App_Audio_Track(uint8_t _id = 0)
        : looper(&components[0], PATTERN_COMPONENT_COUNT)
    {
        id = _id;
        if (!pd.init(0, APP_CHANNELS, SAMPLE_RATE)) {
            SDL_Log("Could not init pd\n");
        }
        pd.computeAudio(true);
        patch = pd.openPatch("hello.pd", "puredata/synth01");
    }

    void next()
    {
        // TODO think? wouldn't it make sense to just pass the step to the synth and out of the step set params
        // then just trigger noteOn and noteOff

        looper.next();
        // FIXME there is no clear note OFF
        // maybe trigger note off earlier???
        // should note OFF run in the same time as the next step but then we need to handle even more polyphony...
        // or should not off trigger before???
        // actually if there is a not a slide, then the note should stop a little bit earlier?!
        // but if there is a slide, it might or not have a note off?
        // on a piano there would be a note off but on a clarinet there would not be

        for (uint8_t i = 0; i < INSTRUMENT_COUNT; i++) {
            Zic_Seq_Step* stepOff = stepOn[i];
            if (stepOff && !stepOff->slide) {
                // synths[i]->asr.off();
                // printf("note off %d\n", i);
                pd.sendNoteOn(1, stepOff->note, 0);
            }
            if (looper.state.playing && looper.stepOn != 255) {
                stepOn[i] = &looper.state.pattern->steps[i][looper.stepOn];
                if (stepOn[i]->note > 0) {
                    pd.sendNoteOn(1, stepOn[i]->note, stepOn[i]->velocity);
                    // printf("note on %d (%d)\n", stepOn[i]->note, stepOn[i]->velocity);
                    // synths[i]->setStep(stepOn[i]);
                    // if (stepOff && stepOff->slide) {
                    //     synths[i]->asr.slide();
                    // } else {
                    //     synths[i]->asr.on();
                    //     // printf("note on %d\n", i);
                    // }
                }
            }
        }
    }

    void sample(float* buf, int len)
    {
        int ticks = len * tickDivider;
        pd.processFloat(ticks, NULL, buf);
    }
};

#endif