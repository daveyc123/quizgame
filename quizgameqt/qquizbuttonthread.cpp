/*

A portion of this code is derived from the adafruit retrogame, available on github.
Copyright statement:

Written by Phil Burgess for Adafruit Industries, distributed under BSD
License.  Adafruit invests time and resources providing this open source
code, please support Adafruit and open-source hardware by purchasing
products from Adafruit!


Copyright (c) 2013 Adafruit Industries.
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

- Redistributions of source code must retain the above copyright notice,
  this list of conditions and the following disclaimer.
- Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.
*/

#include "qquizbuttonthread.h"
#include <QtDebug>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>
#include <signal.h>
#include <sys/mman.h>
#include <linux/input.h>
#include <linux/uinput.h>


#define BCM2708_PERI_BASE 0x20000000
#define GPIO_BASE         (BCM2708_PERI_BASE + 0x200000)
#define BLOCK_SIZE        (4*1024)
#define GPPUD             (0x94 / 4)
#define GPPUDCLK0         (0x98 / 4)


// A few globals ---------------------------------------------------------
char sysfs_root[] = "/sys/class/gpio"; // Location of Sysfs GPIO files
volatile unsigned int
  *gpio;                             // GPIO register table
const int
   debounceTime = 20;                // 20 ms for button debouncing



QQuizButtonThread::QQuizButtonThread(QObject *parent) :
    QThread(parent)
{
    // Nothing to see here
}

QQuizButtonThread::~QQuizButtonThread() {
    cleanup();
}

void QQuizButtonThread::addButton(int gpio, QString id) {
    gpios.append(gpio);
    ids.append(id);
}


void QQuizButtonThread::run() {
    if (gpios.size() != ids.size()) {
        qDebug() << "QQuizButtonThread gpios don't match ids";
        return;
    }

    runAdafruitCode();
}

void QQuizButtonThread::runAdafruitCode() {

    // A few arrays here are declared with 32 elements, even though
    // values aren't needed for io[] members where the 'key' value is
    // GND.  This simplifies the code a bit -- no need for mallocs and
    // tests to create these arrays -- but may waste a handful of
    // bytes for any declared GNDs.
    char                   buf[50],      // For sundry filenames
                           c;            // Pin input value ('0'/'1')
    int                    fd,           // For mmap, sysfs, uinput
                           i, j,         // Asst. counter
                           bitmask,      // Pullup enable bitmask
                           timeout = -1, // poll() timeout
                           intstate[32], // Last-read state
                           extstate[32]; // Debounced state
    unsigned long          bitMask, bit; // For Vulcan pinch detect
    volatile unsigned char shortWait;    // Delay counter

    struct pollfd          p[32];        // GPIO file descriptors

    // ----------------------------------------------------------------
    // Although Sysfs provides solid GPIO interrupt handling, there's
    // no interface to the internal pull-up resistors (this is by
    // design, being a hardware-dependent feature).  It's necessary to
    // grapple with the GPIO configuration registers directly to enable
    // the pull-ups.  Based on GPIO example code by Dom and Gert van
    // Loo on elinux.org

    if((fd = open("/dev/mem", O_RDWR | O_SYNC)) < 0)
        qDebug() << "Can't open /dev/mem";
    gpio = mmap(            // Memory-mapped I/O
      NULL,                 // Any adddress will do
      BLOCK_SIZE,           // Mapped block length
      PROT_READ|PROT_WRITE, // Enable read+write
      MAP_SHARED,           // Shared with other processes
      fd,                   // File to map
      (void*)GPIO_BASE );          // Offset to GPIO registers
    close(fd);              // Not needed after mmap()
    if(gpio == MAP_FAILED) {
        qDebug() << "Can't mmap()";
    }
    // Make combined bitmap of pullup-enabled pins:
    for(bitmask=i=0; i < gpios.size(); i++) {
        bitmask |= (1 << gpios.at(i));
    }
    gpio[GPPUD]     = 2;                    // Enable pullup
    for(shortWait=150;--shortWait;);        // Min 150 cycle wait
    gpio[GPPUDCLK0] = bitmask;              // Set pullup mask
    for(shortWait=150;--shortWait;);        // Wait again
    gpio[GPPUD]     = 0;                    // Reset pullup registers
    gpio[GPPUDCLK0] = 0;
    (void)munmap((void *)gpio, BLOCK_SIZE); // Done with GPIO mmap()


    // ----------------------------------------------------------------
    // All other GPIO config is handled through the sysfs interface.

    sprintf(buf, "%s/export", sysfs_root);
    if((fd = open(buf, O_WRONLY)) < 0) // Open Sysfs export file
        qDebug() << "Can't open GPIO export file";
    for(i=j=0; i < gpios.size(); i++) { // For each pin of interest...
        sprintf(buf, "%d", gpios.at(i));
        write(fd, buf, strlen(buf));             // Export pin
        pinConfig(gpios.at(i), "active_low", "0"); // Don't invert

        // Set pin to input, detect rise+fall events
        if(pinConfig(gpios.at(i), "direction", "in") || pinConfig(gpios.at(i), "edge"     , "both")) {
            qDebug() << "Pin config failed";
        }
        // Get initial pin value
        sprintf(buf, "%s/gpio%d/value",
          sysfs_root, gpios.at(i));
        // The p[] file descriptor array isn't necessarily
        // aligned with the io[] array.  GND keys in the
        // latter are skipped, but p[] requires contiguous
        // entries for poll().  So the pins to monitor are
        // at the head of p[], and there may be unused
        // elements at the end for each GND.  Same applies
        // to the intstate[] and extstate[] arrays.
        if((p[j].fd = open(buf, O_RDONLY)) < 0) {
            qDebug() << "Can't access pin value";
        }
        intstate[j] = 0;
        if((read(p[j].fd, &c, 1) == 1) && (c == '0'))
            intstate[j] = 1;
        extstate[j] = intstate[j];
        p[j].events  = POLLPRI; // Set up poll() events
        p[j].revents = 0;
        j++;

    } // 'j' is now count of non-GND items in io[] table
    close(fd); // Done exporting

    // ----------------------------------------------------------------
    // Monitor GPIO file descriptors for button events.  The poll()
    // function watches for GPIO IRQs in this case; it is NOT
    // continually polling the pins!  Processor load is near zero.

    // Wait for IRQ on pin (or timeout for button debounce)
    while (1) {
        if(poll(p, j, timeout) > 0) { // If IRQ...
            for(i=0; i<j; i++) {       // Scan non-GND pins...
                if(p[i].revents) { // Event received?
                    // Read current pin state, store
                    // in internal state flag, but
                    // don't issue to uinput yet --
                    // must wait for debounce!
                    lseek(p[i].fd, 0, SEEK_SET);
                    read(p[i].fd, &c, 1);
                    if(c == '0')      intstate[i] = 1;
                    else if(c == '1') intstate[i] = 0;
                    p[i].revents = 0; // Clear flag
                }
            }
            timeout = debounceTime; // Set timeout for debounce
            c       = 0;            // Don't issue SYN event
            // Else timeout occurred
        } else if(timeout == debounceTime) { // Button debounce timeout
            // 'j' (number of non-GNDs) is re-counted as
            // it's easier than maintaining an additional
            // remapping table or a duplicate key[] list.
            bitMask = 0L; // Mask of buttons currently pressed
            bit     = 1L;
            for(c=i=j=0; i < gpios.size(); i++, bit<<=1) {
                // Compare internal state against
                // previously-issued value.  Send
                // keystrokes only for changed states.
                if(intstate[j] != extstate[j]) {
                    extstate[j] = intstate[j];
                    c = 1; // Follow w/SYN event

                       if (intstate[j]) {
                           emit buttonPressed(ids.at(i));
                       }
                }
                j++;
                if(intstate[i]) bitMask |= bit;
            }
        }
    }
}

// Set one GPIO pin attribute through the Sysfs interface.
int QQuizButtonThread::pinConfig(int pin, char *attr, char *value) {
    char filename[50];
    int  fd, w, len = strlen(value);
    sprintf(filename, "%s/gpio%d/%s", sysfs_root, pin, attr);
    if((fd = open(filename, O_WRONLY)) < 0) return -1;
    w = write(fd, value, len);
    close(fd);
    return (w != len); // 0 = success
}

// Un-export any Sysfs pins used; don't leave filesystem cruft.  Also
// restores any GND pins to inputs.  Write errors are ignored as pins
// may be in a partially-initialized state.
void QQuizButtonThread::cleanup() {
    char buf[50];
    int  fd, i;
    sprintf(buf, "%s/unexport", sysfs_root);
    if((fd = open(buf, O_WRONLY)) >= 0) {
        for(i=0; i < gpios.size(); i++) {
            // Un-export all items
            sprintf(buf, "%d", gpios.at(i));
            write(fd, buf, strlen(buf));
        }
        close(fd);
    }
}
