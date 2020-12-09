/*
 * Copyright 2020 Xilinx, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <ivas/ivaslogs.h>
#include <ivas/ivas_kernel.h>

#define DEFAULT_PPC	2

typedef struct _kern_priv
{
    float mean_r;
    float mean_g;
    float mean_b;
    float scale_r;
    float scale_g;
    float scale_b;
    int bias; /* for future */
    int ppc;
    IVASFrame *mem;
    int log_level;
} ResizeKernelPriv;

int32_t xlnx_kernel_start(IVASKernel *handle, int start, IVASFrame *input[MAX_NUM_OBJECT], IVASFrame *output[MAX_NUM_OBJECT]);
int32_t xlnx_kernel_done(IVASKernel *handle);
int32_t xlnx_kernel_init(IVASKernel *handle);
uint32_t xlnx_kernel_deinit(IVASKernel *handle);

uint32_t xlnx_kernel_deinit(IVASKernel *handle)
{
    ResizeKernelPriv *kernel_priv;
    kernel_priv = (ResizeKernelPriv *)handle->kernel_priv;
    ivas_free_buffer (handle, kernel_priv->mem);
    free(kernel_priv);
    return 0;
}

int32_t xlnx_kernel_init(IVASKernel *handle)
{
    json_t *jconfig = handle->kernel_config;
    json_t *val; /* kernel config from app */
    ResizeKernelPriv *kernel_priv;
    float *pPtr;

    handle->is_multiprocess = 0;

    kernel_priv = (ResizeKernelPriv *)calloc(1, sizeof(ResizeKernelPriv));
    if (!kernel_priv) {
        printf("Error: Unable to allocate PPE kernel memory\n");
    }

    /* parse config */
    val = json_object_get (jconfig, "debug_level");
    if (!val || !json_is_integer (val))
	    kernel_priv->log_level = LOG_LEVEL_WARNING;
    else
	    kernel_priv->log_level = json_integer_value (val);
    LOG_MESSAGE (LOG_LEVEL_INFO, kernel_priv->log_level, "IVAS PPE: debug_level %d", kernel_priv->log_level);

    val = json_object_get (jconfig, "ppc");
    if (!val || !json_is_integer (val))
	    kernel_priv->ppc = DEFAULT_PPC;
    else
	    kernel_priv->ppc = json_integer_value (val);
    LOG_MESSAGE (LOG_LEVEL_INFO, kernel_priv->log_level, "IVAS PPE: PPC %d", kernel_priv->ppc);

    val = json_object_get(jconfig, "mean_r");
    if (!val || !json_is_number(val))
        kernel_priv->mean_r = 0;
    else {
        kernel_priv->mean_r = json_number_value(val);
    }
    LOG_MESSAGE (LOG_LEVEL_INFO, kernel_priv->log_level, "IVAS PPE: mean R = %lf", kernel_priv->mean_r);

    val = json_object_get(jconfig, "mean_g");
    if (!val || !json_is_number(val))
        kernel_priv->mean_g = 0;
    else {
        kernel_priv->mean_g = json_number_value(val);
    }
    LOG_MESSAGE (LOG_LEVEL_INFO, kernel_priv->log_level, "IVAS PPE: mean G = %lf", kernel_priv->mean_g);

    val = json_object_get(jconfig, "mean_b");
    if (!val || !json_is_number(val))
        kernel_priv->mean_b = 0;
    else {
        kernel_priv->mean_b = json_number_value(val);
    }
    LOG_MESSAGE (LOG_LEVEL_INFO, kernel_priv->log_level, "IVAS PPE: mean B = %lf", kernel_priv->mean_b);

    val = json_object_get(jconfig, "scale_r");
    if (!val || !json_is_number(val))
	kernel_priv->scale_r = 1;
    else
	kernel_priv->scale_r = json_number_value(val);
    LOG_MESSAGE (LOG_LEVEL_INFO, kernel_priv->log_level, "IVAS PPE: Scale R value %lf", kernel_priv->scale_r);

    val = json_object_get(jconfig, "scale_g");
    if (!val || !json_is_number(val))
	kernel_priv->scale_g = 1;
    else
	kernel_priv->scale_g = json_number_value(val);
    LOG_MESSAGE (LOG_LEVEL_INFO, kernel_priv->log_level, "IVAS PPE: Scale G value %lf", kernel_priv->scale_g);

    val = json_object_get(jconfig, "scale_b");
    if (!val || !json_is_number(val))
	kernel_priv->scale_b = 1;
    else
	kernel_priv->scale_b = json_number_value(val);
    LOG_MESSAGE (LOG_LEVEL_INFO, kernel_priv->log_level, "IVAS PPE: Scale B value %lf", kernel_priv->scale_b);

    kernel_priv->mem = ivas_alloc_buffer (handle, 9*(sizeof(float)), IVAS_INTERNAL_MEMORY, NULL);
    if (!kernel_priv->mem) {
    	    LOG_MESSAGE (LOG_LEVEL_ERROR, kernel_priv->log_level, "IVAS PPE: failed to allocate descriptor memory");
	    return -1;
    }

    pPtr = (float *)kernel_priv->mem->vaddr[0];

    pPtr[0] = (float)kernel_priv->mean_r;
    pPtr[1] = (float)kernel_priv->mean_g;
    pPtr[2] = (float)kernel_priv->mean_b;
    pPtr[3] = (float)kernel_priv->scale_r;
    pPtr[4] = (float)kernel_priv->scale_g;
    pPtr[5] = (float)kernel_priv->scale_b;
    pPtr[6] = (float)0;
    pPtr[7] = (float)0;
    pPtr[8] = (float)0;

    handle->kernel_priv = (void *)kernel_priv;

    return 0;
}

int32_t xlnx_kernel_start(IVASKernel *handle, int start, IVASFrame *input[MAX_NUM_OBJECT], IVASFrame *output[MAX_NUM_OBJECT])
{
    int uv_h, uv_w;
    uint64_t coeff_phy;
    int th1, th2;
    ResizeKernelPriv *kernel_priv;
    uv_h = (input[0]->props.height)/2;
    uv_w = (input[0]->props.width)/2;

    th1 = 255;
    th2 = 255;

    kernel_priv = (ResizeKernelPriv *)handle->kernel_priv;
    coeff_phy = kernel_priv->mem->paddr[0];

//    if (output[0]->props.width % kernel_priv->ppc != 0) {
//	    LOG_MESSAGE (LOG_LEVEL_ERROR, kernel_priv->log_level, "IVAS PPE: Out width not align to ppc !!");
//	    return -1;
//    }

    ivas_register_write(handle, &(input[0]->props.height), sizeof(uint32_t), 0x34);  /* In Y height */
    ivas_register_write(handle, &(input[0]->props.width), sizeof(uint32_t), 0x3C);   /* In Y width */

    ivas_register_write(handle, &(uv_h), sizeof(uint32_t), 0x44);  /* In UV height */
    ivas_register_write(handle, &(uv_w), sizeof(uint32_t), 0x4C);  /* In UV width */

    ivas_register_write(handle, &(output[0]->props.height), sizeof(uint32_t), 0x54); /* Out height */
    ivas_register_write(handle, &(output[0]->props.width), sizeof(uint32_t), 0x5C);  /* Out width */

    ivas_register_write(handle, &(input[0]->paddr[0]), sizeof(uint64_t), 0x10);      /* Y input */
    ivas_register_write(handle, &(input[0]->paddr[1]), sizeof(uint64_t), 0x1C);      /* UV input */
    ivas_register_write(handle, &(output[0]->paddr[0]), sizeof(uint64_t), 0x28);     /* Output buffer */

//    ivas_register_write(handle, &(input[0]->props.stride), sizeof(uint32_t), 0x64); /* stride */
    ivas_register_write(handle, &(coeff_phy), sizeof(uint64_t), 0x64); /* coeff mem phy add */

    ivas_register_write(handle, &(th1), sizeof(uint32_t), 0x70);
    ivas_register_write(handle, &(th2), sizeof(uint32_t), 0x78);

    ivas_register_write(handle, &start, sizeof(uint32_t), 0x0);                      /* start */
    return 0;
}

int32_t xlnx_kernel_done(IVASKernel *handle)
{
    uint32_t val = 0, count = 0;
    ResizeKernelPriv *kernel_priv;

    kernel_priv = (ResizeKernelPriv *)handle->kernel_priv;
    do {
        ivas_register_read(handle, &val, sizeof(uint32_t), 0x0); /* start */
        count++;
        if (count > 1000000) {
    	    LOG_MESSAGE (LOG_LEVEL_ERROR, kernel_priv->log_level, "IVAS PPE: kernel done wait TIME OUT !!");
            return 0;
        }
    } while (!(0x4 & val));
    return 1;
}
