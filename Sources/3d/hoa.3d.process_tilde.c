/*
// Copyright (c) 2012-2016 Pierre Guillot, Eliott Paris & Thomas Le Meur CICM, Universite Paris 8.
// For information on usage and redistribution, and for a DISCLAIMER OF ALL
// WARRANTIES, see the file, "LICENSE.txt," in this distribution.
*/

#include "../hoa.pd.h"
#include <g_canvas.h>
#include <m_imp.h>
#include <string.h>

static t_class* hoa_process_inlet_class;

struct _hoa_process_inlet;

typedef struct _hoa_process
{
    t_hoa_processor             f_obj;
    t_float                     f_f;
    t_symbol*                   f_domain;
    t_canvas*                   f_canvas;
    t_object*                   f_block;
    t_bangmethod                f_method;
    t_hoa_process_instance*     f_instances;
    size_t                      f_ninstances;
    size_t                      f_target;
    
    size_t                      f_nins;
    struct _hoa_process_inlet*  f_ins;
    size_t                      f_nsigout;
    t_sample*                   f_sigout;
    size_t                      f_nsigin;
    t_sample*                   f_sigin;
} t_hoa_3d_process_tilde;

typedef struct _hoa_process_inlet
{
    t_class*                x_pd;
    size_t                  x_index;
    t_hoa_3d_process_tilde* x_owner;
} t_hoa_process_inlet;


static t_class*     hoa_3d_process_tilde_class;
static t_symbol*    hoa_sym_switch;
static t_symbol*    hoa_sym_block;
static t_symbol*    hoa_sym_obj;
static t_symbol*    hoa_sym_harmonics;
static t_symbol*    hoa_sym_planewaves;
static t_symbol*    hoa_sym_3d;

static void hoa_3d_process_tilde_perform(t_hoa_3d_process_tilde *x, size_t sampleframes,
                                size_t nins, t_sample **ins,
                                size_t nouts, t_sample **outs)
{
    size_t i;
    memset(x->f_sigout, 0, nouts * (size_t)sampleframes * sizeof(t_sample));
    for(i = 0; i < nins; i++)
    {
        memcpy(x->f_sigin+i*sampleframes, ins[i], (size_t)sampleframes * sizeof(t_sample));
    }
    x->f_method((t_pd *)x->f_block);
    for(i = 0; i < nouts; i++)
    {
        memcpy(outs[i], x->f_sigout+i*sampleframes, (size_t)sampleframes * sizeof(t_sample));
    }
}


static void hoa_3d_process_tilde_dsp(t_hoa_3d_process_tilde *x, t_signal **sp)
{
    size_t i, j, offset, nsamples;
    char hasin = 0, hasout = 0;
    size_t nins = 0, nouts = 0;
    char thasin, thasout;
    size_t tnins, tnouts;
    if(x->f_block && x->f_method)
    {
        for(i = 0; i < x->f_ninstances; ++i)
        {
            thasin = hoa_process_instance_has_inputs_sig_static(x->f_instances+i);
            thasout = hoa_process_instance_has_outputs_sig_static(x->f_instances+i);
            tnins = hoa_process_instance_get_ninputs_sig_extra(x->f_instances+i);
            tnouts = hoa_process_instance_get_noutputs_sig_extra(x->f_instances+i);
            
            hasin = thasin != 0 ? 1 : hasin;
            hasout = thasout != 0 ? 1 : hasout;
            nins = tnins > nins ? tnins : nins;
            nouts = tnouts > nouts ? tnouts : nouts;
        }
        
        if(x->f_sigin && x->f_nsigin)
        {
            freebytes(x->f_sigin, x->f_nsigin * sizeof(t_sample));
            x->f_sigin     = NULL;
            x->f_nsigin    = 0;
        }
        if(nins || hasin)
        {
            nsamples = (x->f_ninstances * (size_t)hasin + nins) * (size_t)sp[0]->s_n;
            x->f_sigin = (t_sample *)getbytes(nsamples * sizeof(t_sample));
            if(x->f_sigin)
            {
                x->f_nsigin = nsamples;
                if(hasin)
                {
                    for(i = 0; i < x->f_ninstances; ++i)
                    {
                        nsamples = i * (size_t)sp[0]->s_n;
                        hoa_process_instance_set_inlet_sig(x->f_instances+i, 0, x->f_sigin + nsamples);
                    }
                }
                if(nins)
                {
                    offset = (size_t)hasin * x->f_ninstances;
                    for(j = 0; j < nins; ++j)
                    {
                        for(i = 0; i < x->f_ninstances; ++i)
                        {
                            nsamples = (offset + j) * (size_t)sp[0]->s_n;
                            hoa_process_instance_set_inlet_sig(x->f_instances+i, j+1, x->f_sigin + nsamples);
                        }
                    }
                }
            }
            else
            {
                pd_error(x, "hoa.3d.process~ can't allocate memory.");
                return;
            }
        }
        
        
        if(x->f_sigout && x->f_nsigout)
        {
            freebytes(x->f_sigout, x->f_nsigout * sizeof(t_sample));
            x->f_sigout     = NULL;
            x->f_nsigout    = 0;
        }
        
        if(nouts || hasout)
        {
            nsamples = (x->f_ninstances * (size_t)hasout + nouts) * (size_t)sp[0]->s_n;
            x->f_sigout = (t_sample *)getbytes(nsamples * sizeof(t_sample));
            if(x->f_sigout)
            {
                x->f_nsigout = nsamples;
                if(hasout)
                {
                    for(i = 0; i < x->f_ninstances; ++i)
                    {
                        nsamples = i * (size_t)sp[0]->s_n;
                        hoa_process_instance_set_outlet_sig(x->f_instances+i, 0, x->f_sigout + nsamples);
                    }
                }
                if(nouts)
                {
                    offset = (size_t)hasout * x->f_ninstances;
                    for(j = 0; j < nouts; ++j)
                    {
                        for(i = 0; i < x->f_ninstances; ++i)
                        {
                            nsamples = (offset + j) * (size_t)sp[0]->s_n;
                            hoa_process_instance_set_outlet_sig(x->f_instances+i, j+1, x->f_sigout + nsamples);
                        }
                    }
                }
            }
            else
            {
                pd_error(x, "hoa.3d.process~ can't allocate memory.");
                return;
            }
        }
        
        mess0((t_pd *)x->f_canvas, gensym("dsp"));
        hoa_processor_prepare(x, (t_hoa_processor_perfm)hoa_3d_process_tilde_perform, sp);
    }
    else
    {
        pd_error(x, "hoa.3d.process~ not initialized can't compile DSP chain.");
    }
}

static void hoa_3d_process_tilde_click(t_hoa_3d_process_tilde *x)
{
    if(x->f_instances)
    {
        hoa_process_instance_show(x->f_instances);
    }
}

static void hoa_3d_process_tilde_open(t_hoa_3d_process_tilde *x, t_float f1, t_float f2)
{
    size_t i, index;
    if(f1 < 0)
    {
        for(i = 0; i < x->f_ninstances; ++i)
        {
            hoa_process_instance_show(x->f_instances+i);
        }
    }
    else
    {
        if(x->f_domain == hoa_sym_harmonics)
        {
            index = hoa_3d_get_index((size_t)f1, f2);
        }
        else
        {
            index = (size_t)f1;
        }
        if(index < x->f_ninstances)
        {
            hoa_process_instance_show(x->f_instances+index);
        }
        else
        {
            pd_error(x, "hoa.3d.process~: open argument out of bounds.");
        }
    }
}

static void hoa_3d_process_tilde_target(t_hoa_3d_process_tilde *x, t_float f1, t_float f2)
{
    if(f1 < 0)
    {
        x->f_target = (size_t)-1;
    }
    else
    {
        if(x->f_domain == hoa_sym_harmonics)
        {
            x->f_target = hoa_3d_get_index((size_t)f1, f2);
        }
        else
        {
            x->f_target = (size_t)f1;
        }
        if(x->f_target >= x->f_ninstances)
        {
            x->f_target = -1;
            pd_error(x, "hoa.3d.process~: open argument out of bounds.");
        }
    }
}

static char hoa_3d_process_tilde_init(t_hoa_3d_process_tilde* x)
{
    t_atom av[3];
    x->f_block      = (t_object *)NULL;
    x->f_method     = (t_bangmethod)NULL;
    x->f_instances  = (t_hoa_process_instance *)NULL;
    x->f_canvas     = canvas_new(NULL, gensym(""), 0, NULL);
    x->f_ins        = NULL;
    x->f_nins       = 0;
    x->f_target     = (size_t)-1;
    x->f_sigout     = NULL;
    x->f_nsigout    = 0;
    if(x->f_canvas)
    {
        pd_popsym((t_pd *)x->f_canvas);
        canvas_vis(x->f_canvas, 0);
        SETFLOAT(av, 10.f);
        SETFLOAT(av+1, 10.f);
        SETSYMBOL(av+2, hoa_sym_switch);
        
        pd_typedmess((t_pd *)x->f_canvas, hoa_sym_obj, 3, av);
        if(x->f_canvas->gl_list->g_pd->c_name == hoa_sym_block)
        {
            x->f_block = (t_object *)x->f_canvas->gl_list;
            x->f_method = x->f_block->te_g.g_pd->c_bangmethod;
            return 1;
        }
    }
    return 0;
}


static char hoa_3d_process_tilde_load_planewaves(t_hoa_3d_process_tilde* x, size_t nplws, t_symbol* name, int argc, t_atom *argv)
{
    int ac;
    t_atom* av;
    size_t i;
    size_t ninstance  = nplws;
    x->f_instances    = (t_hoa_process_instance *)getbytes(nplws * sizeof(t_hoa_process_instance));
    if(x->f_instances)
    {
        ac = 4 + argc;
        av = (t_atom *)getbytes(ac * sizeof(t_atom));
        if(ac && av)
        {
            SETSYMBOL(av, hoa_sym_3d);
            SETSYMBOL(av+1, hoa_sym_planewaves);
            SETFLOAT(av+2, (float)nplws);
            memcpy(av+4, argv, argc * sizeof(t_atom));
            for(i = 0; i < ninstance; ++i)
            {
                SETFLOAT(av+3, (float)i);
                if(!hoa_process_instance_init(x->f_instances+i, x->f_canvas, name, ac, av))
                {
                    freebytes(av, ac * sizeof(t_atom));
                    freebytes(x->f_instances, nplws * sizeof(t_hoa_process_instance));
                    x->f_instances = NULL;
                    return 0;
                }
            }
            x->f_ninstances = ninstance;
            freebytes(av, ac * sizeof(t_atom));
            return 1;
        }
        freebytes(x->f_instances, ninstance * sizeof(t_hoa_process_instance));
        x->f_instances = NULL;
        return 0;
    }
    return 0;
}

static char hoa_3d_process_tilde_load_harmonics(t_hoa_3d_process_tilde* x, size_t order, t_symbol* name, int argc, t_atom *argv)
{
    int ac;
    t_atom* av;
    size_t i;
    size_t ninstance  = hoa_3d_get_number_of_harmonics(order);
    x->f_instances    = (t_hoa_process_instance *)getbytes(ninstance * sizeof(t_hoa_process_instance));
    if(x->f_instances)
    {
        ac = 5 + argc;
        av = (t_atom *)getbytes(ac * sizeof(t_atom));
        if(ac && av)
        {
            SETSYMBOL(av, hoa_sym_3d);
            SETSYMBOL(av+1, hoa_sym_harmonics);
            SETFLOAT(av+2, (float)order);
            memcpy(av+5, argv, argc * sizeof(t_atom));
            for(i = 0; i < ninstance; ++i)
            {
                SETFLOAT(av+3, (float)hoa_3d_get_degree(i));
                SETFLOAT(av+4, (float)hoa_3d_get_azimuthal_order(i));
                if(!hoa_process_instance_init(x->f_instances+i, x->f_canvas, name, ac, av))
                {
                    freebytes(av, ac * sizeof(t_atom));
                    freebytes(x->f_instances, ninstance * sizeof(t_hoa_process_instance));
                    x->f_instances = NULL;
                    return 0;
                }
            }
            x->f_ninstances = ninstance;
            freebytes(av, ac * sizeof(t_atom));
            return 1;
        }
        freebytes(x->f_instances, ninstance * sizeof(t_hoa_process_instance));
        x->f_instances = NULL;
        return 0;
    }
    return 0;
}

static void hoa_3d_process_tilde_alloc_inlets(t_hoa_3d_process_tilde* x)
{
    size_t i = 0;
    size_t ninlets = 0, newval = 0;
    
    for(i = 0; i < x->f_ninstances; ++i)
    {
        newval = hoa_process_instance_get_ninputs(x->f_instances+i);
        if(newval > ninlets)
        {
            ninlets = newval;
        }
    }
    if(ninlets)
    {
        x->f_ins = (t_hoa_process_inlet *)getbytes(ninlets * sizeof(t_hoa_process_inlet));
        if(x->f_ins)
        {
            x->f_nins = ninlets;
            for(i = 0; i < x->f_nins; ++i)
            {
                x->f_ins[i].x_index = i+1;
                x->f_ins[i].x_pd = hoa_process_inlet_class;
                x->f_ins[i].x_owner = x;
                inlet_new((t_object *)x, &(x->f_ins[i].x_pd), 0, 0);
            }
        }
    }
}

static void hoa_3d_process_tilde_alloc_outlets(t_hoa_3d_process_tilde* x)
{
    size_t i = 0, j = 0;
    size_t noutlets = 0, newval = 0;
    t_outlet* outlet;
    for(i = 0; i < x->f_ninstances; ++i)
    {
        newval = hoa_process_instance_get_noutputs(x->f_instances+i);
        if(newval > noutlets)
        {
            noutlets = newval;
        }
    }
    for(i = 0; i < noutlets; ++i)
    {
        outlet = outlet_new((t_object *)x, NULL);
        for(j = 0; j < x->f_ninstances; ++j)
        {
            hoa_process_instance_set_outlet(x->f_instances+j, i+1, outlet);
        }
    }
}

static void hoa_3d_process_tilde_alloc_signals(t_hoa_3d_process_tilde* x)
{
    size_t i;
    char hasin = 0, hasout = 0;
    size_t nins = 0, nouts = 0;
    char thasin, thasout;
    size_t tnins, tnouts;
    for(i = 0; i < x->f_ninstances; ++i)
    {
        thasin = hoa_process_instance_has_inputs_sig_static(x->f_instances+i);
        thasout = hoa_process_instance_has_outputs_sig_static(x->f_instances+i);
        tnins = hoa_process_instance_get_ninputs_sig_extra(x->f_instances+i);
        tnouts = hoa_process_instance_get_noutputs_sig_extra(x->f_instances+i);
        
        hasin = thasin != 0 ? 1 : hasin;
        hasout = thasout != 0 ? 1 : hasout;
        nins = tnins > nins ? tnins : nins;
        nouts = tnouts > nouts ? tnouts : nouts;
    }
    
    hoa_processor_init(x, x->f_ninstances * (size_t)hasin + nins, x->f_ninstances * (size_t)hasout + nouts);
}

static void hoa_3d_process_tilde_free(t_hoa_3d_process_tilde *x)
{
    if(x->f_canvas)
    {
        canvas_free(x->f_canvas);
    }
    if(x->f_instances && x->f_ninstances)
    {
        freebytes(x->f_instances, x->f_ninstances * sizeof(t_hoa_process_instance));
        x->f_instances = NULL;
    }
    if(x->f_ins && x->f_nins)
    {
        freebytes(x->f_ins, x->f_nins * sizeof(t_hoa_process_inlet));
        x->f_ins = NULL;
    }
    if(x->f_sigout && x->f_nsigout)
    {
        freebytes(x->f_sigout, x->f_nsigout * sizeof(t_sample));
        x->f_sigout = NULL;
    }
    hoa_processor_clear(x);
}

static void *hoa_3d_process_tilde_new(t_symbol *s, int argc, t_atom *argv)
{
    size_t order, nplws;
    t_symbol *patch, *domain;
    
    t_canvas* current = canvas_getcurrent();
    t_hoa_3d_process_tilde *x = (t_hoa_3d_process_tilde *)pd_new(hoa_3d_process_tilde_class);
    if(x)
    {
        patch   = atom_getsymbolarg(1, argc, argv);
        domain  = atom_getsymbolarg(2, argc, argv);
        if(hoa_3d_process_tilde_init(x))
        {
            if(patch)
            {
                if(domain == hoa_sym_harmonics)
                {
                    order = hoa_processor_clip_order(x, (size_t)atom_getfloatarg(0, argc, argv));
                    if(!hoa_3d_process_tilde_load_harmonics(x, order, patch, argc < 3 ? 0 : argc-3, argv+3))
                    {
                        pd_error(x, "hoa.3d.process~: can't load the patch %s.pd.", patch->s_name);
                    }
                    x->f_domain = hoa_sym_harmonics;
                }
                else
                {
                    nplws = hoa_processor_clip_nplanewaves(x, (size_t)atom_getfloatarg(0, argc, argv));
                    if(!hoa_3d_process_tilde_load_planewaves(x, nplws, patch, argc < 3 ? 0 : argc-3, argv+3))
                    {
                        pd_error(x, "hoa.3d.process~: can't load the patch %s.pd.", patch->s_name);
                    }
                    if(domain != hoa_sym_planewaves)
                    {
                        pd_error(x, "hoa.3d.process~: bad argument, third argument must harmonics or planewaves.");
                    }
                    x->f_domain = hoa_sym_planewaves;
                }
                hoa_3d_process_tilde_alloc_signals(x);
                hoa_3d_process_tilde_alloc_inlets(x);
                hoa_3d_process_tilde_alloc_outlets(x);
            }
            else
            {
                pd_error(x, "hoa.3d.process~: bad argument, second argument must be a patch name.");
            }
        }
    }
    canvas_setcurrent(current);
    return x;
}








static void hoa_process_inlet_bang(t_hoa_process_inlet *x)
{
    size_t i;
    const size_t target = x->x_owner->f_target;
    if(target != (size_t)-1)
    {
        hoa_process_instance_send_bang(x->x_owner->f_instances+target, x->x_index);
    }
    else
    {
        for(i = 0; i < x->x_owner->f_ninstances; ++i)
        {
            hoa_process_instance_send_bang(x->x_owner->f_instances+i, x->x_index);
        }
    }
}

static void hoa_process_inlet_float(t_hoa_process_inlet *x, float f)
{
    size_t i;
    const size_t target = x->x_owner->f_target;
    if(target != (size_t)-1)
    {
        hoa_process_instance_send_float(x->x_owner->f_instances+target, x->x_index, f);
    }
    else
    {
        for(i = 0; i < x->x_owner->f_ninstances; ++i)
        {
            hoa_process_instance_send_float(x->x_owner->f_instances+i, x->x_index, f);
        }
    }
}

static void hoa_process_inlet_symbol(t_hoa_process_inlet *x, t_symbol* s)
{
    size_t i;
    const size_t target = x->x_owner->f_target;
    if(target != (size_t)-1)
    {
        hoa_process_instance_send_symbol(x->x_owner->f_instances+target, x->x_index, s);
    }
    else
    {
        for(i = 0; i < x->x_owner->f_ninstances; ++i)
        {
            hoa_process_instance_send_symbol(x->x_owner->f_instances+i, x->x_index, s);
        }
    }
}

static void hoa_process_inlet_list(t_hoa_process_inlet *x, t_symbol* s, int argc, t_atom* argv)
{
    size_t i;
    const size_t target = x->x_owner->f_target;
    if(target != (size_t)-1)
    {
        hoa_process_instance_send_list(x->x_owner->f_instances+target, x->x_index, s, argc, argv);
    }
    else
    {
        for(i = 0; i < x->x_owner->f_ninstances; ++i)
        {
            hoa_process_instance_send_list(x->x_owner->f_instances+i, x->x_index, s, argc, argv);
        }
    }
}

static void hoa_process_inlet_anything(t_hoa_process_inlet *x, t_symbol* s, int argc, t_atom* argv)
{
    size_t i;
    const size_t target = x->x_owner->f_target;
    if(target != (size_t)-1)
    {
        hoa_process_instance_send_anything(x->x_owner->f_instances+target, x->x_index, s, argc, argv);
    }
    else
    {
        for(i = 0; i < x->x_owner->f_ninstances; ++i)
        {
            hoa_process_instance_send_anything(x->x_owner->f_instances+i, x->x_index, s, argc, argv);
        }
    }
}

extern void setup_hoa0x2e3d0x2eprocess_tilde(void)
{
    t_class* c = class_new(gensym("hoa.3d.process~"), (t_newmethod)hoa_3d_process_tilde_new, (t_method)hoa_3d_process_tilde_free,
                           (size_t)sizeof(t_hoa_3d_process_tilde), CLASS_DEFAULT, A_GIMME, 0);
    
    if(c)
    {
        CLASS_MAINSIGNALIN(c, t_hoa_3d_process_tilde, f_f);
        class_addmethod(c, (t_method)hoa_3d_process_tilde_click,    gensym("click"),    A_NULL, 0);
        class_addmethod(c, (t_method)hoa_3d_process_tilde_open,     gensym("open"),     A_FLOAT, A_DEFFLOAT, 0);
        class_addmethod(c, (t_method)hoa_3d_process_tilde_target,   gensym("target"),   A_FLOAT, A_DEFFLOAT, 0);
        class_addmethod(c, (t_method)hoa_3d_process_tilde_dsp,      gensym("dsp"),      A_CANT, 0);
    }
    hoa_3d_process_tilde_class = c;
    
    hoa_sym_switch      = gensym("switch~");
    hoa_sym_block       = gensym("block~");
    hoa_sym_obj         = gensym("obj");
    hoa_sym_harmonics   = gensym("harmonics");
    hoa_sym_planewaves  = gensym("planewaves");
    hoa_sym_3d          = gensym("3d");

    hoa_process_instance_setup();
    
    c = class_new(gensym("hoa.3d.process.inlet"), 0, 0, sizeof(t_hoa_process_inlet), CLASS_PD, 0);
    if(c)
    {
        class_addbang(c,    (t_method)hoa_process_inlet_bang);
        class_addfloat(c,   (t_method)hoa_process_inlet_float);
        class_addsymbol(c,  (t_method)hoa_process_inlet_symbol);
        class_addlist(c,    (t_method)hoa_process_inlet_list);
        class_addanything(c,(t_method)hoa_process_inlet_anything);
    }
    hoa_process_inlet_class = c;
}

