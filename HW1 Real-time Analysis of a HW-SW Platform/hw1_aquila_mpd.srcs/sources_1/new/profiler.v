`timescale 1ns / 1ps
// =============================================================================
//  Program : profiler.v
//  Author  : Chih-Ling Chu
//  Date    : Oct/08/2023
// -----------------------------------------------------------------------------
//  Description:
//  This module implements the RISC-V Core Local Interrupt (CLINT) Controller.
//  The ticking signal is currently fixed to the CPU clock instead of an external
//  RTC clock. The OS (e.g., FreeRTOS) must set the frequency of clk_i to the
//  OS timer parameter properly (e.g., configCPU_CLOCK_HZ).
// -----------------------------------------------------------------------------
//  Revision information:
//
//  Aug/24/2022, by Chun-Jen Tsai:
//    Remove the TIMER parameter and use the CPU clock to drive CLINT.
//    Previous code assumes that the interrupt generator is driven by a
//    1000 Hz clock (i.e. 1 msec ticks) and the TIMER parameter is
//    used to set the number of CPU ticks within 1 msec.  Unfortunately,
//    this design does not match the popular SiFive CLINT behavior.
//
// -----------------------------------------------------------------------------
//  License information:
//
//  This software is released under the BSD-3-Clause Licence,
//  see https://opensource.org/licenses/BSD-3-Clause for details.
//  In the following license statements, "software" refers to the
//  "source code" of the complete hardware/software system.
//
//  Copyright 2019,
//                    Embedded Intelligent Systems Lab (EISL)
//                    Deparment of Computer Science
//                    National Chiao Tung Uniersity
//                    Hsinchu, Taiwan.
//
//  All rights reserved.
//
//  Redistribution and use in source and binary forms, with or without
//  modification, are permitted provided that the following conditions are met:
//
//  1. Redistributions of source code must retain the above copyright notice,
//     this list of conditions and the following disclaimer.
//
//  2. Redistributions in binary form must reproduce the above copyright notice,
//     this list of conditions and the following disclaimer in the documentation
//     and/or other materials provided with the distribution.
//
//  3. Neither the name of the copyright holder nor the names of its contributors
//     may be used to endorse or promote products derived from this software
//     without specific prior written permission.
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
//  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
//  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
//  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
//  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
//  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
//  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
//  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
//  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
//  POSSIBILITY OF SUCH DAMAGE.
// =============================================================================
`include "aquila_config.vh"

module profiler
#( parameter XLEN = 32 )
(
    input                   clk_i,
    input                   rst_i,
    
    input      [XLEN-1 : 0] pc_i,
    input                   is_mem_access_i,
    
    input                   stall_data_hazard_i, // The stall signal from Pipeline Control.
    input                   stall_from_exe_i,    // The stall signal from Execute.
    input                   stall_instr_fetch_i,
    input                   stall_data_fetch_i,
    input                   stall_pipeline_i
);

(* mark_debug = "true" *) reg  [XLEN-1 : 0] core_list_find_comp;
(* mark_debug = "true" *) reg  [XLEN-1 : 0] core_list_reverse_comp;
(* mark_debug = "true" *) reg  [XLEN-1 : 0] core_state_transition_comp;
(* mark_debug = "true" *) reg  [XLEN-1 : 0] matrix_mul_matrix_bitextract_comp;
(* mark_debug = "true" *) reg  [XLEN-1 : 0] crcu8_comp;

(* mark_debug = "true" *) reg  [XLEN-1 : 0] core_list_find_mem;
(* mark_debug = "true" *) reg  [XLEN-1 : 0] core_list_reverse_mem;
(* mark_debug = "true" *) reg  [XLEN-1 : 0] core_state_transition_mem;
(* mark_debug = "true" *) reg  [XLEN-1 : 0] matrix_mul_matrix_bitextract_mem;
(* mark_debug = "true" *) reg  [XLEN-1 : 0] crcu8_mem;


(* mark_debug = "true" *) reg  [XLEN-1 : 0] stall_data_hazard_count;
(* mark_debug = "true" *) reg  [XLEN-1 : 0] stall_from_exe_count;
(* mark_debug = "true" *) reg  [XLEN-1 : 0] stall_instr_fetch_count;
(* mark_debug = "true" *) reg  [XLEN-1 : 0] stall_data_fetch_count;
(* mark_debug = "true" *) reg  [XLEN-1 : 0] stall_pipeline_count;

(* mark_debug = "true" *) reg  [XLEN*2-1 : 0] total_clk_cycle;

reg main_start, main_end;

always @(posedge clk_i)
begin
    if (rst_i)
    begin
       core_list_find_comp <= 32'b0;
       core_list_reverse_comp <= 32'b0;
       core_state_transition_comp <= 32'b0;
       matrix_mul_matrix_bitextract_comp <= 32'b0;
       crcu8_comp <= 32'b0;
       
       core_list_find_mem <= 32'b0;
       core_list_reverse_mem <= 32'b0;
       core_state_transition_mem <= 32'b0;
       matrix_mul_matrix_bitextract_mem <= 32'b0;
       crcu8_mem <= 32'b0;
       
       total_clk_cycle <= 64'b0;
       main_start <= 1'b0;
       main_end <= 1'b0;
    end
    else 
    begin
        if (pc_i == 32'h1088) 
            main_start <= 1'b1;
        else if (pc_i == 32'h1804)
            main_end <= 1'b1;
            
        if (main_start && ~main_end)
            total_clk_cycle <= total_clk_cycle + 1;
            
        if (pc_i >= 32'h1d28 && pc_i <= 32'h1d7c)
            if (is_mem_access_i)
                core_list_find_mem <= core_list_find_mem + 1;
            else
                core_list_find_comp <= core_list_find_comp + 1;
                
        else if (pc_i >= 32'h1d80 && pc_i <= 32'h1da0)
            if(is_mem_access_i)
                core_list_reverse_mem <= core_list_reverse_mem + 1;
            else
                core_list_reverse_comp <= core_list_reverse_comp + 1;
                
        else if (pc_i >= 32'h2a14 && pc_i <= 32'h2d0c)
            if (is_mem_access_i)
                core_state_transition_mem <= core_state_transition_mem + 1;
            else
                core_state_transition_comp <= core_state_transition_comp + 1;
                
        else if (pc_i >= 32'h2670 && pc_i <= 32'h272c)
            if (is_mem_access_i)
                matrix_mul_matrix_bitextract_mem <= matrix_mul_matrix_bitextract_mem + 1;
            else 
                matrix_mul_matrix_bitextract_comp <= matrix_mul_matrix_bitextract_comp + 1;
                
        else if (pc_i >= 32'h19e8 && pc_i <= 32'h1a28)
            if (is_mem_access_i)
                crcu8_mem <= crcu8_mem + 1;
            else
                crcu8_comp <= crcu8_comp + 1;
    end
end

endmodule // profiler