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

#include <ivas/ivas_kernel.h>
#include <stdio.h>
#include <unistd.h>

typedef struct _kern_priv
{
    float mean_r;
    float mean_g;
    float mean_b;
    float scale_r;
    float scale_g;
    float scale_b;
    IVASFrame *params;
} ResizeKernelPriv;

int32_t
xlnx_kernel_start(IVASKernel *handle, int start, IVASFrame *input[MAX_NUM_OBJECT], IVASFrame *output[MAX_NUM_OBJECT]);
int32_t xlnx_kernel_done(IVASKernel *handle);
int32_t xlnx_kernel_init(IVASKernel *handle);
uint32_t xlnx_kernel_deinit(IVASKernel *handle);

uint32_t xlnx_kernel_deinit(IVASKernel *handle)
{
    ResizeKernelPriv *kernel_priv;
    kernel_priv = (ResizeKernelPriv *)handle->kernel_priv;
    ivas_free_buffer (handle, kernel_priv->params);
    free(kernel_priv);
    return 0;
}

int32_t xlnx_kernel_init(IVASKernel *handle)
{
    json_t *jconfig = handle->kernel_config;
    json_t *val; /* kernel config from app */
    ResizeKernelPriv *kernel_priv;
    float *pPtr; 

    kernel_priv = (ResizeKernelPriv *)calloc(1, sizeof(ResizeKernelPriv));
    if (!kernel_priv) {
        printf("Error: Unable to allocate resize kernel memory\n");
    }

    /* parse config */
    val = json_object_get(jconfig, "mean_r");
    if (!val || !json_is_number(val))
        kernel_priv->mean_r = 0;
    else {
        kernel_priv->mean_r = json_number_value(val);
    }
    printf("Resize: mean_r=%f\n", kernel_priv->mean_r);

    val = json_object_get(jconfig, "mean_g");
    if (!val || !json_is_number(val))
        kernel_priv->mean_g = 0;
    else {
        kernel_priv->mean_g = json_number_value(val);
    }
    printf("Resize: mean_g=%f\n", kernel_priv->mean_g);

    val = json_object_get(jconfig, "mean_b");
    if (!val || !json_is_number(val))
        kernel_priv->mean_b = 0;
    else {
        kernel_priv->mean_b = json_number_value(val);
    }
    printf("Resize: mean_b=%f\n", kernel_priv->mean_b);

    /* parse config */
    val = json_object_get(jconfig, "scale_r");
    if (!val || !json_is_number(val))
	kernel_priv->scale_r = 1;
    else
	kernel_priv->scale_r = json_number_value(val);
    printf("Resize: scale_r=%f\n", kernel_priv->scale_r);

    val = json_object_get(jconfig, "scale_g");
    if (!val || !json_is_number(val))
	kernel_priv->scale_g = 1;
    else
	kernel_priv->scale_g = json_number_value(val);
    printf("Resize: scale_g=%f\n", kernel_priv->scale_g);

    val = json_object_get(jconfig, "scale_b");
    if (!val || !json_is_number(val))
	kernel_priv->scale_b = 1;
    else
	kernel_priv->scale_b = json_number_value(val);
    printf("Resize: scale_b=%f\n", kernel_priv->scale_b);

    kernel_priv->params = ivas_alloc_buffer (handle, 6*(sizeof(float)), IVAS_INTERNAL_MEMORY, NULL);
    pPtr = kernel_priv->params->vaddr[0];
    pPtr[0] = (float)kernel_priv->mean_r;  
    pPtr[1] = (float)kernel_priv->mean_g;  
    pPtr[2] = (float)kernel_priv->mean_b;  
    pPtr[3] = (float)kernel_priv->scale_r;  
    pPtr[4] = (float)kernel_priv->scale_g;  
    pPtr[5] = (float)kernel_priv->scale_b;  

    handle->kernel_priv = (void *)kernel_priv;

    return 0;
}

int32_t xlnx_kernel_start(IVASKernel *handle, int start, IVASFrame *input[MAX_NUM_OBJECT], IVASFrame *output[MAX_NUM_OBJECT])
{
    ResizeKernelPriv *kernel_priv;
    kernel_priv = (ResizeKernelPriv *)handle->kernel_priv;

    ivas_register_write(handle, &(input[0]->props.width), sizeof(uint32_t), 0x40);   /* In width */
    ivas_register_write(handle, &(input[0]->props.height), sizeof(uint32_t), 0x48);  /* In height */
    ivas_register_write(handle, &(input[0]->props.stride), sizeof(uint32_t), 0x50);  /* In stride */

    ivas_register_write(handle, &(output[0]->props.width), sizeof(uint32_t), 0x58);  /* Out width */
    ivas_register_write(handle, &(output[0]->props.height), sizeof(uint32_t), 0x60); /* Out height */
    ivas_register_write(handle, &(output[0]->props.width), sizeof(uint32_t), 0x68); /* Out stride */

    ivas_register_write(handle, &(input[0]->paddr[0]), sizeof(uint64_t), 0x10);      /* Y Input */
    ivas_register_write(handle, &(input[0]->paddr[1]), sizeof(uint64_t), 0x1C);      /* UV Input */
    ivas_register_write(handle, &(output[0]->paddr[0]), sizeof(uint64_t), 0x28);      /* Output */
    ivas_register_write(handle, &(kernel_priv->params->paddr[0]), sizeof(uint64_t), 0x34);     /* Params */

    ivas_register_write(handle, &start, sizeof(uint32_t), 0x0);                      /* start */
    return 0;
}

int32_t xlnx_kernel_done(IVASKernel *handle)
{
    uint32_t val = 0, count = 0;
    do {
        ivas_register_read(handle, &val, sizeof(uint32_t), 0x0); /* start */
        count++;
        if (count > 1000000) {
            printf("ERROR: kernel done wait TIME OUT !!\n");
            return 0;
        }
    } while (!(0x4 & val));
    return 1;
}
