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

typedef struct _hoa_2d_rotate
{
    t_hoa_processor  f_obj;
    float               f_f;
    hoa::Rotate<hoa::Hoa2d, t_sample>* f_processor;
    t_sample*               f_ins;
    t_sample*               f_outs;
} t_hoa_2d_rotate;

static t_class *hoa_2d_rotate_class;

static void *hoa_2d_rotate_new(t_float f)
{
    t_hoa_2d_rotate *x = (t_hoa_2d_rotate *)pd_new(hoa_2d_rotate_class);
    if(x)
    {
        const size_t order = hoa_processor_clip_order(x, (size_t)f);
        x->f_processor = new hoa::Rotate<hoa::Hoa2d, t_sample>(order);
        x->f_ins   = new t_sample[x->f_processor->getNumberOfHarmonics() * 81092];
        x->f_outs   = new t_sample[x->f_processor->getNumberOfHarmonics() * 81092];
        hoa_processor_init(x, x->f_processor->getNumberOfHarmonics() + 1, x->f_processor->getNumberOfHarmonics());
    }
    return x;
}

static void hoa_2d_rotate_free(t_hoa_2d_rotate *x)
{
    hoa_processor_clear(x);
    delete x->f_processor;
    delete [] x->f_ins;
    delete [] x->f_outs;
}

static void hoa_2d_rotate_perform(t_hoa_2d_rotate *x, size_t sampleframes,
                                 size_t nins, t_sample **ins,
                                 size_t nouts, t_sample **outs)
{
    for(size_t i = 0; i < nins - 1; i++)
    {
        hoa::Signal<t_sample>::copy(sampleframes, ins[i], 1, x->f_ins+i, (nins - 1));
    }
    for(size_t i = 0; i < sampleframes; i++)
    {
        x->f_processor->setYaw(ins[nins - 1][i]);
        x->f_processor->process(x->f_ins + (nins - 1) * i, x->f_outs + nouts * i);
    }
    for(size_t i = 0; i < nouts; i++)
    {
        hoa::Signal<t_sample>::copy(sampleframes, x->f_outs+i, nouts, outs[i], 1);
    }
}

static void hoa_2d_rotate_dsp(t_hoa_2d_rotate *x, t_signal **sp)
{
    hoa_processor_prepare(x, (t_hoa_processor_perfm)hoa_2d_rotate_perform, sp);
}

extern "C" void setup_hoa0x2e2d0x2erotate_tilde(void)
{
    t_class *c = class_new(gensym("hoa.2d.rotate~"), (t_newmethod)hoa_2d_rotate_new, (t_method)hoa_2d_rotate_free,
                           (size_t)sizeof(t_hoa_2d_rotate), CLASS_DEFAULT, A_FLOAT, 0);
    if(c)
    {
        CLASS_MAINSIGNALIN(c, t_hoa_2d_rotate, f_f);
        class_addmethod(c, (t_method)hoa_2d_rotate_dsp, gensym("dsp"), A_CANT, 0);
        class_addcreator((t_newmethod)hoa_2d_rotate_new, gensym("hoa.rotate~"), A_FLOAT, 0);
    }
    hoa_2d_rotate_class = c;
}
