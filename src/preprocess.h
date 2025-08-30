#ifndef DUCC_PREPROCESS_H
#define DUCC_PREPROCESS_H

#include "common.h"
#include "io.h"
#include "token.h"

TokenArray* preprocess(InFile* src, StrArray* included_files);

#endif
