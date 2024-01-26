// =============================================================================
//  Program : neuronet.c
//  Author  : Chun-Jen Tsai
//  Date    : Dec/06/2023
// -----------------------------------------------------------------------------
//  Description:
//      This is a neural network library that can be used to implement
//  a inferencing model of the classical multilayer perceptorn (MLP) neural
//  network. It reads a model weights file to setup the MLP. The MLP
//  can have up to MAX_LAYERS, which is defined in neuronet.h. To avoid using
//  the C math library, the relu() fucntion is used for the activation
//  function. This degrades the recognition accuracy significantly, but it
//  serves the teaching purposes well.
//
//  This program is designed as one of the homework projects for the course:
//  Microprocessor Systems: Principles and Implementation
//  Dept. of CS, NYCU (aka NCTU), Hsinchu, Taiwan.
// -----------------------------------------------------------------------------
//  Revision information:
//
//  None.
// =============================================================================

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "neuronet.h"

void neuronet_init(NeuroNet *nn, int n_layers, int *n_neurons)
{
    int layer_idx, neuron_idx, sum;
    float *head[MAX_LAYERS];  // Pointer to the first neuron value of each layer.

    if (n_layers < 2 || n_layers > MAX_LAYERS)
    {
        printf("ERROR!!!\n");
        printf("layer count is less than 2 or larger than %d\n", MAX_LAYERS);
        return;
    }

    nn->total_neurons = 0;
    for (layer_idx = 0; layer_idx < n_layers; layer_idx++)
    {
        nn->n_neurons[layer_idx] = n_neurons[layer_idx];
        nn->total_neurons += n_neurons[layer_idx];
    }

    nn->neurons = (float *) malloc(sizeof(float *) * nn->total_neurons);
    nn->forward_weights = (float **) malloc(sizeof(float *) * nn->total_neurons);
    nn->previous_neurons = (float **) malloc(sizeof(float *) * nn->total_neurons);
    nn->total_layers = n_layers;

    neuron_idx = 0;
    for (layer_idx = 0; layer_idx < nn->total_layers; layer_idx++)
    {
        head[layer_idx] = &(nn->neurons[neuron_idx]);
        neuron_idx += nn->n_neurons[layer_idx];
    }

    // Set a shortcut pointer to the output layer neuron values.
    nn->output = head[nn->total_layers - 1];

    // Set the previous layer neuron pointers for each hidden & output neuron
    for (neuron_idx = nn->n_neurons[0], layer_idx = 1; layer_idx < nn->total_layers; layer_idx++)
    {
        for (unsigned j = 0; j < nn->n_neurons[layer_idx]; ++j, ++neuron_idx)
        {
            nn->previous_neurons[neuron_idx] = head[layer_idx - 1];
        }
    }

    // Initialize the weight array.
    nn->total_weights = 0;
    for (layer_idx = 1; layer_idx < nn->total_layers; layer_idx++)
    {
        // Accumulating # of weights, including one bias value per neuron.
        nn->total_weights += (nn->n_neurons[layer_idx-1] + 1)*nn->n_neurons[layer_idx];
    }
    nn->weights = (float *) malloc(sizeof(float) * nn->total_weights);

    // Set the starting pointer to the forward weights for each neurons.
    sum = 0, neuron_idx = nn->n_neurons[0];
    for (layer_idx = 1; layer_idx < nn->total_layers; layer_idx++)
    {
        for (int idx = 0; idx < nn->n_neurons[layer_idx]; idx++, neuron_idx++)
        {
            nn->forward_weights[neuron_idx] = &(nn->weights[sum]);
            sum += (nn->n_neurons[layer_idx-1] + 1); // add one for bias.
        }
    }
}

void neuronet_load(NeuroNet *nn, float *weights)
{
    for (int idx = 0; idx < nn->total_weights; idx++)
    {
        nn->weights[idx] = *(weights++);
    }
    return;
}

void neuronet_free(NeuroNet *nn)
{
    free(nn->weights);
    free(nn->previous_neurons);
    free(nn->forward_weights);
    free(nn->neurons);
}

int count = 0;
#define DSA_READY_ADDR      0xC4000000
#define DSA_COUNT_ADDR      0xC4000004
#define DSA_RESULT_ADDR     0xC4000008
#define DSA_TRIGGER_ADDR    0xC400000C
#define DSA_VECTOR_SRAM     0xC4001000
#define DSA_WEIGHT_SRAM     0xC4002000
volatile unsigned int *p_dsa_ready = (unsigned int*) DSA_READY_ADDR;
volatile unsigned int *p_dsa_cnt = (unsigned int*) DSA_COUNT_ADDR;
volatile float *p_dsa_result = (float *) DSA_RESULT_ADDR;
volatile unsigned int *p_dsa_trigger = (unsigned int*) DSA_TRIGGER_ADDR;
volatile float *p_vector_sram = (float*) DSA_VECTOR_SRAM;
volatile float *p_weight_sram = (float*) DSA_WEIGHT_SRAM;

int neuronet_eval(NeuroNet *nn, float *images)
{
    float inner_product, max;
    // float max;
    float *p_neuron, *p_weight;
    int idx, layer_idx, neuron_idx, max_idx;

    // Copy the input image array (784 pixels) to the input neurons.
    memcpy((void *) nn->neurons, (void *) images, nn->n_neurons[0]*sizeof(float));

    // Forward computations
    neuron_idx = nn->n_neurons[0];
    // printf("neuron_idx: %d / nn->total_layers: %d\n", neuron_idx, nn->total_layers);
    for (layer_idx = 1; layer_idx < nn->total_layers; layer_idx++)
    {
        // printf("nn->n_neurons[%d]: %d / nn->neurons[%d]: %d\n", layer_idx, nn->n_neurons[layer_idx], layer_idx-1, nn->n_neurons[layer_idx-1]);
        for (idx = 0; idx < nn->n_neurons[layer_idx]; idx++, neuron_idx++)
        {
            // 'p_weight' points to the first forward weight of a layer.
            p_weight = nn->forward_weights[neuron_idx];
            inner_product = 0.0;

            // // Loop over all forward-connected neural links.
            p_neuron = nn->previous_neurons[neuron_idx];
            // for (int jdx = 0; jdx < nn->n_neurons[layer_idx-1]; jdx++)
            // {
            //      inner_product += (*p_neuron++) * (*p_weight++);
            // }
            // inner_product += *(p_weight); // The last weight of a neuron is the bias.
            // printf("inner_product: %f\n", inner_product);

            // void * memcpy(void * destination, const void * source, size_t num);
            // printf("p_weight=nn->forward_weights[neuron_idx=%d]: %f / p_neuron=nn->previous_neurons[neuron_idx]: %f\n", neuron_idx, *nn->forward_weights[neuron_idx], *nn->previous_neurons[neuron_idx]);
            // memcpy((void *)p_vector_sram, (void *)nn->previous_neurons[neuron_idx], sizeof(float) * (nn->n_neurons[layer_idx-1]));
            memcpy((void *)p_vector_sram, (void *)p_neuron, sizeof(float) * (nn->n_neurons[layer_idx-1]));
            
            // memcpy((void *)p_weight_sram, (void *)nn->forward_weights[neuron_idx], sizeof(float) * (nn->n_neurons[layer_idx-1]));
            memcpy((void *)p_weight_sram, (void *)p_weight, sizeof(float) * (nn->n_neurons[layer_idx-1]));
            // printf("p_vector_sram_0: %f / p_weight_sram_0: %f\n", *p_vector_sram, *p_weight_sram);
            // printf("p_vector_sram_x: %f / p_weight_sram_x: %f\n", *(p_vector_sram+nn->n_neurons[layer_idx-1]-1), *(p_weight_sram+nn->n_neurons[layer_idx-1]-1));
            *p_dsa_cnt = nn->n_neurons[layer_idx-1];
            *p_dsa_trigger = 1;
            // printf("p_dsa_cnt: %d\n", *p_dsa_cnt);

            while(!(*p_dsa_ready));
            /// float* inner_product = (float *)malloc(sizeof(float));
            /// inner_product = p_dsa_result;
            *p_dsa_ready = 0;
            /// *inner_product += *(nn->forward_weights[neuron_idx]);
            inner_product = *p_dsa_result;
            inner_product += *(p_weight);
            // printf("p_dsa_ready: %d / inner_product: %f / p_dsa_result: %f\n", *p_dsa_ready, inner_product, *p_dsa_result);
            
            nn->neurons[neuron_idx] = relu(inner_product);
        }
    }

    // Return the index to the maximal neuron value of the output layer.
    max = -1.0, max_idx = 0;
    for (idx = 0; idx < nn->n_neurons[nn->total_layers-1]; idx++)
    {
        if (max < nn->output[idx])
        {
            max_idx = idx;
            max = nn->output[idx];
        }
    }

    return max_idx;
}

float relu(float x)
{
	return x < 0.0 ? 0.0 : x;
}

