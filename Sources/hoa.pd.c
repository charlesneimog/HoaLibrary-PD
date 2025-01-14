/*
// Copyright (c) 2012-2016 Pierre Guillot, Eliott Paris & Thomas Le Meur CICM, Universite Paris 8.
// For information on usage and redistribution, and for a DISCLAIMER OF ALL
// WARRANTIES, see the file, "LICENSE.txt," in this distribution.
*/

#include "hoa.pd.h"
#include <stdlib.h>
#include <math.h>

static char hoaname[] = "hoa.library v2.3-beta";
static char hoaversion[] = "v2.3-beta";

size_t hoa_2d_get_number_of_harmonics(size_t order)
{
    return order * 2 + 1;
}

size_t hoa_2d_get_index(size_t degree, long order)
{
    return labs(order) *  2 - (long)(order < 0);
}

long hoa_2d_get_azimuthal_order(size_t index)
{
    return (long)((long)(index + index % 2l) / 2l) * (1l - (long)(index % 2) * 2l);
}

size_t hoa_2d_get_degree(size_t index)
{
    return (index + index % 2) / 2ul;
}

size_t hoa_3d_get_number_of_harmonics(size_t order)
{
    return (order + 1) * (order + 1);
}

size_t hoa_3d_get_index(size_t degree, long order)
{
    return (size_t)((long)(degree * (degree + 1)) + order);
}

long hoa_3d_get_azimuthal_order(size_t index)
{
    return (long)(index) - ((long)(sqrt((double)(index))) * ((long)(sqrt((double)(index))) + 1));
}

size_t hoa_3d_get_degree(size_t index)
{
    return (size_t)(sqrt((double)(index)));
}

size_t hoa_processor_clip_order(void* obj, size_t order)
{
    if(order < 1)
    {
        pd_error(obj, "%s: bad order of decomposition.", class_getname(*(t_pd *)obj));
        pd_error(obj, "%s: receive %i but expect at least 1.", class_getname(*(t_pd *)obj), (int)order);
        return 1;
    }
    return order;
}

size_t hoa_processor_clip_nplanewaves(void* obj, size_t nplanewaves)
{
    if(nplanewaves < 1)
    {
        pd_error(obj, "%s: bad number of planewaves.", class_getname(*(t_pd *)obj));
        pd_error(obj, "%s: receive %i but expect at least 1.", class_getname(*(t_pd *)obj), (int)nplanewaves);
        return 1;
    }
    return nplanewaves;
}

void hoa_processor_init(void* obj, size_t nins, size_t nouts)
{
    size_t i;
    t_hoa_processor* x = (t_hoa_processor*)obj;
    x->f_inputs = NULL;
    x->f_nins   = 0;
    x->f_outputs= NULL;
    x->f_nouts  = 0;
    if(nins)
    {
        x->f_nins      = nins;
        x->f_inputs    = (t_sample **)getbytes(x->f_nins * sizeof(t_sample*));
        for(i = 0; i < nins - 1; ++i)
        {
            signalinlet_new((t_object *)obj, 0);
        }
    }
    if(nouts)
    {
        x->f_nouts     = nouts;
        x->f_outputs   = (t_sample **)getbytes(x->f_nouts * sizeof(t_sample*));
        for(i = 0; i < nouts; ++i)
        {
            outlet_new((t_object *)x, &s_signal);
        }
    }
    
    
    if(gensym(hoaname)->s_thing == NULL)
    {
        verbose(0,
                "HOA Library %s (%s) for Pure Data %i.%i\n\
                Organisation: CICM - Université Paris 8 - Labex Arts H2H\n\
                Authors:\n \
                2012: Pierre Guillot, Eliott Paris & Julien Colafrancesco\n\
                2012-2016: Pierre Guillot & Eliott Paris\n\
                2015: Pierre Guillot, Eliott Paris & Thomas Le Meur (Light version)\n\
                2016: Pierre Guillot & Eliott Paris (Light version)\n\
                Pure Data implementation: Pierre Guillot, Eliott Paris & Thomas Le Meur",
                hoaversion, __DATE__, PD_MAJOR_VERSION, PD_MINOR_VERSION);
        gensym("hoa.library v2.3-beta")->s_thing = (struct _class **)1;
    }
}

void hoa_processor_clear(void* obj)
{
    t_hoa_processor* x = (t_hoa_processor*)obj;
    if(x->f_inputs && x->f_nins)
    {
        freebytes(x->f_inputs, x->f_nins * sizeof(t_sample*));
    }
    if(x->f_outputs && x->f_nouts)
    {
        freebytes(x->f_outputs, x->f_nouts * sizeof(t_sample*));
    }
}

static t_int *hoa_processor_perform(t_int *w)
{
    t_hoa_processor *x = (t_hoa_processor *)(w[1]);
    x->f_method(x, x->f_vectorsize, x->f_nins, x->f_inputs, x->f_nouts, x->f_outputs);
    return (w+2);
}

void hoa_processor_prepare(void* obj, t_hoa_processor_perfm m, t_signal **sp)
{
    size_t i;
    t_hoa_processor *x = (t_hoa_processor *)(obj);
    x->f_method = m;
    x->f_vectorsize = sp[0]->s_n;
    for(i = 0; i < x->f_nins; ++i)
    {
        x->f_inputs[i] = sp[i]->s_vec;
    }
    for(i = 0; i < x->f_nouts; ++i)
    {
        x->f_outputs[i] = sp[i+x->f_nins]->s_vec;
    }
    dsp_add(hoa_processor_perform, 1, x);
}


