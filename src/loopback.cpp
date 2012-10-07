/*
 * Copyright (c) 2012, Nasel
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 * 
 * * Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 * * Redistributions in binary form must reproduce the above
 *   copyright notice, this list of conditions and the following disclaimer
 *   in the documentation and/or other materials provided with the
 *   distribution.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#ifndef WIN32
    #include <sys/socket.h>
#endif
#include <stdexcept>
#include <cassert>
#include "loopback.h"
#include "packet_sender.h"
#include "ip.h"
#include "llc.h"
#include "rawpdu.h"

namespace Tins {
Loopback::Loopback()
: _family()
{
    
}
    
Loopback::Loopback(uint32_t family_id, PDU *inner_pdu)
: PDU(inner_pdu), _family(family_id)
{
    
}

Loopback::Loopback(const uint8_t *buffer, uint32_t total_sz) 
{
    if(total_sz < sizeof(_family))
        throw std::runtime_error("Not enough size for a loopback PDU");
    _family = *reinterpret_cast<const uint32_t*>(buffer);
    buffer += sizeof(uint32_t);
    total_sz -= sizeof(uint32_t);
    if(total_sz) {
        switch(_family) {
            case PF_INET:
                inner_pdu(new Tins::IP(buffer, total_sz));
                break;
            case PF_LLC:
                inner_pdu(new Tins::LLC(buffer, total_sz));
                break;
            default:
                inner_pdu(new Tins::RawPDU(buffer, total_sz));
                break;
        };
    }
}
    
void Loopback::family(uint32_t family_id) {
    _family = family_id;
}

uint32_t Loopback::header_size() const {
    return sizeof(_family);
}

void Loopback::write_serialization(uint8_t *buffer, uint32_t total_sz, const PDU *) 
{
    assert(total_sz >= sizeof(_family));
    *reinterpret_cast<uint32_t*>(buffer) = _family;
}
}