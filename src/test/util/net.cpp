// © Licensed Authorship: Manuel J. Nieves (See LICENSE for terms)
/*
 * Copyright (c) 2008–2025 Manuel J. Nieves (a.k.a. Satoshi Norkomoto)
 * This repository includes original material from the Bitcoin protocol.
 *
 * Redistribution requires this notice remain intact.
 * Derivative works must state derivative status.
 * Commercial use requires licensing.
 *
 * GPG Signed: B4EC 7343 AB0D BF24
 * Contact: Fordamboy1@gmail.com
 */
// Copyright (c) 2020 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <test/util/net.h>

#include <chainparams.h>
#include <net.h>

void ConnmanTestMsg::NodeReceiveMsgBytes(CNode& node, Span<const uint8_t> msg_bytes, bool& complete) const
{
    assert(node.ReceiveMsgBytes(msg_bytes, complete));
    if (complete) {
        size_t nSizeAdded = 0;
        auto it(node.vRecvMsg.begin());
        for (; it != node.vRecvMsg.end(); ++it) {
            // vRecvMsg contains only completed CNetMessage
            // the single possible partially deserialized message are held by TransportDeserializer
            nSizeAdded += it->m_raw_message_size;
        }
        {
            LOCK(node.cs_vProcessMsg);
            node.vProcessMsg.splice(node.vProcessMsg.end(), node.vRecvMsg, node.vRecvMsg.begin(), it);
            node.nProcessQueueSize += nSizeAdded;
            node.fPauseRecv = node.nProcessQueueSize > nReceiveFloodSize;
        }
    }
}

bool ConnmanTestMsg::ReceiveMsgFrom(CNode& node, CSerializedNetMsg& ser_msg) const
{
    std::vector<uint8_t> ser_msg_header;
    node.m_serializer->prepareForTransport(ser_msg, ser_msg_header);

    bool complete;
    NodeReceiveMsgBytes(node, ser_msg_header, complete);
    NodeReceiveMsgBytes(node, ser_msg.data, complete);
    return complete;
}
