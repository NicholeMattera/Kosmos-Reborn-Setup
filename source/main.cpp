/*
 * Kosmos Reborn Setup
 * Copyright (c) 2022 Nichole Mattera
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include <switch.h>

#define INNER_HEAP_SIZE 0x80000

extern "C" {
    u32 __nx_applet_type = AppletType_None;

    u32 __nx_fs_num_sessions = 1;

    void __libnx_initheap(void) {
        static u8 inner_heap[INNER_HEAP_SIZE];
        extern void* fake_heap_start;
        extern void* fake_heap_end;

        // Configure the newlib heap.
        fake_heap_start = inner_heap;
        fake_heap_end   = inner_heap + sizeof(inner_heap);
    }

    void __appInit(void) {
        Result rc;

        rc = smInitialize();
        if (R_FAILED(rc))
            diagAbortWithResult(MAKERESULT(Module_Libnx, LibnxError_InitFail_SM));

        rc = setsysInitialize();
        if (R_SUCCEEDED(rc)) {
            SetSysFirmwareVersion fw;
            rc = setsysGetFirmwareVersion(&fw);
            if (R_SUCCEEDED(rc))
                hosversionSet(MAKEHOSVERSION(fw.major, fw.minor, fw.micro));
            setsysExit();
        }

        rc = hidInitialize();
        if (R_FAILED(rc))
            diagAbortWithResult(MAKERESULT(Module_Libnx, LibnxError_InitFail_HID));

        rc = fsInitialize();
        if (R_FAILED(rc))
            diagAbortWithResult(MAKERESULT(Module_Libnx, LibnxError_InitFail_FS));

        fsdevMountSdmc();

        smExit();
    }

    void __appExit(void) {
        fsdevUnmountAll();
        fsExit();
        hidExit();
    }
}

int main(int argc, char* argv[]) {
    return 0;
}
