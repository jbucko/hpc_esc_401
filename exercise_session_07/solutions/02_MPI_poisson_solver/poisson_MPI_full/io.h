#ifndef IO_H
#define IO_H

#include "init.h"
#include <fstream>

void output_source_serial(params p, double** f);
void output_source(params p, double** f, int rank);
void output_source_rank0(params p, double** f, int rank, int size);
void output_serial(params p, int step, double** u_new);
void output(params p, int step, double** u_new, int rank);
void output_rank0(params p, int step, double** u_new, int rank, int size);

#endif