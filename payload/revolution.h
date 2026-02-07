#pragma once

#include "revolution/arc.h"
#include "revolution/ax.h"
#include "revolution/dvd.h"
#include "revolution/dvdex.h"
#include "revolution/gx.h"
#include "revolution/mem.h"
#include "revolution/mtx.h"
#include "revolution/nand.h"
#include "revolution/os.h"
#include "revolution/pad.h"
#include "revolution/sc.h"
#include "revolution/so/so.h"
#include "revolution/vi.h"

#include "revolution/net.h"

#include "revolution/exi.h"

#include "revolution/es.h"

// These must come after os.h and so.h since they depend on types from those headers
#include "revolution/dwc.h"