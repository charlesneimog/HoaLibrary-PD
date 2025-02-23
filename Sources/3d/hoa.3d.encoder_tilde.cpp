/*
// Copyright (c) 2012-2016 Pierre Guillot, Eliott Paris & Thomas Le Meur CICM, Universite Paris 8.
// For information on usage and redistribution, and for a DISCLAIMER OF ALL
// WARRANTIES, see the file, "LICENSE.txt," in this distribution.
*/

extern "C"
{
#include "../hoa.pd.h"
}
#include <Hoa.hpp>

typedef struct _hoa_3d_encoder
{
    t_hoa_processor  f_obj;
    float               f_f;
    hoa::Encoder<hoa::Hoa3d, t_sample>* f_processor;
    t_sample* f_signals;
} t_hoa_3d_encoder;

static t_class *hoa_3d_encoder_class;

static void *hoa_3d_encoder_new(t_float f)
{
    t_hoa_3d_encoder *x = (t_hoa_3d_encoder *)pd_new(hoa_3d_encoder_class);
    if(x)
    {
        const size_t order = hoa_processor_clip_order(x, (size_t)f);
        x->f_processor = new hoa::Encoder<hoa::Hoa3d, t_sample>(order);
        x->f_signals   = new t_sample[x->f_processor->getNumberOfHarmonics() * 81092];
        hoa_processor_init(x, 3, x->f_processor->getNumberOfHarmonics());
    }
    return x;
}

static void hoa_3d_encoder_free(t_hoa_3d_encoder *x)
{
    hoa_processor_clear(x);
    delete x->f_processor;
    delete [] x->f_signals;
}

static inline t_sample hoa_3d_encoder_wrap_pi(t_sample value)
{
    while(value < (t_sample)(-HOA_PI))
    {
        value += (t_sample)(HOA_2PI);
    }
    while(value >= HOA_PI)
    {
        value -= (t_sample)(HOA_2PI);
    }
    return value;
}

static void hoa_3d_encoder_perform(t_hoa_3d_encoder *x, size_t sampleframes,
                                   size_t nins, t_sample **ins,
                                   size_t nouts, t_sample **outs)
{
    for(long i = 0; i < sampleframes; i++)
    {
        const t_sample azimuth   = ins[1][i];
        const t_sample elevation = hoa_3d_encoder_wrap_pi(ins[2][i]);
        if(elevation >= -HOA_PI2 && elevation <= HOA_PI2)
        {
            x->f_processor->setAzimuth(azimuth);
            x->f_processor->setElevation(elevation);
        }
        else
        {
            x->f_processor->setAzimuth(azimuth + HOA_PI);
            x->f_processor->setElevation(elevation);
        }
        x->f_processor->process(ins[0]+i, x->f_signals + nouts * i);
    }
    for(long i = 0; i < nouts; i++)
    {
        hoa::Signal<t_sample>::copy(sampleframes, x->f_signals+i, nouts, outs[i], 1);
    }
}

static void hoa_3d_encoder_dsp(t_hoa_3d_encoder *x, t_signal **sp)
{
    hoa_processor_prepare(x, (t_hoa_processor_perfm)hoa_3d_encoder_perform, sp);
}

extern "C" void setup_hoa0x2e3d0x2eencoder_tilde(void)
{
    t_class *c = class_new(gensym("hoa.3d.encoder~"), (t_newmethod)hoa_3d_encoder_new, (t_method)hoa_3d_encoder_free,
                           (size_t)sizeof(t_hoa_3d_encoder), CLASS_DEFAULT, A_FLOAT, 0);
    if(c)
    {
        CLASS_MAINSIGNALIN(c, t_hoa_3d_encoder, f_f);
        class_addmethod(c, (t_method)hoa_3d_encoder_dsp, gensym("dsp"), A_CANT, 0);
    }
    hoa_3d_encoder_class = c;
}
